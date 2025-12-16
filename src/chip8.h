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
  uint32_t display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
  uint8_t keypad[KEYS_COUNT]; // 16 keys - utilize user input from keyboard
} chip8_t;

/**
 * @brief Function pointer type for CHIP-8 opcode handlers.
 *
 * Each handler receives a pointer to the chip8_t state.
 */
typedef void (*opcodehandler_t)(chip8_t *chip8);
extern opcodehandler_t opcode_table[16];

/**
 * @brief Initialize the CHIP-8 system state.
 *
 * @param chip8 Pointer to the CHIP-8 state structure.
 */
void init(chip8_t *chip8);

/**
 * @brief Set the current opcode for the CHIP-8 system.
 *
 * @param chip8 Pointer to the CHIP-8 state structure.
 */
void set_opcode(chip8_t *chip8);

/**
 * @brief Process the current instruction for the CHIP-8 system.
 *
 * @param chip8 Pointer to the CHIP-8 state structure.
 */
void process_instruction(chip8_t *chip8);

/**
 * @brief Process the current instruction for the CHIP-8 system.
 *
 * @param chip8 Pointer to the CHIP-8 state structure.
 */
void process_instruction(chip8_t *chip8);

/**
 * @brief Execute one emulation cycle for the CHIP-8 system.
 *
 * @param chip8 Pointer to the CHIP-8 state structure.
 */
void cycle(chip8_t *chip8);

/**
 * @brief Update the CHIP-8 delay and sound timers.
 *
 * @param chip8 Pointer to the CHIP-8 state structure.
 */
void update_timers(chip8_t *chip8);

/**
 * @brief Get the value of a display pixel.
 *
 * @param chip8 Pointer to the CHIP-8 state structure.
 * @param x X coordinate of the pixel.
 * @param y Y coordinate of the pixel.
 * @return true if the pixel is set, false otherwise.
 */
bool get_display_pixel(chip8_t *chip8, int x, int y);

/**
 * @brief Set the value of a display pixel.
 *
 * @param chip8 Pointer to the CHIP-8 state structure.
 * @param x X coordinate of the pixel.
 * @param y Y coordinate of the pixel.
 */
void set_display_pixel(chip8_t *chip8, int x, int y);

// Instruction (opcode) handling methods

/**
 * 0nnn - SYS addr
 * Jump to a machine code routine at nnn.
 * pc = nnn
 */
void op_0nnn(chip8_t *chip8);

/**
 * 00E0 - CLS
 * Clear the display.
 */
void op_00E0(chip8_t *chip8);

/**
 * 00EE - RET
 * Return from subroutine.
 * pc = top of stack, sp -= 1
 */
void op_00EE(chip8_t *chip8);

/**
 * 1nnn - JP addr
 * Jump to location nnn.
 * pc = nnn
 */
void op_1nnn(chip8_t *chip8);

/**
 * 2nnn - CALL addr
 * Call subroutine at nnn.
 * sp += 1 and pc -> top stack, pc = nnn
 */
void op_2nnn(chip8_t *chip8);

/**
 * 3xkk - SE Vx, byte
 * Skip next instruction if Vx == kk.
 * if reg Vx == kk, pc += 2
 */
void op3xkk(chip8_t *chip8);

/**
 * 4xkk - SNE Vx, byte
 * Skip next instruction if Vx != kk.
 * if reg Vx != kk, pc += 2
 */
void op_4xkk(chip8_t *chip8);

/**
 * 5xy0 - SE Vx, Vy
 * Skip next instruction if Vx == Vy.
 * if reg Vx == reg Vy, pc += 2
 */
void op_5xy0(chip8_t *chip8);

/**
 * 6xkk - LD Vx, byte
 * Set Vx = kk.
 */
void op_6xkk(chip8_t *chip8);

/**
 * 7xkk - ADD Vx, byte
 * Set Vx = Vx + kk.
 */
void op_7xkk(chip8_t *chip8);

/**
 * 8xy0 - LD Vx, Vy
 * Set Vx = Vy.
 */
void op_8xy0(chip8_t *chip8);

/**
 * 8xy1 - OR Vx, Vy
 * Set Vx = Vx OR Vy (bitwise OR).
 * If either bit is 1, then same bit in result is 1, else 0.
 */
void op_8xy1(chip8_t *chip8);

/**
 * 8xy2 - AND Vx, Vy
 * Set Vx = Vx AND Vy (bitwise AND).
 */
void op_8xy2(chip8_t *chip8);

/**
 * 8xy3 - XOR Vx, Vy
 * Set Vx = Vx XOR Vy (bitwise exclusive OR).
 */
void op_8xy3(chip8_t *chip8);

/**
 * 8xy4 - ADD Vx, Vy
 * Set Vx = Vx + Vy, set VF = carry.
 * If result > 255, VF = 1, otherwise 0. Only last 8 bits are kept.
 */
void op_8xy4(chip8_t *chip8);
/**
 * 8xy5 - SUB Vx, Vy
 * Set Vx = Vx - Vy, set VF = NOT borrow.
 * If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx,
 * and the results stored in Vx.
 */
