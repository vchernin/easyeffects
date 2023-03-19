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

#include "echo_canceller.hpp"

EchoCanceller::EchoCanceller(const std::string& tag,
                             const std::string& schema,
                             const std::string& schema_path,
                             PipeManager* pipe_manager)
    : PluginBase(tag,
                 tags::plugin_name::echo_canceller,
                 tags::plugin_package::speex,
                 schema,
                 schema_path,
                 pipe_manager,
                 true),
      residual_echo_suppression(g_settings_get_int(settings, "residual-echo-suppression")),
      near_end_suppression(g_settings_get_int(settings, "near-end-suppression")) {
  gconnections.push_back(g_signal_connect(settings, "changed::filter-length",
                                          G_CALLBACK(+[](GSettings* settings, char* key, gpointer user_data) {
                                            auto self = static_cast<EchoCanceller*>(user_data);

                                            std::scoped_lock<std::mutex> lock(self->data_mutex);

                                            self->filter_length_ms = g_settings_get_int(settings, key);

                                            self->init_speex();
                                          }),
                                          this));

#ifdef SPEEX_AVAILABLE

  gconnections.push_back(g_signal_connect(
      settings, "changed::residual-echo-suppression",
      G_CALLBACK(+[](GSettings* settings, char* key, EchoCanceller* self) {
        std::scoped_lock<std::mutex> lock(self->data_mutex);

        self->residual_echo_suppression = g_settings_get_int(settings, key);

        if (self->state_left) {
          speex_preprocess_ctl(self->state_left, SPEEX_PREPROCESS_SET_ECHO_SUPPRESS, &self->residual_echo_suppression);
        }

        if (self->state_right) {
          speex_preprocess_ctl(self->state_right, SPEEX_PREPROCESS_SET_ECHO_SUPPRESS, &self->residual_echo_suppression);
        }
      }),
      this));

  gconnections.push_back(g_signal_connect(
      settings, "changed::near-end-suppression", G_CALLBACK(+[](GSettings* settings, char* key, EchoCanceller* self) {
        std::scoped_lock<std::mutex> lock(self->data_mutex);

        self->near_end_suppression = g_settings_get_int(settings, key);

        if (self->state_left) {
          speex_preprocess_ctl(self->state_left, SPEEX_PREPROCESS_SET_ECHO_SUPPRESS_ACTIVE,
                               &self->near_end_suppression);
        }

        if (self->state_right) {
          speex_preprocess_ctl(self->state_right, SPEEX_PREPROCESS_SET_ECHO_SUPPRESS_ACTIVE,
                               &self->near_end_suppression);
        }
      }),
      this));

#endif

  setup_input_output_gain();
}

EchoCanceller::~EchoCanceller() {
  if (connected_to_pw) {
    disconnect_from_pw();
  }

  data_mutex.lock();

  ready = false;

  if (echo_state_L != nullptr) {
    speex_echo_state_destroy(echo_state_L);
  }

  if (echo_state_R != nullptr) {
    speex_echo_state_destroy(echo_state_R);
  }

#ifdef SPEEX_AVAILABLE
  free_speex();
#endif

  data_mutex.unlock();

  util::debug(log_tag + name + " destroyed");
}

void EchoCanceller::setup() {
  std::scoped_lock<std::mutex> const lock(data_mutex);

  ready = false;

  notify_latency = true;

  latency_n_frames = 0U;

  init_speex();
}

