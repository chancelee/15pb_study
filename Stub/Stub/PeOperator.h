#pragma once
#include <windows.h>

class CPeOperator {
public:
	CPeOperator();
	virtual ~CPeOperator();

public:
	HANDLE openPeFile(_In_ const char* path);
	void closePeFile(_In_ HANDLE hFile);
	bool savePeFile(_In_ const char* pFileData, _In_ int nSize, _In_ const char* path);
	char* getFileData(_In_ const char* pFilePath, _Out_opt_ int* nFileSize = NULL);
	void freeFileData(_In_ char* pFileData);
	IMAGE_DOS_HEADER* getDosHeader(_In_ char* pFileData);
	IMAGE_NT_HEADERS* getNtHeader(_In_ char* pFileData);
	IMAGE_FILE_HEADER* getFileHeader(_In_ char* pFileData);
	IMAGE_OPTIONAL_HEADER* getOptionHeader(_In_ char* pFileData);
	int getSectionIndex(_In_ char* pFileData, _In_ const char* scnName);
	bool CompressSection(_In_ char* pFileData, _In_ DWORD newRawSize_NoAlignment, _In_ const char* pSectionName);
	bool SaveCompressSectionFile(_In_ char* pFileData, _In_ const char* pSrc, 
		_In_ DWORD newRawSize_NoAlignment, HANDLE hFile, _In_ const char* pSectionName);
	IMAGE_SECTION_HEADER* getSection(_In_ char* pFileData, _In_ const char* scnName);
	IMAGE_SECTION_HEADER* getLastSection(_In_ char* pFileData);
	void addSectionHeader(char*& pFileData, int& nFileSize, const char* pScnName, int nSize = 0x200);
	DWORD GetAddrSectionRva(_In_ char* pFileData, _In_ DWORD addr);
	DWORD RvaToFileOffset(_In_ char* pFileData, _In_ DWORD addr);
	int aligment(_In_ int size, _In_  int aliginment);

public:
	void fixStubRelocal(HMODULE hStub, DWORD newSectionRva, DWORD default_base);
	void fixStubIAT(HMODULE hStub, DWORD newSectionRva);

private:
	DWORD m_OriginalSectionSize;
};

