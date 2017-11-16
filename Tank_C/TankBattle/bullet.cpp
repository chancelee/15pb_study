#include "bullet.h"
#include "WindowsConsole.h"
#include "map.h"
#include <stdlib.h>
#include <string.h>

void GenerateBullet(PTANK ptank) {
	PBULLET pbullet_generate = NULL;
	PBULLET pbullet_head = NULL;

	if (!ptank->m_is_tank_alive) {
		return;
	}
/*	PlaySoundA("Tank_shoot.wav", NULL, SND_ASYNC | SND_NODEFAULT);*/

	if ((ptank->m_tank_direction == UP && 
		m_gMap[ptank->m_tank_nY - 2][ptank->m_tank_nX] >= WALL_BOUND_ONE && 
		m_gMap[ptank->m_tank_nY - 2][ptank->m_tank_nX] != WALL_BOUND_DAMAGE_FOUR) ||
		(ptank->m_tank_direction == DOWN && 
		m_gMap[ptank->m_tank_nY + 2][ptank->m_tank_nX] >= WALL_BOUND_ONE && 
		m_gMap[ptank->m_tank_nY + 2][ptank->m_tank_nX] != WALL_BOUND_DAMAGE_FOUR) ||
		(ptank->m_tank_direction == LEFT && 
		m_gMap[ptank->m_tank_nY][ptank->m_tank_nX - 2] >= WALL_BOUND_ONE && 
		m_gMap[ptank->m_tank_nY][ptank->m_tank_nX - 2] != WALL_BOUND_DAMAGE_FOUR) ||
		(ptank->m_tank_direction == RIGHT && 
		m_gMap[ptank->m_tank_nY][ptank->m_tank_nX + 2] >= WALL_BOUND_ONE &&
		m_gMap[ptank->m_tank_nY][ptank->m_tank_nX + 2] != WALL_BOUND_DAMAGE_FOUR)) {
		return;
	}

	if (ptank->m_tank_enermy_flag) {
		if ((ptank->m_tank_direction == UP && m_gMap[ptank->m_tank_nY - 2][ptank->m_tank_nX] == WALL_TANK_ENERMY) ||
			(ptank->m_tank_direction == DOWN && m_gMap[ptank->m_tank_nY + 2][ptank->m_tank_nX] == WALL_TANK_ENERMY) ||
			(ptank->m_tank_direction == LEFT && m_gMap[ptank->m_tank_nY][ptank->m_tank_nX - 2] == WALL_TANK_ENERMY) ||
			(ptank->m_tank_direction == RIGHT && m_gMap[ptank->m_tank_nY][ptank->m_tank_nX + 2] == WALL_TANK_ENERMY)) {
			return;
		}
	}

	if (ptank->m_tank_bullet == NULL) {
		ptank->m_tank_bullet = (PBULLET)malloc(sizeof(BULLET));
		memset(ptank->m_tank_bullet, 0, sizeof(BULLET));
		ptank->m_tank_bullet->m_bullet_direction = ptank->m_tank_direction;
		ptank->m_tank_bullet->m_bullet_nX = ptank->m_tank_nX;
		ptank->m_tank_bullet->m_bullet_nY = ptank->m_tank_nY;
		ptank->m_tank_bullet->m_owner_tank = ptank;
		ptank->m_tank_bullet->m_bullet_disapper = false;
		ptank->m_tank_bullet->m_bullet_next = NULL;
		return;
	}
	
	if (pbullet_generate == NULL) {
		pbullet_generate = (PBULLET)malloc(sizeof(BULLET));
		memset(pbullet_generate, 0, sizeof(BULLET));
		pbullet_generate->m_bullet_next = NULL;
	}

	pbullet_head = ptank->m_tank_bullet;
	while (pbullet_head->m_bullet_next != NULL) {
		pbullet_head = pbullet_head->m_bullet_next;
	}
	pbullet_head->m_bullet_next = pbullet_generate;

	pbullet_generate->m_bullet_direction = ptank->m_tank_direction;
	pbullet_generate->m_bullet_nX = ptank->m_tank_nX;
	pbullet_generate->m_bullet_nY = ptank->m_tank_nY;
	pbullet_generate->m_owner_tank = ptank;
	pbullet_generate->m_bullet_disapper = false;
}

