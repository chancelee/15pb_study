#include "encrypt.h"

// This's my key
static int KEY[20] = {
	0x87, 0x26, 0x64, 0x87, 0x19, 0x23, 0x38, 0x92, 0x37, 0x22,
	0xA4, 0x7B, 0xE0, 0x2F, 0xFB, 0xA7, 0xB1, 0x9A, 0x33, 0x66,
};

void encrypt(const char* plain_text, char* cipher_text, int length) {
	for (int i = 0; i < length; i++) {
		*cipher_text++ = *(plain_text + i) ^ KEY[i];
	}
}

void decrypt(const char* cipher_text, char* plain_text, int length) {
	for (int i = 0; i < length; i++) {
		*plain_text++ = *(cipher_text + i) ^ KEY[i];
	}	
}