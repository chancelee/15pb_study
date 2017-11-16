#include "Tank.h"
#include "WindowsConsole.h"
#include "map.h"
#include "bullet.h"
#include <stdio.h>

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

void InitTank(PTANK ptank, int center_nX, int center_nY, DIRECTION directions, char color) {
	ptank->m_tank_nX = center_nX;
	ptank->m_tank_nY = center_nY;
	ptank->m_tank_direction = directions;
	ptank->m_tank_bullet = NULL;
	ptank->m_is_tank_alive = true;
	ptank->m_color = color;

	PrintTank(ptank);
}

bool ShouldTankAlive(PTANK ptank) {
	int nX, nY;

	for (int nPosY = ptank->m_tank_nY - 1; nPosY < ptank->m_tank_nY + 2; nPosY++) {
		for (int nPosX = ptank->m_tank_nX - 1; nPosX < ptank->m_tank_nX + 2; nPosX++) {
			nX = nPosX - ptank->m_tank_nX + 1;
			nY = nPosY - ptank->m_tank_nY + 1;
			if (g_TankShape[ptank->m_tank_direction][nY][nX] && m_gMap[nPosY][nPosX] == WALL_DEAD_ZONE) {
				return false;
			}
		}
	}

	return true;
}

void DestroyTank(PTANK ptank) {
	int nX, nY;

	for (int nPosY = ptank->m_tank_nY - 1; nPosY < ptank->m_tank_nY + 2; nPosY++) {
		for (int nPosX = ptank->m_tank_nX - 1; nPosX < ptank->m_tank_nX + 2; nPosX++) {
			nX = nPosX - ptank->m_tank_nX + 1;
			nY = nPosY - ptank->m_tank_nY + 1;
			if (g_TankShape[ptank->m_tank_direction][nY][nX]) {
				m_gMap[nPosY][nPosX] = 0;
				PrintChar(nPosX, nPosY, "  ", 0x0F);
			}
		}
	}

	PlaySoundA("Tank_dead.wav", NULL, SND_ASYNC | SND_NODEFAULT);

	ptank->m_is_tank_alive = false;
	DelAllBullet(ptank);
	ptank->m_tank_bullet = nullptr;

	if (ptank->m_tank_enermy_flag) {
		GenerateTank(ptank);
	}
}

