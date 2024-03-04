#include "CoreMinimal.h"

Int32 main(Int32 argc, Char** argv) {
  GT_LOG(LOG_INFO, "Project Name:%s", STR(GAME_NAME));
  InitPlatform();

  PWindow* mainWindow = WindowCreate(800, 600, STR(GAME_NAME));

  while (!WindowShouldClose(mainWindow)) {
    WindowUpdate(mainWindow);
  }

  WindowDestroy(mainWindow);
  
  return 0;
}
