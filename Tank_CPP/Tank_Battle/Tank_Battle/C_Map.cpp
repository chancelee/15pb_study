#include "C_Map.h"
#include "ConsoleOperate.h"
#include "Tank.h"
#include <time.h>
#include <stdio.h>

const char bound_str[][3] = { "¡ù", "¡þ", "¡ö", "¨ˆ" };

int C_Map::m_gMap[MAPROW][MAPCOL] = {};

C_Map::C_Map() {
}

C_Map::~C_Map() {
}

void C_Map::InitMap(bool inner_wall) {
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

void C_Map::ClearMap() {
	/* In such occasion, read from file, already storage in buffer, but, when
	* we start a new game, we should abandon such record
	*/
	for (int i = 0; i < MAPROW; i++) {
		for (int j = 0; j < MAPCOL; j++) {
			m_gMap[i][j] = 0;
		}
	}
}

void C_Map::StormComing() {
	CConsoleOperate console_obj;

	// random wall in map
	for (int i = 0; i < 10; i++) {
		int nX = rand() % MAPCOL;
		int nY = rand() % MAPROW;
		if (!m_gMap[nY][nX]) {
			m_gMap[nY][nX] = WALL_INNER_BOUND;
			console_obj.PrintChar(nX, nY, (char*)bound_str[0], COLOR_INNER_BOUND);
		}
	}
}

void C_Map::DrawMap() {
	CConsoleOperate console_obj;

	for (int i = 0; i < MAPROW; i++) {
		for (int j = 0; j < MAPCOL; j++) {
			if (m_gMap[i][j] == WALL_BOUND_THREE) {
				console_obj.PrintChar(j, i, (char*)bound_str[2], COLOR_BOUND);
			} else if (m_gMap[i][j] == WALL_BOUND_TWO) {
				console_obj.PrintChar(j, i, (char*)bound_str[1], COLOR_BOUND);
			} else if (m_gMap[i][j] == WALL_BOUND_ONE) {
				console_obj.PrintChar(j, i, (char*)bound_str[0], COLOR_BOUND);
			} else if (m_gMap[i][j] == WALL_INNER_BOUND) {
				console_obj.PrintChar(j, i, (char*)bound_str[0], COLOR_INNER_BOUND);
			} else if (m_gMap[i][j] == WALL_BOUND_DAMAGE_FOUR) {
				console_obj.PrintChar(j, i, (char*)bound_str[3], COLOR_DAMAGE_BOUND);
			}
		}
	}

	DrawPanel();
}

void C_Map::DrawPanel() {
	char panel_blood[20] = {};
	char panel_score[20] = {};
	CConsoleOperate console_obj;

	sprintf_s(panel_blood, sizeof(panel_blood), "Blood: %5d", CTank::m_tank_blood);
	if (CTank::m_tank_blood < 10) {
		console_obj.PrintChar(45, 10, panel_blood, FOREGROUND_RED);
	} else {
		console_obj.PrintChar(45, 10, panel_blood, FOREGROUND_GREEN);
	}

	sprintf_s(panel_score, sizeof(panel_score), "Score: %5d", CTank::m_tank_score);
	console_obj.PrintChar(45, 20, panel_score, FOREGROUND_GREEN);
}
