#include <stdio.h>
#include <string.h>

void handle_help() {
  printf("Usage: chip8_emulator [options]\n");
  printf("Options:\n");
  printf("  --help,      -h    Show this help message and exit\n");
  printf("  --version,   -v    Show version information and exit\n");
  printf("  --rom <file> -r    Specify the ROM file to load\n");
}

char *handle_params(int argc, char *argv[]) {
  char *filename = NULL;

  if (argc == 1) {
    handle_help();
    return filename;
  }

  for (int i = 1; i < argc; i++) {
    if ((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h") == 0)) {
      printf("Usage: chip8_emulator [options]\n");
      printf("Options:\n");
      printf("  --help, -h       Show this help message and exit\n");
      printf("  --version, -v    Show version information and exit\n");
      printf("  --rom <file> -r     Specify the ROM file to load\n");
    } else if ((strcmp(argv[i], "--version") == 0) ||
               (strcmp(argv[i], "-v") == 0)) {
      printf("chip8_emulator version 1.0.0\n");
    } else if ((strcmp(argv[i], "--rom") == 0) && (i + 1 < argc)) {
      filename = argv[++i];
      printf("ROM file specified: %s\n", filename);

    } else {
      printf("Unknown option: %s\n", argv[i]);
      printf("Use --help or -h for usage information.\n");
    }
  }

  return (char *)filename;
}

int main(int argc, char *argv[]) {
  char *filename = handle_params(argc, argv);
  if (filename != NULL) {
    // Load and run the ROM file
    printf("Loading ROM: %s\n", filename);
    // Add ROM loading and execution logic here
  }

  return 0;
}
