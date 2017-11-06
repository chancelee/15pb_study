#include "MyMap.h"
#include "MyDraw.h"
#include "MyFood.h"
#include "MySnake.h"
#include <time.h>

int CMyMap::m_gMap[MAPROW][MAPCOL] = {};

const char bound_str[][3] = { "¡ù", "¡þ", "¡ö" };

CMyMap::CMyMap() {
}


CMyMap::~CMyMap() {
}

void CMyMap::SetWindowSize(char* pTitle, int nWid, int nHeight) {
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

void CMyMap::InitMap(bool inner_wall) {
	CMyFood food_obj;

	for (int i = 0; i < MAPROW; i++) {
		for (int j = 0; j < MAPCOL; j++) {
			if (i == 0 || i == MAPROW - 1 ||
				j == 0 || j == MAPCOL - 1) {
				if (!m_gMap[i][j]) {
					m_gMap[i][j] = WALL_BOUND_THREE;
				}
			}
		}
	}

	if (inner_wall) {
		// random wall in map
		for (int i = 0; i < 10; i++) {
			int nX = rand() % (MAPCOL - 3) + 2;
			int nY = rand() % (MAPROW - 3) + 2;
			if (!m_gMap[nY][nX]) {
				m_gMap[nY][nX] = WALL_INNER_BOUND;
			}
		}
	}

	DrawMap();

	if (!food_obj.m_food_count) {
		food_obj.RandomFood();
	}
	food_obj.DrawFood();
}

void CMyMap::ClearMap() {
	/* In such occasion, read from file, already storage in buffer, but, when 
	 * we start a new game, we should abandon such record
	 */
	for (int i = 0; i < MAPROW; i++) {
		for (int j = 0; j < MAPCOL; j++) {
			m_gMap[i][j] = 0;
		}
	}
}

void CMyMap::StormComing() {
	CMyDraw map_obj;

	// random wall in map
	for (int i = 0; i < 10; i++) {
		int nX = rand() % MAPCOL;
		int nY = rand() % MAPROW;
		if (!m_gMap[nY][nX]) {
			m_gMap[nY][nX] = WALL_INNER_BOUND;
			map_obj.WriteChar(nX, nY, (char*)bound_str[0], COLOR_INNER_BOUND);
		}
	}
}

void CMyMap::DrawMap() {
	CMyDraw map_obj;

	for (int i = 0; i < MAPROW; i++) {
		for (int j = 0; j < MAPCOL; j++) {
			if (m_gMap[i][j] == WALL_BOUND_THREE) {
				map_obj.WriteChar(j, i, (char*)bound_str[2], COLOR_BOUND);
			} else if (m_gMap[i][j] == WALL_BOUND_TWO) {
				map_obj.WriteChar(j, i, (char*)bound_str[1], COLOR_BOUND);
			} else if (m_gMap[i][j] == WALL_BOUND_ONE) {
				map_obj.WriteChar(j, i, (char*)bound_str[0], COLOR_BOUND);
			} else if (m_gMap[i][j] == WALL_INNER_BOUND) {
				map_obj.WriteChar(j, i, (char*)bound_str[0], COLOR_INNER_BOUND);
			}
		}
	}

	DrawPanel();
}

void CMyMap::DrawPanel() {
	CMyDraw map_obj;
	char panel_blood[20] = {};
	char panel_score[20] = {};

	sprintf_s(panel_blood, sizeof(panel_blood), "Blood: %5d", CMySnake::m_time_blood);
	if (CMySnake::m_time_blood < 10) {
		map_obj.WriteChar(45, 10, panel_blood, FOREGROUND_RED);
	}
	else {
		map_obj.WriteChar(45, 10, panel_blood, FOREGROUND_GREEN);
	}

	sprintf_s(panel_score, sizeof(panel_score), "Score: %5d", CMySnake::m_score);
	map_obj.WriteChar(45, 20, panel_score, FOREGROUND_GREEN);
}