void PrintBullet(PTANK ptank) {
	PBULLET pbullet_head = ptank->m_tank_bullet;
	while (pbullet_head) {
		switch (pbullet_head->m_bullet_direction) {
		case UP:
			m_gMap[pbullet_head->m_bullet_nY - 2][pbullet_head->m_bullet_nX] = ptank->m_tank_enermy_flag ? WALL_BULLET_ENERMY : WALL_BULLET;
			PrintChar(pbullet_head->m_bullet_nX, pbullet_head->m_bullet_nY - 2, "¡ñ", 0x0F);
			break;
		case DOWN:
			m_gMap[pbullet_head->m_bullet_nY + 2][pbullet_head->m_bullet_nX] = ptank->m_tank_enermy_flag ? WALL_BULLET_ENERMY : WALL_BULLET;
			PrintChar(pbullet_head->m_bullet_nX, pbullet_head->m_bullet_nY + 2, "¡ñ", 0x0F);
			break;
		case LEFT:
			m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX - 2] = ptank->m_tank_enermy_flag ? WALL_BULLET_ENERMY : WALL_BULLET;
			PrintChar(pbullet_head->m_bullet_nX - 2, pbullet_head->m_bullet_nY, "¡ñ", 0x0F);
			break;
		case RIGHT:
			m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX + 2] = ptank->m_tank_enermy_flag ? WALL_BULLET_ENERMY : WALL_BULLET;
			PrintChar(pbullet_head->m_bullet_nX + 2, pbullet_head->m_bullet_nY, "¡ñ", 0x0F);
			break;
		default:
			break;
		}
		pbullet_head = pbullet_head->m_bullet_next;
	}
}

