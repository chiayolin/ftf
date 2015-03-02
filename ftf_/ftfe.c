// FTFE

#include <stdio.h>
#include <termios.h>
#include <string.h>

#define MAX 1000

static struct termios old, new;

/* Initialize new terminal i/o settings */
void initTermios(int echo) {
	  tcgetattr(0, &old); /* grab old terminal i/o settings */
	  new = old; /* make new settings same as old settings */
	  new.c_lflag &= ~ICANON; /* disable buffered i/o */
	  new.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
	  tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) {
	  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) {
	char ch;
	initTermios(echo);
	ch = getchar();
	resetTermios();
	return ch;
}

/* Read 1 character without echo */
char getch(void) {
	return getch_(0);
}

/* Read 1 character with echo */
char getche(void) {
	return getch_(1);
}

int main() {
	char input[MAX];
	int ch, i;
	ch = i = 0;
	
	/* read input */
	while((ch = getch()) != '\n')
		input[i++] = ch;
	input[i] = '\0';

	/* get length */
	int len = strlen(input);
	
	/* keys */
	int key_posi = 0, key[] = { 1, 8, 6, 1, 3, 5, 6};
	size_t key_size = sizeof(key) / sizeof(int);
	
	/* party time! */
	char output[MAX];
	int key_current = 0;
	for(key_posi = 0; key_posi < key_size; key_posi++) {
		key_current = key[key_posi];	
		for(i = 0; i < len; i++)
			input[i] = (((input[i] - 32) + (3 * key_current++)) % 94) + 32;
	}
	input[i] = '\0';

	printf("%s\n", input);
	return 0;
}
