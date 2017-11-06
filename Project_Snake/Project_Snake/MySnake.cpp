#include "MySnake.h"
#include "MyDraw.h"
#include "MyMap.h"
#include "MyFood.h"
#include "MyGame.h"
#include <conio.h>
#include <fstream>

char CMySnake::m_choose_level = 0;
int CMySnake::m_time_blood = 100;
int CMySnake::m_score = 0;

const char snake_head_str[][3] = { ">", "¡ñ", "*" };
const char snake_protected_head[3] = "¡÷";
const char snake_color_str[] = { 0x07, 0x02, 0x03, 0x04, 0x05, 0x06 };

CMySnake::CMySnake() {
}


CMySnake::~CMySnake() {
}

void CMySnake::Init(int nX, int nY, SNAKEDIR nDir) {
	CMyDraw snake_head_obj;
	SNAKENODE sn = { nX, nY };

	while (CMyMap::m_gMap[nY][nX] != 0) {
		sn.nX++;
		sn.nY++;
	}

	m_vecBody.push_back(sn);

	m_nDirection = nDir;

	snake_head_obj.WriteChar(nX, nY, ">", snake_color_str[0]);

	CMyMap::m_gMap[nY][nX] = WALL_SANKE;
}

void CMySnake::ChangeProperty(char control_key) {
	CMyDraw test_draw_obj;

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

	switch (m_eat_food) {
	case FOOD_STAR:
		break;
	case FOOD_STORM:
		m_storm_coming = !m_storm_coming;
		break;
	case FOOD_ADD_BLOOD:
		break;
	case FOOD_CHANGE_HEAD:
		m_p_snake_head++;
		if (m_p_snake_head > snake_head_str + sizeof(snake_head_str) / sizeof(snake_head_str[0]) - 1) {
			m_p_snake_head = (char(*)[3])snake_head_str;
		}
		m_protected_head = false;
		break;
	case FOOD_PROTECT_HEAD:
		m_protected_head = true;
		break;
	case FOOD_ADD_TWO_NODE:
		break;
	case FOOD_CHANGE_COLOR:
		m_p_snake_color++;
		if (m_p_snake_color > snake_color_str + sizeof(snake_color_str) / sizeof(snake_color_str[0]) - 1) {
			m_p_snake_color = (char*)snake_color_str;
		}
		break;
	default:
		break;
	}
	m_eat_food = FOOD_ST;

	snake_obj.WriteChar(
		m_vecBody[0].nX, 
		m_vecBody[0].nY, 
		m_protected_head ? (char*)snake_protected_head : (char*)m_p_snake_head,
		*m_p_snake_color);
	CMyMap::m_gMap[m_vecBody[0].nY][m_vecBody[0].nX] = WALL_SANKE;

	for (int i = 1; i < body_count; i++) {
		/* food is random, when snake move, the body maybe at the same location
		 * so, we need deal with such occasion
		 */
		if (CMyMap::m_gMap[m_vecBody[i].nY][m_vecBody[i].nX] <= FOOD_ST || 
			CMyMap::m_gMap[m_vecBody[i].nY][m_vecBody[i].nX] >= FOOD_ED) {
			snake_obj.WriteChar(m_vecBody[i].nX, m_vecBody[i].nY, "-", *m_p_snake_color);
			CMyMap::m_gMap[m_vecBody[i].nY][m_vecBody[i].nX] = WALL_SANKE;
		}
	}
}

bool CMySnake::SnakeMove() {
	int snake_size = 0;

	m_time_blood--;

	if (m_time_blood == 0) {
		return false;
	}

	// Before move, check first
	if (!SnakeCanMove()) {
/*		MessageBox(NULL, L"U R LOSER", L"Game Over", MB_OK);*/
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
		m_score += 3;
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
	case WALL_INNER_BOUND:
		if (m_protected_head) {
			CMyMap::m_gMap[snake_head.nY][snake_head.nX] = 0;
			break;
		}
		return false;
	case WALL_BOUND_THREE:
	case WALL_BOUND_TWO:
	case WALL_BOUND_ONE:
	case WALL_SANKE:
		return false;
	default:
		break;
	}

	return true;

snake_add_node:
	m_time_blood += 50;
	m_score += (m_choose_level - '0');
	m_vecBody.push_back(add_food_node);
	CMyFood::m_food_count--;
	CMyMap::m_gMap[snake_head.nY][snake_head.nX] = 0;

	return true;
}