void BulletDisapper(PTANK ptank) {
	PBULLET pbullet_head = ptank->m_tank_bullet;
	while (pbullet_head) {
		switch (pbullet_head->m_bullet_direction) {
		case UP:
			if (m_gMap[pbullet_head->m_bullet_nY - 3][pbullet_head->m_bullet_nX] >= WALL_BOUND_ONE &&
				m_gMap[pbullet_head->m_bullet_nY - 3][pbullet_head->m_bullet_nX] != WALL_BOUND_DAMAGE_FOUR) {
				pbullet_head->m_bullet_disapper = true;
			}

			if (m_gMap[pbullet_head->m_bullet_nY - 3][pbullet_head->m_bullet_nX] == WALL_BULLET_DEAD_ZONE) {
				pbullet_head->m_bullet_disapper = true;
				m_gMap[pbullet_head->m_bullet_nY - 3][pbullet_head->m_bullet_nX] = 0;
			}

			if (ptank->m_tank_enermy_flag) {
				if (m_gMap[pbullet_head->m_bullet_nY - 3][pbullet_head->m_bullet_nX] == WALL_TANK) {
					pbullet_head->m_bullet_disapper = true;
					m_gMap[pbullet_head->m_bullet_nY - 3][pbullet_head->m_bullet_nX] = WALL_DEAD_ZONE;
				}

				if (m_gMap[pbullet_head->m_bullet_nY - 3][pbullet_head->m_bullet_nX] == WALL_TANK_ENERMY) {
					pbullet_head->m_bullet_disapper = true;
				}

				if (m_gMap[pbullet_head->m_bullet_nY - 3][pbullet_head->m_bullet_nX] == WALL_BULLET) {
					pbullet_head->m_bullet_disapper = true;
					m_gMap[pbullet_head->m_bullet_nY - 3][pbullet_head->m_bullet_nX] = WALL_BULLET_DEAD_ZONE;
				}
			} else {
				if (m_gMap[pbullet_head->m_bullet_nY - 3][pbullet_head->m_bullet_nX] == WALL_TANK_ENERMY) {
					pbullet_head->m_bullet_disapper = true;
					m_gMap[pbullet_head->m_bullet_nY - 3][pbullet_head->m_bullet_nX] = WALL_DEAD_ZONE;
				}

				if (m_gMap[pbullet_head->m_bullet_nY - 3][pbullet_head->m_bullet_nX] == WALL_BULLET_ENERMY) {
					pbullet_head->m_bullet_disapper = true;
					m_gMap[pbullet_head->m_bullet_nY - 3][pbullet_head->m_bullet_nX] = WALL_BULLET_DEAD_ZONE;
				}
			}
			break;
		case DOWN:
			if (m_gMap[pbullet_head->m_bullet_nY + 3][pbullet_head->m_bullet_nX] >= WALL_BOUND_ONE && 
				m_gMap[pbullet_head->m_bullet_nY + 3][pbullet_head->m_bullet_nX] != WALL_BOUND_DAMAGE_FOUR) {
				pbullet_head->m_bullet_disapper = true;
			}

			if (m_gMap[pbullet_head->m_bullet_nY + 3][pbullet_head->m_bullet_nX] == WALL_BULLET_DEAD_ZONE) {
				m_gMap[pbullet_head->m_bullet_nY + 3][pbullet_head->m_bullet_nX] = 0;
				pbullet_head->m_bullet_disapper = true;
			}

			if (ptank->m_tank_enermy_flag) {
				if (m_gMap[pbullet_head->m_bullet_nY + 3][pbullet_head->m_bullet_nX] == WALL_TANK) {
					m_gMap[pbullet_head->m_bullet_nY + 3][pbullet_head->m_bullet_nX] = WALL_DEAD_ZONE;
					pbullet_head->m_bullet_disapper = true;
				}

				if (m_gMap[pbullet_head->m_bullet_nY + 3][pbullet_head->m_bullet_nX] == WALL_TANK_ENERMY) {
					pbullet_head->m_bullet_disapper = true;
				}

				if (m_gMap[pbullet_head->m_bullet_nY + 3][pbullet_head->m_bullet_nX] == WALL_BULLET) {
					m_gMap[pbullet_head->m_bullet_nY + 3][pbullet_head->m_bullet_nX] = WALL_BULLET_DEAD_ZONE;
					pbullet_head->m_bullet_disapper = true;
				}
			} else {
				if (m_gMap[pbullet_head->m_bullet_nY + 3][pbullet_head->m_bullet_nX] == WALL_TANK_ENERMY) {
					m_gMap[pbullet_head->m_bullet_nY + 3][pbullet_head->m_bullet_nX] = WALL_DEAD_ZONE;
					pbullet_head->m_bullet_disapper = true;
				}

				if (m_gMap[pbullet_head->m_bullet_nY + 3][pbullet_head->m_bullet_nX] == WALL_BULLET_ENERMY) {
					m_gMap[pbullet_head->m_bullet_nY + 3][pbullet_head->m_bullet_nX] = WALL_BULLET_DEAD_ZONE;
					pbullet_head->m_bullet_disapper = true;
				}
			}
			break;
		case LEFT:
			if (m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX - 3] >= WALL_BOUND_ONE && 
				m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX - 3] != WALL_BOUND_DAMAGE_FOUR) {
				pbullet_head->m_bullet_disapper = true;
			}

			if (m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX - 3] == WALL_BULLET_DEAD_ZONE) {
				m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX - 3] = 0;
				pbullet_head->m_bullet_disapper = true;
			}

			if (ptank->m_tank_enermy_flag) {
				if (m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX - 3] == WALL_TANK) {
					m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX - 3] = WALL_DEAD_ZONE;
					pbullet_head->m_bullet_disapper = true;
				}

				if (m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX - 3] == WALL_TANK_ENERMY) {
					pbullet_head->m_bullet_disapper = true;
				}

				if (m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX - 3] == WALL_BULLET) {
					m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX - 3] = WALL_BULLET_DEAD_ZONE;
					pbullet_head->m_bullet_disapper = true;
				}
			} else {
				if (m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX - 3] == WALL_TANK_ENERMY) {
					m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX - 3] = WALL_DEAD_ZONE;
					pbullet_head->m_bullet_disapper = true;
				}

				if (m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX - 3] == WALL_BULLET_ENERMY) {
					m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX - 3] = WALL_BULLET_DEAD_ZONE;
					pbullet_head->m_bullet_disapper = true;
				}
			}
			break;
		case RIGHT:
			if (m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX + 3] >= WALL_BOUND_ONE && 
				m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX + 3] != WALL_BOUND_DAMAGE_FOUR) {
				pbullet_head->m_bullet_disapper = true;
			}

			if (m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX + 3] == WALL_BULLET_DEAD_ZONE) {
				m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX + 3] = 0;
				pbullet_head->m_bullet_disapper = true;
			}

			if (ptank->m_tank_enermy_flag) {
				if (m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX + 3] == WALL_TANK) {
					m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX + 3] = WALL_DEAD_ZONE;
					pbullet_head->m_bullet_disapper = true;
				}

				if (m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX + 3] == WALL_TANK_ENERMY) {
					pbullet_head->m_bullet_disapper = true;
				}

				if (m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX + 3] == WALL_BULLET) {
					m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX + 3] = WALL_BULLET_DEAD_ZONE;
					pbullet_head->m_bullet_disapper = true;
				}
			} else {
				if (m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX + 3] == WALL_TANK_ENERMY) {
					m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX + 3] = WALL_DEAD_ZONE;
					pbullet_head->m_bullet_disapper = true;
				}

				if (m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX + 3] == WALL_BULLET_ENERMY) {
					m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX + 3] = WALL_BULLET_DEAD_ZONE;
					pbullet_head->m_bullet_disapper = true;
				}
			}
			break;
		default:
			break;
		}

		pbullet_head = pbullet_head->m_bullet_next;
	}
}

