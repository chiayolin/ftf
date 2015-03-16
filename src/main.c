#include <stdio.h>
#include <string.h>

int main(int atgc, char *argv[]) {
	FILE *ifp, *ofp;
	char *mode = "r";
	char outputFilename[] = "test.ctxt";
	
	ifp = fopen("test.ptxt", mode);
	
	if (ifp == NULL) {
		fprintf(stderr, "Can't open input file test.ptxt\n");
		return 1;
	}

	char output[100];
	int ch, i = 0;
	while(( ch = fgetc(ifp)) != EOF ) {
		if(ch < 32) continue;	
		output[i] = ch + 3;
	}	
	output[i] = '\0';
	
	printf("%s", output);

	ofp = fopen(outputFilename, "w");

	if (ofp == NULL) {
		fprintf(stderr, "Can't open output file %s!\n",
			outputFilename);
		return 1;
	}

	fprintf(ofp, "%s", output);

	fclose(ifp);
	fclose(ofp);

	return 0;
}

