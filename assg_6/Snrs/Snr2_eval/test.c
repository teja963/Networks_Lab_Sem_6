#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main() {
	FILE *fp = fopen("logincred.txt", "r");
	char buffer[1024];
	while(fscanf(fp, "%s", buffer) != -1) {
		printf("%s \n", buffer);
	}
}