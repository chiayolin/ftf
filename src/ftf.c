#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h>

#define MAXLEN 1024
#define IS_NUL 1
#define NO_NUL 0

void encrypt(char *plaintext, char *key);
void decrypt(char *ciphertext, char *key);
bool get_key(char *usr_in);
int scan(char *array, const char *tokens[]);

int main(int argc, char *argv[]) {
	/* exit if is missing arg(s) */
	if(argc < 3) {
		fprintf(stderr, 
			"error: missing arg(s)\n");
		return EXIT_FAILURE;
	}
	
	/* commands patched table */
	enum {
		DECRYPT = 0, D, ENCRYPT, E, ERROR };
	const char *args[] = {
		"decrypt", "-d", "encrypt", "-e", "\0" };
	bool e_flag = false;

	switch(scan(argv[1], args)) {
	case DECRYPT: case D:
		e_flag = DECRYPT;
		break;
	case ENCRYPT: case E:
		e_flag = ENCRYPT;
		break;
	default:
		fprintf(stderr, 
			"error: '%s' isn't an option\n", argv[1]);
		return EXIT_FAILURE;
	}
	
	char key[MAXLEN];
	bool null_key = get_key(key);
	if(null_key) { 
		fprintf(stderr, "error: null key\n");
		return EXIT_FAILURE;
	}
	
	(e_flag) ? encrypt(argv[2], key) : decrypt(argv[2], key);
	fprintf(stdout, "\n%s\n", argv[2]);

	return EXIT_SUCCESS;
}

int scan(char *array, const char *tokens[]) {
	int index = 0;
	while((strcmp(array, tokens[index]) != 0)
		&& !(strcmp(tokens[index], "\0") == 0))
			++index;
	return index;
}

bool get_key(char *usr_in) {
	fprintf(stdout, "key: ");

	static struct termios old, new; 
	tcgetattr(0, &old);
	new = old;
	new.c_lflag &= ~ICANON;
	new.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &new);

	char c;
	int i;
	c = i = 0;
	
	while((c = getchar()) != '\n')
		usr_in[i++] = c;

	tcsetattr(0, TCSANOW, &old);
	fprintf(stdout, "\n");
	
	return i != 0 ? NO_NUL : IS_NUL;
}

void encrypt(char *input, char *key) {
	int p_len = strlen(input),
	    k_len = strlen(key),
	    k_position, k_current, i;
	
	for(k_position = 0; k_position < k_len; k_position++) {
		k_current = key[k_position];
		for(i = 0; i < p_len; i++)
			input[i] = (((input[i] - 32) + (3 * k_current++)) % 94) + 32;
	}
	input[i] = '\0';
}

void decrypt(char *input, char *key) {
	int p_len = strlen(input),
	    k_len = strlen(key),
	    k_position, k_current, i;
	
	for(k_position = k_len - 1; k_position >= 0; k_position--) {
		k_current = key[k_position];
		for(i = 0; i < p_len; i++) {
			input[i] = (((input[i] - 32) - (3 * k_current++)) % 94) + 32;
			
			// add 94 to avoid unprintable char(s);
			if(input[i] < 32) input[i] = input[i] + 94;
		}
	}
	input[i] = '\0';
}

