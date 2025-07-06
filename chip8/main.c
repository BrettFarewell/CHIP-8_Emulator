#include "chip8.h"
#include <stdio.h>

int main(int argc, char* argv[]) {
	// Check to see there is two arguments
	/*if (argc != 2) {
		printf("Please supply ROM file: %s <ROM file>\n", argv[0]);
		return 1;
	}*/

	// Declare rom_path as second argument
	const char* rom_path = argv[1];

	// Declare chip 8 and initialize
	struct chip8 chip;
	chip8_init(&chip);


	// Load rom onto chip 8 or return an error if not
	if (!chip8_load_rom(&chip, rom_path)) {
		printf("Failed to load ROM: %s\n", rom_path);
		return 1;
	}

	// Execute main chip 8 cycle
	chip8_cycle(&chip);

	return 0;
};