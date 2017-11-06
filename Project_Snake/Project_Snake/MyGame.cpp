#include "MyGame.h"
#include "MyDraw.h"
#include <time.h>
#include <conio.h>

const int snake_init_x_coor = 10;
const int snake_init_y_coor = 10;
const int snake_init_speed = 300;
const SNAKEDIR snake_init_dir = UP;

CMyGame::CMyGame() {
}


CMyGame::~CMyGame() {
}

// Map
void CMyGame::StartGame(CMySnake& snake_obj, CMyFood& food_obj, CMyMap& map_obj, CMyWelcome& game_welcome_obj) {
	srand((unsigned int)time(NULL));

	map_obj.SetWindowSize("Snake", SCREEN_WEIGHT, SCREEN_HEIGHT);

	game_welcome_obj.restart_game = false;
	game_welcome_obj.NewGame();
	if (ReadGame(snake_obj) && !game_welcome_obj.StartNewGame()) {
		system("cls");
		// Init map
		map_obj.InitMap(false);
	} else {
		// Clear map buffer here, must before SetGame
		map_obj.ClearMap();
		game_welcome_obj.SetGame();
		food_obj.m_food_count = 0;
		// Init map
		map_obj.InitMap(true);

		// Init snake coordinate and direction
		snake_obj.SnakeClear();
		snake_obj.Init(snake_init_x_coor, snake_init_y_coor, snake_init_dir);
		if (CMySnake::m_choose_level == '1') {
			snake_obj.m_time_interval = snake_init_speed;
		}
		else if (CMySnake::m_choose_level == '2') {
			snake_obj.m_time_interval = snake_init_speed - 100;
		}
		else if (CMySnake::m_choose_level == '3') {
			snake_obj.m_time_interval = snake_init_speed - 200;
		}
		snake_obj.m_storm_coming = false;
		snake_obj.m_protected_head = false;
		snake_obj.m_p_snake_head = (char(*)[3])snake_head_str;
		snake_obj.m_p_snake_color = (char*)snake_color_str;
		snake_obj.m_score = 0;
		snake_obj.m_time_blood = 100;
	}
}

int CMyGame::PlayGame(CMySnake& snake_obj, CMyFood& food_obj, CMyMap& map_obj, CMyWelcome& game_welcome_obj) {
	// time begin
	clock_t tBegin = clock(), tEnd;
	char control_key;

	while (1) {
		if (_kbhit()) {
			control_key = _getch();
			if (control_key == 'q' || control_key == 'Q') {
				//TBD...
				SaveGame(snake_obj);
				return 0;
			}
			snake_obj.ChangeProperty(control_key);
		}

		// Generate Food
		if (food_obj.IsFoodEmpty()) {
			food_obj.RandomFood();
			food_obj.DrawFood();
			if (snake_obj.m_storm_coming) {
				map_obj.StormComing();
			}
		}

		map_obj.DrawPanel();

		// Snake move time
		tEnd = clock();

		// 300 ms
		if (tEnd - tBegin > snake_obj.m_time_interval) {
			tBegin = tEnd;
			if (!snake_obj.SnakeMove()) {
				// Storage Information
				game_welcome_obj.GameOver();

				if (game_welcome_obj.restart_game) {
					RestartGame(snake_obj, food_obj, map_obj, game_welcome_obj);
				} else {
					//TBD..
					//RecordGame();
					return 0;
				}
			}
		}
	}
}

// Remap
void CMyGame::RestartGame(CMySnake& snake_obj, CMyFood& food_obj, CMyMap& map_obj, CMyWelcome& game_welcome_obj) {
	for (int i = 0; i < MAPCOL; i++) {
		for (int j = 0; j < MAPROW; j++) {
			CMyMap::m_gMap[j][i] = 0;
		}
	}

	system("cls");
	snake_obj.SnakeClear();
	snake_obj.m_time_blood = 100;
	snake_obj.m_score = 0;
	food_obj.m_food_count = 0;
	StartGame(snake_obj, food_obj, map_obj, game_welcome_obj);
}

void CMyGame::SaveGame(CMySnake& snake_obj) {
	snake_obj.SaveSnakeInfo();
}

bool CMyGame::ReadGame(CMySnake& snake_obj) {
	return snake_obj.ReadSnakeInfo();
}
