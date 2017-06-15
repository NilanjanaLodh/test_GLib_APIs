#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "mydbus.h"

static void on_name_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data);

static gboolean on_handle_add(MyDBusCalculator *interface,
                              GDBusMethodInvocation *invocation,
                              const gint num1, const gint num2,
                              gpointer user_data);

static gboolean on_handle_sub(MyDBusCalculator *interface,
                              GDBusMethodInvocation *invocation,
                              const gint num1, const gint num2,
                              gpointer user_data);

static gboolean on_handle_configure_alarm(MyDBusAlarm *interface,
                                    GDBusMethodInvocation *invocation,
                                    const guint seconds,
                                    gpointer user_data);

static gboolean emit_alarm_cb(gpointer interface);



int main()
{
    GMainLoop *loop;
    loop = g_main_loop_new(NULL, FALSE);

    g_bus_own_name(G_BUS_TYPE_SESSION, "com.Nilanjana2", 0, NULL, on_name_acquired, NULL, NULL, NULL);

    g_main_loop_run(loop);
}

static void
on_name_acquired(GDBusConnection *connection,
                 const gchar *name,
                 gpointer user_data)
{
    MyDBusCalculator *interface;
    GError *error;

    interface = my_dbus_calculator_skeleton_new();
    g_signal_connect(interface, "handle-add", G_CALLBACK(on_handle_add), NULL);
    g_signal_connect(interface, "handle-sub", G_CALLBACK(on_handle_sub), NULL);
    error = NULL;
    g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(interface), connection, "/", &error);

    MyDBusAlarm *alarm_interface;
    alarm_interface = my_dbus_alarm_skeleton_new();
    g_signal_connect(alarm_interface, "handle-configure-alarm", G_CALLBACK(on_handle_configure_alarm), NULL);
    error = NULL;
    g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(alarm_interface), connection, "/", &error);
}

static gboolean
on_handle_add(MyDBusCalculator *interface,
              GDBusMethodInvocation *invocation,
              const gint num1, const gint num2,
              gpointer user_data)
{
    gint ans = num1 + num2;
    g_print("Added %d and %d.\n", num1, num2);
    g_print("Press enter to send messaage to client .. ");
    getchar();
    my_dbus_calculator_complete_add(interface, invocation, ans);
    g_print("       Message sent!\n");

    return FALSE;
}

static gboolean
on_handle_sub(MyDBusCalculator *interface,
              GDBusMethodInvocation *invocation,
              const gint num1, const gint num2,
              gpointer user_data)
{
    gint ans = num1 - num2;
    my_dbus_calculator_complete_sub(interface, invocation, ans);
    g_print("Subtracted %d from %d.\n", num2, num1);

    return TRUE;
}

static gboolean on_handle_configure_alarm(MyDBusAlarm *interface,
                                    GDBusMethodInvocation *invocation,
                                    const guint seconds,
                                    gpointer user_data)
{
    if (my_dbus_alarm_get_activated(interface))
    {
        g_dbus_method_invocation_return_error_literal(invocation, G_IO_ERROR, G_IO_ERROR_EXISTS, "Alarm already exists");
        return TRUE;
    }

    my_dbus_alarm_set_activated(interface, TRUE);
    g_timeout_add_seconds(seconds, emit_alarm_cb, interface);
    my_dbus_alarm_complete_configure_alarm(interface, invocation);

    return TRUE;
}

static gboolean emit_alarm_cb(gpointer interface)
{
    my_dbus_alarm_emit_beep(MY_DBUS_ALARM(interface));
    my_dbus_alarm_set_activated(MY_DBUS_ALARM(interface),FALSE);
    return FALSE;
}