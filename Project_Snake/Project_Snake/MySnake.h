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

/* we'd better use extern, otherwise, unexpected thing will happen*/
extern const char snake_head_str[][3];
extern const char snake_protected_head[3];
extern const char snake_color_str[];

class CMySnake {
public:
	CMySnake();
	~CMySnake();
	void Init(int nX, int nY, SNAKEDIR nDir);
	void ChangeProperty(char c);
	bool SnakeMove();
	void SnakeClear();
	void SaveSnakeInfo();
	bool ReadSnakeInfo();
	int m_time_interval;
	static char m_choose_level;
	static int m_time_blood;
	static int m_score;
	bool m_storm_coming;
	bool m_protected_head;
	char(*m_p_snake_head)[3];
	char* m_p_snake_color;

private:
	void SnakeErase();
	bool SnakeCanMove();
	void SnakeDraw();
	FOOD_TYPE m_eat_food;
	SNAKEDIR m_nDirection;
	vector<SNAKENODE> m_vecBody;
};
