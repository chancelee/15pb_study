#include "WindowsConsole.h"
#include "map.h"
#include <windows.h>
#include <stdio.h>

static int m_wall_index = 4;

static void DrawInterface() {
	for (int i = 0; i < 60; i++) {
		for (int j = 0; j < SCREEN_HEIGHT; j++) {
			PrintChar(i, j, "  ", FOREGROUND_GREEN);
			if (i < MAPROW) {
				m_gMap[i][j] = 0;
			}
		}
	}

	for (int i = 0; i < 60; i++) {
		PrintChar(i, 0, "��", FOREGROUND_GREEN);
		PrintChar(i, SCREEN_HEIGHT - 1, "��", FOREGROUND_GREEN);
		if (i < MAPCOL) {
			m_gMap[0][i] = WALL_BOUND_ONE;
			m_gMap[MAPROW - 1][i] = WALL_BOUND_ONE;
		}
	}

	for (int i = 0; i < MAPROW; i++) {
		PrintChar(0, i, "��", FOREGROUND_GREEN);
		PrintChar(MAPCOL - 1, i, "��", FOREGROUND_GREEN);
		PrintChar(60, i, "��", FOREGROUND_GREEN);
		m_gMap[i][0] = WALL_BOUND_ONE;
		m_gMap[i][MAPCOL - 1] = WALL_BOUND_ONE;
	}

	PrintChar(45, 5, "������������", FOREGROUND_GREEN);
	PrintChar(45, 6, "������������", FOREGROUND_GREEN);
	PrintChar(45, 14, "������������", FOREGROUND_GREEN);
	PrintChar(45, 15, "������������", FOREGROUND_GREEN);
	PrintChar(45, 22, "������������", FOREGROUND_GREEN);
	PrintChar(45, 23, "������������", FOREGROUND_GREEN);
	PrintChar(45, 30, "������������", FOREGROUND_GREEN);
	PrintChar(45, 31, "������������", FOREGROUND_GREEN);
}

static void OutputPos(int nX, int nY) {
	char szBuf[100] = { 0 };

	sprintf_s(szBuf, sizeof(szBuf), "(%2d,%2d)", nX / 2, nY);

	SetConsoleTitleA(szBuf);

	PrintChar(0, 0, szBuf, 0x50);
}

static bool MouseMove(MOUSE_EVENT_RECORD &stcMouse) {
	if (stcMouse.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
		if (m_wall_index < 4 &&
			stcMouse.dwMousePosition.X < (MAPROW * 2 - 1) &&
			stcMouse.dwMousePosition.Y < MAPCOL) {
			PrintChar(stcMouse.dwMousePosition.X / 2,
				stcMouse.dwMousePosition.Y,
				(char*)bound_str[m_wall_index],
				FOREGROUND_GREEN);
			m_gMap[stcMouse.dwMousePosition.Y][stcMouse.dwMousePosition.X / 2] = WALL_BOUND_ONE + m_wall_index;
		}
	}

	return true;
}

static bool MouseClick(MOUSE_EVENT_RECORD & stcMouse) {
	int x = stcMouse.dwMousePosition.X;
	int y = stcMouse.dwMousePosition.Y;

	if (x >= 79) {
		if (x >= 90) {
			if (5 == y || 6 == y) {
				m_wall_index = 0;//��
			} else if (14 == y || 15 == y) {
				m_wall_index = 1;//��
			} else if (22 == y || 23 == y) {
				m_wall_index = 2;//��
			} else if (30 == y || 31 == y) {
				m_wall_index = 3;//��
			} else {
				m_wall_index = 4;
			}
		} else {
			m_wall_index = 4;
		}
	}

	return false;
}

static bool MouseEventProc(MOUSE_EVENT_RECORD stcMouse) {
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


bool DrawSelfMap() {
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	INPUT_RECORD stcRecord = {};
	DWORD dwRead = 0;

	SetWindowSize("Draw", SCREEN_WEIGHT, SCREEN_HEIGHT);

	DrawInterface();

	SetConsoleMode(hInput, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);

	while (true) {
		ReadConsoleInput(hInput, &stcRecord, 1, &dwRead);

		if (stcRecord.EventType == MOUSE_EVENT) {
			if (MouseEventProc(stcRecord.Event.MouseEvent)) {
				// When exit, should add follow sentence, otherwise, when view changed, color still pink(0x50)
				PrintChar(0, 0, " ", 0);
				return true;
			}
		}
	}

	return false;
}
