/*
 *  Copyright © 2017-2022 Wellington Wallace
 *
 *  This file is part of EasyEffects.
 *
 *  EasyEffects is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  EasyEffects is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with EasyEffects.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "plugin_preset_base.hpp"
#include "tags_equalizer.hpp"

class EqualizerPreset : public PluginPresetBase {
 public:
 explicit EqualizerPreset(PresetType preset_type, const int& index = 0);
  ~EqualizerPreset();

 private:
  GSettings *input_settings_left = nullptr, *input_settings_right = nullptr, *output_settings_left = nullptr,
            *output_settings_right = nullptr;

  void save(nlohmann::json& json) override;

  void load(const nlohmann::json& json) override;

  static void save_channel(nlohmann::json& json, GSettings* settings, const int& nbands);

  void load_channel(const nlohmann::json& json, GSettings* settings, const int& nbands);

  void load_legacy_preset();
};
