#ifndef _TANK_H__
#define _TANK_H__

typedef enum _DIRECTION {
	UP,
	DOWN,
	LEFT,
	RIGHT
} DIRECTION;

typedef enum _TANK_TYPE {
	PANGOLIN = 0x10,
	CHEETAH,
	T72,
	T90,
	ZTZ_99
} TANK_TYPE;

typedef struct _BULLET {
	int m_bullet_nX;
	int m_bullet_nY;
	DIRECTION m_bullet_direction;
	int m_bullet_speed;
	bool m_bullet_disapper;
	struct _BULLET* m_bullet_next;
	struct _TANK* m_owner_tank;
} BULLET, *PBULLET;

typedef struct _TANK {
	/* Coordinate */
	int m_tank_nX;
	int m_tank_nY;

	/* Move Direction */
	DIRECTION m_tank_direction;

	DIRECTION m_go_direction;

	int m_change_direction_count;

	/* Move Speed */
	int m_tank_speed;

	/* Life circle */
	int m_tank_blood;

	/* Destroy one enemy, got score */
	int m_tank_score;

	bool m_tank_enermy_flag;
	
	bool m_is_tank_alive;

	char m_color;

	/* Differ Tank type */
	TANK_TYPE m_tank_type;

	/* Record bullet info, Not so sure, TBD... */
	struct _BULLET* m_tank_bullet;
} TANK, *PTANK;

/* Extern function */
extern void InitTank(PTANK ptank, int center_nX, int center_nY, DIRECTION directions, char color);
extern bool ShouldTankAlive(PTANK ptank);
extern void DestroyTank(PTANK ptank);
extern bool MoveTank(PTANK ptank, DIRECTION direction);
extern void PrintTank(PTANK ptank);
extern void GenerateTank(PTANK ptank_enermy);

#endif
