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

#include "preferences_general.hpp"

namespace ui::preferences::general {

using namespace std::string_literals;

auto constexpr log_tag = "preferences_general: ";

struct _PreferencesGeneral {
  AdwPreferencesPage parent_instance;

  GtkSwitch *enable_autostart, *process_all_inputs, *process_all_outputs, *theme_switch, *shutdown_on_window_close,
      *use_cubic_volumes, *autohide_popovers;

  GSettings* settings;
};

G_DEFINE_TYPE(PreferencesGeneral, preferences_general, ADW_TYPE_PREFERENCES_PAGE)

inline static XdpPortal* portal = nullptr;
static void update_background_portal(const bool& state);
static void on_request_background_called(GObject* source, GAsyncResult* result, gpointer data);

/*
auto on_enable_autostart(GtkSwitch* obj, gboolean state, gpointer user_data) -> gboolean {
  std::filesystem::path autostart_dir{g_get_user_config_dir() + "/autostart"s};

  if (!std::filesystem::is_directory(autostart_dir)) {
    std::filesystem::create_directories(autostart_dir);
  }

  std::filesystem::path autostart_file{g_get_user_config_dir() + "/autostart/easyeffects-service.desktop"s};

  if (state != 0) {
    if (!std::filesystem::exists(autostart_file)) {
      std::ofstream ofs{autostart_file};

      ofs << "[Desktop Entry]\n";
      ofs << "Name=EasyEffects\n";
      ofs << "Comment=EasyEffects Service\n";
      ofs << "Exec=easyeffects --gapplication-service\n";
      ofs << "Icon=easyeffects\n";
      ofs << "StartupNotify=false\n";
      ofs << "Terminal=false\n";
      ofs << "Type=Application\n";

      ofs.close();

      util::debug(log_tag + "autostart file created"s);
    }
  } else {
    if (std::filesystem::exists(autostart_file)) {
      std::filesystem::remove(autostart_file);

      util::debug(log_tag + "autostart file removed"s);
    }
  }

  return 0;
}
*/

void dispose(GObject* object) {
  auto* self = EE_PREFERENCES_GENERAL(object);

  g_object_unref(self->settings);

  util::debug(log_tag + "disposed"s);

  G_OBJECT_CLASS(preferences_general_parent_class)->dispose(object);
}

void preferences_general_class_init(PreferencesGeneralClass* klass) {
  auto* object_class = G_OBJECT_CLASS(klass);
  auto* widget_class = GTK_WIDGET_CLASS(klass);

  object_class->dispose = dispose;

  gtk_widget_class_set_template_from_resource(widget_class, "/com/github/wwmm/easyeffects/ui/preferences_general.ui");

  gtk_widget_class_bind_template_child(widget_class, PreferencesGeneral, enable_autostart);
  gtk_widget_class_bind_template_child(widget_class, PreferencesGeneral, process_all_inputs);
  gtk_widget_class_bind_template_child(widget_class, PreferencesGeneral, process_all_outputs);
  gtk_widget_class_bind_template_child(widget_class, PreferencesGeneral, theme_switch);
  gtk_widget_class_bind_template_child(widget_class, PreferencesGeneral, autohide_popovers);
  gtk_widget_class_bind_template_child(widget_class, PreferencesGeneral, shutdown_on_window_close);
  gtk_widget_class_bind_template_child(widget_class, PreferencesGeneral, use_cubic_volumes);
}

void preferences_general_init(PreferencesGeneral* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  self->settings = g_settings_new("com.github.wwmm.easyeffects");

  if (portal == nullptr) {
     portal = xdp_portal_new();
  }
    
  update_background_portal(steve->get_boolean("enable-autostart"));

  // initializing some widgets

 // gtk_switch_set_active(self->enable_autostart,
 //                      static_cast<gboolean>(std::filesystem::is_regular_file(
  //                          g_get_user_config_dir() + "/autostart/easyeffects-service.desktop"s)));

  gsettings_bind_widgets<"process-all-inputs", "process-all-outputs", "use-dark-theme", "shutdown-on-window-close",
                         "use-cubic-volumes", "autohide-popovers", "enable-autostart">(
      self->settings, self->process_all_inputs, self->process_all_outputs, self->theme_switch,
      self->shutdown_on_window_close, self->use_cubic_volumes, self->autohide_popovers, self->enable_autostart);
      
  update_background_portal(self->get_boolean("enable-autostart"));
  
  settings->signal_changed("enable-autostart").connect([=, this](const auto& key) {
    update_background_portal(settings->get_boolean(key));
  });
}

auto create() -> PreferencesGeneral* {
  return static_cast<PreferencesGeneral*>(g_object_new(EE_TYPE_PREFERENCES_GENERAL, nullptr));
}

void update_background_portal(const bool& state) {
  XdpBackgroundFlags background_flags = XDP_BACKGROUND_FLAG_NONE;

  g_autoptr(GPtrArray) command_line = nullptr;

  if (state) {
    command_line = g_ptr_array_new_with_free_func(g_free);

    g_ptr_array_add(command_line, g_strdup("easyeffects"));
    g_ptr_array_add(command_line, g_strdup("--gapplication-service"));

    background_flags = XDP_BACKGROUND_FLAG_AUTOSTART;
  }

  auto* reason = g_strdup("EasyEffects Autostart");

  xdp_portal_request_background(portal, nullptr, reason, command_line, background_flags, NULL,
                                on_request_background_called, nullptr);

  g_free(reason);
}

void on_request_background_called(GObject* source, GAsyncResult* result, gpointer data) {
  g_autoptr(GError) error = nullptr;

  if (!xdp_portal_request_background_finish(portal, result, &error)) {
    util::warning(std::string("portal: background request failed:") + ((error) ? error->message : "unknown reason"));

    return;
  }

  util::debug("portal: background request successfully completed");
}

}  // namespace ui::preferences::general