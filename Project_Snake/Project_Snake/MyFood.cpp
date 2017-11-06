#include "MyFood.h"
#include "MyDraw.h"

const char food_str[][3] = { "¡ï", "¦·", "$", "¡ñ", "¡÷", "¡Þ", "¡Æ" };

CMyFood::CMyFood() {
}


CMyFood::~CMyFood() {
}

int CMyFood::m_food_count = 0;

void CMyFood::RandomFood() {
	for (int i = 0; i < 2; i++) {
		int nX = rand() % (MAPCOL - 3) + 2;
		int nY = rand() % (MAPROW - 3) + 2;

		while (CMyMap::m_gMap[nY][nX] != 0) {
			nX += 3;
			nY += 3;
		}

		if (nX < MAPCOL / 4 && nY < MAPROW / 2) {
			CMyMap::m_gMap[nY][nX] = FOOD_STAR;
		} else if (nX < MAPCOL / 4) {
			CMyMap::m_gMap[nY][nX] = FOOD_STORM;
		} else if (nX < MAPCOL / 2 && nY < MAPROW / 2) {
			CMyMap::m_gMap[nY][nX] = FOOD_ADD_BLOOD;
		} else if (nX < MAPCOL / 2) {
			CMyMap::m_gMap[nY][nX] = FOOD_CHANGE_HEAD;
		} else if (nY < MAPROW / 4) {
			CMyMap::m_gMap[nY][nX] = FOOD_PROTECT_HEAD;
		} else if (nY < MAPCOL / 2) {
			CMyMap::m_gMap[nY][nX] = FOOD_ADD_TWO_NODE;
		} else {
			CMyMap::m_gMap[nY][nX] = FOOD_CHANGE_COLOR;
		}

		m_food_count++;
	}
}

bool CMyFood::IsFoodEmpty() {
	if (m_food_count)
		return false;
	return true;
}

void CMyFood::DrawFood() {
	CMyDraw food_obj;

	for (int i = 0; i < MAPROW; i++) {
		for (int j = 0; j < MAPCOL; j++) {
			switch (CMyMap::m_gMap[i][j]) {
			case FOOD_STAR:
				food_obj.WriteChar(j, i, (char*)food_str[0], _COLOR_STAR);
				break;
			case FOOD_STORM:
				food_obj.WriteChar(j, i, (char*)food_str[1], _COLOR_STORM);
				break;
			case FOOD_ADD_BLOOD:
				food_obj.WriteChar(j, i, (char*)food_str[2], _COLOR_ADD_BLOOD);
				break;
			case FOOD_CHANGE_HEAD:
				food_obj.WriteChar(j, i, (char*)food_str[3], _COLOR_CHANGE_HEAD);
				break;
			case FOOD_PROTECT_HEAD:
				food_obj.WriteChar(j, i, (char*)food_str[4], _COLOR_PROTECT_HEAD);
				break;
			case FOOD_ADD_TWO_NODE:
				food_obj.WriteChar(j, i, (char*)food_str[5], _COLOR_ADD_TWO_NODE);
				break;
			case FOOD_CHANGE_COLOR:
				food_obj.WriteChar(j, i, (char*)food_str[6], _COLOR_CHANGE_COLOR);
				break;
			default:
				break;
			}
		}
	}
}
