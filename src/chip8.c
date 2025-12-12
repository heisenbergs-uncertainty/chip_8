#include "chip8.h"
#include <_stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static const uint8_t fontset[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

/**
 * @brief Internal state for the random number generator (Xorshift32).
 */
static uint32_t rng_state = 0x12345678;

/**
 * @brief Seed the random number generator.
 *
 * @param seed The seed value. If zero, it is replaced with 1.
 */
static inline void rng_seed(uint32_t seed) {
  if (seed == 0)
    seed = 1;
  rng_state = seed;
}

/**
 * @brief Generate a random 8-bit value using Xorshift32 algorithm.
 *
 * @return uint8_t Random byte.
 */
static inline uint8_t rng_byte(void) {
  // Xorshift32
  rng_state ^= rng_state << 13;
  rng_state ^= rng_state >> 17;
  rng_state ^= rng_state << 5;
  return (uint8_t)(rng_state);
}

void init(chip8_t *chip8) {
  // Initialize PC at 0x200
  chip8->pc = START_ADDRESS;
  // Load Font set into memory
  for (unsigned int i = 0; i < FONTSET_SIZE; ++i) {
    chip8->memory[FONTSET_START_ADDRESS + i] = fontset[i];
  }
  rng_seed((uint32_t)time(NULL));
}

int32_t load_rom(chip8_t *chip8, const char *filename) {
  FILE *fptr = fopen(filename, "rb");
  if (fptr == NULL) {
    return -1;
  }

  // Get file size
  fseek(fptr, 0, SEEK_END);
  long file_size = ftell(fptr);
  fseek(fptr, 0, SEEK_SET);

  // Check if ROM fits in memory (0x200 to 0xFFF = 3584 bytes max)
  if (file_size > 4096 - START_ADDRESS) {
    fclose(fptr);
    return -1; // ROM too large
  }

  char *buffer = (char *)malloc((size_t)file_size);
  if (buffer == NULL) {
    fclose(fptr);
    return -1; // Memory allocation failed
  }

  size_t bytes_read = fread(buffer, 1, (size_t)file_size, fptr);
  fclose(fptr);

  // Copy ROM data into Chip-8 memory starting at 0x200
  for (long i = 0; i < file_size; ++i) {
    chip8->memory[START_ADDRESS + i] = (unsigned char)buffer[i];
  }
  free(buffer);

  return 0;
}

void set_opcode(chip8_t *chip8) {
  uint8_t lhsByte = chip8->memory[chip8->pc];
  uint8_t rhsByte = chip8->memory[chip8->pc + 1];

  // Immediately increment pc since some instrucions will explicitly change our
  // pc location and need to reference the updated pc and not old
  chip8->pc += 2;

  // set opcode for instruction execution
  chip8->opcode = lhsByte << 8 | rhsByte;
}

void update_timers(chip8_t *chip8) {}

bool get_display_pixel(chip8_t *chip8, int x, int y) { return false; }

void set_display_pixel(chip8_t *chip8, int x, int y) {}

void op_0nnn(chip8_t *chip8) {}

// Set entire display buffer to 0
void op_00E0(chip8_t *chip8) {
  memset(chip8->display, 0, sizeof(chip8->display));
}

// RETURN from subroutine
void op_00EE(chip8_t *chip8) {
  --chip8->sp;
  chip8->pc = chip8->stack[chip8->sp];
}

void op_1nnn(chip8_t *chip8) {}

void op_2nnn(chip8_t *chip8) {}

void op3xkk(chip8_t *chip8) {}

void op_4xkk(chip8_t *chip8) {}

void op_5xy0(chip8_t *chip8) {}

void op_6xkk(chip8_t *chip8) {}

void op_7xkk(chip8_t *chip8) {}

void op_8xy0(chip8_t *chip8) {}

void op_8xy1(chip8_t *chip8) {}

void op_8xy2(chip8_t *chip8) {}

void op_8xy3(chip8_t *chip8) {}

void op_8xy4(chip8_t *chip8) {}

void op_8xy5(chip8_t *chip8) {}

void op_8xy6(chip8_t *chip8) {}

void op_8xy7(chip8_t *chip8) {}

void op_8xyE(chip8_t *chip8) {}

void op_9xy0(chip8_t *chip8) {}

void op_Annn(chip8_t *chip8) {}

void op_Bnnn(chip8_t *chip8) {}

void op_Cxkk(chip8_t *chip8) {}

void op_Dxyn(chip8_t *chip8) {}

void op_Ex9E(chip8_t *chip8) {}

void op_ExA1(chip8_t *chip8) {}

void op_Fx07(chip8_t *chip8) {}

void op_Fx0A(chip8_t *chip8) {}

void op_Fx15(chip8_t *chip8) {}

void op_Fx18(chip8_t *chip8) {}

void op_Fx1E(chip8_t *chip8) {}

void op_Fx29(chip8_t *chip8) {}

void op_Fx33(chip8_t *chip8) {}

void op_Fx55(chip8_t *chip8) {}

void op_Fx65(chip8_t *chip8) {}

void cycle(chip8_t *chip8) {}
