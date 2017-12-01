#include "Game.h"
#include "ConsoleOperate.h"
#include "welcome.h"
#include "C_Map.h"
#include "FileOperate.h"
#include "EngineManage.h"
#include "Astar.h"
#include "Bullet.h"
#include <time.h>
#include <stdlib.h>
#include <conio.h>

#pragma comment(lib, "winmm.lib")

#define KEY_DOWN(vk_code)	((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code)		((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

bool CGame::g_restart_game = true;
char CGame::m_choose_level = 4;
int CGame::m_enermy_count = 2;
int CGame::m_cur_enrmy_count = 2;
int CGame::m_recovery_count = 4;

CGame::CGame() {
}

CGame::~CGame() {
}

void CGame::StartGame(CTank& ptank, CEngineManage& engine_manage_obj) {
	CConsoleOperate console_obj;
	CWelcome game_obj;
	C_Map map_obj;

	srand((unsigned int)time(NULL));

	console_obj.SetWindowSize("Tank", SCREEN_WEIGHT, SCREEN_HEIGHT);

	CGame::g_restart_game = false;

	PlaySoundA("Tank.wav", NULL, SND_ASYNC | SND_NODEFAULT);
	game_obj.NewGame();
	if (ReadGame(ptank, engine_manage_obj) && !game_obj.StartNewGame()) {
		system("cls");
		ptank.PrintTank();
		// Init map
		map_obj.InitMap(false);
	} else {
		// Clear map buffer here, must before SetGame
		map_obj.ClearMap();
		game_obj.SetGame();
		ptank.DestroyTank();
		engine_manage_obj.ClearEngine();

		// Init map
		map_obj.InitMap(true);

		ptank.m_tank_blood = 10;
		ptank.m_tank_score = 0;
		ptank.InitTank(20, 30, UP, 0x0F);
		m_recovery_count = 4;
		m_cur_enrmy_count = m_enermy_count;

		for (int i = 0; i < m_enermy_count; i++) {
			CTank* tank_enermy = new CTank();
			tank_enermy->m_p_record = tank_enermy;
			if (tank_enermy->GenerateTank()) {
				engine_manage_obj.AddEngine(tank_enermy);
			}
		}

		engine_manage_obj.AddEngine(&ptank);
	}
}

