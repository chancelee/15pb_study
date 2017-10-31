#include "MySnake.h"
#include "MyDraw.h"
#include "MyMap.h"
#include "MyFood.h"
#include <conio.h>


CMySnake::CMySnake() {
}


CMySnake::~CMySnake() {
}

bool CMySnake::m_snake_in_hole = false;

void CMySnake::Init(int nX, int nY, SNAKEDIR nDir) {
	CMyDraw snake_head_obj;
	SNAKENODE sn = { nX, nY };

	while (CMyMap::m_gMap[nY][nX] != 0) {
		sn.nX++;
		sn.nY++;
	}

	m_vecBody.push_back(sn);

	m_nDirection = nDir;

	snake_head_obj.WriteChar(nX, nY, ">", snake_color[0]);

	CMyMap::m_gMap[nY][nX] = WALL_SANKE;
}

void CMySnake::ChangeProperty(char control_key) {
	switch (control_key) {
	/* Direction */
	case 'K':
	case 'k':
		if (m_nDirection != DOWN || (m_vecBody.size() == 1)) {
			m_nDirection = UP;
		}
		break;
	case 'J':
	case 'j':
		if (m_nDirection != UP || (m_vecBody.size() == 1)) {
			m_nDirection = DOWN;
		}
		break;
	case 'H':
	case 'h':
		if (m_nDirection != RIGHT || (m_vecBody.size() == 1)) {
			m_nDirection = LEFT;
		}
		break;
	case 'L':
	case 'l':
		if (m_nDirection != LEFT || (m_vecBody.size() == 1)) {
			m_nDirection = RIGHT;
		}
		break;
	/* Speed */
	case 'V':
	case 'v':
		m_time_interval -= 50;
		if (m_time_interval <= 0) {
			m_time_interval = 300;
		}
		break;
	case 'F':
	case 'f':
		m_time_interval += 20;
		break;
	/* Pause */
	case 'P':
	case 'p':
		_getch();
		break;
	default:
		break;
	}
}

void CMySnake::SnakeErase() {
	CMyDraw snake_obj;
	int body_count = m_vecBody.size();

	for (int i = 0; i < body_count; i++) {
		/* food is random, when snake move, the body maybe at the same location
		 * so, we need deal with such occasion
		 */
		if (CMyMap::m_gMap[m_vecBody[i].nY][m_vecBody[i].nX] <= FOOD_ST || 
			CMyMap::m_gMap[m_vecBody[i].nY][m_vecBody[i].nX] >= FOOD_ED) {
			snake_obj.WriteChar(m_vecBody[i].nX, m_vecBody[i].nY, "  ", 0);
			CMyMap::m_gMap[m_vecBody[i].nY][m_vecBody[i].nX] = 0;
		}
	}
}

void CMySnake::SnakeDraw() {
	CMyDraw snake_obj;
	int body_count = m_vecBody.size();
	static char (*p_head)[3] = (char(*)[3])snake_head;
	static char* p_color = (char*)snake_color;

	switch (m_eat_food) {
	case FOOD_STAR:
		break;
	case FOOD_STORM:
		m_storm_coming = !m_storm_coming;
		break;
	case FOOD_ADD_BLOOD:
		break;
	case FOOD_CHANGE_HEAD:
		p_head++;
		if (p_head > snake_head + sizeof(snake_head) / sizeof(snake_head[0]) - 1) {
			p_head = (char(*)[3])snake_head;
		}
		m_protected_head = false;
		break;
	case FOOD_PROTECT_HEAD:
		m_protected_head = true;
		break;
	case FOOD_ADD_TWO_NODE:
		break;
	case FOOD_CHANGE_COLOR:
		p_color++;
		if (p_color > snake_color + sizeof(snake_color) / sizeof(snake_color[0]) - 1) {
			p_color = (char*)snake_color;
		}
		break;
	default:
		break;
	}
	m_eat_food = FOOD_ST;

	snake_obj.WriteChar(
		m_vecBody[0].nX, 
		m_vecBody[0].nY, 
		m_protected_head ? (char*)snake_protected_head: (char*)p_head, 
		*p_color);
	CMyMap::m_gMap[m_vecBody[0].nY][m_vecBody[0].nX] = WALL_SANKE;

	for (int i = 1; i < body_count; i++) {
		/* food is random, when snake move, the body maybe at the same location
		 * so, we need deal with such occasion
		 */
		if (CMyMap::m_gMap[m_vecBody[i].nY][m_vecBody[i].nX] <= FOOD_ST || 
			CMyMap::m_gMap[m_vecBody[i].nY][m_vecBody[i].nX] >= FOOD_ED) {
			snake_obj.WriteChar(m_vecBody[i].nX, m_vecBody[i].nY, "-", *p_color);
			CMyMap::m_gMap[m_vecBody[i].nY][m_vecBody[i].nX] = WALL_SANKE;
		}
	}
}

