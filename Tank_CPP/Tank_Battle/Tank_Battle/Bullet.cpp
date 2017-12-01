#include "bullet.h"
#include "ConsoleOperate.h"
#include "C_Map.h"
#include "EngineManage.h"
#include <stdlib.h>
#include <string.h>

CBullet::CBullet() {
	m_interval_move = TIME_INTERVAL_20;
	m_dissapper_flag = false;
	m_user_engine = false;
	m_type = BULLET;
}

CBullet::~CBullet() {
}

void CBullet::GenerateBullet(CTank& tank, CEngineManage& engine_manage) {
	COORD tank_coord = tank.GetCurCoord();

	if (!tank.m_is_tank_alive) {
		return;
	}
	/*	PlaySoundA("Tank_shoot.wav", NULL, SND_ASYNC | SND_NODEFAULT);*/

	if ((tank.GetCurDir() == UP &&
		C_Map::m_gMap[tank_coord.Y - 2][tank_coord.X] >= WALL_BOUND_ONE &&
		C_Map::m_gMap[tank_coord.Y - 2][tank_coord.X] != WALL_BOUND_DAMAGE_FOUR) ||
		(tank.GetCurDir() == DOWN &&
		C_Map::m_gMap[tank_coord.Y + 2][tank_coord.X] >= WALL_BOUND_ONE &&
		C_Map::m_gMap[tank_coord.Y + 2][tank_coord.X] != WALL_BOUND_DAMAGE_FOUR) ||
		(tank.GetCurDir() == LEFT &&
		C_Map::m_gMap[tank_coord.Y][tank_coord.X - 2] >= WALL_BOUND_ONE &&
		C_Map::m_gMap[tank_coord.Y][tank_coord.X - 2] != WALL_BOUND_DAMAGE_FOUR) ||
		(tank.GetCurDir() == RIGHT &&
		C_Map::m_gMap[tank_coord.Y][tank_coord.X + 2] >= WALL_BOUND_ONE &&
		C_Map::m_gMap[tank_coord.Y][tank_coord.X + 2] != WALL_BOUND_DAMAGE_FOUR)) {
		return;
	}

	if (tank.m_tank_enermy_flag) {
		if ((tank.GetCurDir() == UP && C_Map::m_gMap[tank_coord.Y - 2][tank_coord.X] == WALL_TANK_ENERMY) ||
			(tank.GetCurDir() == DOWN && C_Map::m_gMap[tank_coord.Y + 2][tank_coord.X] == WALL_TANK_ENERMY) ||
			(tank.GetCurDir() == LEFT && C_Map::m_gMap[tank_coord.Y][tank_coord.X - 2] == WALL_TANK_ENERMY) ||
			(tank.GetCurDir() == RIGHT && C_Map::m_gMap[tank_coord.Y][tank_coord.X + 2] == WALL_TANK_ENERMY)) {
			return;
		}
	}

	m_bullet_direction = tank.GetCurDir();
	m_bullet_nX = tank_coord.X;
	m_bullet_nY = tank_coord.Y;
	m_owner_tank = &tank;
	m_dissapper_flag = false;

	engine_manage.AddEngine(this);
}

void CBullet::PrintBullet() {
	CConsoleOperate console_obj;

	switch (m_bullet_direction) {
	case UP:
		C_Map::m_gMap[m_bullet_nY - 2][m_bullet_nX] = m_owner_tank->m_tank_enermy_flag ? WALL_BULLET_ENERMY : WALL_BULLET;
		console_obj.PrintChar(m_bullet_nX, m_bullet_nY - 2, "¡ñ", m_owner_tank->m_tank_enermy_flag ? m_owner_tank->m_color : 0x0F);
		break;
	case DOWN:
		C_Map::m_gMap[m_bullet_nY + 2][m_bullet_nX] = m_owner_tank->m_tank_enermy_flag ? WALL_BULLET_ENERMY : WALL_BULLET;
		console_obj.PrintChar(m_bullet_nX, m_bullet_nY + 2, "¡ñ", m_owner_tank->m_tank_enermy_flag ? m_owner_tank->m_color : 0x0F);
		break;
	case LEFT:
		C_Map::m_gMap[m_bullet_nY][m_bullet_nX - 2] = m_owner_tank->m_tank_enermy_flag ? WALL_BULLET_ENERMY : WALL_BULLET;
		console_obj.PrintChar(m_bullet_nX - 2, m_bullet_nY, "¡ñ", m_owner_tank->m_tank_enermy_flag ? m_owner_tank->m_color : 0x0F);
		break;
	case RIGHT:
		C_Map::m_gMap[m_bullet_nY][m_bullet_nX + 2] = m_owner_tank->m_tank_enermy_flag ? WALL_BULLET_ENERMY : WALL_BULLET;
		console_obj.PrintChar(m_bullet_nX + 2, m_bullet_nY, "¡ñ", m_owner_tank->m_tank_enermy_flag ? m_owner_tank->m_color : 0x0F);
		break;
	default:
		break;
	}
}

