static void say_win_set_pixel_format(HDC dc) {
  PIXELFORMATDESCRIPTOR pfd = { 
    sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd  
    1,                    // version number  
    PFD_DRAW_TO_WINDOW |   // support window  
    PFD_SUPPORT_OPENGL |   // support OpenGL  
    PFD_DOUBLEBUFFER,      // double buffered  
    PFD_TYPE_RGBA,         // RGBA type  
    24,                    // 24-bit color depth  
    0, 0, 0, 0, 0, 0,      // color bits ignored  
    8,                     // alpha buffer  
    0,                     // shift bit ignored  
    0,                     // no accumulation buffer  
    0, 0, 0, 0,            // accum bits ignored  
    24,                    // 24-bit z-buffer  
    0,                     // no stencil buffer  
    0,                     // no auxiliary buffer  
    PFD_MAIN_PLANE,        // main layer  
    0,                     // reserved  
    0, 0, 0                // layer masks ignored  
  };

  int format = ChoosePixelFormat(dc, &pfd);
  SetPixelFormat(dc, format, &pfd);
}

void say_imp_context_free(say_imp_context ctxt) {
  if (ctxt->context) {
    if (wglGetCurrentContext() == ctxt->context) {
      wglMakeCurrent(NULL, NULL);
    }

    wglDeleteContext(ctxt->context);
  }

  if (ctxt->device)
    ReleaseDC(ctxt->win, ctxt->device);

  if (ctxt->owns_window && ctxt->win)
    DestroyWindow(ctxt->win);

  free(ctxt);
}

say_imp_context say_imp_context_create() {
  return say_imp_context_create_shared(NULL);
}

say_imp_context say_imp_context_create_shared(say_imp_context shared) {
  say_win_context *ctxt = malloc(sizeof(say_win_context));

  ctxt->owns_window = true;
  ctxt->win = CreateWindowA("STATIC", "", WS_POPUP | WS_DISABLED, 0, 0, 1, 1,
                            NULL, NULL, GetModuleHandle(NULL), NULL);
  ShowWindow(ctxt->win, SW_HIDE);

  ctxt->device = GetDC(ctxt->win);
  say_win_set_pixel_format(ctxt->device);
  
  ctxt->context = wglCreateContext(ctxt->device);
  if (shared)
    wglShareLists(shared->context, ctxt->context);

  return ctxt;
}

say_imp_context  say_imp_context_create_for_window(say_imp_context shared,
                                                   say_imp_window  win) {
  say_win_context *ctxt = malloc(sizeof(say_win_context));

  ctxt->owns_window = false;
  
  ctxt->win    = win->win;
  ctxt->device = GetDC(ctxt->win);
  
  say_win_set_pixel_format(ctxt->device);
  
  ctxt->context = wglCreateContext(ctxt->device);
  if (shared)
    wglShareLists(shared->context, ctxt->context);

  return ctxt;  
}

void say_imp_context_make_current(say_imp_context ctxt) {
  wglMakeCurrent(ctxt->device, ctxt->context);
}

void say_imp_context_update(say_imp_context ctxt) {
  SwapBuffers(ctxt->device);
}