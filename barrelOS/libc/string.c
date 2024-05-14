#include <stdint.h>
#include <libc/string.h>


uint32_t strlen(char* str) {
	uint32_t length = 0;
	for (length = 0; str[length] != 0; length++);
	return length;
}

//add functionality for returning -1 or 1 depending on which is shorter
int8_t strcmp(char* string1, char* string2) {
	uint16_t i = 0;
	while(!((string1[i] == 0) && (string2[i] == 0))) {
		if (string1[i] != string2[i]) return 1;
		i++;
	}
	return 0;
}


char* strcpy(char* dest, char* src) {
	uint32_t i = 0;
	do {
		dest[i] = src[i];
		i++;
	} while(src[i - 1] != 0);
	return dest;
}

char* strcat(char* to, char* from) {
	strcpy(to + strlen(to), from);
	return to;
}

//standard implementation doesn't replace all delimiters on first call
char* strtok(char* str, char* delim) {
	static char* token;
	static char* str_end;

	if (str != NULL) {
		token = str;
		str_end = str + strlen(str);
		//replace all delim characters with null terminator
		for (uint32_t i = 0; str[i] != 0; i++) {
			for (uint8_t j = 0; j < strlen(delim); j++) {
				if (str[i] == delim[j]) str[i] = 0;
			}
		}
	} else {
		if (token == NULL) return NULL; //DO NOT strlen(NULL) or else
		token += strlen(token) + 1;
	}
	
	while((*token == 0) && (token < str_end)) token++;
	if (token >= str_end) token = NULL;
	return token;
}