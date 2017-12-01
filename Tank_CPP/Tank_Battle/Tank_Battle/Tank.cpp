#include "Tank.h"
#include "ConsoleOperate.h"
#include "C_Map.h"
#include "bullet.h"
#include <stdio.h>
#include <windows.h>

#pragma comment(lib, "winmm.lib")

const char g_TankShape[4][3][3] = {
		// UP
		{
			{ 0, 1, 0 },
			{ 1, 1, 1 },
			{ 1, 0, 1 },
		},
		// DOWN
		{
			{ 1, 0, 1 },
			{ 1, 1, 1 },
			{ 0, 1, 0 },
		},
		// LEFT
		{
			{ 0, 1, 1 },
			{ 1, 1, 0 },
			{ 0, 1, 1 },
		},
		// RIGHT
		{
			{ 1, 1, 0 },
			{ 0, 1, 1 },
			{ 1, 1, 0 },
		}
};

int CTank::m_tank_blood = 10;
int CTank::m_tank_score = 0;

CTank::CTank() {
	m_tank_enermy_flag = false;
	m_is_tank_alive = true;
	m_dissapper_flag = false;
	m_user_engine = true;
	m_type = TANK;
	m_interval_move = TIME_INTERVAL_20;
	m_interval_move_storage = m_interval_move;
	m_interval_change_dst_dir = TIME_INTERVAL_20;
	m_interval_change_dst_dir_storage = m_interval_change_dst_dir;
	m_interval_auto_bullet = TIME_INTERVAL_100;
	m_dst_direction = DOWN;
}

CTank::~CTank() {
}

void CTank::InitTank(int center_nX, int center_nY, DIRECTION directions, char color) {
	m_tank_nX = center_nX;
	m_tank_nY = center_nY;
	m_tank_direction = directions;
	m_is_tank_alive = true;
	m_color = color;

	PrintTank();
}

bool CTank::ShouldTankAlive() {
	int nX, nY;

	for (int nPosY = m_tank_nY - 1; nPosY < m_tank_nY + 2; nPosY++) {
		for (int nPosX = m_tank_nX - 1; nPosX < m_tank_nX + 2; nPosX++) {
			nX = nPosX - m_tank_nX + 1;
			nY = nPosY - m_tank_nY + 1;
			if (g_TankShape[m_tank_direction][nY][nX] && C_Map::m_gMap[nPosY][nPosX] == WALL_DEAD_ZONE) {
				return false;
			}
		}
	}

	return true;
}

void CTank::DestroyTank() {
	int nX, nY;
	CConsoleOperate console_obj;

	if (!m_is_tank_alive)
		return;

	for (int nPosY = m_tank_nY - 1; nPosY < m_tank_nY + 2; nPosY++) {
		for (int nPosX = m_tank_nX - 1; nPosX < m_tank_nX + 2; nPosX++) {
			nX = nPosX - m_tank_nX + 1;
			nY = nPosY - m_tank_nY + 1;
			if (g_TankShape[m_tank_direction][nY][nX]) {
				C_Map::m_gMap[nPosY][nPosX] = 0;
				console_obj.PrintChar(nPosX, nPosY, "  ", 0x0F);
			}
		}
	}

	PlaySoundA("Tank_dead.wav", NULL, SND_ASYNC | SND_NODEFAULT);

	m_is_tank_alive = false;

	m_dissapper_flag = true;

#if 0
	if (m_tank_enermy_flag) {
		GenerateTank();
		m_p_record = this;
	}
#endif
}

