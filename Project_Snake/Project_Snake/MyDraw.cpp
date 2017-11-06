#include "MyDraw.h"
#include "MyMap.h"
#include "MyGame.h"
#include <iostream>

using namespace std;

CMyDraw::CMyDraw() : m_wall_index(4){
}


CMyDraw::~CMyDraw() {
}

template<typename T>
void CMyDraw::WriteChar(int nX, int nY, T pChar, WORD wAttr) {
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

	cout << pChar;
}

bool CMyDraw::DrawSelfMap() {
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD stcRecord = {};
	DWORD dwRead = 0;
	CMyMap map_obj;

	//map_obj.SetWindowSize("Draw", SCREEN_WEIGHT, SCREEN_HEIGHT);

	DrawInterface();

	SetConsoleMode(hInput, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

	while (true) {
		ReadConsoleInput(hInput, &stcRecord, 1, &dwRead);

		if (stcRecord.EventType == MOUSE_EVENT) {
			if (MouseEventProc(stcRecord.Event.MouseEvent)) {
				// When exit, should add follow sentence, otherwise, when view changed, color still pink(0x50)
				WriteChar(0, 0, " ", 0);
				return true;
			}
		}
	}

	return 0;
}

void CMyDraw::DrawInterface() {
	for (int i = 0; i < 60; i++) {
		for (int j = 0; j < SCREEN_HEIGHT; j++) {
			WriteChar(i, j, "  ", FOREGROUND_GREEN);
			if (i < MAPROW) {
				CMyMap::m_gMap[i][j] = 0;
			}
		}
	}

	for (int i = 0; i < 60; i++) {
		WriteChar(i, 0, "¡ù", FOREGROUND_GREEN);
		WriteChar(i, SCREEN_HEIGHT - 1, "¡ù", FOREGROUND_GREEN);
		if (i < MAPCOL) {
			CMyMap::m_gMap[0][i] = WALL_BOUND_ONE;
			CMyMap::m_gMap[MAPROW - 1][i] = WALL_BOUND_ONE;
		}
	}

	for (int i = 0; i < MAPROW; i++) {
		WriteChar(0, i, "¡ù", FOREGROUND_GREEN);
		WriteChar(MAPCOL - 1, i, "¡ù", FOREGROUND_GREEN);
		WriteChar(60, i, "¡ù", FOREGROUND_GREEN);
		CMyMap::m_gMap[i][0] = WALL_BOUND_ONE;
		CMyMap::m_gMap[i][MAPCOL - 1] = WALL_BOUND_ONE;
	}

	WriteChar(45, 10, "¡ù¡ù¡ù¡ù¡ù¡ù", FOREGROUND_GREEN);
	WriteChar(45, 11, "¡ù¡ù¡ù¡ù¡ù¡ù", FOREGROUND_GREEN);
	WriteChar(45, 20, "¡þ¡þ¡þ¡þ¡þ¡þ", FOREGROUND_GREEN);
	WriteChar(45, 21, "¡þ¡þ¡þ¡þ¡þ¡þ", FOREGROUND_GREEN);
	WriteChar(45, 30, "¡ö¡ö¡ö¡ö¡ö¡ö", FOREGROUND_GREEN);
	WriteChar(45, 31, "¡ö¡ö¡ö¡ö¡ö¡ö", FOREGROUND_GREEN);
}

void CMyDraw::OutputPos(int nX, int nY) {
	char szBuf[100] = { 0 };

	sprintf_s(szBuf, sizeof(szBuf), "(%2d,%2d)", nX / 2, nY);

	SetConsoleTitleA(szBuf);

	WriteChar(0, 0, szBuf, 0x50);
}

bool CMyDraw::MouseEventProc(MOUSE_EVENT_RECORD stcMouse) {
	switch (stcMouse.dwEventFlags) {
	case 0:
		MouseClick(stcMouse);
		break;
	case DOUBLE_CLICK:
		return true;
	case MOUSE_MOVED:
		MouseMove(stcMouse);
		break;
	default:
		break;
	}

	OutputPos(stcMouse.dwMousePosition.X, stcMouse.dwMousePosition.Y);

	return false;
}

bool CMyDraw::MouseMove(MOUSE_EVENT_RECORD &stcMouse) {
	if (stcMouse.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
		if (m_wall_index < 4 && 
			stcMouse.dwMousePosition.X < (MAPROW*2 - 1) && 
			stcMouse.dwMousePosition.Y < MAPCOL) {
			WriteChar(stcMouse.dwMousePosition.X / 2, 
				stcMouse.dwMousePosition.Y, 
				bound_str[m_wall_index],
				FOREGROUND_GREEN);
			CMyMap::m_gMap[stcMouse.dwMousePosition.Y][stcMouse.dwMousePosition.X / 2] = WALL_BOUND_ONE + m_wall_index;
		}
	}

	return true;
}

bool CMyDraw::MouseClick(MOUSE_EVENT_RECORD & stcMouse) {
	int x = stcMouse.dwMousePosition.X;
	int y = stcMouse.dwMousePosition.Y;

	if (x >= 79) {
		if (x >= 90) {
			if (10 == y || 11 == y) {
				m_wall_index = 0;//¡ù
			} else if (20 == y || 21 == y) {
				m_wall_index = 1;//¡þ
			} else if (30 == y || 31 == y) {
				m_wall_index = 2;//¡ö
			} else {
				m_wall_index = 4;
			}
		} else {
			m_wall_index = 4;
		}
	}

	return false;
}