static bool TankCanMove(PTANK ptank, DIRECTION direction) {
	switch (direction) {
	case UP:
		/* UP */
		if (m_gMap[ptank->m_tank_nY - 2][ptank->m_tank_nX] >= WALL_BOUND_ONE ||
			m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] >= WALL_BOUND_ONE || 
			m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] >= WALL_BOUND_ONE ||
			(ptank->m_tank_direction == LEFT && m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] >= WALL_BOUND_ONE) ||
			(ptank->m_tank_direction == RIGHT && m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] >= WALL_BOUND_ONE)) {
			return false;
		}
		
		if (ptank->m_tank_enermy_flag) {
			if (m_gMap[ptank->m_tank_nY - 2][ptank->m_tank_nX] == WALL_TANK ||
				m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] == WALL_TANK ||
				m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] == WALL_TANK ||
				(ptank->m_tank_direction == LEFT && m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] == WALL_TANK) ||
				(ptank->m_tank_direction == RIGHT && m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] == WALL_TANK)) {
				return false;
			}

			if (m_gMap[ptank->m_tank_nY - 2][ptank->m_tank_nX] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY - 2][ptank->m_tank_nX - 1] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY - 2][ptank->m_tank_nX + 1] == WALL_TANK_ENERMY)
				return false;
		} else {
			if (m_gMap[ptank->m_tank_nY - 2][ptank->m_tank_nX] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] == WALL_TANK_ENERMY ||
				(ptank->m_tank_direction == LEFT && m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] == WALL_TANK_ENERMY) ||
				(ptank->m_tank_direction == RIGHT && m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] == WALL_TANK_ENERMY)) {
				return false;
			}
		}

		break;
	case DOWN:
		/* DOWN */
		if (m_gMap[ptank->m_tank_nY + 2][ptank->m_tank_nX] >= WALL_BOUND_ONE ||
			m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] >= WALL_BOUND_ONE ||
			m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] >= WALL_BOUND_ONE ||
			(ptank->m_tank_direction == LEFT && m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] >= WALL_BOUND_ONE) ||
			(ptank->m_tank_direction == RIGHT && m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] >= WALL_BOUND_ONE)) {
			return false;
		}

		if (ptank->m_tank_enermy_flag) {
			if (m_gMap[ptank->m_tank_nY + 2][ptank->m_tank_nX] == WALL_TANK ||
				m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] == WALL_TANK ||
				m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] == WALL_TANK ||
				(ptank->m_tank_direction == LEFT && m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] == WALL_TANK) ||
				(ptank->m_tank_direction == RIGHT && m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] == WALL_TANK)) {
				return false;
			}

			if (m_gMap[ptank->m_tank_nY + 2][ptank->m_tank_nX] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY + 2][ptank->m_tank_nX - 1] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY + 2][ptank->m_tank_nX + 1] == WALL_TANK_ENERMY)
				return false;
		} else {
			if (m_gMap[ptank->m_tank_nY + 2][ptank->m_tank_nX] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] == WALL_TANK_ENERMY ||
				(ptank->m_tank_direction == LEFT && m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] == WALL_TANK_ENERMY) ||
				(ptank->m_tank_direction == RIGHT && m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] == WALL_TANK_ENERMY)) {
				return false;
			}
		}

		break;
	case LEFT:
		/* LEFT */
		if (m_gMap[ptank->m_tank_nY][ptank->m_tank_nX - 2] >= WALL_BOUND_ONE ||
			m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] >= WALL_BOUND_ONE ||
			m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] >= WALL_BOUND_ONE ||
			(ptank->m_tank_direction == UP && 
			(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] >= WALL_BOUND_ONE) && 
			(m_gMap[ptank->m_tank_nY][ptank->m_tank_nX + 2] >= WALL_BOUND_ONE)) ||
			(ptank->m_tank_direction == DOWN &&
			(m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] >= WALL_BOUND_ONE) &&
			(m_gMap[ptank->m_tank_nY][ptank->m_tank_nX + 2] >= WALL_BOUND_ONE))) {
			return false;
		}

		if (ptank->m_tank_enermy_flag) {
			if (m_gMap[ptank->m_tank_nY][ptank->m_tank_nX - 2] == WALL_TANK ||
				m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] == WALL_TANK ||
				m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] == WALL_TANK ||
				(ptank->m_tank_direction == UP &&
				(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] == WALL_TANK) &&
				(m_gMap[ptank->m_tank_nY][ptank->m_tank_nX + 2] == WALL_TANK)) ||
				(ptank->m_tank_direction == DOWN &&
				(m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] == WALL_TANK) &&
				(m_gMap[ptank->m_tank_nY][ptank->m_tank_nX + 2] == WALL_TANK))) {
				return false;
			}

			if (m_gMap[ptank->m_tank_nY][ptank->m_tank_nX - 2] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 2] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 2] == WALL_TANK_ENERMY)
				return false;
		} else {
			if (m_gMap[ptank->m_tank_nY][ptank->m_tank_nX - 2] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] == WALL_TANK_ENERMY ||
				(ptank->m_tank_direction == UP &&
				(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] == WALL_TANK_ENERMY) &&
				(m_gMap[ptank->m_tank_nY][ptank->m_tank_nX + 2] == WALL_TANK_ENERMY)) ||
				(ptank->m_tank_direction == DOWN &&
				(m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] == WALL_TANK_ENERMY) &&
				(m_gMap[ptank->m_tank_nY][ptank->m_tank_nX + 2] == WALL_TANK_ENERMY))) {
				return false;
			}
		}

		break;
	case RIGHT:
		/* RIGHT */
		if (m_gMap[ptank->m_tank_nY][ptank->m_tank_nX + 2] >= WALL_BOUND_ONE ||
			m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] >= WALL_BOUND_ONE ||
			m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] >= WALL_BOUND_ONE ||
			(ptank->m_tank_direction == UP &&
			(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] >= WALL_BOUND_ONE) &&
			(m_gMap[ptank->m_tank_nY][ptank->m_tank_nX - 2] >= WALL_BOUND_ONE)) ||
			(ptank->m_tank_direction == DOWN &&
			(m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] >= WALL_BOUND_ONE) &&
			(m_gMap[ptank->m_tank_nY][ptank->m_tank_nX - 2] >= WALL_BOUND_ONE))) {
			return false;
		}

		if (ptank->m_tank_enermy_flag) {
			if (m_gMap[ptank->m_tank_nY][ptank->m_tank_nX + 2] == WALL_TANK ||
				m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] == WALL_TANK ||
				m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] == WALL_TANK ||
				(ptank->m_tank_direction == UP &&
				(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] == WALL_TANK) &&
				(m_gMap[ptank->m_tank_nY][ptank->m_tank_nX - 2] == WALL_TANK)) ||
				(ptank->m_tank_direction == DOWN &&
				(m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] == WALL_TANK) &&
				(m_gMap[ptank->m_tank_nY][ptank->m_tank_nX - 2] == WALL_TANK))) {
				return false;
			}

			if (m_gMap[ptank->m_tank_nY][ptank->m_tank_nX + 2] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 2] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 2] == WALL_TANK_ENERMY)
				return false;
		} else {
			if (m_gMap[ptank->m_tank_nY][ptank->m_tank_nX + 2] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] == WALL_TANK_ENERMY ||
				(ptank->m_tank_direction == UP &&
				(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] == WALL_TANK_ENERMY) &&
				(m_gMap[ptank->m_tank_nY][ptank->m_tank_nX - 2] == WALL_TANK_ENERMY)) ||
				(ptank->m_tank_direction == DOWN &&
				(m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] == WALL_TANK_ENERMY) &&
				(m_gMap[ptank->m_tank_nY][ptank->m_tank_nX - 2] == WALL_TANK_ENERMY))) {
				return false;
			}
		}

		break;
	default:
		break;
	}

	return true;
}

