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
	FILE* rom = fopen("./chip8-roms/programs/IBM Logo.ch8", "rb");
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
	// fetch, decode, excute -> opcodes
	// 
	// update timers -> delay & sound
	update_timers(chip);
	// render screen
	// handle input
	// emulate 60Hz -> look at CPU cycles
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
