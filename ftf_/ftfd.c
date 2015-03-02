// FTFD

#include <stdio.h>
#include <string.h>

#define MAX 1000

int main() {
	char input[MAX];
	int ch, i;
	ch = i = 0;
	
	/* read input */
	while((ch = getchar()) != '\n')
		input[i++] = ch;
	input[i] = '\0';

	/* get length */
	int len = strlen(input);
	
	/* keys */
	int key_posi = 0, key[] = { 16, 8, 6, 1, 3, 5, 6};
	size_t key_size = sizeof(key) / sizeof(int);
	
	/* party time! */
	char output[MAX];
	int key_current = 0;
	for(key_posi = 6; key_posi >= 0; key_posi--) {
		key_current = key[key_posi];
		
		for(i = 0; i < len; i++) {
			input[i] = (((input[i] - 32) - (3 * key_current++)) % 94) + 32;
			
			/* add 94 to avoid unprintable char(s) */
			if(input[i] < 32)
				input[i] = input[i] + 94;
		}
	}
	input[i] = '\0';

	printf("%s\n", input);
	return 0;
}
