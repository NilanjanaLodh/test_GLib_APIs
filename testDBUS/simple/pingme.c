#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "mydbus.h"
static void beep_cb(MyDBusAlarm *proxy, gpointer user_data)
{
    printf("Beep!\a\n");
    exit(0);
}
int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: ./pingme seconds\n");
        return 0;
    }
    guint seconds = atoi(argv[1]);

    MyDBusAlarm *proxy;
    GError *error;
    error = NULL;
    proxy = NULL;
    proxy = my_dbus_alarm_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION,
                                                 0, "com.Nilanjana",
                                                 "/", NULL, &error);

    g_signal_connect(proxy, "beep", G_CALLBACK(beep_cb) , NULL);

    error= NULL;
    my_dbus_alarm_call_configure_alarm_sync(proxy , seconds , NULL , &error);
    if(error!=NULL)
    {
        printf("Error setting alarm!\n");
        return 0;
    }
    g_print("Alarm set for after %d seconds. Waiting for it to ring...\n",seconds);
    GMainLoop *loop = g_main_loop_new(NULL,FALSE);
    g_main_loop_run(loop);

    return 0;
}