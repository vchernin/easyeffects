/*
 *  Copyright © 2017-2020 Wellington Wallace
 *
 *  This file is part of PulseEffects.
 *
 *  PulseEffects is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  PulseEffects is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with PulseEffects.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "bass_enhancer.hpp"

BassEnhancer::BassEnhancer(const std::string& tag,
                           const std::string& schema,
                           const std::string& schema_path,
                           PipeManager* pipe_manager)
    : PluginBase(tag, "bass_enhancer", schema, schema_path, pipe_manager) {}

BassEnhancer::~BassEnhancer() {
  util::debug(log_tag + name + " destroyed");
}

void BassEnhancer::setup() {
  // data.resize(n_samples * 2);
}

void BassEnhancer::process(const std::vector<float>& left_in,
                           const std::vector<float>& right_in,
                           std::span<float>& left_out,
                           std::span<float>& right_out) {
  if (bypass) {
    std::copy(left_in.begin(), left_in.end(), left_out.begin());
    std::copy(right_in.begin(), right_in.end(), right_out.begin());

    return;
  }

  std::copy(left_in.begin(), left_in.end(), left_out.begin());
  std::copy(right_in.begin(), right_in.end(), right_out.begin());

  if (post_messages) {
    get_peaks(left_in, right_in, left_out, right_out);

    notification_dt += sample_duration;

    if (notification_dt >= notification_time_window) {
      notify();

      notification_dt = 0.0F;
    }
  }
}

void BassEnhancer::bind_to_gsettings() {
  // g_settings_bind_with_mapping(settings, "input-gain", bass_enhancer, "level-in", G_SETTINGS_BIND_DEFAULT,
  //                              util::db20_gain_to_linear, util::linear_gain_to_db20, nullptr, nullptr);

  // g_settings_bind_with_mapping(settings, "output-gain", bass_enhancer, "level-out", G_SETTINGS_BIND_DEFAULT,
  //                              util::db20_gain_to_linear, util::linear_gain_to_db20, nullptr, nullptr);

  // g_settings_bind_with_mapping(settings, "amount", bass_enhancer, "amount", G_SETTINGS_BIND_DEFAULT,
  //                              util::db20_gain_to_linear, util::linear_gain_to_db20, nullptr, nullptr);

  // g_settings_bind_with_mapping(settings, "harmonics", bass_enhancer, "drive", G_SETTINGS_BIND_GET,
  //                              util::double_to_float, nullptr, nullptr, nullptr);

  // g_settings_bind_with_mapping(settings, "scope", bass_enhancer, "freq", G_SETTINGS_BIND_GET,
  // util::double_to_float,
  //                              nullptr, nullptr, nullptr);

  // g_settings_bind_with_mapping(settings, "floor", bass_enhancer, "floor", G_SETTINGS_BIND_GET,
  // util::double_to_float,
  //                              nullptr, nullptr, nullptr);

  // g_settings_bind_with_mapping(settings, "blend", bass_enhancer, "blend", G_SETTINGS_BIND_GET,
  // util::double_to_float,
  //                              nullptr, nullptr, nullptr);

  // g_settings_bind(settings, "floor-active", bass_enhancer, "floor-active", G_SETTINGS_BIND_DEFAULT);

  // g_settings_bind(settings, "listen", bass_enhancer, "listen", G_SETTINGS_BIND_DEFAULT);
}
