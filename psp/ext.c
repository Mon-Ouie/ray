/*Copyright (C) 2009-2010 Verhetsel Kilian

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*/

#include "ray.h"

#include <pspwlan.h>

#include <pspnet.h>
#include <pspnet_inet.h>
#include <pspnet_resolver.h>
#include <pspnet_apctl.h>

#include <psputility.h>

#include <time.h>

VALUE ray_mWlan = Qnil;
VALUE ray_eTimeoutError = Qnil;

void Init_socket();
void Init_bigdecimal();
void Init_digest();
void Init_bubblebabble();
void Init_md5();
void Init_sha1();
void Init_sha2();
void Init_enumerator();
void Init_fcntl();
void Init_stringio();
void Init_strscan();
void Init_thread();
void Init_zlib();
void Init_nkf();
void Init_etc();
void Init_syck();

/*
  Inits the wlan connection. You have to call this before calling the socket
  function.
*/
VALUE Wlan_init(VALUE self) {
    int err;
    err = sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
    if (err < 0)
       rb_raise(rb_eRuntimeError, "Failled to load net module");

    err = sceUtilityLoadNetModule(PSP_NET_MODULE_INET);
    if (err < 0)
       rb_raise(rb_eRuntimeError, "Failled to load inet module");

    err = sceNetInit(0x20000, 0x20, 0x1000, 0x20, 0x1000);
    if (err < 0)
        rb_raise(rb_eRuntimeError, "Failled to init net module");

    err = sceNetInetInit();
    if (err < 0)
       rb_raise(rb_eRuntimeError, "Failled to init inet module");

    err = sceNetResolverInit();
    if (err < 0)
        rb_raise(rb_eRuntimeError, "Failled to init resolver");

    err = sceNetApctlInit(0x1400, 0x42);
    if (err < 0)
        rb_raise(rb_eRuntimeError, "Failled to init apctl");

    return Qnil;
}

/*
  Connects to a given acces point. You can specify a timeout in seconds.
  If the connection isn't established after the specified timeout,
  a TimeoutError is raised.
*/
VALUE Wlan_connect(VALUE self, VALUE config, VALUE timeout) {
   if (sceNetApctlConnect(FIX2INT(config)))
      rb_raise(rb_eRuntimeError, "Failled to connect to acces point %d",
               FIX2INT(config));

   time_t startTime, currTime;
   time(&startTime);

   int last_state = -1;

   while (1) {
      time(&currTime);
      if (currTime - startTime >= FIX2INT(timeout)) {
         sceNetApctlDisconnect();
         rb_raise(ray_eTimeoutError, "Connection timeouted after %d seconds",
                  FIX2INT(timeout));
         break;
      }

      int state;
      if (sceNetApctlGetState(&state)) {
         rb_raise(rb_eRuntimeError,
                  "Error occured while getting connection state");
         break;
      }

      if (state > last_state) {
         last_state = state;
      }

      if (state == PSP_NET_APCTL_STATE_GOT_IP)
         break;

      sceKernelDelayThread(50 * 1000);
   }

   return Qnil;
}

/*
  Ends a Wlan connection.
*/
VALUE Wlan_disconnect(VALUE self) {
   sceNetApctlDisconnect();
   return Qnil;
}

/*
  Unloads net modules.
*/
VALUE Wlan_stop(VALUE self) {
   sceNetApctlTerm();

   sceUtilityUnloadNetModule(PSP_NET_MODULE_INET);
   sceUtilityUnloadNetModule(PSP_NET_MODULE_COMMON);

   return Qnil;
}

/*
  Returns whether the wlan button is enabled.
*/
VALUE Wlan_button_enabled(VALUE self) {
   return sceWlanDevIsPowerOn() ? Qtrue : Qfalse;
}

/*
  Returns whether we're connected to an acces point.
*/
VALUE Wlan_is_connected(VALUE self) {
   union SceNetApctlInfo apctlInfo;
   return sceNetApctlGetInfo(PSP_NET_APCTL_INFO_IP, &apctlInfo) == 0 ?
      Qtrue :
      Qfalse;
}

