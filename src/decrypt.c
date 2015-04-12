#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>

#define MAXLEN 1024
#define BUFFNS 16
#define IS_NUL 1

struct _ftf {
	int k_len, k_pos, k_cur;
};

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
	
	return i != 0 ? !IS_NUL : IS_NUL;
}

char *buff_name(size_t size) {
	static char charset[] = 
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	char *_buff_name = malloc(sizeof(char) * (size + 1));
	
	if(_buff_name) {
		int i, key;
		
		srand(time(NULL));

		_buff_name[0] = '.';
		for (i = 1; i < size; i++) {
			key = rand() % (int)(sizeof(charset) - 1);
			_buff_name[i] = charset[key];
		}
		_buff_name[size] = '\0';
	}

	return _buff_name;
}

int main(int argc, char *argv[]) {
	FILE *ifp, *ofp, *bfp;
	
	ifp = fopen(argv[1], "r"); /* open input file */
	
	/* generate random buffer name with a size of BUFFNS */
	char *buffer = buff_name(BUFFNS);
	printf("%s\n", buffer);
	
	bfp = fopen(buffer, "w+"); /* open buffer file */

	if(ifp == NULL || bfp == NULL)
		 return EXIT_FAILURE;
	
	int ifp_ch = 0;
	while((ifp_ch = fgetc(ifp)) != EOF)
		fputc(ifp_ch, bfp);

	fclose(ifp); /* close input file */
	ofp = fopen(argv[2], "w+");  /* open output file */ 
	
	struct _ftf de; 
	char key[MAXLEN];	
	get_key(key);
	de.k_len = strlen(key);
	
	for(de.k_pos = de.k_len - 1; de.k_pos >= 0; de.k_pos--) {
		de.k_cur = key[de.k_pos]; /* set current key */
		
		/* reset *ofp and *bfp */
		rewind(ofp);
		rewind(bfp);
		
		/* start encrypting */
		int ch, _ch;
		ch = _ch = 0;
		
		while((ch = fgetc(bfp)) != EOF) {
			if(ch < 32) {
				fputc(ch, ofp);
				continue; /* skips control char(s) */
			}

			_ch = (((ch - 32) - (3 * de.k_cur++)) % 94) + 32;
			_ch = (_ch < 32) ? _ch + 94 : _ch;
			fputc(_ch, ofp);
		}
	}
	
	/* add a new line in the end of output file */
	fseek(ofp, 0, SEEK_END);
	fputc('\n', ofp);
	
	/* close input and buffer file */
	fclose(bfp);
	fclose(ofp);
	
	unlink(buffer); /* delete buffer */

	return EXIT_SUCCESS;
}

