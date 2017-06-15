#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>
#include "frontend_interface.h"
#include "backend_interface.h"
#include "commondefs.h"

/**********STRUCTURE DEFINITIONS (used later)*************/
#define DIALOG_BUS_NAME "com.MyDialog"

struct _PrintBackendObj
{
    PrintBackend *proxy;
    char *bus_name;
    char *obj_path;
};

struct _Dialog
{
    int num_backends;
    GArray *backends;
};

typedef struct _PrintBackendObj PrintBackendObj;
typedef struct _Dialog Dialog;

/*********************************************************/

/********Helper functions********************************/
Dialog *get_new_Dialog()
{
    Dialog *d = malloc(sizeof(Dialog));
    d->num_backends = 0;
    d->backends = g_array_new(FALSE, FALSE, sizeof(PrintBackendObj *));
}

void add_backend(Dialog *d, const char *bus_name, const char *obj_path)
{
    PrintBackendObj *pb_obj = malloc(sizeof(PrintBackendObj));
    pb_obj->bus_name = malloc(sizeof(bus_name) + 1);
    strcpy(pb_obj->bus_name, bus_name);

    pb_obj->obj_path = malloc(sizeof(obj_path) + 1);
    strcpy(pb_obj->obj_path, obj_path);

    GError *error = NULL;
    pb_obj->proxy = print_backend_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION, 0,
                                                         bus_name, obj_path,
                                                         NULL, &error);
    g_assert_no_error(error);

    g_array_append_val(d->backends, pb_obj);
    d->num_backends = d->num_backends + 1;
}

/*******************************************************/

/**********Function prototypes**************************/
static void on_name_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data);

static void on_backend_reply(GDBusConnection *connection,
                             const gchar *sender_name,
                             const gchar *object_path,
                             const gchar *interface_name,
                             const gchar *signal_name,
                             GVariant *parameters,
                             gpointer user_data);

static gboolean idle_function(gpointer user_data);
/*******************************************************/

int main()
{
    Dialog *d;
    d = get_new_Dialog();

    g_bus_own_name(G_BUS_TYPE_SESSION, DIALOG_BUS_NAME, 0, NULL, on_name_acquired, NULL, d, NULL);

    //g_idle_add(idle_function,d);
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
}

static void
on_name_acquired(GDBusConnection *connection,
                 const gchar *name,
                 gpointer user_data)
{
    Dialog *d = (Dialog *)user_data;
    PrintFrontend *interface;
    interface = print_frontend_skeleton_new();
    GError *error;

    g_dbus_connection_signal_subscribe(connection,
                                       NULL,         /**listen to all senders**/
                                       NULL,         /**match on all interfaces**/
                                       REPLY_SIGNAL, ///////
                                       NULL,         /**match on all object paths**/
                                       NULL,         /**match on all arguments**/
                                       0,
                                       on_backend_reply,
                                       user_data, /**user_data** argument passed to the callback function **/
                                       NULL);

    error = NULL;
    g_dbus_interface_skeleton_export(G_DBUS_INTERFACE_SKELETON(interface), connection, "/", &error);
    g_assert_no_error(error);

    print_frontend_emit_activate_backend(PRINT_FRONTEND(interface));
}

static void on_backend_reply(GDBusConnection *connection,
                             const gchar *sender_name,
                             const gchar *object_path,
                             const gchar *interface_name,
                             const gchar *signal_name,
                             GVariant *parameters,
                             gpointer user_data)
{
    Dialog *d = (Dialog *)user_data;
    g_print("Sender name %s\nObject Path %s\nInterface name %s\n", sender_name, object_path, interface_name);
    add_backend(d, sender_name, object_path);
    g_print("%d Backends available now\n", d->num_backends);
}

static gboolean idle_function(gpointer user_data)
{
    Dialog *d = (Dialog *)user_data;
    int i;
    g_print("%d backends available.\n", d->num_backends);
    PrintBackendObj *pb_obj;
    for (i = 0; i < (d->num_backends); i++)
    {
        pb_obj = g_array_index(d->backends, PrintBackendObj *, i);
        print_backend_call_hello_world_sync(pb_obj->proxy, NULL, NULL);
    }

    return  G_SOURCE_CONTINUE ;
}
int main_old(int argc, char **argv)
{

    PrintBackend *proxy_cups, *proxy_dummy;
    GError *error;
    error = NULL;
    proxy_cups = print_backend_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION, 0,
                                                      "com.CUPS", "/",
                                                      NULL, &error);

    proxy_dummy = print_backend_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION, 0,
                                                       "com.DUMMY", "/",
                                                       NULL, &error);

    print_backend_call_hello_world_sync(proxy_cups, NULL, &error);
    print_backend_call_hello_world_sync(proxy_dummy, NULL, &error);
}