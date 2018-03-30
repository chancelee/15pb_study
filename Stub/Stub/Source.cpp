#include <windows.h>
#include "Compress.h"
#include "PeOperator.h"
#include "Encrypt.h"
#include <stdio.h>

struct StubConfigure {
	unsigned int g_targetOep;
	DWORD dwTextRva;
	DWORD dwTextSize;
	DWORD dwIATRva;
	DWORD dwIATSize;
	DWORD dwRelocationRva;
	DWORD dwRelocationSize;
	DWORD dwTlsRva;
	DWORD dwTlsSize;
	DWORD dwImageBase;
	DWORD cryptkey;
};

struct StubCompress {
	DWORD dwSectionAddr;
	DWORD dwSectionSize;
	DWORD dwSectionCompressSize;
	DWORD dwSectionDecryptSize;
};

#define _COMPRESS_SECTION_NUM_ 0x2
#define _MAX_SECTION__ 0x10

int main(int argc, char** argv) {
	char path[MAX_PATH] = { 0 };
	CPeOperator pe_obj;
	char* pFileData = NULL;
	int   nFileSize = 0;

	printf("Please Input File Path: ");
	scanf_s("%s", path, MAX_PATH);
	getchar();

	pFileData = pe_obj.getFileData(path, &nFileSize);
	if (pFileData == NULL) {
		printf("Open File Failed\n");
		getchar();
		return 0;
	}

	HMODULE hStub = LoadLibraryExA("..\\Release\\StubDll.dll", 0, DONT_RESOLVE_DLL_REFERENCES);
	if (hStub == NULL) {
		printf("Stub Dll Load Failed\n");
		getchar();
		return 0;
	}

	/* Before Operate Any Section, Obtain Original Data, Saved!! */
	StubConfigure* pRecovery_Necessary_Data = (StubConfigure*)GetProcAddress(hStub, "g_Storage_Recovery_Data");
	IMAGE_OPTIONAL_HEADER* pOptional_header = pe_obj.getOptionHeader(pFileData);
	pRecovery_Necessary_Data->g_targetOep = pe_obj.getOptionHeader(pFileData)->AddressOfEntryPoint;
	pRecovery_Necessary_Data->dwIATRva = pOptional_header->DataDirectory[1].VirtualAddress;
	pRecovery_Necessary_Data->dwIATSize = pOptional_header->DataDirectory[1].Size;
	pRecovery_Necessary_Data->dwRelocationRva = pOptional_header->DataDirectory[5].VirtualAddress;
	pRecovery_Necessary_Data->dwRelocationSize = pOptional_header->DataDirectory[5].Size;
	pRecovery_Necessary_Data->dwImageBase = pOptional_header->ImageBase;
	if (pOptional_header->DataDirectory[9].Size) {
		pRecovery_Necessary_Data->dwTlsRva = pOptional_header->DataDirectory[9].VirtualAddress;
		pRecovery_Necessary_Data->dwTlsSize = pOptional_header->DataDirectory[9].Size;
	}

	/* Obtain Compress Original Info */
	StubCompress* pRecovery_Compress_Section_Data = (StubCompress*)GetProcAddress(hStub, "g_Storage_Compress_Section_Data");
	unsigned int* pCompress_Section_Num = (unsigned int*)GetProcAddress(hStub, "g_Compress_Section_Num");
	CCompress compress_obj;
	char* pCompressed_Section_Sample = nullptr;
	int Compressed_Section_Sample_Size = 0;
	IMAGE_SECTION_HEADER* pSection_Header_Sample;

	/* Compress .text && .rdata */
	for (int i = 0; i < _COMPRESS_SECTION_NUM_; i++) {
		pSection_Header_Sample = i ? pe_obj.getSection(pFileData, ".rdata") : pe_obj.getSection(pFileData, ".text");
		pRecovery_Compress_Section_Data[i].dwSectionAddr = pSection_Header_Sample->VirtualAddress;
		pRecovery_Compress_Section_Data[i].dwSectionSize = pe_obj.aligment(pSection_Header_Sample->Misc.VirtualSize, pOptional_header->SectionAlignment);
		compress_obj.compress_code(
			pSection_Header_Sample->PointerToRawData + pFileData, pSection_Header_Sample->SizeOfRawData,
			pCompressed_Section_Sample, &Compressed_Section_Sample_Size);
		pRecovery_Compress_Section_Data[i].dwSectionCompressSize = Compressed_Section_Sample_Size;
		pRecovery_Compress_Section_Data[i].dwSectionDecryptSize = pe_obj.aligment(Compressed_Section_Sample_Size, pOptional_header->FileAlignment);
		*pCompress_Section_Num += 1;

		if (pCompressed_Section_Sample) {
			delete[] pCompressed_Section_Sample;
			pCompressed_Section_Sample = nullptr;
		}
		Compressed_Section_Sample_Size = 0;
	}

	/* 1. Add Stub->.text To PE */
	/*----------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------*/
	IMAGE_SECTION_HEADER* pStub_Section_Text_Header;
	IMAGE_SECTION_HEADER* pNewStub_Section_Text_Header;
	char* pNewStub_Section_Text_Content = nullptr;

	pStub_Section_Text_Header = pe_obj.getSection((char*)hStub, ".text");
	pe_obj.addSectionHeader(pFileData, nFileSize, ".stubtxt", pStub_Section_Text_Header->SizeOfRawData);
	pNewStub_Section_Text_Header = pe_obj.getSection(pFileData, ".stubtxt");
	pNewStub_Section_Text_Content = pNewStub_Section_Text_Header->PointerToRawData + pFileData;
	/*----------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------*/

	/* 2. Add Stub->.idata To PE  */
	/*----------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------*/
	IMAGE_SECTION_HEADER* pStub_Section_Idata_Header;
	IMAGE_SECTION_HEADER* pNewStub_Section_Idata_Header;
	char* pNewStub_Section_Idata_Content = nullptr;

	pStub_Section_Idata_Header = pe_obj.getSection((char*)hStub, ".idata");
	pe_obj.addSectionHeader(pFileData, nFileSize, ".stubiat", pStub_Section_Idata_Header->SizeOfRawData);
	pNewStub_Section_Idata_Header = pe_obj.getSection(pFileData, ".stubiat");
	pNewStub_Section_Idata_Content = pNewStub_Section_Idata_Header->PointerToRawData + pFileData;

	/* First Fix Data, Then Copy It */
	pe_obj.fixStubIAT(hStub, pNewStub_Section_Idata_Header->VirtualAddress);

	/* Copy Fixed Data */
	memcpy_s(
		pNewStub_Section_Idata_Content, 
		pStub_Section_Idata_Header->SizeOfRawData, 
		pStub_Section_Idata_Header->VirtualAddress + (char*)hStub, 
		pStub_Section_Idata_Header->SizeOfRawData);
	/*----------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------*/

	/* 3. Add Stub->.reloc To PE  */
	/*----------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------*/
	IMAGE_SECTION_HEADER* pStub_Section_Relocation_Header;
	IMAGE_SECTION_HEADER* pNewStub_Section_Relocation_Header;
	char* pNewStub_Section_Relocation_Content;

	pStub_Section_Relocation_Header = pe_obj.getSection((char*)hStub, ".reloc");
	pe_obj.addSectionHeader(pFileData, nFileSize, ".stubrlc", pStub_Section_Relocation_Header->SizeOfRawData);
	pNewStub_Section_Relocation_Header = pe_obj.getSection(pFileData, ".stubrlc");
	pNewStub_Section_Relocation_Content = pNewStub_Section_Relocation_Header->PointerToRawData + pFileData;

	/* First Fix Data, Then Copy£¬Notice, Re-Obtain! */
	pNewStub_Section_Text_Header = pe_obj.getSection(pFileData, ".stubtxt");
	pNewStub_Section_Text_Content = pNewStub_Section_Text_Header->PointerToRawData + pFileData;
	pe_obj.fixStubRelocal(hStub, pNewStub_Section_Text_Header->VirtualAddress, pe_obj.getOptionHeader(pFileData)->ImageBase);

	/* Copy Fixed Data */
	memcpy_s(
		pNewStub_Section_Relocation_Content,
		pStub_Section_Relocation_Header->SizeOfRawData,
		pStub_Section_Relocation_Header->VirtualAddress + (char*)hStub,
		pStub_Section_Relocation_Header->SizeOfRawData);
	/*----------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------*/

	/* 4.0 Add New TLS Section */
	IMAGE_OPTIONAL_HEADER* pOriginalStubOptionHeader = pe_obj.getOptionHeader((char*)hStub);
	IMAGE_OPTIONAL_HEADER* pDstOptionHeader = pe_obj.getOptionHeader((char*)pFileData);
	if (pDstOptionHeader->DataDirectory[9].Size) {
		pNewStub_Section_Text_Header = pe_obj.getSection(pFileData, ".stubtxt");
		IMAGE_TLS_DIRECTORY32* pOriginalStubTls = (IMAGE_TLS_DIRECTORY32*)(pOriginalStubOptionHeader->DataDirectory[9].VirtualAddress + (char*)hStub);
		IMAGE_TLS_DIRECTORY32* pDstTls = (IMAGE_TLS_DIRECTORY32*)(
			pe_obj.RvaToFileOffset(pFileData, pDstOptionHeader->DataDirectory[9].VirtualAddress) + (char*)pFileData);

		IMAGE_SECTION_HEADER* pOriginal_Section_Text_Header = pe_obj.getSection((char*)hStub, ".text");
		int* pOrigianlTlsCallBack = (int*)(pOriginalStubTls->AddressOfCallBacks - pDstOptionHeader->ImageBase - pNewStub_Section_Text_Header->VirtualAddress +
			pOriginal_Section_Text_Header->VirtualAddress + (char*)hStub);
		DWORD oldProt;
		do {
			VirtualProtect(pOrigianlTlsCallBack, 4, PAGE_READWRITE, &oldProt);
			*pOrigianlTlsCallBack -= (pDstOptionHeader->ImageBase + pNewStub_Section_Text_Header->VirtualAddress);
			*pOrigianlTlsCallBack += (pOriginal_Section_Text_Header->VirtualAddress + (DWORD)hStub);
			*pOrigianlTlsCallBack -= (DWORD)hStub;
			*pOrigianlTlsCallBack -= pe_obj.GetAddrSectionRva((char*)hStub, *pOrigianlTlsCallBack);
			*pOrigianlTlsCallBack += (DWORD)pNewStub_Section_Text_Header->VirtualAddress;
			*pOrigianlTlsCallBack += pDstOptionHeader->ImageBase;
			VirtualProtect(pOrigianlTlsCallBack, 4, oldProt, &oldProt);
			pOrigianlTlsCallBack++;
		} while (*pOrigianlTlsCallBack);

		pNewStub_Section_Text_Header = pe_obj.getSection(pFileData, ".stubtxt");
		pDstOptionHeader = pe_obj.getOptionHeader((char*)pFileData);

		VirtualProtect(pOriginalStubTls, sizeof(IMAGE_TLS_DIRECTORY32), PAGE_READWRITE, &oldProt);
		int* p_tls = (int*)pOriginalStubTls;
		for (int i = 0; i < 3; i++) {
			*p_tls = ((int*)pDstTls)[i];
			p_tls++;
		}

		pOriginalStubTls->AddressOfCallBacks -= (pDstOptionHeader->ImageBase + pNewStub_Section_Text_Header->VirtualAddress);
		pOriginalStubTls->AddressOfCallBacks += (pOriginal_Section_Text_Header->VirtualAddress + (DWORD)hStub);
		pOriginalStubTls->AddressOfCallBacks -= (DWORD)hStub;
		pOriginalStubTls->AddressOfCallBacks -= pe_obj.GetAddrSectionRva((char*)hStub, pOriginalStubTls->AddressOfCallBacks);
		pOriginalStubTls->AddressOfCallBacks += (DWORD)pNewStub_Section_Text_Header->VirtualAddress;
		pOriginalStubTls->AddressOfCallBacks += pDstOptionHeader->ImageBase;

		pOriginalStubTls->Characteristics = pDstTls->Characteristics;
		VirtualProtect(pOriginalStubTls, sizeof(IMAGE_TLS_DIRECTORY32), oldProt, &oldProt);
	}

	/*----------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------*/

	/* 4.1 Because Stub.text Section have Some Data need Relocation, So It's necessary Copy it in the end */
	/*----------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------*/
	memcpy_s(
		pNewStub_Section_Text_Content,
		pStub_Section_Text_Header->SizeOfRawData,
		pStub_Section_Text_Header->VirtualAddress + (char*)hStub,
		pStub_Section_Text_Header->SizeOfRawData);
	/*----------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------*/

	/* 5. Modify Nt->Optional->DataDirectory */
	pOptional_header = pe_obj.getOptionHeader(pFileData); // re-obtain, because pFileData Maybe changed
	IMAGE_OPTIONAL_HEADER* pOptional_Stub_header = pe_obj.getOptionHeader((char*)hStub);
	DWORD dwStubIatRva = pStub_Section_Idata_Header->VirtualAddress;
	DWORD dwStubRelocationRva = pStub_Section_Relocation_Header->VirtualAddress;

	/* Re-Obtain The Section Info */
	pNewStub_Section_Idata_Header = pe_obj.getSection(pFileData, ".stubiat");
	pNewStub_Section_Relocation_Header = pe_obj.getSection(pFileData, ".stubrlc");
	pNewStub_Section_Text_Header = pe_obj.getSection(pFileData, ".stubtxt");
	pOriginalStubOptionHeader = pe_obj.getOptionHeader((char*)hStub);
	pOptional_header->DataDirectory[1].VirtualAddress =
		pOptional_Stub_header->DataDirectory[1].VirtualAddress - dwStubIatRva + pNewStub_Section_Idata_Header->VirtualAddress;
	pOptional_header->DataDirectory[1].Size = pOptional_Stub_header->DataDirectory[1].Size;

	pOptional_header->DataDirectory[5].VirtualAddress = 
		pOptional_Stub_header->DataDirectory[5].VirtualAddress - dwStubRelocationRva + pNewStub_Section_Relocation_Header->VirtualAddress;
	pOptional_header->DataDirectory[5].Size = pOptional_Stub_header->DataDirectory[5].Size;

	pDstOptionHeader = pe_obj.getOptionHeader((char*)pFileData);
	if (pDstOptionHeader->DataDirectory[9].Size) {
		DWORD NewOffset = pOriginalStubOptionHeader->DataDirectory[9].VirtualAddress -
			pe_obj.GetAddrSectionRva((char*)hStub, pOriginalStubOptionHeader->DataDirectory[9].VirtualAddress) +
			pNewStub_Section_Text_Header->VirtualAddress;
		pOptional_header->DataDirectory[9].VirtualAddress = NewOffset;
	}
	/*----------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------*/

	/* 6. Set OEP */
	pNewStub_Section_Text_Header = pe_obj.getSection(pFileData, ".stubtxt");
	SIZE_T pStubStart = (SIZE_T)GetProcAddress(hStub, "start");
	pStubStart -= (SIZE_T)hStub;
	pStubStart -= pStub_Section_Text_Header->VirtualAddress;
	pStubStart += pNewStub_Section_Text_Header->VirtualAddress;
	pOptional_header->AddressOfEntryPoint = pStubStart;
	/*----------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------------*/

	/* 7. Save PE To Disk */
	char pack_file[MAX_PATH] = {};
	WIN32_FIND_DATAA w32FindData = {};
	FindFirstFileA(path, &w32FindData);
	memcpy_s(pack_file, MAX_PATH, w32FindData.cFileName, strlen(w32FindData.cFileName));
	char* p = strstr(pack_file, ".exe");
	do {
		*p++ = '\0';
	} while (*p);
	strcat_s(pack_file, MAX_PATH, "_pack.exe");
	if (!pe_obj.savePeFile(pFileData, nFileSize, pack_file)) {
		MessageBox(0, L"Write pack file failed", L"Notice", 0);
	}

	/* ================================================================================================== */
	/* ================================================================================================== */
	/* ================================================================================================== */
	/* Compress Code, For Safe, Create Another File */
	/* Write To A New File */
	CEncrypt encrypt_obj;
	p = strstr(pack_file, "_pack.exe");
	do {
		*p++ = '\0';
	} while (*p);
	strcat_s(pack_file, MAX_PATH, "_compress_pack.exe");
	HANDLE hCompFile = CreateFileA(
		pack_file, GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hCompFile == INVALID_HANDLE_VALUE) {
		MessageBox(0, L"CreateCompressFile", L"Failed", 0);
		return false;
	}

	/* I. Write Original PE Header To File */
	SIZE_T dwSectionOfPointer = pe_obj.getSection(pFileData, ".rdata")->PointerToRawData;
	DWORD dwSectionOfSize = pe_obj.getSection(pFileData, ".rdata")->SizeOfRawData;
	DWORD dwHeaderSize = pe_obj.getSection(pFileData, ".text")->PointerToRawData;
	DWORD dwRealWriteSize = 0;
	DWORD dwCompressSection_OriginalTotalSize = pe_obj.getSection(pFileData, ".text")->SizeOfRawData + dwSectionOfSize;

	/* II. Write Compress Section To File */
	pSection_Header_Sample = pe_obj.getSection(pFileData, ".text");
	compress_obj.compress_code(
		pSection_Header_Sample->PointerToRawData + pFileData, pSection_Header_Sample->SizeOfRawData,
		pCompressed_Section_Sample, &Compressed_Section_Sample_Size);
	pe_obj.CompressSection(pFileData,
		Compressed_Section_Sample_Size, ".text");
	/* Encrypt Text Section */
	encrypt_obj.EncryptSection(pCompressed_Section_Sample, Compressed_Section_Sample_Size);

	/* Write Other Compress Sections */
	char* pCompressed_Section_Sample_Others = nullptr;
	int Compressed_Section_Sample_Size_Others = 0;

	compress_obj.compress_code(
		dwSectionOfPointer + pFileData, dwSectionOfSize,
		pCompressed_Section_Sample_Others, &Compressed_Section_Sample_Size_Others);
	pe_obj.CompressSection(pFileData,
		Compressed_Section_Sample_Size_Others, ".rdata");
	/* Encrypt Text Section */
	encrypt_obj.EncryptSection(pCompressed_Section_Sample_Others, Compressed_Section_Sample_Size_Others);

	/* Real Write */
	/* ======================================= */
	WriteFile(hCompFile, pFileData, dwHeaderSize, &dwRealWriteSize, NULL);
	if (dwRealWriteSize != dwHeaderSize) {
		MessageBox(0, L"Err", L"Write Compress Header", 0);
		return -1;
	}

	pe_obj.SaveCompressSectionFile(pFileData, pCompressed_Section_Sample,
		Compressed_Section_Sample_Size, hCompFile, ".text");

	pe_obj.SaveCompressSectionFile(pFileData, pCompressed_Section_Sample_Others,
		Compressed_Section_Sample_Size_Others, hCompFile, ".rdata");
	/* ======================================= */

	/* Clear buffer */
	if (pCompressed_Section_Sample) {
		delete[] pCompressed_Section_Sample;
		pCompressed_Section_Sample = nullptr;
	}

	if (pCompressed_Section_Sample_Others) {
		delete[] pCompressed_Section_Sample_Others;
		pCompressed_Section_Sample_Others = nullptr;
	}

	Compressed_Section_Sample_Size = 0;
	Compressed_Section_Sample_Size_Others = 0;

	/* III. Write Others To File */
	DWORD dwTailSize = nFileSize - (dwHeaderSize + dwCompressSection_OriginalTotalSize);
	WriteFile(hCompFile, pFileData + dwHeaderSize + dwCompressSection_OriginalTotalSize, dwTailSize, &dwRealWriteSize, NULL);
	if (dwRealWriteSize != dwTailSize) {
		MessageBox(0, L"Err", L"Write Compress Tail", 0);
		return -1;
	}

	CloseHandle(hCompFile);

	/* ================================================================================================== */
	/* ================================================================================================== */
	/* ================================================================================================== */

	pe_obj.freeFileData(pFileData);

	MessageBox(0, L"Success", L"DONE", 0);

	return 0;
}
