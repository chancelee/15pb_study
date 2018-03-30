#include "Compress.h"
#define ZLIB_WINAPI
#include "zlib.h"
#include <stdlib.h>
#pragma comment(lib,"zlibstat.lib")

CCompress::CCompress() {
}

CCompress::~CCompress() {
}

bool CCompress::
compress_code(_In_ char* pSrc, _In_ int srcLen, _Out_opt_ char*& pDst, _Out_opt_ int* pDstLen) {
	*pDstLen = compressBound(srcLen);

	pDst = new char[*pDstLen]();
	if (pDst == nullptr) {
		MessageBox(0, L"Failed", L"new", 0);
		return false;
	}

	if (compress((Bytef*)pDst, (uLongf*)pDstLen, (byte*)pSrc, srcLen) != Z_OK) {
		MessageBox(0, L"Failed", L"compress", 0);
		return false;
	}

	return true;
}

bool CCompress::
decompress_code( _Out_opt_ char*& pDst, _Out_opt_ int* pDstLen, _In_ char* pSrc, _In_ int srcLen) {
	if (uncompress((byte*)pDst, (uLongf*)pDstLen, (Bytef*)pSrc, srcLen) != Z_OK) {
		MessageBox(0, L"Failed", L"decompress", 0);
		return false;
	}

	return true;
}