static void DelBullet(PTANK ptank, PBULLET pbullet) {
	PBULLET pbullet_head = ptank->m_tank_bullet;

	if (pbullet_head == nullptr) {
		return;
	}

	if (pbullet_head == pbullet) {
		ptank->m_tank_bullet = ptank->m_tank_bullet->m_bullet_next;
		free(pbullet);
		pbullet = NULL;
		return;
	}
	
	while (pbullet_head->m_bullet_next != pbullet) {
		pbullet_head = pbullet_head->m_bullet_next;

		if (pbullet_head == nullptr) {
			return;
		}
	}

	pbullet_head->m_bullet_next = pbullet->m_bullet_next;
	free(pbullet);
	pbullet = NULL;
}

static void DelAllDisapperBullet(PTANK ptank) {
	PBULLET pbullet_head = ptank->m_tank_bullet;
	PBULLET pbullet_disapper = NULL;

	while (pbullet_head) {
		if (pbullet_head->m_bullet_disapper) {
			pbullet_disapper = pbullet_head;
			pbullet_head = pbullet_disapper->m_bullet_next;
			DelBullet(ptank, pbullet_disapper);
			continue;
		}
		pbullet_head = pbullet_head->m_bullet_next;
	}
}

void DelAllBullet(PTANK ptank) {
	PBULLET pbullet_head = ptank->m_tank_bullet;

	if (pbullet_head == nullptr) {
		return;
	}

	while (pbullet_head) {
		switch (pbullet_head->m_bullet_direction) {
		case UP:
			PrintChar(pbullet_head->m_bullet_nX, pbullet_head->m_bullet_nY - 2, " ", 0x0F);
			m_gMap[pbullet_head->m_bullet_nY - 2][pbullet_head->m_bullet_nX] = 0;
			break;
		case DOWN:
			PrintChar(pbullet_head->m_bullet_nX, pbullet_head->m_bullet_nY + 2, " ", 0x0F);
			m_gMap[pbullet_head->m_bullet_nY + 2][pbullet_head->m_bullet_nX] = 0;
			break;
		case LEFT:
			PrintChar(pbullet_head->m_bullet_nX - 2, pbullet_head->m_bullet_nY, " ", 0x0F);
			m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX - 2] = 0;
			break;
		case RIGHT:
			PrintChar(pbullet_head->m_bullet_nX + 2, pbullet_head->m_bullet_nY, " ", 0x0F);
			m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX + 2] = 0;
			break;
		default:
			break;
		}

		DelBullet(ptank, pbullet_head);
		pbullet_head = ptank->m_tank_bullet;
		if (pbullet_head == nullptr) {
			break;
		}

		/* Because DelBullet(ptank, pbullet_head) pbullet_head is the head -- Reginald.S */
		//pbullet_head = pbullet_head->m_bullet_next;
	}
}

