#include "chip8.h"

void chip8_init(struct chip8* chip) {
	chip->pc = 0x200;			// program counter, init at 512
	chip->i = 0x000;			// index register

	chip->sp = 0x00;			// stack pointer, init at 0

	chip->delay_timer = 0x00;	// 8-bit delay timer, init at 0
	chip->sound_timer = 0x00;	// 8-bit sound timer, init at 0

	memset(chip->screen, 0, sizeof(chip->screen));	// 64x32 monochrome screen, init all false

	chip->audio_playing = false;
	
	memset(chip->memory, 0, sizeof(chip->memory));  // clear memory
	memset(chip->stack, 0, sizeof(chip->stack));    // clear stack

	chip8_load_fonts(chip);
};

void chip8_load_fonts(struct chip8* chip) {
	uint8_t font_bytes[80] = {
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
	0xF0, 0x80, 0xF0, 0x80, 0x80 }; // F

	int font_bytes_size = sizeof(font_bytes);

	for (int i = 0; i < font_bytes_size; i++) {
		chip->memory[i + 0x50] = font_bytes[i];
	}
}

bool chip8_load_rom(chip8* chip, const char* filename) {
	FILE* rom = fopen("./chip8-roms/programs/IBM Logo.ch8", "rb"); // Hard-coded IBM Logo, CHANGE!!!
	if (rom == NULL) {
		printf("No ROM found at: %s\n", filename);
		return false;
	}

	fseek(rom, 0, SEEK_END);
	long file_size = ftell(rom);
	rewind(rom);

	if (file_size > (0x1000 - 0x200)) {
		printf("File size too big of ROM found at: %s\n", filename);
		fclose(rom);
		return false;
	}

	fread(&chip->memory[0x200], 1, file_size, rom);
	fclose(rom);

	printf("ROM loaded: %s\n", filename);
	return true;
};

void chip8_cycle(struct chip8* chip) {
	const int INSTRUCTIONS_PER_FRAME = 10;
	const int FRAMES_DURATION_MS = 1000 / 60;
	Uint32 last_frame_time = SDL_GetTicks();
	while (true) {
		// handle input

		// fetch, decode, excute opcodes
		for (uint8_t i = 0; i < INSTRUCTIONS_PER_FRAME; i++) {
			execute_opcode(chip);
		}

		// update timers -> delay & sound
		update_timers(chip);

		// render screen
		render(chip);
		
		// emulate 60Hz -> look at CPU cycles
		Uint32 current_frame_time = SDL_GetTicks();
		int elapsed_time = current_frame_time - last_frame_time;
		int delay = FRAMES_DURATION_MS - elapsed_time;
		if (delay > 0) {
			SDL_Delay(delay);
		}
		last_frame_time += FRAMES_DURATION_MS;
	}
};

void update_timers(struct chip8* chip) {
	uint8_t* delay = &chip->delay_timer;
	uint8_t* sound = &chip->sound_timer;

	// decrement delay timer
	if (*delay != 0) {
		*delay--;
	}

	// decrement sound timer
	if (*sound != 0) {
		*sound--;
	}

	check_sound(chip);
};

void check_sound(chip8* chip) {
	uint8_t* sound = &chip->sound_timer;
	if (*sound > 0) {
		if (!chip->audio_playing) {
			play_beep();
		}
	} else {
		/*SDL_ClearQueuedAudio(audio_device);*/
		chip->audio_playing = false;
	}
};

void play_beep() {

};

void render(chip8* chip) {

};

void execute_opcode(chip8* chip) {
	uint16_t opcode = (chip->memory[chip->pc] << 8) | chip->memory[chip->pc + 1]; // fetch opcode at pc and pc + 1 of memory
	uint8_t hexit0 = (opcode & 0xf000) >> 12;
	uint8_t hexit1 = (opcode & 0x0f00) >> 8;
	uint8_t hexit2 = (opcode & 0x00f0) >> 4;
	uint8_t hexit3 = (opcode & 0x000f);
	uint8_t byte1 = (opcode & 0x00ff);
	uint8_t address = (opcode & 0x0fff);
	bool do_increment = true;
	switch (hexit0) {
		case 0x0: // clear screen
			switch (byte1) {
				case 0xE0: // clear screen
					memset(chip->screen, 0, sizeof(chip->screen));
					break;
				case 0xEE:  // break 2NNN subroutine
					chip->pc = chip->stack[chip->sp];
					chip->sp--;
					break;
				default:
					break;
			}
			break;
		case 0x1: // jump to NNN of opcode (1NNN), do not increment pc
			chip->pc = 0x0fff & opcode;
			do_increment = false;
			break;
		case 0x6: { // set register VX
			chip->V[hexit1] = byte1;
			break;
		}
		case 0x7: { // add value to register VX
			chip->V[hexit1] += byte1;
			break;
		}
		case 0xA: { // set index register chip->i
			chip->i = address;
			break;
		}
		case 0xD: { // display vertical line N tall starting at value in VX (x-coord) and VY (y-coord) DXYN
			uint8_t start_x_coord = chip->V[hexit1] % SCREEN_WIDTH; // fetch starting x coord from x register
			uint8_t start_y_coord = chip->V[hexit2] % SCREEN_HEIGHT; // fetch starting y coord from y register
			
			chip->V[0xF] = 0; // set VF flag to 0

			for (uint8_t i = 0; i < hexit3; i++) {
				uint8_t sprite = chip->memory[chip->i + i]; // find sprite in memory
				uint8_t y_coord = start_y_coord + i; // set y coord to y coord + offset of 0 to height - 1
				if (y_coord >= SCREEN_HEIGHT) {
					break;
				}
				for (uint8_t j = 0; j < 8; j++) {
					uint8_t x_coord = start_x_coord + j; // set x coord to x coord + offset of 0 to 7
					if (x_coord >= SCREEN_WIDTH) {
						break;
					}
					if ((sprite >> (0x7 - j)) & 0x1) { // check to see if bit in sprite is 0x1 or 0x0
						if (chip->screen[y_coord][x_coord] == true) { // check to see if pixel is true
							chip->screen[y_coord][x_coord] = false;   // if so, change to false and set VF flag to 0x1
							chip->V[0xf] = 0x1;
						}
						else {
							chip->screen[y_coord][x_coord] = true;
						}
					}
				}
			}
			break;
		}
		default:
			break;
	}
	if (do_increment) {
		chip->pc += 2;
	}
};
