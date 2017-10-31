#pragma once
#include <windows.h>

#define MAPROW 40
#define MAPCOL 40

typedef enum _WALL_ZONE {
	WALL_BOUND = 0x5A,
	WALL_INNER_BOUND,
	WALL_SANKE,
	SNAKE_HOLE,
} WALL_ZONE;

typedef enum _WALL_COLOR {
	COLOR_BOUND = 0x0F,
	COLOR_INNER_BOUND = 0x07,
	COLOR_SNAKE_HOLE = 0x04
} WALL_COLOR;

const char bound_str[][3] = { "ก๖", "กั"};

class CMyMap {
public:
	CMyMap();
	~CMyMap();
	void SetWindowSize(char* pTitle, int nWid, int nHeight);
	// Init Map
	void InitMap();
	void StormComing();

private:
	void DrawMap();

public:
	static int m_gMap[MAPROW][MAPCOL];
	static int m_gMap_storage[MAPROW][MAPCOL];
};
