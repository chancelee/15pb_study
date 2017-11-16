#ifndef _MAP_H__
#define _MAP_H__

#include <windows.h>

#ifndef MAPROW
#define MAPROW 35
#endif

#ifndef MAPCOL
#define MAPCOL 40
#endif

#ifndef SCREEN_WEIGHT
#define SCREEN_WEIGHT	122
#endif

#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT	35
#endif

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
	COLOR_DAMAGE_BOUND = 0x13,
	COLOR_INNER_BOUND = 0x17
} WALL_COLOR;

extern const char bound_str[][3];
extern int m_gMap[MAPROW][MAPCOL];

// Init Map
extern void InitMap(bool inner_wall);
extern void ClearMap();
extern void StormComing();
extern void DrawMap();
extern void DrawPanel();

#endif
