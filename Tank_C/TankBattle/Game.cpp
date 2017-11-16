#include "WindowsConsole.h"
#include "welcome.h"
#include "Tank.h"
#include "bullet.h"
#include "map.h"
#include "FileOperate.h"
#include "StupidSniff.h"
#include <time.h>
#include <stdlib.h>
#include <conio.h>

#pragma comment(lib, "winmm.lib")

#define KEY_DOWN(vk_code)	((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)		((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

bool g_restart_game = true;

void SaveGame(PTANK ptank, PTANK ptank_enermy);
bool ReadGame(PTANK ptank, PTANK ptank_enermy);
void RestartGame(PTANK ptank, PTANK ptank_enermy);

extern int g_enermy_count;

void StartGame(PTANK ptank, PTANK ptank_enermy) {
	g_enermy_count = 2;

	srand((unsigned int)time(NULL));

	SetWindowSize("Tank", SCREEN_WEIGHT, SCREEN_HEIGHT);

	g_restart_game = false;

	ptank->m_tank_bullet = nullptr;
	for (int i = 0; i < 2; i++) {
		ptank_enermy[i].m_tank_bullet = nullptr;
	}

	PlaySoundA("Tank.wav", NULL, SND_ASYNC | SND_NODEFAULT);
	NewGame();
	if (ReadGame(ptank, ptank_enermy) && !StartNewGame()) {
		system("cls");
		PrintTank(ptank);

		for (int i = 0; i < 2; i++) {
			if (ptank_enermy[i].m_is_tank_alive) {
				PrintTank(&ptank_enermy[i]);
			}
		}
		// Init map
		InitMap(false);
	} else {
		// Clear map buffer here, must before SetGame
		ClearMap();
		SetGame();

		InitTank(ptank, 5, 10, UP, 0x0F);
		ptank->m_tank_enermy_flag = false;
		ptank->m_is_tank_alive = true;

		for (int i = 0; i < 2; i++) {
			ptank_enermy[i].m_tank_enermy_flag = true;
			ptank_enermy[i].m_is_tank_alive = true;
			ptank_enermy[i].m_go_direction = (DIRECTION)(rand() % 4);
			ptank_enermy[i].m_change_direction_count = 0;
			InitTank(&ptank_enermy[i], 10 + 6 * i, 10 + 2 * (i + 2), (DIRECTION)(rand() % 4), 0x05);
		}

		// Init map
		InitMap(true);
	}
}

