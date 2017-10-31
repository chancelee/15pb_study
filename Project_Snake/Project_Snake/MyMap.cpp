#include "MyMap.h"
#include "MyDraw.h"
#include "MyFood.h"
#include <time.h>

int CMyMap::m_gMap[MAPROW][MAPCOL] = {};
int CMyMap::m_gMap_storage[MAPROW][MAPCOL] = {};

CMyMap::CMyMap() {
}


CMyMap::~CMyMap() {
}

void CMyMap::SetWindowSize(char* pTitle, int nWid, int nHeight) {
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
}

void CMyMap::InitMap() {
	CMyFood food_obj;

	for (int i = 0; i < MAPROW; i++) {
		for (int j = 0; j < MAPCOL; j++) {
			if (i == 0 || i == MAPROW - 1 ||
				j == 0 || j == MAPCOL - 1) {
				m_gMap[i][j] = WALL_BOUND;
			}
		}
	}

	// random wall in map
	for (int i = 0; i < 10; i++) {
		int nX = rand() % (MAPCOL - 3) + 2;
		int nY = rand() % (MAPROW - 3) + 2;
		if (!m_gMap[nY][nX]) {
			m_gMap[nY][nX] = WALL_INNER_BOUND;
		}
	}

	m_gMap[25][25] = SNAKE_HOLE;

	DrawMap();

	food_obj.RandomFood();
	food_obj.DrawFood();
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
			if (m_gMap[i][j] == WALL_BOUND) {
				map_obj.WriteChar(j, i, (char*)bound_str[0], COLOR_BOUND);
			} else if (m_gMap[i][j] == WALL_INNER_BOUND) {
				map_obj.WriteChar(j, i, (char*)bound_str[0], COLOR_INNER_BOUND);
			} else if (m_gMap[i][j] == SNAKE_HOLE) {
				map_obj.WriteChar(j, i, (char*)bound_str[1], COLOR_SNAKE_HOLE);
			}
		}
	}
}