#include <X11/X.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef PLATFORM_LINUX
#include "CoreMinimal.h"

#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <dlfcn.h>

static const Char* apiX11Names[] = {
    "XCreateBitmapFromData",      //
    "XCreatePixmapCursor",        //
    "XFreePixmap",                //
    "XCreateFontCursor",          //
    "XDefineCursor",              //
    "XFreeCursor",                //
    "XkbSetDetectableAutoRepeat", //
    "XInternAtom",                //
    "XSetWMProtocols",            //
    "XStoreName",                 //
    "XOpenDisplay",               //
    "XCloseDisplay",              //
    "XMapWindow",                 //
    "XUnmapWindow",               //
    "XSelectInput",               //
    "XNextEvent",                 //
    "XSendEvent",                 //
    "XPending",                   //
    "XSetWMNormalHints",          //
    "XAllocSizeHints",            //
    "XLookupKeysym",              //
    "XDestroyWindow",             //
    "XCreateSimpleWindow",        //
    "\0"};

static struct {
  Pixmap (*XCreateBitmapFromData)(Display*, Drawable, const char*, unsigned int, unsigned int);
  Cursor (*XCreatePixmapCursor)(Display*, Pixmap, Pixmap, XColor*, XColor*, unsigned int, unsigned int);
  int (*XFreePixmap)(Display*, Pixmap);
  Cursor (*XCreateFontCursor)(Display*, unsigned int);
  int (*XDefineCursor)(Display*, Window, Cursor);
  int (*XFreeCursor)(Display*, Cursor);
  int (*XkbSetDetectableAutoRepeat)(Display*, int, int*);
  Atom (*XInternAtom)(Display*, const char*, int);
  Atom (*XSetWMProtocols)(Display*, Window, Atom*, int);
  int (*XStoreName)(Display*, Window, const char*);
  Display* (*XOpenDisplay)(const char*);
  int (*XCloseDisplay)(Display*);
  int (*XMapWindow)(Display*, Window);
  int (*XUnmapWindow)(Display*, Window);
  int (*XSelectInput)(Display*, Window, long);
  int (*XNextEvent)(Display*, XEvent*);
  int (*XSendEvent)(Display*, Window, int, long, XEvent*);
  int (*XPending)(Display*);
  void (*XSetWMNormalHints)(Display*, Window, XSizeHints*);
  XSizeHints* (*XAllocSizeHints)();
  KeySym (*XLookupKeysym)(XKeyEvent*, int);
  int (*XDestroyWindow)(Display*, Window);
  Window (*XCreateSimpleWindow)(Display*, Window, int, int, unsigned int, unsigned int, unsigned int, unsigned long, unsigned long);
} apiX11;

struct PWindow {
  Int32 width;
  Int32 height;
  Char* title;
  bool shouldClose;
  bool fullscreen;
  Window window;    // X11 window
  Display* display; // X11 Display
  Int32 screen;     // X11 Screen
  XSizeHints* sizeHints;
  Atom wmProtocols;
  Atom wmDelete;
};

Void LogCreate(const ELogLevel level, const Char* Context, const Char* Format, ...) {
  const Char* logTag = NULL;
  char logBuffer[LOG_BUFFER_SIZE] = "";

  switch (level) {
  case LOG_INFO: {
    logTag = (const Char*)"\033[0;97m[LOG INFO]";
  } break;
  case LOG_SUCCESS: {
    logTag = (const Char*)"\033[0;92m[LOG SUCCESS]";
  } break;
  case LOG_WARNING: {
    logTag = (const Char*)"\033[0;93m[LOG WARNING]";
  } break;
  case LOG_ERROR: {
    logTag = (const Char*)"\033[0;91m[LOG ERROR]";
  } break;
  case LOG_FATAL: {
    logTag = (const Char*)"\033[0;31m[LOG FATAL]";
  } break;
  }

  strcpy(logBuffer, logTag);
  if (level != LOG_INFO) {
    strcat(logBuffer, Context);
  }
  strcat(logBuffer, Format);
  strcat(logBuffer, "\033[0m\n");

  va_list args;
  va_start(args, Format);
  vprintf(logBuffer, args);
  va_end(args);
}

Void InitPlatform() {
  Void* libX11 = ModuleLoad("libX11.so");
  ModuleGetAllFuncs(libX11, &apiX11, apiX11Names);
  GT_LOG(LOG_INFO, "API X11 Initialized");
}

