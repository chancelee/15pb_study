#include "StupidSniff.h"
#include "map.h"

#define MAX(A,B)	((A) > (B) ? (A) : (B))
/* TBD, Reginald.S */
int StupidSniff(PTANK ptank_enermy, int sensibility) {
	for (int nX = MAX(ptank_enermy->m_tank_nX - sensibility, 0); nX < ptank_enermy->m_tank_nX; nX++) {
		for (int nY = MAX(ptank_enermy->m_tank_nY - sensibility, 0); nY < ptank_enermy->m_tank_nY; nY++) {
			if (m_gMap[nY][nX] == WALL_TANK) {
				return (TEND_LEFT | TEND_UP);
			}
		}
	}

	for (int nX = ptank_enermy->m_tank_nX; nX < MAX(ptank_enermy->m_tank_nX + sensibility, MAPCOL); nX++) {
		for (int nY = MAX(ptank_enermy->m_tank_nY - sensibility, 0); nY < ptank_enermy->m_tank_nY; nY++) {
			if (m_gMap[nY][nX] == WALL_TANK) {
				return (TEND_RIGHT | TEND_UP);
			}
		}
	}

	for (int nX = MAX(ptank_enermy->m_tank_nX - sensibility, 0); nX < ptank_enermy->m_tank_nX; nX++) {
		for (int nY = ptank_enermy->m_tank_nY; nY < MAX(ptank_enermy->m_tank_nY + sensibility, MAPROW); nY++) {
			if (m_gMap[nY][nX] == WALL_TANK) {
				return (TEND_LEFT | TEND_DOWN);
			}
		}
	}

	for (int nX = ptank_enermy->m_tank_nX; nX < MAX(ptank_enermy->m_tank_nX + sensibility, MAPCOL); nX++) {
		for (int nY = ptank_enermy->m_tank_nY; nY < MAX(ptank_enermy->m_tank_nY + sensibility, MAPROW); nY++) {
			if (m_gMap[nY][nX] == WALL_TANK) {
				return (TEND_RIGHT | TEND_DOWN);
			}
		}
	}

	return 0;
}
