#pragma once
#include "MyMap.h"

typedef enum _FOOD_TYPE{
	FOOD_ST = 0x01,
	FOOD_STAR,
	FOOD_STORM,
	FOOD_ADD_BLOOD,
	FOOD_CHANGE_HEAD,
	FOOD_PROTECT_HEAD,
	FOOD_ADD_TWO_NODE,
	FOOD_CHANGE_COLOR,
	FOOD_ED
}FOOD_TYPE;

typedef enum _FOOD_COLOR{
	_COLOR_STAR = 0x08,
	_COLOR_STORM,
	_COLOR_ADD_BLOOD,
	_COLOR_CHANGE_HEAD,
	_COLOR_PROTECT_HEAD,
	_COLOR_ADD_TWO_NODE,
	_COLOR_CHANGE_COLOR
}FOOD_COLOR;

const char food_str[][3] = { "¡ï", "¦·", "$", "¡ñ", "¡÷", "¡Þ", "¡Æ" };

class CMyFood {
public:
	CMyFood();
	~CMyFood();
	void RandomFood();
	bool IsFoodEmpty();
	void DrawFood();

public:
	static int m_food_count;
	static int m_food_count_storage;
};