bool CTank::TankCanMove(DIRECTION direction) {
	switch (direction) {
	case UP:
		/* UP */
		if (C_Map::m_gMap[m_tank_nY - 2][m_tank_nX] >= WALL_BOUND_ONE ||
			C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] >= WALL_BOUND_ONE ||
			C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] >= WALL_BOUND_ONE ||
			(m_tank_direction == LEFT && C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] >= WALL_BOUND_ONE) ||
			(m_tank_direction == RIGHT && C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] >= WALL_BOUND_ONE)) {
			return false;
		}

		if (m_tank_enermy_flag) {
			if (C_Map::m_gMap[m_tank_nY - 2][m_tank_nX] == WALL_TANK ||
				C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] == WALL_TANK ||
				C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] == WALL_TANK ||
				(m_tank_direction == LEFT && C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] == WALL_TANK) ||
				(m_tank_direction == RIGHT && C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] == WALL_TANK)) {
				return false;
			}

			if (C_Map::m_gMap[m_tank_nY - 2][m_tank_nX] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY - 2][m_tank_nX - 1] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY - 2][m_tank_nX + 1] == WALL_TANK_ENERMY)
				return false;
		} else {
			if (C_Map::m_gMap[m_tank_nY - 2][m_tank_nX] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] == WALL_TANK_ENERMY ||
				(m_tank_direction == LEFT && C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] == WALL_TANK_ENERMY) ||
				(m_tank_direction == RIGHT && C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] == WALL_TANK_ENERMY)) {
				return false;
			}
		}

		break;
	case DOWN:
		/* DOWN */
		if (C_Map::m_gMap[m_tank_nY + 2][m_tank_nX] >= WALL_BOUND_ONE ||
			C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] >= WALL_BOUND_ONE ||
			C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] >= WALL_BOUND_ONE ||
			(m_tank_direction == LEFT && C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] >= WALL_BOUND_ONE) ||
			(m_tank_direction == RIGHT && C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] >= WALL_BOUND_ONE)) {
			return false;
		}

		if (m_tank_enermy_flag) {
			if (C_Map::m_gMap[m_tank_nY + 2][m_tank_nX] == WALL_TANK ||
				C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] == WALL_TANK ||
				C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] == WALL_TANK ||
				(m_tank_direction == LEFT && C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] == WALL_TANK) ||
				(m_tank_direction == RIGHT && C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] == WALL_TANK)) {
				return false;
			}

			if (C_Map::m_gMap[m_tank_nY + 2][m_tank_nX] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY + 2][m_tank_nX - 1] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY + 2][m_tank_nX + 1] == WALL_TANK_ENERMY)
				return false;
		} else {
			if (C_Map::m_gMap[m_tank_nY + 2][m_tank_nX] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] == WALL_TANK_ENERMY ||
				(m_tank_direction == LEFT && C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] == WALL_TANK_ENERMY) ||
				(m_tank_direction == RIGHT && C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] == WALL_TANK_ENERMY)) {
				return false;
			}
		}

		break;
	case LEFT:
		/* LEFT */
		if (C_Map::m_gMap[m_tank_nY][m_tank_nX - 2] >= WALL_BOUND_ONE ||
			C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] >= WALL_BOUND_ONE ||
			C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] >= WALL_BOUND_ONE ||
			(m_tank_direction == UP &&
			(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] >= WALL_BOUND_ONE) &&
			(C_Map::m_gMap[m_tank_nY][m_tank_nX + 2] >= WALL_BOUND_ONE)) ||
			(m_tank_direction == DOWN &&
			(C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] >= WALL_BOUND_ONE) &&
			(C_Map::m_gMap[m_tank_nY][m_tank_nX + 2] >= WALL_BOUND_ONE))) {
			return false;
		}

		if (m_tank_enermy_flag) {
			if (C_Map::m_gMap[m_tank_nY][m_tank_nX - 2] == WALL_TANK ||
				C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] == WALL_TANK ||
				C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] == WALL_TANK ||
				(m_tank_direction == UP &&
				(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] == WALL_TANK) &&
				(C_Map::m_gMap[m_tank_nY][m_tank_nX + 2] == WALL_TANK)) ||
				(m_tank_direction == DOWN &&
				(C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] == WALL_TANK) &&
				(C_Map::m_gMap[m_tank_nY][m_tank_nX + 2] == WALL_TANK))) {
				return false;
			}

			if (C_Map::m_gMap[m_tank_nY][m_tank_nX - 2] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 2] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 2] == WALL_TANK_ENERMY)
				return false;
		} else {
			if (C_Map::m_gMap[m_tank_nY][m_tank_nX - 2] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] == WALL_TANK_ENERMY ||
				(m_tank_direction == UP &&
				(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] == WALL_TANK_ENERMY) &&
				(C_Map::m_gMap[m_tank_nY][m_tank_nX + 2] == WALL_TANK_ENERMY)) ||
				(m_tank_direction == DOWN &&
				(C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] == WALL_TANK_ENERMY) &&
				(C_Map::m_gMap[m_tank_nY][m_tank_nX + 2] == WALL_TANK_ENERMY))) {
				return false;
			}
		}

		break;
	case RIGHT:
		/* RIGHT */
		if (C_Map::m_gMap[m_tank_nY][m_tank_nX + 2] >= WALL_BOUND_ONE ||
			C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] >= WALL_BOUND_ONE ||
			C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] >= WALL_BOUND_ONE ||
			(m_tank_direction == UP &&
			(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] >= WALL_BOUND_ONE) &&
			(C_Map::m_gMap[m_tank_nY][m_tank_nX - 2] >= WALL_BOUND_ONE)) ||
			(m_tank_direction == DOWN &&
			(C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] >= WALL_BOUND_ONE) &&
			(C_Map::m_gMap[m_tank_nY][m_tank_nX - 2] >= WALL_BOUND_ONE))) {
			return false;
		}

		if (m_tank_enermy_flag) {
			if (C_Map::m_gMap[m_tank_nY][m_tank_nX + 2] == WALL_TANK ||
				C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] == WALL_TANK ||
				C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] == WALL_TANK ||
				(m_tank_direction == UP &&
				(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] == WALL_TANK) &&
				(C_Map::m_gMap[m_tank_nY][m_tank_nX - 2] == WALL_TANK)) ||
				(m_tank_direction == DOWN &&
				(C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] == WALL_TANK) &&
				(C_Map::m_gMap[m_tank_nY][m_tank_nX - 2] == WALL_TANK))) {
				return false;
			}

			if (C_Map::m_gMap[m_tank_nY][m_tank_nX + 2] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 2] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 2] == WALL_TANK_ENERMY)
				return false;
		} else {
			if (C_Map::m_gMap[m_tank_nY][m_tank_nX + 2] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] == WALL_TANK_ENERMY ||
				(m_tank_direction == UP &&
				(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] == WALL_TANK_ENERMY) &&
				(C_Map::m_gMap[m_tank_nY][m_tank_nX - 2] == WALL_TANK_ENERMY)) ||
				(m_tank_direction == DOWN &&
				(C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] == WALL_TANK_ENERMY) &&
				(C_Map::m_gMap[m_tank_nY][m_tank_nX - 2] == WALL_TANK_ENERMY))) {
				return false;
			}
		}

		break;
	default:
		break;
	}

	return true;
}