static void EraseTank(PTANK ptank) {
	int nX, nY;

	for (int nPosY = ptank->m_tank_nY - 1; nPosY < ptank->m_tank_nY + 2; nPosY++) {
		for (int nPosX = ptank->m_tank_nX - 1; nPosX < ptank->m_tank_nX + 2; nPosX++) {
			nX = nPosX - ptank->m_tank_nX + 1;
			nY = nPosY - ptank->m_tank_nY + 1;
			if (g_TankShape[ptank->m_tank_direction][nY][nX]) {
				m_gMap[nPosY][nPosX] = 0;
				PrintChar(nPosX, nPosY, "  ", 0x0F);
			}
		}
	}
}

bool MoveTank(PTANK ptank, DIRECTION direction) {
	if (!ShouldTankAlive(ptank)) {
		DestroyTank(ptank);
	}

	if (!ptank->m_is_tank_alive) {
		return false;
	}

	if (!TankCanMove(ptank, direction)) {
		return false;
	}

	EraseTank(ptank);

	switch (direction) {
	case UP:
		/* UP */
		if (ptank->m_tank_direction == DOWN &&
			(m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] >= WALL_BOUND_ONE ||
			m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] >= WALL_BOUND_ONE)) {
			ptank->m_tank_nY--;
			ptank->m_tank_direction = UP;
			break;
		}

		if (ptank->m_tank_enermy_flag) {
			if (ptank->m_tank_direction == DOWN &&
				(m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] == WALL_TANK ||
				m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] == WALL_TANK)) {
				ptank->m_tank_nY--;
				ptank->m_tank_direction = UP;
				break;
			}
		} else {
			if (ptank->m_tank_direction == DOWN &&
				(m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] == WALL_TANK_ENERMY)) {
				ptank->m_tank_nY--;
				ptank->m_tank_direction = UP;
				break;
			}
		}

		if (ptank->m_tank_direction != UP) {
			ptank->m_tank_direction = UP;
		} else {
			ptank->m_tank_nY--;
		}
		break;
	case DOWN:
		/* DOWN */
		if (ptank->m_tank_direction == UP && 
			(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] >= WALL_BOUND_ONE || 
			m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] >= WALL_BOUND_ONE)) {
			ptank->m_tank_nY++;
			ptank->m_tank_direction = DOWN;
			break;
		}

		if (ptank->m_tank_enermy_flag) {
			if (ptank->m_tank_direction == UP &&
				(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] == WALL_TANK ||
				m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] == WALL_TANK)) {
				ptank->m_tank_nY++;
				ptank->m_tank_direction = DOWN;
				break;
			}
		} else {
			if (ptank->m_tank_direction == UP &&
				(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] == WALL_TANK_ENERMY)) {
				ptank->m_tank_nY++;
				ptank->m_tank_direction = DOWN;
				break;
			}
		}

		if (ptank->m_tank_direction != DOWN) {
			ptank->m_tank_direction = DOWN;
		} else {
			ptank->m_tank_nY++;
		}
		break;
	case LEFT:
		/* LEFT */
		if (ptank->m_tank_direction == UP &&
			(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] >= WALL_BOUND_ONE)) {
			ptank->m_tank_nX++;
			ptank->m_tank_nY++;
			ptank->m_tank_direction = LEFT;
			break;
		}

		if (ptank->m_tank_direction == DOWN &&
			(m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] >= WALL_BOUND_ONE)) {
			ptank->m_tank_nX++;
			ptank->m_tank_nY--;
			ptank->m_tank_direction = LEFT;
			break;
		}

		if (ptank->m_tank_direction == RIGHT &&
			(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] >= WALL_BOUND_ONE ||
			m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] >= WALL_BOUND_ONE)) {
			ptank->m_tank_nX--;
			ptank->m_tank_direction = LEFT;
			break;
		}

		if (ptank->m_tank_enermy_flag) {
			if (ptank->m_tank_direction == UP &&
				(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] == WALL_TANK)) {
				ptank->m_tank_nX++;
				ptank->m_tank_nY++;
				ptank->m_tank_direction = LEFT;
				break;
			}

			if (ptank->m_tank_direction == DOWN &&
				(m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] == WALL_TANK)) {
				ptank->m_tank_nX++;
				ptank->m_tank_nY--;
				ptank->m_tank_direction = LEFT;
				break;
			}

			if (ptank->m_tank_direction == RIGHT &&
				(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] == WALL_TANK ||
				m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] == WALL_TANK)) {
				ptank->m_tank_nX--;
				ptank->m_tank_direction = LEFT;
				break;
			}
		} else {
			if (ptank->m_tank_direction == UP &&
				(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] == WALL_TANK_ENERMY)) {
				ptank->m_tank_nX++;
				ptank->m_tank_nY++;
				ptank->m_tank_direction = LEFT;
				break;
			}

			if (ptank->m_tank_direction == DOWN &&
				(m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] == WALL_TANK_ENERMY)) {
				ptank->m_tank_nX++;
				ptank->m_tank_nY--;
				ptank->m_tank_direction = LEFT;
				break;
			}

			if (ptank->m_tank_direction == RIGHT &&
				(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX + 1] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX + 1] == WALL_TANK_ENERMY)) {
				ptank->m_tank_nX--;
				ptank->m_tank_direction = LEFT;
				break;
			}
		}

		if (ptank->m_tank_direction != LEFT) {
			ptank->m_tank_direction = LEFT;
		} else {
			ptank->m_tank_nX--;
		}
		break;
	case RIGHT:
		/* RIGHT */
		if (ptank->m_tank_direction == UP && 
			(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] >= WALL_BOUND_ONE)) {
			ptank->m_tank_nX--;
			ptank->m_tank_nY++;
			ptank->m_tank_direction = RIGHT;
			break;
		}

		if (ptank->m_tank_direction == DOWN &&
			(m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] >= WALL_BOUND_ONE)) {
			ptank->m_tank_nX--;
			ptank->m_tank_nY--;
			ptank->m_tank_direction = RIGHT;
			break;
		}

		if (ptank->m_tank_direction == LEFT &&
			(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] >= WALL_BOUND_ONE ||
			m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] >= WALL_BOUND_ONE)) {
			ptank->m_tank_nX++;
			ptank->m_tank_direction = RIGHT;
			break;
		}

		if (ptank->m_tank_enermy_flag) {
			if (ptank->m_tank_direction == UP &&
				(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] == WALL_TANK)) {
				ptank->m_tank_nX--;
				ptank->m_tank_nY++;
				ptank->m_tank_direction = RIGHT;
				break;
			}

			if (ptank->m_tank_direction == DOWN &&
				(m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] == WALL_TANK)) {
				ptank->m_tank_nX--;
				ptank->m_tank_nY--;
				ptank->m_tank_direction = RIGHT;
				break;
			}

			if (ptank->m_tank_direction == LEFT &&
				(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] == WALL_TANK ||
				m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] == WALL_TANK)) {
				ptank->m_tank_nX++;
				ptank->m_tank_direction = RIGHT;
				break;
			}
		} else {
			if (ptank->m_tank_direction == UP &&
				(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] == WALL_TANK_ENERMY)) {
				ptank->m_tank_nX--;
				ptank->m_tank_nY++;
				ptank->m_tank_direction = RIGHT;
				break;
			}

			if (ptank->m_tank_direction == DOWN &&
				(m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] == WALL_TANK_ENERMY)) {
				ptank->m_tank_nX--;
				ptank->m_tank_nY--;
				ptank->m_tank_direction = RIGHT;
				break;
			}

			if (ptank->m_tank_direction == LEFT &&
				(m_gMap[ptank->m_tank_nY - 1][ptank->m_tank_nX - 1] == WALL_TANK_ENERMY ||
				m_gMap[ptank->m_tank_nY + 1][ptank->m_tank_nX - 1] == WALL_TANK_ENERMY)) {
				ptank->m_tank_nX++;
				ptank->m_tank_direction = RIGHT;
				break;
			}
		}

		if (ptank->m_tank_direction != RIGHT) {
			ptank->m_tank_direction = RIGHT;
		} else {
			ptank->m_tank_nX++;
		}
		break;
	default:
		break;
	}

	PrintTank(ptank);

	return true;
}

