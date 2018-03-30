#pragma once

#include <windows.h>

class CCompress {
public:
	CCompress();
	virtual ~CCompress();
	bool compress_code(_In_ char* pSrc, _In_ int srcLen, _Out_opt_ char*& pDst, _Out_opt_ int* pDstLen);
	bool decompress_code(_Out_opt_ char*& pDst, _Out_opt_ int* pDstLen, _In_ char* pSrc, _In_ int srcLen);
};