PWindow* WindowCreate(Int32 Width, Int32 Height, const Char* Title) {
  PWindow win = {0};
  Int32 mask = ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask | PointerMotionMask;

  win.display = apiX11.XOpenDisplay(NULL);
  GT_LOG(LOG_WARNING, "Temp Test");
  win.screen = DefaultScreen(win.display);
  win.window = apiX11.XCreateSimpleWindow(win.display, DefaultRootWindow(win.display), 0, 0, Width, Height, 1, BlackPixel(win.display, win.screen), WhitePixel(win.display, win.screen));
  apiX11.XMapWindow(win.display, win.window);
  apiX11.XSelectInput(win.display, win.window, mask);
  apiX11.XStoreName(win.display, win.window, Title);
  apiX11.XkbSetDetectableAutoRepeat(win.display, true, NULL);
  win.wmProtocols = apiX11.XInternAtom(win.display, "WM_PROTOCOLS", false);
  win.wmDelete = apiX11.XInternAtom(win.display, "WM_DELETE_WINDOW", false);
  apiX11.XSetWMProtocols(win.display, win.window, &win.wmDelete, true);

  // Set mim window Size
  win.sizeHints = apiX11.XAllocSizeHints();
  win.sizeHints->flags = PMinSize | PMaxSize;
  win.sizeHints->min_width = Width;
  win.sizeHints->min_height = Height;
  win.sizeHints->max_width = Width;
  win.sizeHints->max_height = Height;
  apiX11.XSetWMNormalHints(win.display, win.window, win.sizeHints);

  win.width = Width;
  win.height = Height;
  win.title = (Char*)Title;
  win.fullscreen = false;
  win.shouldClose = false;

  GT_LOG(LOG_INFO, "Create Window => Width:%d, Height:%d, Title:%s", Width, Height, Title);
  PWindow* pWin = MemoryAlloc(sizeof(PWindow));
  MemoryCopy(pWin, &win, sizeof(PWindow));
  return pWin;
}

Void WindowUpdate(PWindow* Window) {
  XEvent event;
  while (apiX11.XPending(Window->display)) {
    apiX11.XNextEvent(Window->display, &event);
    switch (event.type) {
    case KeyPress: {
      WindowClose(Window);
    } break;
    case KeyRelease: {
    } break;
    case ClientMessage: {
      if (event.xclient.message_type == Window->wmProtocols) {
        if (event.xclient.data.l[0] == (long)Window->wmDelete) {
          WindowClose(Window);
        }
      }
    } break;
    }
  }
}

Void WindowClose(PWindow* Window) {
  Window->shouldClose = true;
  GT_LOG(LOG_INFO, "CloseWindow");
}

Void WindowDestroy(PWindow* Window) {
  apiX11.XUnmapWindow(Window->display, Window->window);
  apiX11.XDestroyWindow(Window->display, Window->window);
  apiX11.XCloseDisplay(Window->display);
  MemoryFree(Window);
  Window = NULL;
}

bool WindowShouldClose(PWindow* Window) {
  return Window->shouldClose;
}

Void* MemoryAlloc(const UInt64 Size) {
  return calloc(1, Size);
}

void MemoryFree(Void* Memory) {
  if (!Memory) {
    free(Memory);
  }
}

Void* MemoryRealoc(Void* Memory, const UInt64 Size) {
  return realloc(Memory, Size);
}

Void* MemoryCopy(Void* Destiny, Void* Source, const UInt64 Size) {
  return memcpy(Destiny, Source, Size);
}

Void* ModuleLoad(const Char* Name) {
  Void* module = dlopen(Name, RTLD_LAZY);
  return module;
}

Void ModuleFree(Void* Module) {
  if (Module != NULL) {
    dlclose(Module);
  }
}

Void* ModuleGetFunc(Void* Module, const Char* Name) {
  Void* function = dlsym(Module, Name);
  return function;
}

Void ModuleGetAllFuncs(Void* Module, Void* Api, const Char** Names) {
  const Char** names = Names;
  Void* func = NULL;
  Void* api = Api;
  Int32 index = 0;

  while (**names) {
    func = ModuleGetFunc(Module, *names);
    if (func) {
      Void* addr = api + index * sizeof(Void*);
      MemoryCopy(addr, &func, sizeof(Void*));
      // GT_LOG(LOG_SUCCESS, "Function Loaded:%s", *Names);
    }
    ++names;
    ++index;
  }
}

#endif // PLATFORM_LINUX