void CBullet::BulletDisapper() {
	CConsoleOperate console_obj;

	switch (m_bullet_direction) {
	case UP:
		if (C_Map::m_gMap[m_bullet_nY - 3][m_bullet_nX] >= WALL_BOUND_ONE) {
			if (C_Map::m_gMap[m_bullet_nY - 3][m_bullet_nX] == WALL_BOUND_DAMAGE_FOUR) {
				console_obj.PrintChar(m_bullet_nX, m_bullet_nY - 3, " ", 0x0F);
				C_Map::m_gMap[m_bullet_nY - 3][m_bullet_nX] = 0;
			}
			m_dissapper_flag = true;
		}

		if (C_Map::m_gMap[m_bullet_nY - 2][m_bullet_nX] == WALL_BULLET_DEAD_ZONE) {
			m_dissapper_flag = true;
			C_Map::m_gMap[m_bullet_nY - 2][m_bullet_nX] = 0;
		}

		if (m_owner_tank->m_tank_enermy_flag) {
			if (C_Map::m_gMap[m_bullet_nY - 3][m_bullet_nX] == WALL_TANK) {
				m_dissapper_flag = true;
				CTank::m_tank_blood--;
			}

			if (CTank::m_tank_blood <= 0) {
				C_Map::m_gMap[m_bullet_nY - 3][m_bullet_nX] = WALL_DEAD_ZONE;
			}

			if (C_Map::m_gMap[m_bullet_nY - 3][m_bullet_nX] == WALL_TANK_ENERMY) {
				m_dissapper_flag = true;
			}

			if (C_Map::m_gMap[m_bullet_nY - 3][m_bullet_nX] == WALL_BULLET) {
				m_dissapper_flag = true;
				C_Map::m_gMap[m_bullet_nY - 3][m_bullet_nX] = WALL_BULLET_DEAD_ZONE;
			}
		} else {
			if (C_Map::m_gMap[m_bullet_nY - 3][m_bullet_nX] == WALL_TANK_ENERMY) {
				m_dissapper_flag = true;
				C_Map::m_gMap[m_bullet_nY - 3][m_bullet_nX] = WALL_DEAD_ZONE;
			}

			if (C_Map::m_gMap[m_bullet_nY - 3][m_bullet_nX] == WALL_BULLET_ENERMY) {
				m_dissapper_flag = true;
				C_Map::m_gMap[m_bullet_nY - 3][m_bullet_nX] = WALL_BULLET_DEAD_ZONE;
			}
		}
		break;
	case DOWN:
		if (C_Map::m_gMap[m_bullet_nY + 3][m_bullet_nX] >= WALL_BOUND_ONE) {
			if (C_Map::m_gMap[m_bullet_nY + 3][m_bullet_nX] == WALL_BOUND_DAMAGE_FOUR) {
				console_obj.PrintChar(m_bullet_nX, m_bullet_nY + 3, " ", 0x0F);
				C_Map::m_gMap[m_bullet_nY + 3][m_bullet_nX] = 0;
			}
			m_dissapper_flag = true;
		}

		if (C_Map::m_gMap[m_bullet_nY + 2][m_bullet_nX] == WALL_BULLET_DEAD_ZONE) {
			C_Map::m_gMap[m_bullet_nY + 2][m_bullet_nX] = 0;
			m_dissapper_flag = true;
		}

		if (m_owner_tank->m_tank_enermy_flag) {
			if (C_Map::m_gMap[m_bullet_nY + 3][m_bullet_nX] == WALL_TANK) {
				CTank::m_tank_blood--;
				m_dissapper_flag = true;
			}

			if (CTank::m_tank_blood <= 0) {
				C_Map::m_gMap[m_bullet_nY + 3][m_bullet_nX] = WALL_DEAD_ZONE;
			}

			if (C_Map::m_gMap[m_bullet_nY + 3][m_bullet_nX] == WALL_TANK_ENERMY) {
				m_dissapper_flag = true;
			}

			if (C_Map::m_gMap[m_bullet_nY + 3][m_bullet_nX] == WALL_BULLET) {
				C_Map::m_gMap[m_bullet_nY + 3][m_bullet_nX] = WALL_BULLET_DEAD_ZONE;
				m_dissapper_flag = true;
			}
		} else {
			if (C_Map::m_gMap[m_bullet_nY + 3][m_bullet_nX] == WALL_TANK_ENERMY) {
				C_Map::m_gMap[m_bullet_nY + 3][m_bullet_nX] = WALL_DEAD_ZONE;
				m_dissapper_flag = true;
			}

			if (C_Map::m_gMap[m_bullet_nY + 3][m_bullet_nX] == WALL_BULLET_ENERMY) {
				C_Map::m_gMap[m_bullet_nY + 3][m_bullet_nX] = WALL_BULLET_DEAD_ZONE;
				m_dissapper_flag = true;
			}
		}
		break;
	case LEFT:
		if (C_Map::m_gMap[m_bullet_nY][m_bullet_nX - 3] >= WALL_BOUND_ONE) {
			if (C_Map::m_gMap[m_bullet_nY][m_bullet_nX - 3] == WALL_BOUND_DAMAGE_FOUR) {
				console_obj.PrintChar(m_bullet_nX - 3, m_bullet_nY, " ", 0x0F);
				C_Map::m_gMap[m_bullet_nY][m_bullet_nX - 3] = 0;
			}
			m_dissapper_flag = true;
		}

		if (C_Map::m_gMap[m_bullet_nY][m_bullet_nX - 2] == WALL_BULLET_DEAD_ZONE) {
			C_Map::m_gMap[m_bullet_nY][m_bullet_nX - 2] = 0;
			m_dissapper_flag = true;
		}

		if (m_owner_tank->m_tank_enermy_flag) {
			if (C_Map::m_gMap[m_bullet_nY][m_bullet_nX - 3] == WALL_TANK) {
				CTank::m_tank_blood--;
				m_dissapper_flag = true;
			}

			if (CTank::m_tank_blood <= 0) {
				C_Map::m_gMap[m_bullet_nY][m_bullet_nX - 3] = WALL_DEAD_ZONE;
			}

			if (C_Map::m_gMap[m_bullet_nY][m_bullet_nX - 3] == WALL_TANK_ENERMY) {
				m_dissapper_flag = true;
			}

			if (C_Map::m_gMap[m_bullet_nY][m_bullet_nX - 3] == WALL_BULLET) {
				C_Map::m_gMap[m_bullet_nY][m_bullet_nX - 3] = WALL_BULLET_DEAD_ZONE;
				m_dissapper_flag = true;
			}
		} else {
			if (C_Map::m_gMap[m_bullet_nY][m_bullet_nX - 3] == WALL_TANK_ENERMY) {
				C_Map::m_gMap[m_bullet_nY][m_bullet_nX - 3] = WALL_DEAD_ZONE;
				m_dissapper_flag = true;
			}

			if (C_Map::m_gMap[m_bullet_nY][m_bullet_nX - 3] == WALL_BULLET_ENERMY) {
				C_Map::m_gMap[m_bullet_nY][m_bullet_nX - 3] = WALL_BULLET_DEAD_ZONE;
				m_dissapper_flag = true;
			}
		}
		break;
	case RIGHT:
		if (C_Map::m_gMap[m_bullet_nY][m_bullet_nX + 3] >= WALL_BOUND_ONE) {
			if (C_Map::m_gMap[m_bullet_nY][m_bullet_nX + 3] == WALL_BOUND_DAMAGE_FOUR) {
				console_obj.PrintChar(m_bullet_nX + 3, m_bullet_nY, " ", 0x0F);
				C_Map::m_gMap[m_bullet_nY][m_bullet_nX + 3] = 0;
			}
			m_dissapper_flag = true;
		}

		if (C_Map::m_gMap[m_bullet_nY][m_bullet_nX + 2] == WALL_BULLET_DEAD_ZONE) {
			C_Map::m_gMap[m_bullet_nY][m_bullet_nX + 2] = 0;
			m_dissapper_flag = true;
		}

		if (m_owner_tank->m_tank_enermy_flag) {
			if (C_Map::m_gMap[m_bullet_nY][m_bullet_nX + 3] == WALL_TANK) {
				CTank::m_tank_blood--;
				m_dissapper_flag = true;
			}

			if (CTank::m_tank_blood <= 0) {
				C_Map::m_gMap[m_bullet_nY][m_bullet_nX + 3] = WALL_DEAD_ZONE;
			}

			if (C_Map::m_gMap[m_bullet_nY][m_bullet_nX + 3] == WALL_TANK_ENERMY) {
				m_dissapper_flag = true;
			}

			if (C_Map::m_gMap[m_bullet_nY][m_bullet_nX + 3] == WALL_BULLET) {
				C_Map::m_gMap[m_bullet_nY][m_bullet_nX + 3] = WALL_BULLET_DEAD_ZONE;
				m_dissapper_flag = true;
			}
		} else {
			if (C_Map::m_gMap[m_bullet_nY][m_bullet_nX + 3] == WALL_TANK_ENERMY) {
				C_Map::m_gMap[m_bullet_nY][m_bullet_nX + 3] = WALL_DEAD_ZONE;
				m_dissapper_flag = true;
			}

			if (C_Map::m_gMap[m_bullet_nY][m_bullet_nX + 3] == WALL_BULLET_ENERMY) {
				C_Map::m_gMap[m_bullet_nY][m_bullet_nX + 3] = WALL_BULLET_DEAD_ZONE;
				m_dissapper_flag = true;
			}
		}
		break;
	default:
		break;
	}
}

