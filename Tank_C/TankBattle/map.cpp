#include "map.h"
#include "WindowsConsole.h"
#include <time.h>
#include <stdio.h>

int m_gMap[MAPROW][MAPCOL] = {};

const char bound_str[][3] = { "¡ù", "¡þ", "¡ö", "¨ˆ" };

void InitMap(bool inner_wall) {
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
}

void ClearMap() {
	/* In such occasion, read from file, already storage in buffer, but, when
	* we start a new game, we should abandon such record
	*/
	for (int i = 0; i < MAPROW; i++) {
		for (int j = 0; j < MAPCOL; j++) {
			m_gMap[i][j] = 0;
		}
	}
}

void StormComing() {
	// random wall in map
	for (int i = 0; i < 10; i++) {
		int nX = rand() % MAPCOL;
		int nY = rand() % MAPROW;
		if (!m_gMap[nY][nX]) {
			m_gMap[nY][nX] = WALL_INNER_BOUND;
			PrintChar(nX, nY, (char*)bound_str[0], COLOR_INNER_BOUND);
		}
	}
}


void DrawPanel() {
	int m_time_blood = 100;
	char panel_blood[20] = {};
	char panel_score[20] = {};

	sprintf_s(panel_blood, sizeof(panel_blood), "Blood: %5d", m_time_blood);
	if (m_time_blood < 10) {
		PrintChar(45, 10, panel_blood, FOREGROUND_RED);
	}
	else {
		PrintChar(45, 10, panel_blood, FOREGROUND_GREEN);
	}

	sprintf_s(panel_score, sizeof(panel_score), "Score: %5d", 10);
	PrintChar(45, 20, panel_score, FOREGROUND_GREEN);
}

void DrawMap() {
	for (int i = 0; i < MAPROW; i++) {
		for (int j = 0; j < MAPCOL; j++) {
			if (m_gMap[i][j] == WALL_BOUND_THREE) {
				PrintChar(j, i, (char*)bound_str[2], COLOR_BOUND);
			} else if (m_gMap[i][j] == WALL_BOUND_TWO) {
				PrintChar(j, i, (char*)bound_str[1], COLOR_BOUND);
			} else if (m_gMap[i][j] == WALL_BOUND_ONE) {
				PrintChar(j, i, (char*)bound_str[0], COLOR_BOUND);
			} else if (m_gMap[i][j] == WALL_INNER_BOUND) {
				PrintChar(j, i, (char*)bound_str[0], COLOR_INNER_BOUND);
			} else if (m_gMap[i][j] == WALL_BOUND_DAMAGE_FOUR) {
				PrintChar(j, i, (char*)bound_str[3], COLOR_DAMAGE_BOUND);
			}
		}
	}

	DrawPanel();
}
