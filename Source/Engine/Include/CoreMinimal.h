#pragma once
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NO_EXPAND(a)                   #a
#define STR(a)                         NO_EXPAND(a)
#define FIND_ASSET(AssetPath)          STR(CONTENT_PATH) AssetPath
#define LOG_CONTEXT                    " File:" STR(__FILE__) "Line:" STR(__LINE__) " => "
#define LOG_BUFFER_SIZE                1024
#define GT_LOG(ELogLevel, Format, ...) LogCreate(ELogLevel, LOG_CONTEXT, Format, ##__VA_ARGS__)

#ifndef __cplusplus
typedef unsigned char bool;
#define false 0x00
#define true 0x01
#endif // __cplusplus

typedef void Void;
typedef char Char;
typedef short Int16;
typedef int Int32;
typedef long long Int64;
typedef unsigned char UChar;
typedef unsigned short UInt16;
typedef unsigned int UInt32;
typedef unsigned long long UInt64;

typedef enum {
  LOG_INFO,
  LOG_SUCCESS,
  LOG_WARNING,
  LOG_ERROR,
  LOG_FATAL
} ELogLevel;

// Platform Types
typedef struct PWindow PWindow;
typedef struct PAudioBuffer PAudioBuffer;
typedef struct PAudioProcessor PAudioProcessor;

// System
Void LogCreate(const ELogLevel level, const Char* Context, const Char* Format, ...);
Void InitPlatform();

// Window
PWindow* WindowCreate(Int32 Width, Int32 Height, const Char* Title);
Void WindowUpdate(PWindow* Window);
Void WindowClose(PWindow* Window);
Void WindowDestroy(PWindow* Window);
bool WindowShouldClose(PWindow* Window);

// Memory
Void* MemoryAlloc(const UInt64 Size);
void MemoryFree(Void* Memory);
Void* MemoryRealoc(Void* Memory, const UInt64 Size);
Void* MemoryCopy(Void* Destiny, Void* Source, const UInt64 Size);

// Module
Void* ModuleLoad(const Char* Name);
Void ModuleFree(Void* Module);
Void* ModuleGetFunc(Void* Module, const Char* Name);
Void ModuleGetAllFuncs(Void* Module, Void* Api, const Char** Names);
