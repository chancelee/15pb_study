#pragma once
#include <windows.h>

typedef enum _WALL_ZONE {
	WALL_BULLET = 0x29,
	WALL_BULLET_ENERMY,
	WALL_BULLET_DEAD_ZONE,
	WALL_TANK,
	WALL_TANK_ENERMY,
	WALL_BOUND_ONE = 0x5A,
	WALL_BOUND_TWO,
	WALL_BOUND_THREE,
	WALL_BOUND_DAMAGE_FOUR,
	WALL_INNER_BOUND,
	WALL_DEAD_ZONE
} WALL_ZONE;

typedef enum _WALL_COLOR {
	COLOR_BOUND = 0x0F,
	COLOR_DAMAGE_BOUND = 0x03,
	COLOR_INNER_BOUND = 0x17
} WALL_COLOR;

const int MAPROW = 35;
const int MAPCOL = 40;
const int SCREEN_WEIGHT = 122;
const int SCREEN_HEIGHT = 35;
extern const char bound_str[][3];

class C_Map {
public:
	C_Map();
	~C_Map();

public:
	// Init Map
	void InitMap(bool inner_wall);
	void DrawPanel();
	void ClearMap();
	void StormComing();

private:
	void DrawMap();

public:
	static int m_gMap[MAPROW][MAPCOL];
};
