#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "backend_interface.h"
#include "commondefs.h"

#define BUS_NAME "com.DUMMY"
#define OBJ_PATH "/"

static void on_name_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data);

static gboolean on_handle_hello_world(PrintBackend *interface,
                                      GDBusMethodInvocation *invocation,
                                      gpointer user_data);

static void on_beep_beep(GDBusConnection *connection,
                         const gchar *sender_name,
                         const gchar *object_path,
                         const gchar *interface_name,
                         const gchar *signal_name,
                         GVariant *parameters,
                         gpointer user_data);

int main()
{
    GMainLoop *loop;
    loop = g_main_loop_new(NULL, FALSE);

    g_bus_own_name(G_BUS_TYPE_SESSION, BUS_NAME, 0, NULL, on_name_acquired, NULL, NULL, NULL);

    g_main_loop_run(loop);
}

static void
on_name_acquired(GDBusConnection *connection,
                 const gchar *name,
                 gpointer user_data)
{
    PrintBackend *interface;
    GError *error;

    interface = print_backend_skeleton_new();
    g_signal_connect(interface, "handle-hello-world", G_CALLBACK(on_handle_hello_world), NULL);

    g_dbus_connection_signal_subscribe(connection,
                                       NULL, /**listen to all senders**/
                                       NULL, /**match on all interfaces**/
                                       ACTIVATE_SIGNAL, ///////
                                       NULL, /**match on all object paths**/
                                       NULL, /**match on all arguments**/
                                       0,
                                       on_beep_beep,
                                       interface, /**user_data** argument passed to the callback function **/
                                       NULL);
    error = NULL;
    g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(interface), connection, OBJ_PATH, &error);
}

static gboolean on_handle_hello_world(PrintBackend *interface,
                                      GDBusMethodInvocation *invocation,
                                      gpointer user_data)
{
    g_print("Hello there, from the DUMMY backend!\n");
    print_backend_complete_hello_world(interface, invocation);

    return TRUE;
}

static void on_beep_beep(GDBusConnection *connection,
                         const gchar *sender_name,
                         const gchar *object_path,
                         const gchar *interface_name,
                         const gchar *signal_name,
                         GVariant *parameters,
                         gpointer user_data)
{

    g_print("Yaaaaaaaaaaaaaaay signal caught! :) %s \n", signal_name);
    print_backend_emit_backend_reply(PRINT_BACKEND(user_data));
}