void EchoCanceller::process(std::span<float>& left_in,
                            std::span<float>& right_in,
                            std::span<float>& left_out,
                            std::span<float>& right_out,
                            std::span<float>& probe_left,
                            std::span<float>& probe_right) {
  std::scoped_lock<std::mutex> const lock(data_mutex);

  if (bypass || !ready) {
    std::copy(left_in.begin(), left_in.end(), left_out.begin());
    std::copy(right_in.begin(), right_in.end(), right_out.begin());

    return;
  }

  if (input_gain != 1.0F) {
    apply_gain(left_in, right_in, input_gain);
  }

  for (size_t j = 0U; j < left_in.size(); j++) {
    data_L[j] = static_cast<spx_int16_t>(left_in[j] * (SHRT_MAX + 1));
    data_R[j] = static_cast<spx_int16_t>(right_in[j] * (SHRT_MAX + 1));

    /*
      This is a very naive and not corect attempt to mitigate the shortcomes discussed at
      https://github.com/wwmm/easyeffects/issues/1566.
    */

    probe_mono[j] = static_cast<spx_int16_t>(0.5F * (probe_left[j] + probe_right[j]) * (SHRT_MAX + 1));
  }

  speex_echo_cancellation(echo_state_L, data_L.data(), probe_mono.data(), filtered_L.data());
  speex_echo_cancellation(echo_state_R, data_R.data(), probe_mono.data(), filtered_R.data());

#ifdef SPEEX_AVAILABLE
  speex_preprocess_run(state_left, filtered_L.data());
  speex_preprocess_run(state_right, filtered_R.data());
#endif

  for (size_t j = 0U; j < filtered_L.size(); j++) {
    left_out[j] = static_cast<float>(filtered_L[j]) * inv_short_max;

    right_out[j] = static_cast<float>(filtered_R[j]) * inv_short_max;
  }

  if (output_gain != 1.0F) {
    apply_gain(left_out, right_out, output_gain);
  }

  if (notify_latency) {
    const float latency_value = static_cast<float>(latency_n_frames) / static_cast<float>(rate);

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

void EchoCanceller::init_speex() {
  if (n_samples == 0U || rate == 0U) {
    return;
  }

  data_L.resize(n_samples);
  data_R.resize(n_samples);
  probe_mono.resize(n_samples);
  filtered_L.resize(n_samples);
  filtered_R.resize(n_samples);

  const uint filter_length = static_cast<uint>(0.001F * static_cast<float>(filter_length_ms * rate));

  util::debug(log_tag + name + " filter length: " + util::to_string(filter_length));

  if (echo_state_L != nullptr) {
    speex_echo_state_destroy(echo_state_L);
  }

  echo_state_L = speex_echo_state_init(static_cast<int>(n_samples), static_cast<int>(filter_length));

  if (speex_echo_ctl(echo_state_L, SPEEX_ECHO_SET_SAMPLING_RATE, &rate) != 0) {
    util::warning(log_tag + name + "SPEEX_ECHO_SET_SAMPLING_RATE: unknown request");
  }

  if (echo_state_R != nullptr) {
    speex_echo_state_destroy(echo_state_R);
  }

  echo_state_R = speex_echo_state_init(static_cast<int>(n_samples), static_cast<int>(filter_length));

  if (speex_echo_ctl(echo_state_R, SPEEX_ECHO_SET_SAMPLING_RATE, &rate) != 0) {
    util::warning(log_tag + name + "SPEEX_ECHO_SET_SAMPLING_RATE: unknown request");
  }

#ifdef SPEEX_AVAILABLE
  if (state_left != nullptr) {
    speex_preprocess_state_destroy(state_left);
  }

  if (state_right != nullptr) {
    speex_preprocess_state_destroy(state_right);
  }

  state_left = speex_preprocess_state_init(static_cast<int>(n_samples), static_cast<int>(rate));
  state_right = speex_preprocess_state_init(static_cast<int>(n_samples), static_cast<int>(rate));

  if (state_left != nullptr) {
    speex_preprocess_ctl(state_left, SPEEX_PREPROCESS_SET_ECHO_STATE, echo_state_L);

    speex_preprocess_ctl(state_left, SPEEX_PREPROCESS_SET_ECHO_SUPPRESS, &residual_echo_suppression);

    speex_preprocess_ctl(state_left, SPEEX_PREPROCESS_SET_ECHO_SUPPRESS_ACTIVE, &near_end_suppression);
  }

  if (state_right != nullptr) {
    speex_preprocess_ctl(state_right, SPEEX_PREPROCESS_SET_ECHO_STATE, echo_state_R);

    speex_preprocess_ctl(state_right, SPEEX_PREPROCESS_SET_ECHO_SUPPRESS, &residual_echo_suppression);

    speex_preprocess_ctl(state_right, SPEEX_PREPROCESS_SET_ECHO_SUPPRESS_ACTIVE, &near_end_suppression);
  }

#else
  util::warning("The Speex library was not available at compilation time. The noise reduction filter won't work");
#endif

  ready = true;
}

#ifdef SPEEX_AVAILABLE

void EchoCanceller::free_speex() {
  if (state_left != nullptr) {
    speex_preprocess_state_destroy(state_left);
  }

  if (state_right != nullptr) {
    speex_preprocess_state_destroy(state_right);
  }

  state_left = nullptr;
  state_right = nullptr;
}

#endif

auto EchoCanceller::get_latency_seconds() -> float {
  return latency_value;
}
