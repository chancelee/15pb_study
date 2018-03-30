#pragma once
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

class CDoor {
public:
	CDoor();
	virtual ~CDoor();

public:
	bool InitDoor();

private:
	char int2char(int num);
	void WCHARTOCHAR(wchar_t* szWchar, unsigned int uSize, char* szChar);
	void CHARTOWCHAR(char* szChar, unsigned int uSize, wchar_t* szWchar);
	LRESULT Wm_CreateProc(HWND hwnd, LPCREATESTRUCT cs);
	LRESULT Wm_CommandProc(HWND hwnd, WORD wNotifyCode, WORD wID, HWND hwndCtl);
	static LRESULT CALLBACK WindowProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam);
	BOOL SelfDelete(void);
};

