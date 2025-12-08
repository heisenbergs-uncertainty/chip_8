
#ifndef CHIP8_H
#define CHIP8_H

#include <stdbool.h>
#include <stdint.h>

#define MEMORY_SIZE 4096
#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define V_REG_COUNT 16
#define STACK_SIZE 16
#define KEYS_COUNT 16
#define START_ADDRESS 0x200
#define FONTSET_START_ADDRESS 0x50
#define FONTSET_SIZE 80

// --- MEMORY MAP ---
// +---------------+= 0xFFF (4095) End of Chip-8 RAM
// |               |
// |               |
// |               |
// |               |
// |               |
// | 0x200 to 0xFFF|
// |     Chip-8    |
// | Program / Data|
// |     Space     |
// |               |
// |               |
// |               |
// +- - - - - - - -+= 0x600 (1536) Start of ETI 660 Chip-8 programs
// |               |
// |               |
// |               |
// +---------------+= 0x200 (512) Start of most Chip-8 programs
// | 0x000 to 0x1FF|
// | Reserved for  |
// |  interpreter  |
// +---------------+= 0x000 (0) Start of Chip-8 RAM

typedef struct {
  // General Purpose 8bit registers
  // Referred to as Vx where x is a hexadecimal digit (0-F)
  uint8_t registers[V_REG_COUNT]; // 16 General Purpose 8-bit registers

  // Memory - 4KB
  uint8_t memory[MEMORY_SIZE]; // Chip-8 had access to 4KB memory
  uint16_t index; // (I) Memory addresses - only lowest (rightmost) 12 used

  // Special Purpose 8-bit registers
  // When either is non-zero they automaticallly decrement at a rate of 60Hz.
  uint8_t sound_timer; // Special 8-bit register for sounds - when
  uint8_t delay_timer; // Special 8-bit regist for delays

  // Pseudo-Registers
  // Inaccessible to chip8 programs
  uint16_t pc; // Program counter - stores currently executing address
  uint8_t sp;  // Stack Pointer - Used to point to topmost level of the stack
  uint16_t opcode;

  // Stack
  // Array of 16 16-bit values - store address that interpreper should return to
  // when finished with subroutine Chip-8 allows for up to 16 levels of nested
  // subroutines
  uint16_t stack[STACK_SIZE];

  // Display and Input
  bool diplay[DISPLAY_WIDTH]
             [DISPLAY_HEIGHT]; // True or false to display each pixel
  uint8_t keypad[KEYS_COUNT];  // 16 keys - utilize user input from keyboard
} chip8_t;

typedef void (*opcodehandler_t)(chip8_t *chip8);
void init(chip8_t *chip8);
void set_opcode(chip8_t *chip8);
void process_instruction(chip8_t *chip8);
void cycle(chip8_t *chip8);
void update_timers(chip8_t *chip8);
bool get_display_pixel(chip8_t *chip8, int x, int y);
void set_display_pixel(chip8_t *chip8, int x, int y);

// Op code handling methods

// SYS addr - jump to a machine code routine at nnn
void op_0nnn(chip8_t *chip8);
// CLS - Clear display
void op_00E0(chip8_t *chip8);
// RET - Returnf rom subroutine
void op_00E0(chip8_t *chip8);
void op_1nnn(chip8_t *chip8);
void op_2nnn(chip8_t *chip8);
void op3xkk(chip8_t *chip8);

#endif // !CHIP8_H
