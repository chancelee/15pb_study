#pragma once
#include "MySnake.h"
#include "MyMap.h"
#include "MyFood.h"
#include "MyWelcome.h"

#ifndef SCREEN_WEIGHT
#define SCREEN_WEIGHT	122
#endif

#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT	35
#endif

extern const int snake_init_x_coor;
extern const int snake_init_y_coor;
extern const int snake_init_speed;
extern const SNAKEDIR snake_init_dir;

class CMyGame {
public:
	CMyGame();
	~CMyGame();
	void StartGame(CMySnake& snake_obj, CMyFood& food_obj, CMyMap& map_obj, CMyWelcome& game_welcome_obj);
	int PlayGame(CMySnake& snake_obj, CMyFood& food_obj, CMyMap& map_obj, CMyWelcome& game_welcome_obj);
	void RestartGame(CMySnake& snake_obj, CMyFood& food_obj, CMyMap& map_obj, CMyWelcome& game_welcome_obj);
	void SaveGame(CMySnake& snake_obj);
	bool ReadGame(CMySnake& snake_obj);
};
