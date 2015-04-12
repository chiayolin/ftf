#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h>

#define ENCRYPTION 1
#define DECRYPTION 0
#define MAXLEN 1024
#define BUFFNS 16
#define IS_NUL 1

struct _ftf {
	int p_len, k_len,
	    k_pos, k_cur;
};

int ftf_encrypt(char *_plaintext, char *_ciphertext, char *key);
void _ftf_encrypt(FILE *buffer, FILE *output, char *key);
int ftf_decrypt(char *_ciphertext, char *_plaintext, char *key);
void _ftf_decrypt(FILE *buffer, FILE *output, char *key);

int scan(char *array, const char *tokens[]);
void cpyfile(FILE *input, FILE *output);
char *gen_path(char *in, int mode);
char *buff_name(size_t size);
bool get_key(char *usr_in);

int scan(char *array, const char *tokens[]) {
	int index = 0;
	while((strcmp(array, tokens[index]) != 0)
		&& !(strcmp(tokens[index], "\0") == 0))
			++index;
	return index;
}

void cpyfile(FILE *input, FILE *output) {
	int ch = 0;
	rewind(input);
	rewind(output);

	while((ch = fgetc(input)) != EOF) {
		fputc(ch, output);
	}
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

int ftf_encrypt(char *_plaintext, char *_ciphertext, char *key) {
	/* generate a random buffer name with a size of BUFFNS */
	char *_buffer = buff_name(BUFFNS);
	//fprintf(stdout, "%s\n", _buffer);
	FILE *plaintext = fopen(_plaintext, "r"); /* open plaintext */
	FILE *buffer = fopen(_buffer, "w+"); /* open buffer file */
	
	/* null checks */
	if(plaintext == NULL || buffer == NULL)
		 return EXIT_FAILURE;
	
	cpyfile(plaintext, buffer);  /* clone plaintext to buffer */
	fclose(plaintext); /* close plaintext */
	FILE *ciphertext = fopen(_ciphertext, "w+"); /* open ciphertext */ 
	_ftf_encrypt(buffer, ciphertext, key); /* sexy encryption */
	unlink(_buffer); /* delete buffer */

	return EXIT_SUCCESS;
}

void _ftf_encrypt(FILE *buffer, FILE *output, char *key) {
	struct _ftf en;
	en.k_len = strlen(key); /* get key length */
	puts("_ftf_encrypt");
	for(en.k_pos = 0; en.k_pos < en.k_len; en.k_pos++) {
		en.k_cur = key[en.k_pos]; /* set current key */
		/* reset buffer and output */
		rewind(buffer);
		rewind(output);
		
		/* start encrypting */
		int ch = 0;
		while((ch = fgetc(buffer)) != EOF) {
			/* keep control char(s) */
			if(ch < 32) { 
				fputc(ch, output);
				continue;
			}
			ch = (((ch - 32) + (3 * en.k_cur++)) % 94) + 32;
			fputc(ch, output);
		}
	}
	
	/* add a new line in the end of output file */
	fseek(output, -1, SEEK_END);
	fputc('\n', output);
}

int ftf_decrypt(char *_ciphertext, char *_plaintext, char *key) {
	/* generate a random buffer name with a size of BUFFNS */
	char *_buffer = buff_name(BUFFNS);
	fprintf(stdout, "%s\n", _buffer);
	FILE *ciphertext = fopen(_ciphertext, "r"); /* open ciphertext */
	FILE *buffer = fopen(_buffer, "w+"); /* open buffer file */
	
	/* null checks */
	if(ciphertext == NULL || buffer == NULL)
		 return EXIT_FAILURE;
	
	cpyfile(ciphertext, buffer);  /* clone ciphertext to buffer */
	fclose(ciphertext); /* close ciphertext */
	FILE *plaintext = fopen(_plaintext, "w+"); /* open plaintext */ 
	_ftf_decrypt(buffer, plaintext, key); /* sexy decryption */
	
	/* close input and buffer file */
	fclose(buffer);
	fclose(plaintext);
	
	unlink(_buffer); /* delete buffer */

	return EXIT_SUCCESS;
}

void _ftf_decrypt(FILE *buffer, FILE *output, char *key) {
	struct _ftf de;
	de.k_len = strlen(key); /* get key length */
       	printf("key= %d\n", de.k_len);

	/* read init. factors reversely */
	for(de.k_pos = de.k_len - 1; de.k_pos >= 0; de.k_pos--) {
		de.k_cur = key[de.k_pos]; /* set current key */
		
		/* reset *ofp and *bfp */
		rewind(output);
		rewind(buffer);
		puts("+");

		/* start encrypting */
		int ch, _ch;
		ch = _ch = 0;
		while((ch = fgetc(buffer)) != EOF) {
			if(ch < 32) {
				fputc(ch, output);
				continue; /* skips control char(s) */
			}

			_ch = (((ch - 32) - (3 * de.k_cur++)) % 94) + 32;
			_ch = (_ch < 32) ? _ch + 94 : _ch;
			printf("_ch= %c\n", _ch);
			fputc(_ch, output);
		}
	}
}

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
		"--decrypt", "-d", "--encrypt", "-e", "\0" };

	int i = 0, counter = 0;
	char *in_f_path = NULL;
	bool e_flag = false;
	
	while(argv[++i] != NULL) {
		printf("argv[%d]= %s\n", i, argv[i]);
		printf("scan = %d\n", scan(argv[i], args));
		switch(scan(argv[i], args)) {
		case DECRYPT: case D: {
			e_flag = false;
			counter++;
			i++; /* skip one argument */
			
			if(argv[i] == NULL)
				fprintf(stderr, "error: missing arg(s)\n");
			in_f_path = argv[i];
			break;
		}
		case ENCRYPT: case E: {
			e_flag = true;
			counter++;
			i++; /* skip one argument */
			
			if(argv[i] == NULL)
				fprintf(stderr, "error: missing arg(s)\n");
			in_f_path = argv[i];
			break;
		}
		default: {
			fprintf(stderr, 
				"error: '%s' isn't an option\n", argv[1]);
			return EXIT_FAILURE;
		}
		}

		if(counter > 1) {
			fprintf(stderr, "error: too many arg(s)\n");
			return EXIT_FAILURE;
		}
	}

	char key[MAXLEN];
	bool null_key = get_key(key);
	if(null_key) { 
		fprintf(stderr, "error: null key\n");
		return EXIT_FAILURE;
	}
	
	char *out_f_path = gen_path(in_f_path, e_flag);
	printf("out_f_path= %s\n", out_f_path);
	(e_flag) ? ftf_encrypt(in_f_path, out_f_path, key) : 
		ftf_decrypt(in_f_path, out_f_path, key);

	return EXIT_SUCCESS;
}

char *gen_path(char *in, int mode) {
	static char extention[] = ".ftf";
	size_t size2 = strlen(extention);
	size_t size1 = strlen(in);
	char *out;
	
	/* error checks and allocate array */
	if((in == NULL) || ((out = malloc(size1 + size2))) == NULL)
		return NULL;
	memcpy(out, in, size1);

	switch(mode) {
	case ENCRYPTION: {
		memcpy(out + size1, extention, size2);	
		break;
	}
	case DECRYPTION: {
		puts("decryption gen_path");
		char *last_dot = strrchr(out, '.');
		*last_dot = '\0';
		printf("last_dot= %s\n", out);
		break;
	}
	default: {
		return NULL;
		break;
	}
	}

	return out;
}

