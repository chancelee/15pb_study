#pragma once
#include <vector>
#include "MyFood.h"

using namespace std;

typedef struct _SNAKENODE {
	int nX;
	int nY;
} SNAKENODE, *PSNAKENODE;

typedef enum _SNAKEDIR {
	UP,
	DOWN,
	LEFT,
	RIGHT
} SNAKEDIR;

const char snake_head[][3] = {">", "¡ñ", "*"};
const char snake_protected_head[3] = "¡÷";
const char snake_color[] = { 0x07, 0x02, 0x03, 0x04, 0x05, 0x06 };

class CMySnake {
public:
	CMySnake();
	~CMySnake();
	void Init(int nX, int nY, SNAKEDIR nDir);
	void ChangeProperty(char c);
	bool SnakeMove();
	int m_time_interval;
	bool m_storm_coming;
	bool m_protected_head;
	static bool m_snake_in_hole;

private:
	void SnakeErase();
	bool SnakeCanMove();
	void SnakeDraw();
	FOOD_TYPE m_eat_food;
	SNAKEDIR m_nDirection;
	vector<SNAKENODE> m_vecBody;
};
