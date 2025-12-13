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

void op_0nnn(chip8_t *chip8) { chip8->pc = chip8->opcode & 0x0FFFu; }

// Set entire display buffer to 0
void op_00E0(chip8_t *chip8) {
  memset(chip8->display, 0, sizeof(chip8->display));
}

// RETURN from subroutine
void op_00EE(chip8_t *chip8) {
  --chip8->sp;
  chip8->pc = chip8->stack[chip8->sp];
}

// JP addr
void op_1nnn(chip8_t *chip8) {
  uint16_t nnn = chip8->opcode & 0x0FFFu;
  chip8->pc = nnn;
}

// Call subroutine at nnn
void op_2nnn(chip8_t *chip8) {
  uint16_t nnn = chip8->opcode & 0x0FFFu;
  chip8->stack[chip8->sp] = chip8->pc;
  ++chip8->sp;
  chip8->pc = nnn;
}

// SE Vx, byte
void op3xkk(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  // 3rd + 4th nibble - 0000 0000 1111 1111
  uint8_t kk = chip8->opcode & 0x00FFu;
  if (chip8->registers[Vx] == kk) {
    chip8->pc += 2;
  }
}

// SNE Vx, byte
void op_4xkk(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  // 3rd + 4th nibble - 0000 0000 1111 1111
  uint8_t kk = chip8->opcode & 0x00FFu;
  if (chip8->registers[Vx] != kk) {
    chip8->pc += 2;
  }
}

// SE Vx, Vy
void op_5xy0(chip8_t *chip8) {
  // 2nd nibble - 0000 1111 0000 0000
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  // 3rd nibble - 0000 0000 1111 0000
  uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;
  if (chip8->registers[Vx] == chip8->registers[Vy]) {
    chip8->pc += 2;
  }
}

// LD Vx, byte
void op_6xkk(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  // 3rd + 4th nibble - 0000 0000 1111 1111
  uint8_t kk = chip8->opcode & 0x00FFu;
  chip8->registers[Vx] = kk;
}

// ADD Vx, byte
void op_7xkk(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  uint8_t kk = chip8->opcode & 0x00FFu;

  chip8->registers[Vx] += kk;
}

// LD Vx, Vy
void op_8xy0(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;

  chip8->registers[Vx] = chip8->registers[Vy];
}

// OR Vx, Vy
void op_8xy1(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;
  chip8->registers[Vx] |= chip8->registers[Vy];
}

// AND Vx, Vy
void op_8xy2(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;
  chip8->registers[Vx] &= chip8->registers[Vy];
}

// XOR Vx, Vy
void op_8xy3(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;
  chip8->registers[Vx] ^= chip8->registers[Vy];
}

// ADD Vx, Vy
void op_8xy4(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;

  uint16_t sum = chip8->registers[Vx] + chip8->registers[Vy];

  // VF = OxF - special register for carry
  if (sum > 0xFFu) {
    chip8->registers[0xF] = 1; // Set carry flag
  } else {
    chip8->registers[0xF] = 0; // Clear carry flag
  }

  chip8->registers[Vx] = (uint8_t)(sum & 0xFFu);
}

// SUB Vx, Vy
void op_8xy5(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;

  uint16_t sum = chip8->registers[Vx] - chip8->registers[Vy];

  if (chip8->registers[Vx] > chip8->registers[Vy]) {
    chip8->registers[0xF] = 1; // Set borrow flag
  } else {
    chip8->registers[0xF] = 0; // Clear borrow flag
  }

  chip8->registers[Vx] = chip8->registers[Vx] - chip8->registers[Vy];
}

// SHR Vx {, Vy}
void op_8xy6(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  // 0x1u = 0001
  // VF = Vx & 0x1u
  chip8->registers[0xF] = chip8->registers[Vx] & 0x01u; // LSB to VF

  // Right shift Vx by 1 (div by 2)
  chip8->registers[Vx] >>= 1u;
}

// SUBN Vx, Vy
void op_8xy7(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;

  if (chip8->registers[Vy] > chip8->registers[Vx]) {
    chip8->registers[0xF] = 1; // Set borrow flag
  } else {
    chip8->registers[0xF] = 0; // Clear borrow flag
  }
  chip8->registers[Vx] = chip8->registers[Vy] - chip8->registers[Vx];
}

// SHL Vx {, Vy}
void op_8xyE(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

  chip8->registers[0xF] = (chip8->registers[Vx] & 0x80u) >> 7u; // MSB to VF

  chip8->registers[Vx] <<= 1u;
}

// SNE Vx, Vy
void op_9xy0(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  uint8_t Vy = (chip8->opcode & 0x00F0u) >> 4u;

  if (chip8->registers[Vx] != chip8->registers[Vy]) {
    chip8->pc += 2;
  }
}

// LD I, addr
void op_Annn(chip8_t *chip8) {
  uint16_t nnn = chip8->opcode & 0x0FFFu;
  chip8->index = nnn;
}

// JP V0, addr
void op_Bnnn(chip8_t *chip8) {
  uint16_t nnn = chip8->opcode & 0x0FFFu;
  chip8->pc = nnn + chip8->registers[0];
}

