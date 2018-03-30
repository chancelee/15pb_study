#include "Encrypt.h"

const unsigned char RoundKey[] = {
	0x11, 0x43, 0x91, 0x72,
	0x19, 0xA1, 0x2B, 0x9C,
	0x4F, 0x9D, 0x82, 0xFC,
	0x7D, 0x3C, 0x21, 0x69, 0x76
};

CEncrypt::CEncrypt() {
}

CEncrypt::~CEncrypt() {
}

bool CEncrypt::
EncryptSection(_In_ char*& pSrc, _In_ int srcLen) {
	for (int i = 0; i < srcLen; i++) {
		pSrc[i] ^= RoundKey[i % 0x11];
	}

	return true;
}
