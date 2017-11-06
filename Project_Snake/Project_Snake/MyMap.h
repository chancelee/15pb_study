#pragma once
#include <windows.h>

#define MAPROW 35
#define MAPCOL 40

typedef enum _WALL_ZONE {
	WALL_BOUND_ONE = 0x5A,
	WALL_BOUND_TWO,
	WALL_BOUND_THREE,
	WALL_INNER_BOUND,
	WALL_SANKE
} WALL_ZONE;

typedef enum _WALL_COLOR {
	COLOR_BOUND = 0x0F,
	COLOR_INNER_BOUND = 0x17
} WALL_COLOR;

extern const char bound_str[][3];

class CMyMap {
public:
	CMyMap();
	~CMyMap();
	void SetWindowSize(char* pTitle, int nWid, int nHeight);
	// Init Map
	void InitMap(bool inner_wall);
	void ClearMap();
	void StormComing();
	void DrawPanel();

private:
	void DrawMap();

public:
	static int m_gMap[MAPROW][MAPCOL];
};