int CGame::PlayGame(CTank& ptank, CEngineManage& engine_manage_obj) {
	int up_count = 0;
	int down_count = 0;
	int left_count = 0;
	int right_count = 0;
	int bullet_count = 0;
	CWelcome game_obj;
	clock_t t_begin_move, t_end_move;
	C_Map map_obj;

	/*PlaySoundA("Tank_move.wav", NULL, SND_ASYNC | SND_NODEFAULT | SND_LOOP);*/

	t_begin_move = clock();

#if 1
	clock_t t_tank_move_start, t_tank_move_end;
	CTank tank_enermy_03;
	COORD temp_cord = {};
	tank_enermy_03.GenerateTank();
	temp_cord.X = tank_enermy_03.GetCurCoord().X;
	temp_cord.Y = tank_enermy_03.GetCurCoord().Y;
	CAstar astar_obj;
	astar_obj.Init(temp_cord, ptank.GetCurCoord());
	if (astar_obj.FindPath()) {
		astar_obj.GetPath();
		//astar_obj.PrintPath();
	}

	t_tank_move_start = clock();
#endif

	while (1) {
		if (_kbhit()) {
			if (KEY_DOWN('K') || KEY_DOWN('k')) {
				up_count++;

				if (up_count > 400) {
					up_count = 0;
					engine_manage_obj.ManageEngine(ptank);
					ptank.Move(UP);
					/*astar_obj.Init(tank_enermy_03.GetCurCoord(), ptank.GetCurCoord());*/
					astar_obj.Init(tank_enermy_03.GetCurCoord(), ptank.GetCurCoord());
					if (astar_obj.FindPath()) {
						astar_obj.GetPath();
						//astar_obj.PrintPath();
					}
				}
			} else if (KEY_DOWN('J') || KEY_DOWN('j')) {
				down_count++;

				if (down_count > 400) {
					down_count = 0;
					engine_manage_obj.ManageEngine(ptank);
					ptank.Move(DOWN);
					/*astar_obj.Init(tank_enermy_03.GetCurCoord(), ptank.GetCurCoord());*/
					astar_obj.Init(tank_enermy_03.GetCurCoord(), ptank.GetCurCoord());
					if (astar_obj.FindPath()) {
						astar_obj.GetPath();
						//astar_obj.PrintPath();
					}
				}
			} else if (KEY_DOWN('H') || KEY_DOWN('h')) {
				left_count++;

				if (left_count > 400) {
					left_count = 0;
					engine_manage_obj.ManageEngine(ptank);
					ptank.Move(LEFT);
					/*astar_obj.Init(tank_enermy_03.GetCurCoord(), ptank.GetCurCoord());*/
					astar_obj.Init(tank_enermy_03.GetCurCoord(), ptank.GetCurCoord());
					if (astar_obj.FindPath()) {
						astar_obj.GetPath();
						//astar_obj.PrintPath();
					}
				}
			} else if (KEY_DOWN('L') || KEY_DOWN('l')) {
				right_count++;

				if (right_count > 400) {
					right_count = 0;
					engine_manage_obj.ManageEngine(ptank);
					ptank.Move(RIGHT);
					/*astar_obj.Init(tank_enermy_03.GetCurCoord(), ptank.GetCurCoord());*/
					astar_obj.Init(tank_enermy_03.GetCurCoord(), ptank.GetCurCoord());
					if (astar_obj.FindPath()) {
						astar_obj.GetPath();
						//astar_obj.PrintPath();
					}
				}
			}

			if (KEY_DOWN('V') || KEY_DOWN('v')) {
				bullet_count++;

				if (bullet_count > 400) {
					bullet_count = 0;
					CBullet* bullet_obj = new CBullet();
					bullet_obj->m_p_record = bullet_obj;
					bullet_obj->GenerateBullet(ptank, engine_manage_obj);
				}
			}

			if (KEY_DOWN('P') || KEY_DOWN('p')) {
				/* The first receive character p, another pause */
				_getch();
				_getch();
			}

			if (KEY_DOWN('Q') || KEY_DOWN('q')) {
				tank_enermy_03.DestroyTank();
				SaveGame(ptank, engine_manage_obj);
				return 0;
			}
		}

		if (!engine_manage_obj.IsEngineEmpty()) {
			t_end_move = clock();

			engine_manage_obj.ManageEngine();
			engine_manage_obj.DstDirectionChange();

			if (t_end_move - t_begin_move > 1) {
				t_begin_move = t_end_move;
				engine_manage_obj.IntervalManage();
			}
		}

		if (m_cur_enrmy_count != m_enermy_count && m_recovery_count) {
			m_cur_enrmy_count = m_enermy_count;
			CTank* tank_enermy = new CTank();
			tank_enermy->m_p_record = tank_enermy;
			if (tank_enermy->GenerateTank()) {
				engine_manage_obj.AddEngine(tank_enermy);
			}
		}

		if (!m_cur_enrmy_count) {
			MessageBox(0, L"    U Win -_-", L"Congratulations", MB_OK);
			RestartGame(ptank, engine_manage_obj);
		}

#if 1
		t_tank_move_end = clock();
		if (t_tank_move_end - t_tank_move_start > 400) {
			t_tank_move_start = t_tank_move_end;
			if (!astar_obj.IsPathEmpty()) {
				tank_enermy_03.m_dst_direction = astar_obj.GetDirection();
				if (!tank_enermy_03.TankCanMove(tank_enermy_03.m_dst_direction)) {
					tank_enermy_03.m_dst_direction = (DIRECTION)(rand() % 4);
					tank_enermy_03.Move(tank_enermy_03.m_dst_direction);
					astar_obj.Init(tank_enermy_03.GetCurCoord(), ptank.GetCurCoord());
					if (astar_obj.FindPath()) {
						astar_obj.GetPath();
						//astar_obj.PrintPath();
					}
				} else {
					tank_enermy_03.Move(tank_enermy_03.m_dst_direction);
				}
			}
		}
#endif

		if (!ptank.m_is_tank_alive) {
			game_obj.GameOver();

			if (CGame::g_restart_game) {
				RestartGame(ptank, engine_manage_obj);
			} else {
				/* Destroy Tank&&Bullet In Engine */
				engine_manage_obj.ClearEngine();
				return 0;
			}
		}
	}
}

// Remap
void CGame::RestartGame(CTank& ptank, CEngineManage& engine_manage_obj) {
	/* Destroy Tank In Engine */
	engine_manage_obj.ClearEngine();

	for (int i = 0; i < MAPCOL; i++) {
		for (int j = 0; j < MAPROW; j++) {
			C_Map::m_gMap[j][i] = 0;
		}
	}

	system("cls");
	ptank.m_tank_blood = 10;
	StartGame(ptank, engine_manage_obj);
}

void CGame::SaveGame(CTank& ptank, CEngineManage& engine_manage_obj) {
	CFileOperate file_obj;

	file_obj.SaveTankInfo(engine_manage_obj);

	/* Destroy Tank In Engine */
	engine_manage_obj.ClearEngine();
}

bool CGame::ReadGame(CTank& ptank, CEngineManage& engine_manage_obj) {
	CFileOperate file_obj;

#if 1
	return file_obj.ReadTankInfo(ptank, engine_manage_obj);
#else
	return false;
#endif
}
