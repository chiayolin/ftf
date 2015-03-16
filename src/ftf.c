#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int scan(char *array, const char *tokens[]);

int main(int argc, char *argv[]) {
	/* exit if is missing arg(s) */
	if(argc < 2) {
		fprintf(stderr, 
			"error: missing arg(s)\n");
		return EXIT_FAILURE;
	}
	
	/* else continue */
	enum {
		DECRYPT = 0, D, ENCRYPT, E };
	const char *args[] = {
		"decrypt", "-d", "encrypt", "-e", "\0" };

	switch(scan(argv[1], args)) {
		case DECRYPT: case D:
			fprintf(stdout,
				"mode: decryption\n");
			break;
		case ENCRYPT: case E:
			fprintf(stdout,
				"mode: encryption\n");
			break;
		default:
			fprintf(stderr, 
				"error: '%s' isn't an option\n", argv[1]);
			return EXIT_FAILURE;
	}
		
	return EXIT_SUCCESS;
}

int scan(char *array, const char *tokens[]) {
	int index = 0;
	while((strcmp(array, tokens[index]) != 0)
		&& !(strcmp(tokens[index], "\0") == 0))
			++index;
	return index;
}