int PlayGame(PTANK ptank, PTANK ptank_enermy) {
	clock_t tBegin_enermy_tank_move, tEnd_enermy_tank_move;
	clock_t tBegin_enermy_tank_move_loop, tEnd_enermy_tank_move_loop;
	clock_t tBegin_bullet_move, tEnd_bullet_move;
	int up_count = 0;
	int down_count = 0;
	int left_count = 0;
	int right_count = 0;
	int bullet_count = 0;

	tBegin_enermy_tank_move = clock();
	tBegin_enermy_tank_move_loop = clock();
	tBegin_bullet_move = clock();

	/*PlaySoundA("Tank_move.wav", NULL, SND_ASYNC | SND_NODEFAULT | SND_LOOP);*/

	while (1) {
		if (_kbhit()) {
			if (KEY_DOWN('K') || KEY_DOWN('k')) {
				up_count++;

				if (up_count > 1000) {
					up_count = 0;
					if (ptank->m_tank_bullet) {
						BulletMove(ptank);
					}
					MoveTank(ptank, UP);
				}
			}

			if (KEY_DOWN('J') || KEY_DOWN('j')) {
				down_count++;

				if (down_count > 1000) {
					down_count = 0;
					if (ptank->m_tank_bullet) {
						BulletMove(ptank);
					}
					MoveTank(ptank, DOWN);
				}
			}

			if (KEY_DOWN('H') || KEY_DOWN('h')) {
				left_count++;

				if (left_count > 1000) {
					left_count = 0;
					if (ptank->m_tank_bullet) {
						BulletMove(ptank);
					}
					MoveTank(ptank, LEFT);
				}
			}

			if (KEY_DOWN('L') || KEY_DOWN('l')) {
				right_count++;

				if (right_count > 1000) {
					right_count = 0;
					if (ptank->m_tank_bullet) {
						BulletMove(ptank);
					}
					MoveTank(ptank, RIGHT);
				}
			}

			if (KEY_DOWN('V') || KEY_DOWN('v')) {
				bullet_count++;

				if (bullet_count > 600) {
					bullet_count = 0;
					GenerateBullet(ptank);
				}
			}

			if (KEY_DOWN('P') || KEY_DOWN('p')) {
				/* The first receive character p, another pause */
				_getch();
				_getch();
			}

			if (KEY_DOWN('Q') || KEY_DOWN('q')) {
				SaveGame(ptank, ptank_enermy);
				return 0;
			}
		}

		tEnd_enermy_tank_move = clock();
		tEnd_enermy_tank_move_loop = clock();
		tEnd_bullet_move = clock();

		if (tEnd_bullet_move - tBegin_bullet_move > 20) {
			tBegin_bullet_move = tEnd_bullet_move;

			if (ptank->m_tank_bullet) {
				BulletMove(ptank);
			}

			for (int i = 0; i < 2; i++) {
				if (ptank_enermy[i].m_tank_bullet) {
					BulletMove(&ptank_enermy[i]);
					if (!ShouldTankAlive(ptank)) {
						MoveTank(ptank, ptank->m_tank_direction);
					}
				}
			}
		}

		if (tEnd_enermy_tank_move - tBegin_enermy_tank_move > 300) {
			tBegin_enermy_tank_move = tEnd_enermy_tank_move;

			for (int i = 0; i < 2; i++) {
				ptank_enermy[i].m_change_direction_count++;

				GenerateBullet(&ptank_enermy[i]);

				if (ptank_enermy[i].m_tank_bullet) {
					BulletMove(&ptank_enermy[i]);
					if (!ShouldTankAlive(ptank)) {
						MoveTank(ptank, ptank->m_tank_direction);
					}
				}

				if (!MoveTank(&ptank_enermy[i], ptank_enermy[i].m_go_direction) || ptank_enermy[i].m_change_direction_count >= 8) {
					ptank_enermy[i].m_change_direction_count = 0;
					switch (StupidSniff(&ptank_enermy[i], 8)) {
					case TEND_LEFT | TEND_UP:
						ptank_enermy[i].m_go_direction = (rand() % 2 ? LEFT : UP);
						break;
					case TEND_RIGHT | TEND_UP:
						ptank_enermy[i].m_go_direction = (rand() % 2 ? RIGHT : UP);
						break;
					case TEND_LEFT | TEND_DOWN:
						ptank_enermy[i].m_go_direction = (rand() % 2 ? LEFT : DOWN);
						break;
					case TEND_RIGHT | TEND_DOWN:
						ptank_enermy[i].m_go_direction = (rand() % 2 ? RIGHT : DOWN);
						break;
					default:
						ptank_enermy[i].m_go_direction = (DIRECTION)(rand() % 4);
						break;
					}
				}
			}
		}

		if (tEnd_enermy_tank_move_loop - tBegin_enermy_tank_move_loop > 4000) {
			tBegin_enermy_tank_move_loop = tEnd_enermy_tank_move_loop;

			for (int i = 0; i < 2; i++) {
				ptank_enermy[i].m_go_direction = (DIRECTION)(rand() % 4);
			}
			/*PlaySoundA("Tank_move.wav", NULL, SND_ASYNC | SND_NODEFAULT | SND_LOOP);*/
		}

		if (!ptank->m_is_tank_alive) {
			GameOver();

			if (g_restart_game) {
				RestartGame(ptank, ptank_enermy);
			} else {
				return 0;
			}
		}
	}
}

// Remap
void RestartGame(PTANK ptank, PTANK ptank_enermy) {
	for (int i = 0; i < MAPCOL; i++) {
		for (int j = 0; j < MAPROW; j++) {
			m_gMap[j][i] = 0;
		}
	}

	for (int i = 0; i < 2; i++) {
		if (ptank_enermy[i].m_is_tank_alive) {
			DestroyTank(&ptank_enermy[i]);
		}
	}

	if (ptank->m_is_tank_alive) {
		DestroyTank(ptank);
	}

	system("cls");
	StartGame(ptank, ptank_enermy);
}

void SaveGame(PTANK ptank, PTANK ptank_enermy) {
	SaveTankInfo(ptank, ptank_enermy);

	for (int i = 0; i < 2; i++) {
		if (ptank_enermy[i].m_is_tank_alive) {
			DestroyTank(&ptank_enermy[i]);
		}
	}

	if (ptank->m_is_tank_alive) {
		DestroyTank(ptank);
	}
}

bool ReadGame(PTANK ptank, PTANK ptank_enermy) {
	return ReadTankInfo(ptank, ptank_enermy);
}
