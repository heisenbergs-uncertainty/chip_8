#include "chip8.h"
#include "platform.h"
#include <stdio.h>
#include <string.h>
#include <wchar.h>

void handle_help()
{
  printf("Usage: chip8_emulator [options]\n");
  printf("Options:\n");
  printf("  --help,      -h    Show this help message and exit\n");
  printf("  --version,   -v    Show version information and exit\n");
  printf("  --rom <file> -r    Specify the ROM file to load\n");
}

char *handle_params(int argc, char *argv[])
{
  char *filename = NULL;

  if (argc == 1)
  {
    handle_help();
    return filename;
  }

  for (int i = 1; i < argc; i++)
  {
    if ((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h") == 0))
    {
      printf("Usage: chip8_emulator [options]\n");
      printf("Options:\n");
      printf("  --help, -h       Show this help message and exit\n");
      printf("  --version, -v    Show version information and exit\n");
      printf("  --rom <file> -r     Specify the ROM file to load\n");
      printf("  --scale <num> -s   Set the window scale (default is 10)\n");
      printf("  --delay <num> -d   Set the instruction delay in milliseconds (default is 0)\n");
    }

    else if ((strcmp(argv[i], "--version") == 0) ||
             (strcmp(argv[i], "-v") == 0))
    {
      printf("chip8_emulator version 1.0.0\n");
    }
    else if ((strcmp(argv[i], "--rom") == 0) && (i + 1 < argc))
    {
      filename = argv[++i];
      printf("ROM file specified: %s\n", filename);
    }
    else if ((strcmp(argv[i], "--scale") == 0) && (i + 1 < argc))
    {
      int scale = atoi(argv[++i]);
      printf("Window scale set to: %d\n", scale);
    }
    else if ((strcmp(argv[i], "--delay") == 0) && (i + 1 < argc))
    {
      int delay = atoi(argv[++i]);
      printf("Instruction delay set to: %d ms\n", delay);
    }
    else
    {
      printf("Unknown option: %s\n", argv[i]);
      printf("Use --help or -h for usage information.\n");
    }
  }

  return (char *)filename;
}

int main(int argc, char *argv[])
{
  char *filename = handle_params(argc, argv);
  if (filename == NULL)
  {
    return 0;
  }

  int videoScale = 10;
  int instructionDelay = 0;

  platform_t platform;
  init_platform(&platform, "Chip-8 Emulator", DISPLAY_WIDTH * videoScale,
                DISPLAY_HEIGHT * videoScale, DISPLAY_WIDTH, DISPLAY_HEIGHT);

  chip8_t chip8;
  init_chip8(&chip8);

  if (load_rom(&chip8, filename) != 0)
  {
    fprintf(stderr, "Failed to load ROM: %s\n", filename);
    destroy_platform(&platform);
    return 1;
  }

  int videoPitch = sizeof(chip8.display[0]) * DISPLAY_WIDTH;

  const int FPS = 60;
  const int frameDelay = 1000 / FPS;

  uint32_t frameStart;
  int frameTime;

  // Instructions per frame
  const int instructionsPerFrame = 10;

  bool quit = false;
  while (!quit)
  {
    frameStart = SDL_GetTicks();

    quit = process_input(chip8.keypad);

    for (int i = 0; i < instructionsPerFrame; ++i)
    {
      cycle(&chip8);
    }

    update_timers(&chip8);
    update_platform(&platform, chip8.display, videoPitch);

    frameTime = SDL_GetTicks() - frameStart;
    if (frameDelay > frameTime)
    {
      SDL_Delay(frameDelay - frameTime);
    }
  }

  destroy_platform(&platform);

  return 0;
}
