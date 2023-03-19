/*
 *  Copyright © 2017-2023 Wellington Wallace
 *
 *  This file is part of Easy Effects.
 *
 *  Easy Effects is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Easy Effects is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Easy Effects. If not, see <https://www.gnu.org/licenses/>.
 */

#include "rnnoise.hpp"

RNNoise::RNNoise(const std::string& tag,
                 const std::string& schema,
                 const std::string& schema_path,
                 PipeManager* pipe_manager)
    : PluginBase(tag, tags::plugin_name::rnnoise, tags::plugin_package::rnnoise, schema, schema_path, pipe_manager),
      rnnoise_ready(true), data_L(0),
      data_R(0), state_left(rnnoise_create(model)), state_right(rnnoise_create(model)) {
  data_L.reserve(blocksize);
  data_R.reserve(blocksize);

  gconnections.push_back(g_signal_connect(settings, "changed::model-path",
                                          G_CALLBACK(+[](GSettings*  /*settings*/, char*  /*key*/, gpointer user_data) {
                                            auto self = static_cast<RNNoise*>(user_data);

                                            self->data_mutex.lock();

                                            self->rnnoise_ready = false;

                                            self->data_mutex.unlock();

#ifdef RNNOISE_AVAILABLE
                                            self->free_rnnoise();

                                            auto* m = self->get_model_from_file();

                                            self->model = m;

                                            self->state_left = rnnoise_create(self->model);
                                            self->state_right = rnnoise_create(self->model);

                                            self->rnnoise_ready = true;
#endif
                                          }),
                                          this));

  setup_input_output_gain();

#ifdef RNNOISE_AVAILABLE

  auto* m = get_model_from_file();

  model = m;

  
  

  
#else
  util::warning("The RNNoise library was not available at compilation time. The noise reduction filter won't work");
#endif
}

RNNoise::~RNNoise() {
  if (connected_to_pw) {
    disconnect_from_pw();
  }

  std::scoped_lock<std::mutex> const lock(data_mutex);

  resampler_ready = false;

#ifdef RNNOISE_AVAILABLE
  free_rnnoise();
#endif

  util::debug(log_tag + name + " destroyed");
}

void RNNoise::setup() {
  std::scoped_lock<std::mutex> const lock(data_mutex);

  resampler_ready = false;

  latency_n_frames = 0U;

  resample = rate != rnnoise_rate;

  data_L.resize(0U);
  data_R.resize(0U);

  deque_out_L.resize(0U);
  deque_out_R.resize(0U);

  resampler_inL = std::make_unique<Resampler>(rate, rnnoise_rate);
  resampler_inR = std::make_unique<Resampler>(rate, rnnoise_rate);

  resampler_outL = std::make_unique<Resampler>(rnnoise_rate, rate);
  resampler_outR = std::make_unique<Resampler>(rnnoise_rate, rate);

  resampler_ready = true;
}

