#include "PeOperator.h"

CPeOperator::CPeOperator() : m_OriginalSectionSize(0){
}

CPeOperator::~CPeOperator() {
}

HANDLE CPeOperator::
openPeFile(_In_ const char* path) {
	return CreateFileA(
		path, GENERIC_READ, FILE_SHARE_READ, NULL, 
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

void CPeOperator::
closePeFile(_In_ HANDLE hFile) {
	CloseHandle(hFile);
}

bool CPeOperator::
savePeFile(_In_ const char* pFileData, _In_ int nSize, _In_ const char* path) {
	HANDLE hFile = CreateFileA(
		path, GENERIC_WRITE, FILE_SHARE_READ, NULL, 
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return false;

	DWORD dwWrite = 0;

	WriteFile(hFile, pFileData, nSize, &dwWrite, NULL);
	CloseHandle(hFile);

	return dwWrite == nSize;
}

char* CPeOperator::
getFileData(_In_ const char* pFilePath, _Out_opt_ int* nFileSize) {
	HANDLE hFile = openPeFile(pFilePath);
	if (hFile == INVALID_HANDLE_VALUE)
		return NULL;

	DWORD dwSize = GetFileSize(hFile, NULL);
	if (nFileSize)
		*nFileSize = dwSize;

	char* pFileBuff = (char*)malloc(dwSize);
	memset(pFileBuff, 0, dwSize);

	DWORD dwRead = 0;

	ReadFile(hFile, pFileBuff, dwSize, &dwRead, NULL);
	CloseHandle(hFile);

	return pFileBuff;
}

void CPeOperator::
freeFileData(_In_ char* pFileData) {
	free(pFileData);
}

IMAGE_DOS_HEADER* CPeOperator::
getDosHeader(_In_ char* pFileData) {
	return (IMAGE_DOS_HEADER *)pFileData;
}

IMAGE_NT_HEADERS* CPeOperator::
getNtHeader(_In_ char* pFileData) {
	return (IMAGE_NT_HEADERS*)(getDosHeader(pFileData)->e_lfanew + (SIZE_T)pFileData);
}

IMAGE_FILE_HEADER* CPeOperator::
getFileHeader(_In_ char* pFileData) {
	return &getNtHeader(pFileData)->FileHeader;
}

IMAGE_OPTIONAL_HEADER* CPeOperator::
getOptionHeader(_In_ char* pFileData) {
	return &getNtHeader(pFileData)->OptionalHeader;
}

int CPeOperator::
getSectionIndex(_In_ char* pFileData, _In_ const char* scnName) {
	DWORD dwScnCount = getFileHeader(pFileData)->NumberOfSections;
	IMAGE_SECTION_HEADER* pScn = IMAGE_FIRST_SECTION(getNtHeader(pFileData));
	char buff[10] = { 0 };

	for (DWORD i = 0; i < dwScnCount; ++i) {
		memcpy_s(buff, 8, (char*)pScn[i].Name, 8);
		if (strcmp(buff, scnName) == 0)
			return i;
	}

	return -1;
}

bool CPeOperator::
CompressSection(_In_ char* pFileData, _In_ DWORD newRawSize_NoAlignment, _In_ const char* pSectionName) {
	IMAGE_OPTIONAL_HEADER* pNtOptionHeader = getOptionHeader(pFileData);
	int SectionIndex = getSectionIndex(pFileData, pSectionName);
	if (SectionIndex < 0) {
		MessageBox(0, L"Err", L"NoSuchSection", 0);
		return false;
	}
	DWORD NewFileSize_Alignment = aligment(newRawSize_NoAlignment, pNtOptionHeader->FileAlignment);
	DWORD NewMemSize_NoAlignment = newRawSize_NoAlignment;
	DWORD NewMemSize_Alignment = aligment(NewMemSize_NoAlignment, pNtOptionHeader->SectionAlignment);
	DWORD dwScnCount = getFileHeader(pFileData)->NumberOfSections;
	IMAGE_SECTION_HEADER* pFirstScn = IMAGE_FIRST_SECTION(getNtHeader(pFileData));

	for (DWORD i = SectionIndex + 1; i < dwScnCount; i++) {
		pFirstScn[i].PointerToRawData -= (pFirstScn[SectionIndex].SizeOfRawData - NewFileSize_Alignment);
	}

	m_OriginalSectionSize = pFirstScn[SectionIndex].SizeOfRawData;
	pFirstScn[SectionIndex].SizeOfRawData = NewFileSize_Alignment;

	return true;
}

bool CPeOperator::
SaveCompressSectionFile(_In_ char* pFileData, _In_ const char* pSrc, 
	_In_ DWORD newRawSize_NoAlignment, HANDLE hFile, _In_ const char* pSectionName) {
	IMAGE_OPTIONAL_HEADER* pNtOptionHeader = getOptionHeader(pFileData);
	DWORD NewFileSize_Alignment = aligment(newRawSize_NoAlignment, pNtOptionHeader->FileAlignment);
	DWORD dwWrite = 0;

	WriteFile(hFile, pSrc, newRawSize_NoAlignment, &dwWrite, NULL);
	if (dwWrite != newRawSize_NoAlignment)
		goto err;

	if (newRawSize_NoAlignment < NewFileSize_Alignment) {
		int i = 0;
		char zero_fill = 0;
		do {
			i++;
			WriteFile(hFile, &zero_fill, 1, &dwWrite, NULL);
			if (dwWrite != 1)
				goto err;
		} while (newRawSize_NoAlignment + i != NewFileSize_Alignment);
	}

	return true;

err:
	MessageBox(0, L"WriteCompressFile", L"Err", 0);
	return false;
}

IMAGE_SECTION_HEADER* CPeOperator::
getSection(_In_ char* pFileData, _In_ const char* scnName) {
	DWORD dwScnCount = getFileHeader(pFileData)->NumberOfSections;
	IMAGE_SECTION_HEADER* pScn = IMAGE_FIRST_SECTION(getNtHeader(pFileData));
	char buff[10] = { 0 };

	for (DWORD i = 0; i < dwScnCount; ++i) {
		memcpy_s(buff, 8, (char*)pScn[i].Name, 8);
		if (strcmp(buff, scnName) == 0)
			return pScn + i;
	}

	return nullptr;
}

IMAGE_SECTION_HEADER* CPeOperator::
getLastSection(_In_ char* pFileData) {
	DWORD dwScnCount = getFileHeader(pFileData)->NumberOfSections;
	IMAGE_SECTION_HEADER* pScn = IMAGE_FIRST_SECTION(getNtHeader(pFileData));
	return pScn + (dwScnCount - 1);
}

void CPeOperator::
addSectionHeader(char*& pFileData, int& nFileSize, const char* pScnName, int nSize) {
	/* When Execute This Operator, PE will add a new section header, Notice */
	++getFileHeader(pFileData)->NumberOfSections;

	IMAGE_SECTION_HEADER* pNewScn = getLastSection(pFileData);
	pNewScn->Misc.VirtualSize = nSize;

	/* Notice, SizeofImage Means Memory Size */
	/* First, The Whole Size */
	int SectionAlignment = getOptionHeader(pFileData)->SectionAlignment;
	getOptionHeader(pFileData)->SizeOfImage += aligment(nSize, SectionAlignment);

	/* Configure The New Section */
	int fileAlignment = getOptionHeader(pFileData)->FileAlignment;
	// 1. [New Section]->SizeOfRawData, In File, Need Alignment
	pNewScn->SizeOfRawData = aligment(nSize, fileAlignment);
	// 2. [New Section]->PointerToRawData, Start Address In file
	pNewScn->PointerToRawData = (pNewScn - 1)->PointerToRawData;
	pNewScn->PointerToRawData += (pNewScn - 1)->SizeOfRawData;
	// 3. [New Section]->VirtualAddress, Start Address In Memory, Need Alignment
	pNewScn->VirtualAddress = (pNewScn - 1)->VirtualAddress;
	int LastSectionSize = aligment((pNewScn - 1)->SizeOfRawData, SectionAlignment);
	pNewScn->VirtualAddress += LastSectionSize;
	// 4. [New Section]->Name
	memcpy_s(pNewScn->Name, 8, pScnName, 8);
	// 5. [New Section]->Character
	pNewScn->Characteristics = 0xE00000E0;

	// Finally, associate a new space
	nFileSize += pNewScn->SizeOfRawData;
	pFileData = (char*)realloc(pFileData, nFileSize);
}

DWORD CPeOperator::
GetAddrSectionRva(_In_ char* pFileData, _In_ DWORD addr) {
	DWORD dwScnCount = getFileHeader(pFileData)->NumberOfSections;
	IMAGE_SECTION_HEADER* pScn = IMAGE_FIRST_SECTION(getNtHeader(pFileData));

	for (DWORD i = 0; i < dwScnCount; ++i) {
		if (pScn[i].VirtualAddress <= addr && (pScn[i].VirtualAddress + pScn[i].SizeOfRawData) > addr) {
			return pScn[i].VirtualAddress;
		}
	}

	return -1;
}

DWORD CPeOperator::
RvaToFileOffset(_In_ char* pFileData, _In_ DWORD addr) {
	DWORD dwScnCount = getFileHeader(pFileData)->NumberOfSections;
	IMAGE_SECTION_HEADER* pScn = IMAGE_FIRST_SECTION(getNtHeader(pFileData));

	for (DWORD i = 0; i < dwScnCount; ++i) {
		if (pScn[i].VirtualAddress <= addr && (pScn[i].VirtualAddress + pScn[i].SizeOfRawData) > addr) {
			return addr - pScn[i].VirtualAddress + pScn[i].PointerToRawData;
		}
	}

	return -1;
}

int CPeOperator::
aligment(_In_ int size, _In_  int aliginment) {
	return (size) % (aliginment) == 0 ? (size) : 
		((size) / (aliginment) + 1 ) * (aliginment);
}

void CPeOperator::
fixStubRelocal(HMODULE hStub, DWORD newSectionRva, DWORD default_base) {
	DWORD relRva = getOptionHeader((char*)hStub)->DataDirectory[5].VirtualAddress;
	IMAGE_BASE_RELOCATION* pRelocation = (IMAGE_BASE_RELOCATION*)(relRva + (SIZE_T)hStub);

	struct TypeOffset {
		WORD ofs : 12;
		WORD type : 4;
	};

	TypeOffset* pTypOfs;
	DWORD oldProt;

	while (pRelocation->SizeOfBlock != 0) {
		pTypOfs = (TypeOffset*)(pRelocation + 1);
		int count = (pRelocation->SizeOfBlock - 8) / 2;

		for (int i = 0; i < count; ++i) {
			if (pTypOfs[i].type == 3) {
				DWORD rva = pTypOfs[i].ofs + pRelocation->VirtualAddress;
				SIZE_T* va = (SIZE_T*)(rva + (SIZE_T)hStub);
				VirtualProtect(va, 1, PAGE_READWRITE, &oldProt);
				DWORD BaseAddr2SectionRVA = GetAddrSectionRva((char*)hStub, pRelocation->VirtualAddress);
				if (BaseAddr2SectionRVA < 0) {
					MessageBox(0, L"Error", L"FixReLocal", 0);
					return;
				}
				*va -= (SIZE_T)hStub;
				*va -= BaseAddr2SectionRVA;
				*va += default_base;
				*va += newSectionRva;
				VirtualProtect(va, 1, oldProt, &oldProt);
			}
		}

		VirtualProtect(pRelocation, 1, PAGE_READWRITE, &oldProt);
		pRelocation->VirtualAddress -= GetAddrSectionRva((char*)hStub, pRelocation->VirtualAddress);
		pRelocation->VirtualAddress += newSectionRva;
		VirtualProtect(pRelocation, 1, oldProt, &oldProt);

		pRelocation = (IMAGE_BASE_RELOCATION*)((char*)pRelocation + pRelocation->SizeOfBlock);
	}
}

void CPeOperator::
fixStubIAT(HMODULE hStub, DWORD newSectionRva) {
	DWORD IatRva = getOptionHeader((char*)hStub)->DataDirectory[1].VirtualAddress;
	IMAGE_IMPORT_DESCRIPTOR* pIat = (IMAGE_IMPORT_DESCRIPTOR*)(IatRva + (SIZE_T)hStub);

	DWORD dwStubIatRva = getSection((char*)hStub, ".idata")->VirtualAddress;
	DWORD oldProt;
	while (pIat->OriginalFirstThunk) {
		IMAGE_THUNK_DATA32* sub_int = (IMAGE_THUNK_DATA32*)(pIat->OriginalFirstThunk + (SIZE_T)hStub);

		do {
			VirtualProtect(sub_int, 1, PAGE_READWRITE, &oldProt);
			sub_int->u1.AddressOfData -= dwStubIatRva;
			sub_int->u1.AddressOfData += newSectionRva;
			VirtualProtect(sub_int, 1, oldProt, &oldProt);
			sub_int++;
		} while (sub_int->u1.AddressOfData);

		VirtualProtect(pIat, 1, PAGE_READWRITE, &oldProt);
		pIat->OriginalFirstThunk -= dwStubIatRva;
		pIat->OriginalFirstThunk += newSectionRva;
		pIat->FirstThunk -= dwStubIatRva;
		pIat->FirstThunk += newSectionRva;
		pIat->Name -= dwStubIatRva;
		pIat->Name += newSectionRva;
		VirtualProtect(pIat, 1, oldProt, &oldProt);
		pIat++;
	}
}