void BulletMove(PTANK ptank) {
	PBULLET pbullet_head = ptank->m_tank_bullet;

	BulletDisapper(ptank);

	while (pbullet_head) {
		switch (pbullet_head->m_bullet_direction) {
		case UP:
			PrintChar(pbullet_head->m_bullet_nX, pbullet_head->m_bullet_nY - 2, " ", 0x0F);
			m_gMap[pbullet_head->m_bullet_nY - 2][pbullet_head->m_bullet_nX] = 0;
			if (!pbullet_head->m_bullet_disapper) {
				pbullet_head->m_bullet_nY--;
			}
			break;
		case DOWN:
			PrintChar(pbullet_head->m_bullet_nX, pbullet_head->m_bullet_nY + 2, " ", 0x0F);
			m_gMap[pbullet_head->m_bullet_nY + 2][pbullet_head->m_bullet_nX] = 0;
			if (!pbullet_head->m_bullet_disapper) {
				pbullet_head->m_bullet_nY++;
			}
			break;
		case LEFT:
			PrintChar(pbullet_head->m_bullet_nX - 2, pbullet_head->m_bullet_nY, " ", 0x0F);
			m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX - 2] = 0;
			if (!pbullet_head->m_bullet_disapper) {
				pbullet_head->m_bullet_nX--;
			}
			break;
		case RIGHT:
			PrintChar(pbullet_head->m_bullet_nX + 2, pbullet_head->m_bullet_nY, " ", 0x0F);
			m_gMap[pbullet_head->m_bullet_nY][pbullet_head->m_bullet_nX + 2] = 0;
			if (!pbullet_head->m_bullet_disapper) {
				pbullet_head->m_bullet_nX++;
			}
			break;
		default:
			break;
		}

		pbullet_head = pbullet_head->m_bullet_next;
	}

	DelAllDisapperBullet(ptank);

	PrintBullet(ptank);
}

int BulletNum(PTANK ptank) {
	int bullet_num = 0;
	PBULLET pbullet_head = ptank->m_tank_bullet;

	while (pbullet_head) {
		bullet_num++;
		pbullet_head = pbullet_head->m_bullet_next;
	}

	return bullet_num;
}

void BulletAdd(PTANK ptank, PBULLET pbullet) {
	PBULLET pbullet_head = ptank->m_tank_bullet;

	if (ptank->m_tank_bullet == nullptr) {
		ptank->m_tank_bullet = pbullet;
		return;
	}

	while (pbullet_head->m_bullet_next) {
		pbullet_head = pbullet_head->m_bullet_next;
	}

	pbullet_head->m_bullet_next = pbullet;
}
