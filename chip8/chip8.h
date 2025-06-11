#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef chipheader
#define chipheader

typedef struct chip8 {
	uint8_t memory[4096];    // 4 kilobytes memory
	uint16_t pc;			 // program counter
	uint16_t i;				 // index register

	uint16_t stack[16];      // 16 16-bit stack
	uint8_t sp;              // stack pointer

	uint8_t V[16];           // 16 8-bit variable registers

	uint8_t delay_timer;     // 8-bit delay timer
	uint8_t sound_timer;     // 8-but sound timer

	bool screen[64][32];     // 64x32 monochrome screen

	bool audio_playing;      // is audio currently playing check
} chip8;

void chip8_init(struct chip8* chip);

bool chip8_load_rom(struct chip8* chip, const char* filename);

void chip8_load_fonts(struct chip8* chip);

void chip8_cycle(struct chip8* chip);

void check_sound(chip8* chip);

void play_beep();

#endif // !chipheader

