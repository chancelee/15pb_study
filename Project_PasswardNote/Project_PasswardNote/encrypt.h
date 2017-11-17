#ifndef __ENCRYPT_H__
#define __ENCRYPT_H__

#include <stdio.h>
#include <stdlib.h>

//extern function
extern void encrypt(const char* plain_text, char* cipher_text, int length);
extern void decrypt(const char* cipher_text, char* plain_text, int length);

#endif
