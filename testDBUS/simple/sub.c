#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "mydbus.h"

int main(int argc , char **argv)
{
    if(argc<3)
    {
        printf("Usage: ./sub num1 num2\n");
        return 0;
    }

    MyDBusCalculator *proxy;
    GError *error;
    gint retval;

    error = NULL;
    proxy = my_dbus_calculator_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION, 0, "com.Nilanjana" ,"/" , NULL , &error);

    gint a, b;
    a = atoi(argv[1]);
    b = atoi(argv[2]);

    error = NULL;
    my_dbus_calculator_call_sub_sync(proxy , a, b, &retval, NULL , &error );
    
    if(error==NULL)
        g_print("%d - %d = %d\n",a,b,retval);
    else
        printf("ERROR!\n");
        
    g_object_unref(proxy);

    return 0;
}