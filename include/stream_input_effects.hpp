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

#include "effects_base.hpp"

class StreamInputEffects : public EffectsBase {
 public:
  explicit StreamInputEffects(PipeManager* pipe_manager);
  StreamInputEffects(const StreamInputEffects&) = delete;
  auto operator=(const StreamInputEffects&) -> StreamInputEffects& = delete;
  StreamInputEffects(const StreamInputEffects&&) = delete;
  auto operator=(const StreamInputEffects&&) -> StreamInputEffects& = delete;
  ~StreamInputEffects() override;

  void set_bypass(const bool& state);

  void set_listen_to_mic(const bool& state);

 private:
  bool bypass = false;

  void connect_filters(const bool& bypass = false);

  void disconnect_filters();

  const auto apps_want_to_play() -> bool;

  void on_app_added(const NodeInfo &node_info);

  void on_link_changed(const LinkInfo& link_info);
};