void CMySnake::SnakeClear() {
	m_vecBody.clear();
}

void CMySnake::SaveSnakeInfo() {
	char head_distance = m_p_snake_head - snake_head_str;
	char color_distance = m_p_snake_color - snake_color_str;
	int snake_length = m_vecBody.size();
	CMyFood food_obj;
	int food_count = food_obj.m_food_count;

	fstream fout("snake_info", ios::out | ios::binary);

	fout.write((char*)&m_nDirection, sizeof(m_nDirection));  // 4B
	fout.write((char*)&m_protected_head, sizeof(m_protected_head)); // 1B
 	fout.write((char*)&m_storm_coming, sizeof(m_storm_coming)); // 1B
 	fout.write((char*)&m_choose_level, sizeof(m_choose_level)); // 1B
 	fout.write((char*)&m_time_blood, sizeof(m_time_blood)); // 4B
 	fout.write((char*)&m_score, sizeof(m_score)); // 4B
	fout.write(&head_distance, sizeof(head_distance)); // 1B
	fout.write(&color_distance, sizeof(color_distance)); // 1B

	fout.write((char*)&food_count, sizeof(food_count)); // 4B
	fout.write((char*)&snake_length, sizeof(snake_length)); // 4B
	// Every node contain (x,y) 8B
 	for (SNAKENODE &each : m_vecBody) {
		// write order: head-->tail
 		fout.write((char*)&each, sizeof(each));
 	}

	// each element 4B
 	fout.write((char*)CMyMap::m_gMap, sizeof(CMyMap::m_gMap));

	fout.close();

	// The third para can't use 0, otherwise, occur error in the view framework
	MessageBox(0, L"Save Success", L" ", MB_OK);
}

bool CMySnake::ReadSnakeInfo() {
	char head_distance = 0;
	char color_distance = 0;
	int snake_length = 0;
	SNAKENODE each = {};
	CMyFood food_obj;
	int food_count = 0;

	// Must add ios::binary, otherwise, sometimes, read large buffer can't work
	fstream fin("snake_info", ios::in | ios::binary);
	if (!fin) {
		return false;
	}

	fin.read((char*)&m_nDirection, sizeof(m_nDirection));  // 4B
	fin.read((char*)&m_protected_head, sizeof(m_protected_head)); // 1B
	fin.read((char*)&m_storm_coming, sizeof(m_storm_coming)); // 1B
	fin.read((char*)&m_choose_level, sizeof(m_choose_level)); // 1B
	fin.read((char*)&m_time_blood, sizeof(m_time_blood)); // 4B
	fin.read((char*)&m_score, sizeof(m_score)); // 4B
	fin.read(&head_distance, sizeof(head_distance)); // 1B
	fin.read(&color_distance, sizeof(color_distance)); // 1B

	m_p_snake_head = head_distance + (char(*)[3])snake_head_str;
	m_p_snake_color = color_distance + (char*)snake_color_str;
	if (m_choose_level == '1') {
		m_time_interval = snake_init_speed;
	} else if (m_choose_level == '2') {
		m_time_interval = snake_init_speed - 100;
	} else if (CMySnake::m_choose_level == '3') {
		m_time_interval = snake_init_speed - 200;
	}

	fin.read((char*)&food_count, sizeof(food_count)); // 4B
	food_obj.m_food_count = food_count;

	fin.read((char*)&snake_length, sizeof(snake_length)); // 4B
	// Every node contain (x,y) 8B
	for (int i = 0; i < snake_length; i++) {
		fin.read((char*)&each, sizeof(each));
		m_vecBody.push_back(each);
	}

	// each element 4B
	fin.read((char*)CMyMap::m_gMap, sizeof(CMyMap::m_gMap));

	fin.close();

	// The third para can't use 0, otherwise, occur error in the view framework
	/*MessageBox(0, L"Read Success", L" ", MB_OK);*/

	return true;
}
