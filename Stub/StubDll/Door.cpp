#include "Door.h"

#define ID_EDIT1	1010
#define ID_EDIT2	1020
#define ID_BUTTON1	1030
#define ID_BUTTON2	1040
#define ID_LABLE1	1050
#define ID_LABLE2	1060

#define HI_BYTE(c) (((c) >> 4) & 0x0F)
#define LO_BYTE(c) ((c) & 0x0F)

unsigned char g_High[] = {
	0x6B, 0x45, 0x79, 0x7B,
	0x55, 0x61, 0x72, 0x65,
	0x53, 0x30, 0x63, 0x4C,
	0x65, 0x56, 0x65, 0x72, 0x7D };

unsigned char g_Low[] = {
	0x7D, 0x72, 0x65, 0x56,
	0x65, 0x4C, 0x63, 0x30,
	0x53, 0x65, 0x72, 0x61,
	0x55, 0x7B, 0x79, 0x45, 0x6B };

HWND g_hEWndUser, g_hEWndPasswd, g_hBWndOk, g_hBWndCancel;

CDoor::CDoor() {
}

CDoor::~CDoor() {
}

bool CDoor::
InitDoor() {
	WNDCLASS wce;

	wce.style = CS_HREDRAW | CS_VREDRAW;
	wce.lpfnWndProc = CDoor::WindowProc;
	wce.cbClsExtra = 0;
	wce.cbWndExtra = 0;
	wce.hInstance = GetModuleHandleA(NULL);
	wce.hIcon = LoadIcon(GetModuleHandleA(NULL), IDI_APPLICATION);
	wce.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wce.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wce.lpszMenuName = NULL;
	wce.lpszClassName = _T("First");

	RegisterClass(&wce);

	HWND hWnd = CreateWindow(
		_T("First"),
		_T("My First Window"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		400,
		180,
		nullptr,
		nullptr,
		GetModuleHandleA(NULL),
		nullptr);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

char CDoor::
int2char(int num) {
	if (num <= 9 && num >= 0) {
		return num + '0';
	} else if (num <= 0xF && num >= 0xA) {
		return num + 0x57;
	} else {
		return -1;
	}
}

void CDoor::
WCHARTOCHAR(wchar_t* szWchar, unsigned int uSize, char* szChar) {
	WideCharToMultiByte(CP_ACP, NULL, szWchar, -1, szChar, uSize, NULL, FALSE);
}

void CDoor::
CHARTOWCHAR(char* szChar, unsigned int uSize, wchar_t* szWchar) {
	MultiByteToWideChar(CP_ACP, NULL, szChar, -1, szWchar, uSize);
}

LRESULT CDoor::
Wm_CreateProc(HWND hwnd, LPCREATESTRUCT cs) {
	// Create usr label 
	CreateWindow(TEXT("STATIC"),
		TEXT("User:"),
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		25, 22, 80, 20,
		hwnd,
		(HMENU)ID_LABLE1,
		cs->hInstance,
		NULL);

	// Create passwd label 
	CreateWindow(TEXT("STATIC"),
		TEXT("Password:"),
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		25, 62, 80, 20,
		hwnd,
		(HMENU)ID_LABLE2,
		cs->hInstance,
		NULL);

	// Create password textBox
	g_hEWndUser = CreateWindow(TEXT("EDIT"),
		TEXT(""),
		WS_CHILD | WS_VISIBLE | WS_BORDER,
		110, 20, 200, 20,
		hwnd,
		(HMENU)ID_EDIT1,
		cs->hInstance,
		NULL);

	// Create password textBox
	g_hEWndPasswd = CreateWindow(TEXT("EDIT"),
		TEXT(""),
		WS_CHILD | WS_VISIBLE | ES_PASSWORD | WS_BORDER | ES_AUTOHSCROLL,
		110, 60, 200, 20,
		hwnd,
		(HMENU)ID_EDIT2,
		cs->hInstance,
		NULL);

	// Create OK button
	g_hBWndOk = CreateWindow(TEXT("BUTTON"),
		TEXT("Ok"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		60, 100, 60, 25,
		hwnd,
		(HMENU)ID_BUTTON1,
		cs->hInstance,
		NULL);

	// Create Cancel button
	g_hBWndCancel = CreateWindow(TEXT("BUTTON"),
		TEXT("Cancel"),
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		180, 100, 60, 25,
		hwnd,
		(HMENU)ID_BUTTON2,
		cs->hInstance,
		NULL);

	return 0;
}

LRESULT CDoor::
Wm_CommandProc(HWND hwnd, WORD wNotifyCode, WORD wID, HWND hwndCtl) {
	TCHAR usr[128] = {};
	TCHAR passwd[128] = {};
	char usr_ascii[512] = {};
	char passwd_ascii[512] = {};
	char serial_result[512] = {};

	if (wID == ID_BUTTON1 && wNotifyCode == STN_CLICKED) {
		// Get the usr/passwd
		GetWindowText(g_hEWndUser, usr, 128);
		GetWindowText(g_hEWndPasswd, passwd, 128);

		WCHARTOCHAR(usr, 512, usr_ascii);
		WCHARTOCHAR(passwd, 512, passwd_ascii);

		if (!strlen(usr_ascii) || !strlen(passwd_ascii)) {
			MessageBox(0, L"Empty", L"Warning", 0);
			return 0;
		}

		for (unsigned int i = 0; i < strlen(usr_ascii); i++) {
			char hibyte = int2char(HI_BYTE(usr_ascii[i]));
			char lobyte = int2char(LO_BYTE(usr_ascii[i]));
			int j = (HI_BYTE(usr_ascii[i]) + i) % 0x11;
			short serial = 0;

			serial = (hibyte ^ g_High[j]) << 8;
			serial |= lobyte ^ g_Low[j];

			sprintf_s(serial_result + 4 * i, 512 - 4 * i, "%04x", serial);
		}

		char* p_head = serial_result;
		char* p_tail = serial_result + strlen(serial_result) - 1;

		while (p_head < p_tail) {
			*p_head ^= *p_tail;
			*p_tail ^= *p_head;
			*p_head ^= *p_tail;

			p_head++;
			p_tail--;
		}

		// Check passwords
		if (strcmp(serial_result, passwd_ascii)) {
			MessageBox(hwnd, TEXT("Not equal"),
				TEXT("Warning"), MB_ICONINFORMATION | MB_OK);
			PostQuitMessage(0);
			//CloseWindow(hwnd);
			DestroyWindow(hwnd);
			SelfDelete();
			ExitProcess(0);
			return 0;
		} else {
			PostQuitMessage(0);
			//CloseWindow(hwnd);
			DestroyWindow(hwnd);
		}
	}

	if (wID == ID_BUTTON2 && wNotifyCode == STN_CLICKED) {
		ExitProcess(0);
	}

	return 0;
}

LRESULT CALLBACK CDoor::
WindowProc(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam) {
	CDoor door_obj;
	switch (uMsg) {
	case WM_CREATE:
		return door_obj.Wm_CreateProc(hWnd, (LPCREATESTRUCT)lParam);
	case WM_COMMAND:
		return door_obj.Wm_CommandProc(hWnd, HIWORD(wParam), LOWORD(wParam), (HWND)lParam);
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

BOOL CDoor::
SelfDelete(void) {
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