void CTank::EraseTank() {
	int nX, nY;
	CConsoleOperate console_obj;

	for (int nPosY = m_tank_nY - 1; nPosY < m_tank_nY + 2; nPosY++) {
		for (int nPosX = m_tank_nX - 1; nPosX < m_tank_nX + 2; nPosX++) {
			nX = nPosX - m_tank_nX + 1;
			nY = nPosY - m_tank_nY + 1;
			if (g_TankShape[m_tank_direction][nY][nX]) {
				C_Map::m_gMap[nPosY][nPosX] = 0;
				console_obj.PrintChar(nPosX, nPosY, "  ", 0x0F);
			}
		}
	}
}

bool CTank::Move(DIRECTION direction) {
	if (!ShouldTankAlive()) {
		DestroyTank();
	}

	if (!m_is_tank_alive) {
		return false;
	}

	if (!TankCanMove(direction)) {
		return false;
	}

	EraseTank();

	switch (direction) {
	case UP:
		/* UP */
		if (m_tank_direction == DOWN &&
			(C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] >= WALL_BOUND_ONE ||
			C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] >= WALL_BOUND_ONE)) {
			m_tank_nY--;
			m_tank_direction = UP;
			break;
		}

		if (m_tank_enermy_flag) {
			if (m_tank_direction == DOWN &&
				(C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] == WALL_TANK ||
				C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] == WALL_TANK)) {
				m_tank_nY--;
				m_tank_direction = UP;
				break;
			}
		} else {
			if (m_tank_direction == DOWN &&
				(C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] == WALL_TANK_ENERMY)) {
				m_tank_nY--;
				m_tank_direction = UP;
				break;
			}
		}

		if (m_tank_direction != UP) {
			m_tank_direction = UP;
		} else {
			m_tank_nY--;
		}
		break;
	case DOWN:
		/* DOWN */
		if (m_tank_direction == UP &&
			(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] >= WALL_BOUND_ONE ||
			C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] >= WALL_BOUND_ONE)) {
			m_tank_nY++;
			m_tank_direction = DOWN;
			break;
		}

		if (m_tank_enermy_flag) {
			if (m_tank_direction == UP &&
				(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] == WALL_TANK ||
				C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] == WALL_TANK)) {
				m_tank_nY++;
				m_tank_direction = DOWN;
				break;
			}
		} else {
			if (m_tank_direction == UP &&
				(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] == WALL_TANK_ENERMY)) {
				m_tank_nY++;
				m_tank_direction = DOWN;
				break;
			}
		}

		if (m_tank_direction != DOWN) {
			m_tank_direction = DOWN;
		} else {
			m_tank_nY++;
		}
		break;
	case LEFT:
		/* LEFT */
		if (m_tank_direction == UP &&
			(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] >= WALL_BOUND_ONE)) {
			m_tank_nX++;
			m_tank_nY++;
			m_tank_direction = LEFT;
			break;
		}

		if (m_tank_direction == DOWN &&
			(C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] >= WALL_BOUND_ONE)) {
			m_tank_nX++;
			m_tank_nY--;
			m_tank_direction = LEFT;
			break;
		}

		if (m_tank_direction == RIGHT &&
			(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] >= WALL_BOUND_ONE ||
			C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] >= WALL_BOUND_ONE)) {
			m_tank_nX--;
			m_tank_direction = LEFT;
			break;
		}

		if (m_tank_enermy_flag) {
			if (m_tank_direction == UP &&
				(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] == WALL_TANK)) {
				m_tank_nX++;
				m_tank_nY++;
				m_tank_direction = LEFT;
				break;
			}

			if (m_tank_direction == DOWN &&
				(C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] == WALL_TANK)) {
				m_tank_nX++;
				m_tank_nY--;
				m_tank_direction = LEFT;
				break;
			}

			if (m_tank_direction == RIGHT &&
				(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] == WALL_TANK ||
				C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] == WALL_TANK)) {
				m_tank_nX--;
				m_tank_direction = LEFT;
				break;
			}
		} else {
			if (m_tank_direction == UP &&
				(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] == WALL_TANK_ENERMY)) {
				m_tank_nX++;
				m_tank_nY++;
				m_tank_direction = LEFT;
				break;
			}

			if (m_tank_direction == DOWN &&
				(C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] == WALL_TANK_ENERMY)) {
				m_tank_nX++;
				m_tank_nY--;
				m_tank_direction = LEFT;
				break;
			}

			if (m_tank_direction == RIGHT &&
				(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX + 1] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY + 1][m_tank_nX + 1] == WALL_TANK_ENERMY)) {
				m_tank_nX--;
				m_tank_direction = LEFT;
				break;
			}
		}

		if (m_tank_direction != LEFT) {
			m_tank_direction = LEFT;
		} else {
			m_tank_nX--;
		}
		break;
	case RIGHT:
		/* RIGHT */
		if (m_tank_direction == UP &&
			(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] >= WALL_BOUND_ONE)) {
			m_tank_nX--;
			m_tank_nY++;
			m_tank_direction = RIGHT;
			break;
		}

		if (m_tank_direction == DOWN &&
			(C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] >= WALL_BOUND_ONE)) {
			m_tank_nX--;
			m_tank_nY--;
			m_tank_direction = RIGHT;
			break;
		}

		if (m_tank_direction == LEFT &&
			(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] >= WALL_BOUND_ONE ||
			C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] >= WALL_BOUND_ONE)) {
			m_tank_nX++;
			m_tank_direction = RIGHT;
			break;
		}

		if (m_tank_enermy_flag) {
			if (m_tank_direction == UP &&
				(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] == WALL_TANK)) {
				m_tank_nX--;
				m_tank_nY++;
				m_tank_direction = RIGHT;
				break;
			}

			if (m_tank_direction == DOWN &&
				(C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] == WALL_TANK)) {
				m_tank_nX--;
				m_tank_nY--;
				m_tank_direction = RIGHT;
				break;
			}

			if (m_tank_direction == LEFT &&
				(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] == WALL_TANK ||
				C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] == WALL_TANK)) {
				m_tank_nX++;
				m_tank_direction = RIGHT;
				break;
			}
		} else {
			if (m_tank_direction == UP &&
				(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] == WALL_TANK_ENERMY)) {
				m_tank_nX--;
				m_tank_nY++;
				m_tank_direction = RIGHT;
				break;
			}

			if (m_tank_direction == DOWN &&
				(C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] == WALL_TANK_ENERMY)) {
				m_tank_nX--;
				m_tank_nY--;
				m_tank_direction = RIGHT;
				break;
			}

			if (m_tank_direction == LEFT &&
				(C_Map::m_gMap[m_tank_nY - 1][m_tank_nX - 1] == WALL_TANK_ENERMY ||
				C_Map::m_gMap[m_tank_nY + 1][m_tank_nX - 1] == WALL_TANK_ENERMY)) {
				m_tank_nX++;
				m_tank_direction = RIGHT;
				break;
			}
		}

		if (m_tank_direction != RIGHT) {
			m_tank_direction = RIGHT;
		} else {
			m_tank_nX++;
		}
		break;
	default:
		break;
	}

	PrintTank();

	return true;
}