void PrintTank(PTANK ptank) {
	int nX, nY;

	for (int nPosY = ptank->m_tank_nY - 1; nPosY < ptank->m_tank_nY + 2; nPosY++) {
		for (int nPosX = ptank->m_tank_nX - 1; nPosX < ptank->m_tank_nX + 2; nPosX++) {
			nX = nPosX - ptank->m_tank_nX + 1;
			nY = nPosY - ptank->m_tank_nY + 1;
			if (g_TankShape[ptank->m_tank_direction][nY][nX]) {
				m_gMap[nPosY][nPosX] = ptank->m_tank_enermy_flag ? WALL_TANK_ENERMY : WALL_TANK;
				PrintChar(nPosX, nPosY, "¡õ", ptank->m_color);
			}
		}
	}
}

bool CanGenerateTank(int nX, int nY) {
	for (int nPosY = nY - 1; nPosY < nY + 2; nPosY++) {
		for (int nPosX = nX - 1; nPosX < nX + 2; nPosX++) {
			if (m_gMap[nPosY][nPosX]) {
				return false;
			}
		}
	}

	return true;
}

void GenerateTank(PTANK ptank_enermy) {
	for (int i = 4; i < MAPCOL; i++) {
		for (int j = 4; j < MAPROW; j++) {
			if (CanGenerateTank(i, j)) {
				if (!ptank_enermy->m_is_tank_alive) {
					ptank_enermy->m_tank_enermy_flag = true;
					InitTank(ptank_enermy, i, j, (DIRECTION)(rand() % 4), 0x09);
					return;
				}
			}
		}
	}
}