void op_8xy5(chip8_t *chip8);

/**
 * 8xy6 - SHR Vx {, Vy}
 * Set Vx = Vx SHR 1.
 * If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0.
 * Then Vx is divided by 2.
 */
void op_8xy6(chip8_t *chip8);

/**
 * 8xy7 - SUBN Vx, Vy
 * Set Vx = Vy - Vx, set VF = NOT borrow.
 * If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy,
 * and the results stored in Vx.
 */
void op_8xy7(chip8_t *chip8);

/**
 * 8xyE - SHL Vx {, Vy}
 * Set Vx = Vx SHL 1.
 * If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0.
 * Then Vx is multiplied by 2.
 */
void op_8xyE(chip8_t *chip8);

/**
 * 9xy0 - SNE Vx, Vy
 * Skip next instruction if Vx != Vy.
 * The values of Vx and Vy are compared, and if they are not equal, the program
 * counter is increased by 2.
 */
void op_9xy0(chip8_t *chip8);

/**
 * Annn - LD I, addr
 * Set I = nnn.
 * The value of register I is set to nnn.
 */
void op_Annn(chip8_t *chip8);

/**
 * Bnnn - JP V0, addr
 * Jump to location nnn + V0.
 * The program counter is set to nnn plus the value of V0.
 */
void op_Bnnn(chip8_t *chip8);

/**
 * Cxkk - RND Vx, byte
 * Set Vx = random byte AND kk.
 * The interpreter generates a random number from 0 to 255, which is then ANDed
 * with the value kk. The results are stored in Vx.
 */
void op_Cxkk(chip8_t *chip8);

/**
 * Dxyn - DRW Vx, Vy, nibble
 * Display n-byte sprite starting at memory location I at (Vx, Vy), set VF =
 * collision. The interpreter reads n bytes from memory, starting at the address
 * stored in I. These bytes are then displayed as sprites on screen at
 * coordinates (Vx, Vy). Sprites are XORed onto the existing screen. If this
 * causes any pixels to be erased, VF is set to 1, otherwise it is set to 0. If
 * the sprite is positioned so part of it is outside the coordinates of the
 * display, it wraps around to the opposite side of the screen.
 */
void op_Dxyn(chip8_t *chip8);

/**
 * Ex9E - SKP Vx
 * Skip next instruction if key with the value of Vx is pressed.
 * Checks the keyboard, and if the key corresponding to the value of Vx is
 * currently in the down position, PC is increased by 2.
 */
void op_Ex9E(chip8_t *chip8);

/**
 * ExA1 - SKNP Vx
 * Skip next instruction if key with the value of Vx is not pressed.
 * Checks the keyboard, and if the key corresponding to the value of Vx is
 * currently in the up position, PC is increased by 2.
 */
void op_ExA1(chip8_t *chip8);

/**
 * Fx07 - LD Vx, DT
 * Set Vx = delay timer value.
 * The value of DT is placed into Vx.
 */
void op_Fx07(chip8_t *chip8);

/**
 * Fx0A - LD Vx, K
 * Wait for a key press, store the value of the key in Vx.
 * All execution stops until a key is pressed, then the value of that key is
 * stored in Vx.
 */
void op_Fx0A(chip8_t *chip8);

/**
 * Fx15 - LD DT, Vx
 * Set delay timer = Vx.
 * DT is set equal to the value of Vx.
 */
void op_Fx15(chip8_t *chip8);

/**
 * Fx18 - LD ST, Vx
 * Set sound timer = Vx.
 * ST is set equal to the value of Vx.
 */
void op_Fx18(chip8_t *chip8);

/**
 * Fx1E - ADD I, Vx
 * Set I = I + Vx.
 * The values of I and Vx are added, and the results are stored in I.
 */
void op_Fx1E(chip8_t *chip8);

/**
 * Fx29 - LD F, Vx
 * Set I = location of sprite for digit Vx.
 * The value of I is set to the location for the hexadecimal sprite
 * corresponding to the value of Vx.
 */
void op_Fx29(chip8_t *chip8);

/**
 * Fx33 - LD B, Vx
 * Store BCD representation of Vx in memory locations I, I+1, and I+2.
 * The interpreter takes the decimal value of Vx, and places the hundreds digit
 * in memory at location in I, the tens digit at location I+1, and the ones
 * digit at location I+2.
 */
void op_Fx33(chip8_t *chip8);

/**
 * Fx55 - LD [I], Vx
 * Store registers V0 through Vx in memory starting at location I.
 * The interpreter copies the values of registers V0 through Vx into memory,
 * starting at the address in I.
 */
void op_Fx55(chip8_t *chip8);

/**
 * Fx65 - LD Vx, [I]
 * Read registers V0 through Vx from memory starting at location I.
 * The interpreter reads values from memory starting at location I into
 * registers V0 through Vx.
 */
void op_Fx65(chip8_t *chip8);

#endif // !CHIP8_H
