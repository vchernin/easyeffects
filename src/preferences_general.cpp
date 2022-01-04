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

  gboolean is_autostart_switch;

  gboolean reset_shutdown, reset_autostart;

};

G_DEFINE_TYPE(PreferencesGeneral, preferences_general, ADW_TYPE_PREFERENCES_PAGE)

inline static XdpPortal* portal = nullptr;
static void update_background_portal(const bool& state, PreferencesGeneral* self);
static void on_request_background_called(GObject* source, GAsyncResult* result, gpointer data);

// generic portal update function
void update_background_portal(const bool& state, PreferencesGeneral* self) {
  XdpBackgroundFlags background_flags = XDP_BACKGROUND_FLAG_NONE;

  g_autoptr(GPtrArray) command_line = nullptr;

  if (state) {
    command_line = g_ptr_array_new_with_free_func(g_free);

    g_ptr_array_add(command_line, g_strdup("easyeffects"));
    g_ptr_array_add(command_line, g_strdup("--gapplication-service"));

    background_flags = XDP_BACKGROUND_FLAG_AUTOSTART;
  }

  auto* reason = g_strdup("EasyEffects Background Access");

  // libportal portal request
  xdp_portal_request_background(portal, nullptr, reason, command_line, background_flags, NULL,
                                on_request_background_called, self);

  g_free(reason);
}

// callback
void on_request_background_called(GObject* source, GAsyncResult* result, gpointer self) {
  g_autoptr(GError) error = nullptr;

  // libportal check if portal request worked
  // todo here check preferences with self
  if (!xdp_portal_request_background_finish(portal, result, &error)) {
    util::warning(std::string("portal: a background request failed: ") + ((error) ? error->message : "unknown reason"));
    util::warning(std::string("portal: Background portal access has likely been denied"));
    util::warning(std::string("portal: To let EasyEffects ask for the portal again, run flatpak permission-reset com.github.wwmm.easyeffects"));

    // reset switches in case there was a problem
    if (((PreferencesGeneral *)self)->is_autostart_switch || ((PreferencesGeneral *)self)->reset_autostart) {
        ((PreferencesGeneral *)self)->reset_autostart = true;
        g_settings_reset(((PreferencesGeneral *)self)->settings, "enable-autostart");
        util::warning(std::string("portal: Setting autostart state and switch to false"));
        //gtk_switch_set_state(((PreferencesGeneral *)self)->enable_autostart, false);
       // gtk_switch_set_active(((PreferencesGeneral *)self)->enable_autostart, false);
        ((PreferencesGeneral *)self)->reset_autostart = false;
    }
    if (!((PreferencesGeneral *)self)->is_autostart_switch || ((PreferencesGeneral *)self)->reset_shutdown) {
        ((PreferencesGeneral *)self)->reset_shutdown = true;
        g_settings_reset(((PreferencesGeneral *)self)->settings, "shutdown-on-window-close");
        util::warning(std::string("portal: Setting shutdown on window close state and switch to true"));
       // gtk_switch_set_state(((PreferencesGeneral *)self)->shutdown_on_window_close, true);
        //gtk_switch_set_active(((PreferencesGeneral *)self)->shutdown_on_window_close, true);
        ((PreferencesGeneral *)self)->reset_shutdown = false;
    }

    return;
  }

  // set switches to the correct setting in case it worked
  if (((PreferencesGeneral *)self)->is_autostart_switch) {
        gtk_switch_set_state(((PreferencesGeneral *)self)->enable_autostart, gtk_switch_get_active(((PreferencesGeneral *)self)->enable_autostart));
       // gtk_switch_set_active(((PreferencesGeneral *)self)->enable_autostart, true);
  }
  else if (!((PreferencesGeneral *)self)->is_autostart_switch) {
        gtk_switch_set_state(((PreferencesGeneral *)self)->shutdown_on_window_close, gtk_switch_get_active(((PreferencesGeneral *)self)->shutdown_on_window_close));
      //  gtk_switch_set_active(((PreferencesGeneral *)self)->shutdown_on_window_close, false);
  }

  util::debug("portal: a background request successfully completed");
  return;
}


