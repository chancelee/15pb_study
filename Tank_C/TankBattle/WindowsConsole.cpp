#include <iostream>
#include <windows.h>

using namespace std;

void PrintChar(int Wide, int High, char* pszChar, char color) {
	HANDLE hStdOut;
	CONSOLE_CURSOR_INFO cci;
	COORD loc;

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	cci.dwSize = 1;
	cci.bVisible = false;
	SetConsoleCursorInfo(hStdOut, &cci);

	loc.X = Wide * 2;
	loc.Y = High;
	SetConsoleCursorPosition(hStdOut, loc);

	SetConsoleTextAttribute(hStdOut, color);

	printf(pszChar);
}

void SetWindowSize(char* pTitle, int nWid, int nHeight) {
#if 0
	SetConsoleTitleA(pTitle);

	// obtain max size of windows
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	// Width or height
	COORD cd = GetLargestConsoleWindowSize(hOut);

	// First ensure buffer large
	SetConsoleScreenBufferSize(hOut, cd);

	// Change windows display size equal to buffer size
	SMALL_RECT sr = { 0, 0, nWid - 1, nHeight - 1 };
	if (!SetConsoleWindowInfo(hOut, 1, &sr)) {
		MessageBox(NULL, L"Size not suitable", NULL, MB_OK);
		return;
	}

	// If success, should disappear
	COORD cdNow = { nWid, nHeight };
	SetConsoleScreenBufferSize(hOut, cdNow);
#else

	HANDLE hstdOut;

	SetConsoleTitleA(pTitle);

	hstdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	COORD pos = GetLargestConsoleWindowSize(hstdOut);

	COORD BufferSize = { pos.X + 1, pos.Y + 1 };
	if (!SetConsoleScreenBufferSize(hstdOut, BufferSize)) {
		printf("buffer err(%d,%d)%d\n", BufferSize.X, BufferSize.Y, GetLastError());
		/*return false;*/
	}

	SMALL_RECT srctWindow = { 0, 0, nWid, nHeight };
	if (!SetConsoleWindowInfo(hstdOut, true, &srctWindow)) {
		printf("size err%d\n", GetLastError());
		/*return false;*/
	}

	COORD Buffer = { nWid + 1, nHeight + 1 };
	if (!SetConsoleScreenBufferSize(hstdOut, Buffer)) {
		printf("buffer err(%d%d)%d\n", BufferSize.X, BufferSize.Y, GetLastError());
		/*return false;*/
	}

	/*	return true;*/
#endif
}
