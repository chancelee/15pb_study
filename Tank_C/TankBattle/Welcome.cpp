#include <windows.h>
#include <iostream>
#include <conio.h>
#include "DrawOwnMap.h"

using namespace std;

#define KEY_DOWN(vk_code)	((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)		((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

extern bool g_restart_game;

void NewGame() {
	CONSOLE_CURSOR_INFO cursor_info;
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	system("cls");

	SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_RED);

	COORD position = { 17, 6 };
	SetConsoleCursorPosition(hOut, position);
	SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << " ¡ö¡ö¡ö¡ö¡ö      ¡ö        ¡ö     ¡ö    ¡ö  ¡ö";


	SetConsoleTextAttribute(hOut, FOREGROUND_GREEN);
	position.Y = 7;
	SetConsoleCursorPosition(hOut, position);
	cout << "     ¡ö        ¡ö  ¡ö      ¡ö   ¡ö¡ö    ¡ö¡ö";

	position.Y = 8;
	SetConsoleCursorPosition(hOut, position);
	SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "     ¡ö      ¡ö  ¡ö  ¡ö    ¡ö ¡ö  ¡ö    ¡ö¡ö";

	position.Y = 9;
	SetConsoleCursorPosition(hOut, position);
	SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << "     ¡ö     ¡ö        ¡ö   ¡ö     ¡ö    ¡ö  ¡ö";

	position.X = 27;
	position.Y = 20;
	SetConsoleCursorPosition(hOut, position);

	cursor_info.bVisible = FALSE;
	cursor_info.dwSize = 1;
	SetConsoleCursorInfo(hOut, &cursor_info);
	cout << "Press any key to start...";

	while (true) {
		if (_kbhit()) {
			_getch();
			break;
		}
	}
}

bool StartNewGame() {
	COORD position = { 27, 20 };
	CONSOLE_CURSOR_INFO cursor_info;
	char ch = 0;

	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleCursorPosition(hOut, position);

	cursor_info.bVisible = FALSE;
	cursor_info.dwSize = 1;
	SetConsoleCursorInfo(hOut, &cursor_info);
	cout << "Start A New Game(Y/N)?   ";

	while (true) {
		if (_kbhit()) {
			ch = _getch();
		}

		if (ch == 'n' || ch == 'N') {
			return false;
		} else if (ch == 'y' || ch == 'Y') {
			return true;
		}
	}
}

void SetGame() {
	HANDLE hOut;
	COORD position = {};
	char m_choose_level;

	system("cls");

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOut, position);
	SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << " ¡ö¡ö¡ö  ¡ö¡ö¡ö¡ö ¡ö¡ö¡ö ¡ö¡ö¡ö  ¡ö¡ö  ¡ö     ¡ö  ¡ö¡ö¡ö   ¡ö¡ö¡ö " << endl;

	SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << "¡ö    ¡ö ¡ö         ¡ö     ¡ö     ¡ö   ¡ö¡ö   ¡ö ¡ö    ¡ö ¡ö    ¡ö" << endl;

	SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "¡ö       ¡ö         ¡ö     ¡ö     ¡ö   ¡ö ¡ö  ¡ö ¡ö       ¡ö     " << endl;

	SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << " ¡ö¡ö¡ö  ¡ö¡ö¡ö     ¡ö     ¡ö     ¡ö   ¡ö  ¡ö ¡ö ¡ö  ¡ö¡ö  ¡ö¡ö¡ö " << endl;

	SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "      ¡ö ¡ö         ¡ö     ¡ö     ¡ö   ¡ö   ¡ö¡ö ¡ö    ¡ö       ¡ö" << endl;

	SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "¡ö    ¡ö ¡ö         ¡ö     ¡ö     ¡ö   ¡ö     ¡ö ¡ö    ¡ö ¡ö    ¡ö" << endl;

	SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	cout << " ¡ö¡ö¡ö  ¡ö¡ö¡ö¡ö   ¡ö     ¡ö    ¡ö¡ö  ¡ö     ¡ö  ¡ö¡ö¡ö   ¡ö¡ö¡ö " << endl;

	position.X = 9;
	position.Y = 15;
	SetConsoleCursorPosition(hOut, position);
	cout << "Chose difficulity : " << endl;
	cout << "                                           1) Easy" << endl
		<< "                                           2) Normal" << endl
		<< "                                           3) Hard" << endl
		<< "                                           4) Build u map" << endl;

	while (true) {
		if (KEY_DOWN('1')) {
			system("cls");
			m_choose_level = _getch();
			break;
		} else if (KEY_DOWN('2')) {
			system("cls");
			m_choose_level = _getch();
			break;
		} else if (KEY_DOWN('3')) {
			system("cls");
			m_choose_level = _getch();
			break;
		} else if (KEY_DOWN('4')) {
			DrawSelfMap();
			SetGame();
			// clear screen, return to fortune view
			break;
		} else {
			if (_kbhit()) {
				_getch();
			}
		}
	}
}

void GameOver() {
	char ch = 0;
	int x = 28;
	int y = 3;
	COORD position = { x, y };

	system("cls");
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	SetConsoleCursorPosition(hOut, position);
	cout << "       ¡ö¡ö¡ö¡ö¡ö";

	position.Y += 1;
	SetConsoleCursorPosition(hOut, position);
	cout << "    ¡ö¡ö¡ö¡ö¡ö¡ö¡ö¡ö ";

	position.Y += 1;
	SetConsoleCursorPosition(hOut, position);
	cout << "  ¡ö¡ö¡ö ¡ö¡ö¡ö ¡ö¡ö¡ö ";

	position.Y += 1;
	SetConsoleCursorPosition(hOut, position);
	cout << " ¡ö¡ö¡ö    ¡ö    ¡ö¡ö¡ö ";

	position.Y += 1;
	SetConsoleCursorPosition(hOut, position);
	cout << "  ¡ö¡ö¡ö  ¡ö¡ö  ¡ö¡ö¡ö";

	position.Y += 1;
	SetConsoleCursorPosition(hOut, position);
	cout << "   ¡ö¡ö¡ö¡ö¡ö¡ö¡ö¡ö¡ö";

	position.Y += 1;
	SetConsoleCursorPosition(hOut, position);
	cout << "     ¡ö¡ö¡ö¡ö¡ö¡ö¡ö";

	position.Y += 1;
	SetConsoleCursorPosition(hOut, position);
	cout << "     ¡ö¡ö ¡ö¡ö ¡ö¡ö";

	position.Y += 1;
	SetConsoleCursorPosition(hOut, position);
	cout << "                        ";

	position.Y += 1;
	SetConsoleCursorPosition(hOut, position);
	cout << "     ¡ö¡ö ¡ö¡ö ¡ö¡ö ";

	position.Y += 1;
	SetConsoleCursorPosition(hOut, position);
	cout << "      ¡ö¡ö¡ö¡ö¡ö¡ö";

	position.X += 6;
	position.Y += 3;
	SetConsoleCursorPosition(hOut, position);
	cout << "GAME  OVER";

	position.Y += 2;
	SetConsoleCursorPosition(hOut, position);
	cout << "Score : " << 10;

	position.Y += 2;
	SetConsoleCursorPosition(hOut, position);
	cout << "Play Again?(Y/N)";

	while (true) {
		if (_kbhit()) {
			ch = _getch();
		}

		if (ch == 'n' || ch == 'N') {
			g_restart_game = false;
			break;
		} else if (ch == 'y' || ch == 'Y') {
			g_restart_game = true;
			break;
		}
	}

	position.Y += 2;
	SetConsoleCursorPosition(hOut, position);
}