// RND Vx, byte
void op_Cxkk(chip8_t *chip8) {
  int8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  int8_t kk = chip8->opcode & 0x00FFu;

  chip8->registers[Vx] = rng_byte() & kk;
}

// DRW Vx, Vy, nibble
void op_Dxyn(chip8_t *chip8) {
  uint8_t Vx = chip8->opcode & 0x0F00u >> 8u;
  uint8_t Vy = chip8->opcode & 0x00F0u >> 4u;
  // n (last nibble)
  uint8_t height = chip8->opcode & 0x000Fu;

  uint8_t xPos = chip8->registers[Vx] % DISPLAY_WIDTH;
  uint8_t yPos = chip8->registers[Vy] % DISPLAY_HEIGHT;

  chip8->registers[0xF] = 0; // Reset collision flag

  for (unsigned int i = 0; i < height; ++i) {
    uint8_t spriteByte = chip8->memory[chip8->index + i];
    for (unsigned int j = 0; j < 8; ++j) {
      uint8_t spritePixel = spriteByte & (0x80 >> j);
      uint32_t *screenPixel =
          &chip8->display[(yPos + i) * DISPLAY_WIDTH + (xPos + j)];

      // Sprite Pixel is on
      if (spritePixel) {
        // Screen Pixel is on - collision
        if (*screenPixel == 0xFFFFFFFF) {
          chip8->registers[0xF] = 1; // Set collision flag
        }
        // XOR the pixel onto the display
        *screenPixel ^= 0xFFFFFFFF;
      }
    }
  }
}

// SKP Vx
void op_Ex9E(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  uint8_t instr = chip8->registers[Vx];
  if (chip8->keypad[instr]) {
    chip8->pc += 2;
  }
}

// SKNP Vx
void op_ExA1(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  uint8_t instr = chip8->registers[Vx];
  if (!chip8->keypad[instr]) {
    chip8->pc += 2;
  }
}

// LD Vx, DT
void op_Fx07(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  chip8->registers[Vx] = chip8->delay_timer;
}

// LD Vx, K
void op_Fx0A(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

  if (chip8->keypad[0]) {
    chip8->registers[Vx] = 0;
  } else if (chip8->keypad[1]) {
    chip8->registers[Vx] = 1;
  } else if (chip8->keypad[2]) {
    chip8->registers[Vx] = 2;
  } else if (chip8->keypad[3]) {
    chip8->registers[Vx] = 3;
  } else if (chip8->keypad[4]) {
    chip8->registers[Vx] = 4;
  } else if (chip8->keypad[5]) {
    chip8->registers[Vx] = 5;
  } else if (chip8->keypad[6]) {
    chip8->registers[Vx] = 6;
  } else if (chip8->keypad[7]) {
    chip8->registers[Vx] = 7;
  } else if (chip8->keypad[8]) {
    chip8->registers[Vx] = 8;
  } else if (chip8->keypad[9]) {
    chip8->registers[Vx] = 9;
  } else if (chip8->keypad[10]) {
    chip8->registers[Vx] = 10;
  } else if (chip8->keypad[11]) {
    chip8->registers[Vx] = 11;
  } else if (chip8->keypad[12]) {
    chip8->registers[Vx] = 12;
  } else if (chip8->keypad[13]) {
    chip8->registers[Vx] = 13;
  } else if (chip8->keypad[14]) {
    chip8->registers[Vx] = 14;
  } else if (chip8->keypad[15]) {
    chip8->registers[Vx] = 15;
  } else {
    // No key pressed, repeat this instruction
    chip8->pc -= 2;
  }
}

// LD DT, Vx
void op_Fx15(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

  chip8->delay_timer = chip8->registers[Vx];
}

// LD ST, Vx
void op_Fx18(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

  chip8->sound_timer = chip8->registers[Vx];
}

// ADD I, Vx
void op_Fx1E(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

  chip8->index += chip8->registers[Vx];
}

// LD F, Vx
void op_Fx29(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

  uint8_t digit = chip8->registers[Vx];
  chip8->index = FONTSET_START_ADDRESS + (digit * 5);
}

// LD B, Vx
void op_Fx33(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;
  uint8_t digit = chip8->registers[Vx];

  chip8->memory[chip8->index] = digit % 10;
  digit /= 10;

  chip8->memory[chip8->index + 1] = digit % 10;
  digit /= 10;

  chip8->memory[chip8->index + 2] = digit % 10;
}

// LD [I], Vx
void op_Fx55(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

  for (int i = 0; i < Vx; ++i) {
    chip8->memory[chip8->index + i] = chip8->registers[i];
  }
}

// LD  Vx, [I]
void op_Fx65(chip8_t *chip8) {
  uint8_t Vx = (chip8->opcode & 0x0F00u) >> 8u;

  for (int i = 0; i < Vx; ++i) {
    chip8->registers[i] = chip8->memory[chip8->index + i];
  }
}

void cycle(chip8_t *chip8) {}