gboolean on_enable_autostart(GtkSwitch* obj, gboolean state, PreferencesGeneral* self) {
    if (!self->reset_autostart) {
        self->is_autostart_switch = true;
        update_background_portal(state, self);
    }

    return true;
}

gboolean on_shutdown_on_window_close_called(GtkSwitch* btn, gboolean state, PreferencesGeneral* self) {
    // TODO should use state here, no reason to call when state is true (i.e. enabling shutdown on window close)
    // TODO THIS IS ONE OF TWO KNOWN BUGS

    // TODO the other known bug is that when resetting, and disabling autstart the portal will be called.
    // this makes sense since it's treated like a normal button switch, but here it's done by us.
    // We don't need to do this excessive call,
    // how can we reset both just one switch, or both switches with minimal portal calls?
    if (!self->reset_shutdown) {
        if (g_settings_get_boolean(self->settings, "enable-autostart")) {
            util::debug("portal: requesting both background access and autostart file since autostart is enabled");
            self->is_autostart_switch = false;
            update_background_portal(true, self);
        }
        else {
            util::debug("portal: requesting only background access, not creating autostart file");
            self->is_autostart_switch = false;
            update_background_portal(false, self);
        }
    }

    return true;

}


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

  g_settings_unbind(self->enable_autostart, "active");
  g_settings_unbind(self->shutdown_on_window_close, "active");

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

  gtk_widget_class_bind_template_callback(widget_class, on_enable_autostart);
  gtk_widget_class_bind_template_callback(widget_class, on_shutdown_on_window_close_called);
}

void preferences_general_init(PreferencesGeneral* self) {
  gtk_widget_init_template(GTK_WIDGET(self));

  self->settings = g_settings_new("com.github.wwmm.easyeffects");

  if (portal == nullptr) {
     portal = xdp_portal_new();
  }

  gsettings_bind_widgets<"process-all-inputs", "process-all-outputs", "use-dark-theme", "shutdown-on-window-close",
                         "use-cubic-volumes", "autohide-popovers", "enable-autostart">(
      self->settings, self->process_all_inputs, self->process_all_outputs, self->theme_switch,
      self->shutdown_on_window_close, self->use_cubic_volumes, self->autohide_popovers, self->enable_autostart);

  g_signal_connect(self->enable_autostart, "state-set", G_CALLBACK(on_enable_autostart), self);
  g_signal_connect(self->shutdown_on_window_close, "state-set", G_CALLBACK(on_shutdown_on_window_close_called), self);



  // sanity checks in case switch(es) was somehow already set previously.
  if (!gtk_switch_get_active(self->shutdown_on_window_close) && !gtk_switch_get_active(self->enable_autostart)) {
    util::warning(std::string("portal: Running portal sanity check, autostart and shutdown switches are disabled"));
    // self->reset_shutdown = true;
    on_shutdown_on_window_close_called(self->shutdown_on_window_close, false, self);
  }

  else if (gtk_switch_get_active(self->shutdown_on_window_close) && gtk_switch_get_active(self->enable_autostart)) {
    util::warning(std::string("portal: Running portal sanity check, autostart and shutdown switches are enabled"));
    // self->reset_autostart = true;
    on_enable_autostart(self->enable_autostart, true, self);
  }

  // first two ifs confirmed to work perfectly
  // but not this one...
  else if (!gtk_switch_get_active(self->shutdown_on_window_close) && gtk_switch_get_active(self->enable_autostart)) {
    util::warning(std::string("portal: Running portal sanity check, autostart switch is enabled and shutdown switch is disabled"));
    // bool passed to update_background_portal should not matter
   // self->reset_shutdown = true;
   // self->reset_autostart = true;
    update_background_portal(true, self);
  }
}


auto create() -> PreferencesGeneral* {
  return static_cast<PreferencesGeneral*>(g_object_new(EE_TYPE_PREFERENCES_GENERAL, nullptr));
}



}  // namespace ui::preferences::general
