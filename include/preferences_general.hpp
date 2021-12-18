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

#include <adwaita.h>
#include <fstream>
#include <string>
#include "ui_helpers.hpp"
#include "util.hpp"
#include "libportal/background.h"

namespace ui::preferences::general {

G_BEGIN_DECLS

#define EE_TYPE_PREFERENCES_GENERAL (preferences_general_get_type())

G_DECLARE_FINAL_TYPE(PreferencesGeneral, preferences_general, EE, PREFERENCES_GENERAL, AdwPreferencesPage)

G_END_DECLS

auto create() -> PreferencesGeneral*;

}  // namespace ui::preferences::general