void CBullet::Del() {
	CConsoleOperate console_obj;

	switch (m_bullet_direction) {
	case UP:
		console_obj.PrintChar(m_bullet_nX, m_bullet_nY - 2, " ", 0x0F);
		C_Map::m_gMap[m_bullet_nY - 2][m_bullet_nX] = 0;
		break;
	case DOWN:
		console_obj.PrintChar(m_bullet_nX, m_bullet_nY + 2, " ", 0x0F);
		C_Map::m_gMap[m_bullet_nY + 2][m_bullet_nX] = 0;
		break;
	case LEFT:
		console_obj.PrintChar(m_bullet_nX - 2, m_bullet_nY, " ", 0x0F);
		C_Map::m_gMap[m_bullet_nY][m_bullet_nX - 2] = 0;
		break;
	case RIGHT:
		console_obj.PrintChar(m_bullet_nX + 2, m_bullet_nY, " ", 0x0F);
		C_Map::m_gMap[m_bullet_nY][m_bullet_nX + 2] = 0;
		break;
	default:
		break;
	}
}

void CBullet::DelAllBullet() {

}

bool CBullet::Move(DIRECTION direction) {
	CConsoleOperate console_obj;

	BulletDisapper();

	switch (m_bullet_direction) {
	case UP:
		console_obj.PrintChar(m_bullet_nX, m_bullet_nY - 2, " ", 0x0F);
		C_Map::m_gMap[m_bullet_nY - 2][m_bullet_nX] = 0;
		if (!m_dissapper_flag) {
			m_bullet_nY--;
		}
		break;
	case DOWN:
		console_obj.PrintChar(m_bullet_nX, m_bullet_nY + 2, " ", 0x0F);
		C_Map::m_gMap[m_bullet_nY + 2][m_bullet_nX] = 0;
		if (!m_dissapper_flag) {
			m_bullet_nY++;
		}
		break;
	case LEFT:
		console_obj.PrintChar(m_bullet_nX - 2, m_bullet_nY, " ", 0x0F);
		C_Map::m_gMap[m_bullet_nY][m_bullet_nX - 2] = 0;
		if (!m_dissapper_flag) {
			m_bullet_nX--;
		}
		break;
	case RIGHT:
		console_obj.PrintChar(m_bullet_nX + 2, m_bullet_nY, " ", 0x0F);
		C_Map::m_gMap[m_bullet_nY][m_bullet_nX + 2] = 0;
		if (!m_dissapper_flag) {
			m_bullet_nX++;
		}
		break;
	default:
		break;
	}

	if (!m_dissapper_flag)
		PrintBullet();

	return true;
}
