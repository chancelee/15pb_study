#include <iostream>
#include "MyMap.h"
#include "MySnake.h"
#include "MyFood.h"
#include <time.h>
#include <conio.h>

#ifndef SCREEN_WEIGHT
#define SCREEN_WEIGHT	90
#endif

#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT	40
#endif

const int snake_init_x_coor = 10;
const int snake_init_y_coor = 10;
const int snake_init_speed = 300;
const SNAKEDIR snake_init_dir = UP;

using namespace std;

int main(int argc, char** argv) {
	CMyMap map_obj;
	CMyFood food_obj;
	CMySnake snake_obj;

	srand((unsigned int) time(NULL));

	// Init map
	map_obj.SetWindowSize("Snake", SCREEN_WEIGHT, SCREEN_HEIGHT);
	map_obj.InitMap();

	// Init snake coordinate and direction
	snake_obj.Init(snake_init_x_coor, snake_init_y_coor, snake_init_dir);
	snake_obj.m_time_interval = snake_init_speed;
	snake_obj.m_storm_coming = false;
	snake_obj.m_protected_head = false;
	snake_obj.m_snake_in_hole = false;

	// time begin
	clock_t tBegin = clock(), tEnd;

	while (1) {
		if (_kbhit()) {
			snake_obj.ChangeProperty(_getch());
		}

		// Generate Food
		if (food_obj.IsFoodEmpty()) {
			food_obj.RandomFood();
			food_obj.DrawFood();
			if (snake_obj.m_storm_coming) {
				map_obj.StormComing();
			}
		}

		// Snake move time
		tEnd = clock();
		
		// 300 ms
		if (tEnd - tBegin > snake_obj.m_time_interval) {
			tBegin = tEnd;
			if (!snake_obj.SnakeMove()) {
				return 0;
			}
		}
	}

	system("pause");

	return 0;
}