void RNNoise::process(std::span<float>& left_in,
                      std::span<float>& right_in,
                      std::span<float>& left_out,
                      std::span<float>& right_out) {
  std::scoped_lock<std::mutex> const lock(data_mutex);

  if (bypass || !rnnoise_ready) {
    std::copy(left_in.begin(), left_in.end(), left_out.begin());
    std::copy(right_in.begin(), right_in.end(), right_out.begin());

    return;
  }

  if (input_gain != 1.0F) {
    apply_gain(left_in, right_in, input_gain);
  }

  if (resample) {
    if (resampler_ready) {
      const auto resampled_inL = resampler_inL->process(left_in, false);
      const auto resampled_inR = resampler_inR->process(right_in, false);

      resampled_data_L.resize(0U);
      resampled_data_R.resize(0U);

#ifdef RNNOISE_AVAILABLE
      remove_noise(resampled_inL, resampled_inR, resampled_data_L, resampled_data_R);
#endif

      auto resampled_outL = resampler_outL->process(resampled_data_L, false);
      auto resampled_outR = resampler_outR->process(resampled_data_R, false);

      for (const auto& v : resampled_outL) {
        deque_out_L.push_back(v);
      }

      for (const auto& v : resampled_outR) {
        deque_out_R.push_back(v);
      }
    } else {
      for (const auto& v : left_in) {
        deque_out_L.push_back(v);
      }

      for (const auto& v : right_in) {
        deque_out_R.push_back(v);
      }
    }
  } else {
#ifdef RNNOISE_AVAILABLE
    remove_noise(left_in, right_in, deque_out_L, deque_out_R);
#endif
  }

  if (deque_out_L.size() >= left_out.size()) {
    for (float& v : left_out) {
      v = deque_out_L.front();

      deque_out_L.pop_front();
    }

    for (float& v : right_out) {
      v = deque_out_R.front();

      deque_out_R.pop_front();
    }
  } else {
    const uint offset = 2U * (left_out.size() - deque_out_L.size());

    if (offset != latency_n_frames) {
      latency_n_frames = offset;

      notify_latency = true;
    }

    for (uint n = 0U; !deque_out_L.empty() && n < left_out.size(); n++) {
      if (n < offset) {
        left_out[n] = 0.0F;
        right_out[n] = 0.0F;
      } else {
        left_out[n] = deque_out_L.front();
        right_out[n] = deque_out_R.front();

        deque_out_R.pop_front();
        deque_out_L.pop_front();
      }
    }
  }

  if (output_gain != 1.0F) {
    apply_gain(left_out, right_out, output_gain);
  }

  if (notify_latency) {
    latency_value = static_cast<float>(latency_n_frames) / static_cast<float>(rate);

    util::debug(log_tag + name + " latency: " + util::to_string(latency_value, "") + " s");

    util::idle_add([=, this]() {
      if (!post_messages || latency.empty()) {
        return;
      }

      latency.emit();
    });

    spa_process_latency_info latency_info{};

    latency_info.ns = static_cast<uint64_t>(latency_value * 1000000000.0F);

    std::array<char, 1024U> buffer{};

    spa_pod_builder b{};

    spa_pod_builder_init(&b, buffer.data(), sizeof(buffer));

    const spa_pod* param = spa_process_latency_build(&b, SPA_PARAM_ProcessLatency, &latency_info);

    pw_filter_update_params(filter, nullptr, &param, 1);

    notify_latency = false;
  }

  if (post_messages) {
    get_peaks(left_in, right_in, left_out, right_out);

    if (send_notifications) {
      notify();
    }
  }
}

#ifdef RNNOISE_AVAILABLE

auto RNNoise::get_model_from_file() -> RNNModel* {
  RNNModel* m = nullptr;

  const auto path = util::gsettings_get_string(settings, "model-path");

  // Standard Model
  if (path.empty()) {
    standard_model = true;

    util::debug(log_tag + name + " using the standard model.");

    model_changed.emit(false);

    return m;
  }

  // Custom Model
  util::debug(log_tag + name + " loading custom model from file: " + path);

  if (FILE* f = fopen(path.c_str(), "r"); f != nullptr) {
    m = rnnoise_model_from_file(f);

    fclose(f);
  }

  standard_model = (m == nullptr);

  if (standard_model) {
    util::warning(log_tag + name + " failed to load the custom model. Using the standard one.");
  }

  model_changed.emit(standard_model);

  return m;
}

void RNNoise::free_rnnoise() {
  rnnoise_ready = false;

  if (state_left != nullptr) {
    rnnoise_destroy(state_left);
  }

  if (state_right != nullptr) {
    rnnoise_destroy(state_right);
  }

  if (model != nullptr) {
    rnnoise_model_free(model);
  }

  state_left = nullptr;
  state_right = nullptr;
  model = nullptr;
}

#endif

auto RNNoise::get_latency_seconds() -> float {
  return latency_value;
}
