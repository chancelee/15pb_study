#pragma once
#include <windows.h>

class CEncrypt {
public:
	CEncrypt();
	virtual ~CEncrypt();
	bool EncryptSection(_In_ char*& pSrc, _In_ int srcLen);
};

