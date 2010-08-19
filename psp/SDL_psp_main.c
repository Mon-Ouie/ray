/*
  SDL - Simple DirectMedia Layer
  Copyright (C) 1997-2004 Sam Lantinga

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Sam Lantinga
  slouken@libsdl.org
*/

/*
  PSP port contributed by:
  Marcus R. Brown <mrbrown@ocgnet.org>
  Jim Paris <jim@jtan.com>
  Matthew H <matthewh@webone.com.au>
*/

#include <SDL/SDL_main.h>

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspsdk.h>
#include <pspthreadman.h>
#include <stdlib.h>
#include <stdio.h>

/* If application's main() is redefined as SDL_main, and libSDLmain is
   linked, then this file will create the standard exit callback,
   define the PSP_MODULE_INFO macro, and exit back to the browser when
   the program is finished.

   You can still override other parameters in your own code if you
   desire, such as PSP_HEAP_SIZE_KB, PSP_MAIN_THREAD_ATTR,
   PSP_MAIN_THREAD_STACK_SIZE, etc.
*/

extern int SDL_main(int argc, char *argv[]);

PSP_MODULE_INFO("ray", 0, 1, 1);

int sdl_psp_exit_callback(int arg1, int arg2, void *common) {
   exit(0);
   return 0;
}

int sdl_psp_callback_thread(SceSize args, void *argp) {
   int cbid;
   cbid = sceKernelCreateCallback("Exit Callback",
                                  sdl_psp_exit_callback, NULL);
   sceKernelRegisterExitCallback(cbid);
   sceKernelSleepThreadCB();
   return 0;
}

int sdl_psp_setup_callbacks(void) {
   int thid = 0;
   thid = sceKernelCreateThread("update_thread",
                                sdl_psp_callback_thread, 0x11, 0xFA0, 0, 0);
   if(thid >= 0)
      sceKernelStartThread(thid, 0, 0);
   return thid;
}

int main(int argc, char *argv[]) {
   pspDebugScreenInit();
   sdl_psp_setup_callbacks();

   /* Register sceKernelExitGame() to be called when we exit */
   atexit(sceKernelExitGame);

   return SDL_main(argc, argv);
}