void CTank::Del() {
	DestroyTank();
}

void CTank::PrintTank() {
	int nX, nY;
	CConsoleOperate console_obj;
	C_Map map_obj;

	for (int nPosY = m_tank_nY - 1; nPosY < m_tank_nY + 2; nPosY++) {
		for (int nPosX = m_tank_nX - 1; nPosX < m_tank_nX + 2; nPosX++) {
			nX = nPosX - m_tank_nX + 1;
			nY = nPosY - m_tank_nY + 1;
			if (g_TankShape[m_tank_direction][nY][nX]) {
				C_Map::m_gMap[nPosY][nPosX] = m_tank_enermy_flag ? WALL_TANK_ENERMY : WALL_TANK;
				console_obj.PrintChar(nPosX, nPosY, "¡õ", m_color);
			}
		}
	}

	if (!m_tank_enermy_flag) {
		map_obj.DrawPanel();
	}
}

bool CTank::CanGenerateTank(int nX, int nY) {
	for (int nPosY = nY - 1; nPosY < nY + 2; nPosY++) {
		for (int nPosX = nX - 1; nPosX < nX + 2; nPosX++) {
			if (C_Map::m_gMap[nPosY][nPosX]) {
				return false;
			}
		}
	}

	return true;
}

bool CTank::GenerateTank() {
	for (int i = rand() % MAPCOL / 2; i < MAPCOL; i++) {
		for (int j = rand() % MAPROW / 2; j < MAPROW; j++) {
			if (CanGenerateTank(i, j)) {
				m_tank_enermy_flag = true;
				m_user_engine = false;
				m_tank_type = (rand() % 4) + 1;
				switch (m_tank_type) {
				case ENERMY_Z01:
					m_color = 0x09;
					m_interval_move = TIME_INTERVAL_200;
					m_interval_change_dst_dir = TIME_INTERVAL_20;
					m_interval_change_dst_dir_storage = m_interval_change_dst_dir;
					m_interval_auto_bullet = TIME_INTERVAL_100;
					break;
				case ENERMY_Z02:
					m_color = 0x06;
					m_interval_move = TIME_INTERVAL_120;
					m_interval_change_dst_dir = TIME_INTERVAL_80;
					m_interval_change_dst_dir_storage = m_interval_change_dst_dir;
					m_interval_auto_bullet = TIME_INTERVAL_80;
					break;
				case ENERMY_Z03:
					m_color = 0x04;
					m_interval_move = TIME_INTERVAL_40;
					m_interval_change_dst_dir = TIME_INTERVAL_80;
					m_interval_change_dst_dir_storage = m_interval_change_dst_dir;
					m_interval_auto_bullet = TIME_INTERVAL_40;
					break;
				case ENERMY_Z04:
					m_color = 0x02;
					m_interval_move = TIME_INTERVAL_10;
					m_interval_change_dst_dir = TIME_INTERVAL_40;
					m_interval_change_dst_dir_storage = m_interval_change_dst_dir;
					m_interval_auto_bullet = TIME_INTERVAL_20;
					break;
				default:
					break;
				}
				m_interval_move_storage = m_interval_move;
				InitTank(i, j, (DIRECTION)(rand() % 4), m_color);
				return true;
			}
		}
	}

	return false;
}

COORD CTank::GetCurCoord() {
	COORD cur_coordinate;

	cur_coordinate.X = m_tank_nX;
	cur_coordinate.Y = m_tank_nY;

	return cur_coordinate;
}

DIRECTION CTank::GetCurDir() {
	return m_tank_direction;
}

void CTank::SetTimeInterval(int interval_move, int interval_change_dir) {
	if (interval_move) {
		m_interval_move = interval_move;
	}

	if (interval_change_dir) {
		m_interval_change_dst_dir = interval_change_dir;
		m_interval_change_dst_dir_storage = m_interval_change_dst_dir;
	}
}