/*
  Returns the wlan access points in an array of arrays.
  It returns something in the following form :
    [
      ["ip", "name"], # Access point at 1
      ["ip", "name"], # Access point at 2
      ["ip", "name"], # Access point at 3
      ...
    ]
*/
VALUE Wlan_configs(VALUE self) {
   VALUE ret = rb_ary_new();
   
   int i = 0;
   for (i = 1; i < 20; ++i) {
      if (sceUtilityCheckNetParam(i))
          break;

      VALUE entry = rb_ary_new();

      netData data;
      sceUtilityGetNetParam(i, PSP_NETPARAM_IP, &data);
      rb_ary_push(entry, rb_str_new2(data.asString));

      sceUtilityGetNetParam(i, PSP_NETPARAM_NAME, &data);
      rb_ary_push(entry, rb_str_new2(data.asString));

      rb_ary_push(ret, entry);
   }

   return ret;
}

/*
  Returns our IP.
*/
VALUE Wlan_ip(VALUE self) {
   union SceNetApctlInfo apctlInfo;
   sceNetApctlGetInfo(PSP_NET_APCTL_INFO_IP, &apctlInfo);

   return rb_str_new2(apctlInfo.ip);
}

/*
  Inits a builtin module. You should never call it by yourself.
  This avoids to load unneeded module from the stdlib (Ray's are all loaded
  anyway). For instance, the zlib library needs to be enabled by this function.
*/
VALUE ray_init_internal(VALUE self, VALUE module_name) {
   VALUE rb_str = rb_obj_as_string(module_name);
   char *str = StringValuePtr(rb_str);

   if (strcmp(str, "socket") == 0)
      Init_socket();
   else if (strcmp(str, "bigdecimal") == 0)
      Init_bigdecimal();
   else if (strcmp(str, "digest") == 0)
      Init_digest();
   else if (strcmp(str, "bubblebabble") == 0)
      Init_bubblebabble();
   else if (strcmp(str, "md5") == 0)
      Init_md5();
   else if (strcmp(str, "sha1") == 0)
      Init_sha1();
   else if (strcmp(str, "sha2") == 0)
      Init_sha2();
   else if (strcmp(str, "enumerator") == 0)
      Init_enumerator();
   else if (strcmp(str, "fcntl") == 0)
      Init_fcntl();
   else if (strcmp(str, "stringio") == 0)
      Init_stringio();
   else if (strcmp(str, "strscan") == 0)
      Init_strscan();
   else if (strcmp(str, "thread") == 0)
      Init_thread();
   else if (strcmp(str, "zlib") == 0)
      Init_zlib();
   else if (strcmp(str, "nkf") == 0)
      Init_nkf();
   else if (strcmp(str, "etc") == 0)
      Init_etc();
   else if (strcmp(str, "syck") == 0)
      Init_syck();

   return Qnil;
}

/*
  Document-class: Ray::Wlan

  Ruby's stdlib includes a Socket class, as well as classes easying
  the use of HTTP and FTP protocols. These classes can be used,
  but the PSP requires some initialisation before being able to use them.

  This module contains the basic methods which can allow you to use
  theme. This means :

  * Loading net modules
  * Connecting to one of the access points
  * Checking the access points

  Once you're connected, everything is transparent: you can use sockets as
  usual.
*/
void Init_ray_psp() {
   rb_define_module_function(ray_mRay, "init_internal", ray_init_internal, 1);

   ray_eTimeoutError = rb_define_class_under(ray_mRay, "TimeoutError",
                                            rb_eStandardError);

   VALUE ray_mWlan = rb_define_module_under(ray_mRay, "Wlan"); 
   rb_define_module_function(ray_mWlan, "init", Wlan_init, 0);
   rb_define_module_function(ray_mWlan, "stop", Wlan_stop, 0);
   rb_define_module_function(ray_mWlan, "connect", Wlan_connect, 2);
   rb_define_module_function(ray_mWlan, "disconnect", Wlan_disconnect, 0);
   rb_define_module_function(ray_mWlan, "button_enabled?", Wlan_button_enabled,
                             0);
   rb_define_module_function(ray_mWlan, "configs", Wlan_configs, 0);
   rb_define_module_function(ray_mWlan, "connected?", Wlan_is_connected, 0);
   rb_define_module_function(ray_mWlan, "ip", Wlan_ip, 0);
}
