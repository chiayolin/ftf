#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h>

#define MAXLEN 1024

void encrypt(char *plaintext, char *key);
void decrypt(char *ciphertext, char *key);
int scan(char *array, const char *tokens[]);
int get_key(char *usr_in);

int main(int argc, char *argv[]) {
	/* exit if is missing arg(s) */
	if(argc < 2) {
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
		fprintf(stdout,
			"mode: decryption\n");
		break;
	case ENCRYPT: case E:
		e_flag = ENCRYPT;
		puts("hello");
		fprintf(stdout,
			"mode: encryption\n");
		break;
	default:
		fprintf(stderr, 
			"error: '%s' isn't an option\n", argv[1]);
		return EXIT_FAILURE;
	}
	
	puts("1");
	// (e_flag) ? encrypt(argv[2]) : decrypt(argv[2]);
	// fprintf(stdout, "%s\n", argv[2]);
	
	char usr_in[MAXLEN];
	puts("in");
	int len = get_key(usr_in);
	printf("%s %d\n", usr_in, len);

	return EXIT_SUCCESS;
}

int scan(char *array, const char *tokens[]) {
	int index = 0;
	while((strcmp(array, tokens[index]) != 0)
		&& !(strcmp(tokens[index], "\0") == 0))
			++index;
	return index;
}

// void encrypt(char *plaintext) {
	
int get_key(char *usr_in) {
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
	usr_in[i] = '\0';

	tcsetattr(0, TCSANOW, &old);
	fprintf(stdout, "\n");	
	return i;
}

