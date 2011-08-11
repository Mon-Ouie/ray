#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092

#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB         1
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 2

typedef HGLRC (*say_gl_create_context)(HDC dc, HGLRC share, const GLint *attr);

static void say_win_set_pixel_format(HDC dc) {
  say_context_config *conf = say_context_get_config();

  PIXELFORMATDESCRIPTOR pfd = { 
    sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd  
    1,                     // version number  
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
    conf->depth_size,      // z-buffer  
    conf->stencil_size,    // stencil buffer  
    0,                     // no auxiliary buffer  
    PFD_MAIN_PLANE,        // main layer  
    0,                     // reserved  
    0, 0, 0                // layer masks ignored  
  };
  int format = ChoosePixelFormat(dc, &pfd);
  SetPixelFormat(dc, format, &pfd);
}

static HGLRC say_win_context_build(HDC device, HGLRC share) {
  say_gl_create_context create = (say_gl_create_context)
    say_get_proc("wglCreateContextAttribsARB");

  say_context_config *conf = say_context_get_config();

  if (create && conf->major_version > 3) {
    GLint attribs[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB, conf->major_version,
      WGL_CONTEXT_MINOR_VERSION_ARB, conf->minor_version,
      WGL_CONTEXT_PROFILE_MASK_ARB, conf->core_profile ?
      WGL_CONTEXT_CORE_PROFILE_BIT_ARB :
      WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
    };
    
    HGLRC ctxt = create(device, share, attribs);
    if (ctxt)
      return ctxt;
  }

  HGLRC ctxt = wglCreateContext(device);
  if (share) wglShareLists(share, ctxt);

  return ctxt;
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
  
  ctxt->context = say_win_context_build(ctxt->device,
					shared ? shared->context : NULL);

  return ctxt;
}

say_imp_context  say_imp_context_create_for_window(say_imp_context shared,
                                                   say_imp_window  win) {
  say_win_context *ctxt = malloc(sizeof(say_win_context));

  ctxt->owns_window = false;
  
  ctxt->win    = win->win;
  ctxt->device = GetDC(ctxt->win);
  
  say_win_set_pixel_format(ctxt->device);
  
  ctxt->context = say_win_context_build(ctxt->device,
					shared ? shared->context : NULL);

  return ctxt;  
}

void say_imp_context_make_current(say_imp_context ctxt) {
  wglMakeCurrent(ctxt->device, ctxt->context);
}

void say_imp_context_update(say_imp_context ctxt) {
  SwapBuffers(ctxt->device);
}