bool CMySnake::SnakeMove() {
	int snake_size = 0;

	// Before move, check first
	if (!SnakeCanMove()) {
		MessageBox(NULL, L"U R LOSER", L"Game Over", MB_OK);
		return false;
	}

	// first erase
	SnakeErase();

	// Then Move
	snake_size = m_vecBody.size();
	/* except snake head, all of node should move forward */
	for (int i = snake_size - 2; i >= 0; i--) {
		m_vecBody[i + 1] = m_vecBody[i];
	}

	switch (m_nDirection) {
	case UP:
		m_vecBody[0].nY--;
		break;
	case DOWN:
		m_vecBody[0].nY++;
		break;
	case LEFT:
		m_vecBody[0].nX--;
		break;
	case RIGHT:
		m_vecBody[0].nX++;
		break;
	default:
		break;
	}

	// Finally Rewrite
	SnakeDraw();

	return true;
}

bool CMySnake::SnakeCanMove() {
	SNAKENODE snake_head = m_vecBody[0];
	/* no specific meaning, when draw, will be overwrite */
	SNAKENODE add_food_node = {5,5};
	static CMyFood food_storage_obj;
	CMyMap snake_hole_map;
	CMyDraw snake_hole_draw_obj;

	switch (m_nDirection) {
	case UP:
		snake_head.nY--;
		break;
	case DOWN:
		snake_head.nY++;
		break;
	case LEFT:
		snake_head.nX--;
		break;
	case RIGHT:
		snake_head.nX++;
		break;
	default:
		break;
	}

	switch (CMyMap::m_gMap[snake_head.nY][snake_head.nX]) {
	case FOOD_STAR:
		m_eat_food = FOOD_STAR;
		goto snake_add_node;
	case FOOD_STORM:
		m_eat_food = FOOD_STORM;
		goto snake_add_node;
	case FOOD_ADD_BLOOD:
		m_eat_food = FOOD_ADD_BLOOD;
		goto snake_add_node;
	case FOOD_CHANGE_HEAD:
		m_eat_food = FOOD_CHANGE_HEAD;
		goto snake_add_node;
	case FOOD_PROTECT_HEAD:
		m_eat_food = FOOD_PROTECT_HEAD;
		goto snake_add_node;
	case FOOD_ADD_TWO_NODE:
		m_eat_food = FOOD_ADD_TWO_NODE;
		goto snake_add_node;
	case FOOD_CHANGE_COLOR:
		m_eat_food = FOOD_CHANGE_COLOR;
		goto snake_add_node;
	case SNAKE_HOLE:
		m_snake_in_hole = !m_snake_in_hole;
		if (m_snake_in_hole) {
			for (int i = 0; i < MAPROW; i++) {
				for (int j = 0; j < MAPCOL; j++) {
					CMyMap::m_gMap_storage[i][j] = CMyMap::m_gMap[i][j];
					CMyMap::m_gMap[i][j] = 0;
				}
			}
			food_storage_obj.m_food_count_storage = food_storage_obj.m_food_count;
			food_storage_obj.m_food_count = 0;

			system("cls");
			snake_hole_map.InitMap();
			snake_hole_draw_obj.WriteChar(1, 1, "  ", 0);
			snake_hole_draw_obj.WriteChar(1, 1, (char*)bound_str[1], COLOR_SNAKE_HOLE);
			CMyMap::m_gMap[1][1] = SNAKE_HOLE;
			Init(1, 1, RIGHT);
		} else {
			for (int i = 0; i < MAPROW; i++) {
				for (int j = 0; j < MAPCOL; j++) {
					CMyMap::m_gMap[i][j] = CMyMap::m_gMap_storage[i][j];
				}
			}
			food_storage_obj.m_food_count = food_storage_obj.m_food_count_storage;

			system("cls");
			snake_hole_map.InitMap();
			Arc();
		}
		break;
	case WALL_INNER_BOUND:
		if (m_protected_head) {
			CMyMap::m_gMap[snake_head.nY][snake_head.nX] = 0;
			break;
		}
		return false;
	case WALL_BOUND:
	case WALL_SANKE:
		return false;
	default:
		break;
	}

	return true;

snake_add_node:
	m_vecBody.push_back(add_food_node);
	CMyFood::m_food_count--;
	CMyMap::m_gMap[snake_head.nY][snake_head.nX] = 0;

	return true;
}
