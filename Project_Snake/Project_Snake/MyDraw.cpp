#include "MyDraw.h"


CMyDraw::CMyDraw() {
}


CMyDraw::~CMyDraw() {
}

void CMyDraw::WriteChar(int nX, int nY, char* pChar, WORD wAttr) {
	CONSOLE_CURSOR_INFO ci = {};
	COORD cd = { nX * 2, nY };

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	// Disappear cursor
	ci.bVisible = FALSE;
	ci.dwSize = 1;
	SetConsoleCursorInfo(hOut, &ci);

	// Set Cursor location
	SetConsoleCursorPosition(hOut, cd);

	// Set color
	SetConsoleTextAttribute(hOut, wAttr);

	printf("%s", pChar);
}
