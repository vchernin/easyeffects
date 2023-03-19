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

#pragma once

#include "fir_filter_base.hpp"

class FirFilterLowpass : public FirFilterBase {
 public:
  explicit FirFilterLowpass(std::string tag);
  FirFilterLowpass(const FirFilterLowpass&) = delete;
  auto operator=(const FirFilterLowpass&) -> FirFilterLowpass& = delete;
  FirFilterLowpass(const FirFilterLowpass&&) = delete;
  auto operator=(const FirFilterLowpass&&) -> FirFilterLowpass& = delete;
  ~FirFilterLowpass() override;

  void setup() override;
};
