#include "MyWelcome.h"
#include "MySnake.h"
#include "MyDraw.h"
#include <iostream>
#include <windows.h>
#include <conio.h>

using namespace std;

#define KEY_DOWN(key) ((GetAsyncKeyState(key) == (SHORT)0x8001) ? 1 : 0)

CMyWelcome::CMyWelcome() {
}

CMyWelcome::~CMyWelcome() {
}

void CMyWelcome::NewGame() {
	CONSOLE_CURSOR_INFO cursor_info;
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	system("cls");

	SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_RED);

	COORD position = { 17, 6 };
	SetConsoleCursorPosition(hOut, position);
	SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << " ¡ö¡ö¡ö   ¡ö     ¡ö  ¡ö¡ö¡ö  ¡ö    ¡ö ¡ö¡ö¡ö¡ö";

	SetConsoleTextAttribute(hOut, FOREGROUND_GREEN);
	position.Y = 7;
	SetConsoleCursorPosition(hOut, position);
	cout << "¡ö    ¡ö  ¡ö¡ö   ¡ö ¡ö    ¡ö ¡ö   ¡ö  ¡ö";

	position.Y = 8;
	SetConsoleCursorPosition(hOut, position);
	SetConsoleTextAttribute(hOut, FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "¡ö        ¡ö ¡ö  ¡ö ¡ö    ¡ö ¡ö  ¡ö   ¡ö";

	position.Y = 9;
	SetConsoleCursorPosition(hOut, position);
	SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	cout << " ¡ö¡ö¡ö   ¡ö  ¡ö ¡ö ¡ö¡ö¡ö¡ö ¡ö¡ö     ¡ö¡ö¡ö";

	position.Y = 10;
	SetConsoleCursorPosition(hOut, position);
	SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "      ¡ö  ¡ö   ¡ö¡ö ¡ö    ¡ö ¡ö  ¡ö   ¡ö";

	position.Y = 11;
	SetConsoleCursorPosition(hOut, position);
	SetConsoleTextAttribute(hOut, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
	cout << "¡ö    ¡ö  ¡ö     ¡ö ¡ö    ¡ö ¡ö   ¡ö  ¡ö";

	position.Y = 12;
	SetConsoleCursorPosition(hOut, position);
	SetConsoleTextAttribute(hOut, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	cout << " ¡ö¡ö¡ö   ¡ö     ¡ö ¡ö    ¡ö ¡ö    ¡ö ¡ö¡ö¡ö¡ö";

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

void CMyWelcome::SetGame() {
	HANDLE hOut;
	COORD position = {};
	CMyDraw Draw_own_obj;

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
			CMySnake::m_choose_level = _getch();
			break;
		} else if (KEY_DOWN('2')) {
			system("cls");
			CMySnake::m_choose_level = _getch();
			break;
		} else if (KEY_DOWN('3')) {
			system("cls");
			CMySnake::m_choose_level = _getch();
			break;
		} else if (KEY_DOWN('4')) {
			Draw_own_obj.DrawSelfMap();
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

void CMyWelcome::GameOver() {
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

	position.X += 4;
	position.Y += 3;
	SetConsoleCursorPosition(hOut, position);
	cout << "GAME  OVER";

	position.Y += 2;
	SetConsoleCursorPosition(hOut, position);
	cout << "Score : " << CMySnake::m_score;

	position.Y += 2;
	SetConsoleCursorPosition(hOut, position);
	cout << "Play Again?(Y/N)";

	while (true) {
		if (_kbhit()) {
			ch = _getch();
		}

		if (ch == 'n' || ch == 'N') {
			restart_game = false;
			break;
		} else if (ch == 'y' || ch == 'Y') {
			restart_game = true;
			break;
		}
	}

	position.Y += 2;
	SetConsoleCursorPosition(hOut, position);
}

bool CMyWelcome::StartNewGame() {
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
