#pragma once
class CEngineManage;

typedef enum _DIRECTION {
	UP,
	DOWN,
	LEFT,
	RIGHT
} DIRECTION;

typedef enum _ENGINE_TYPE {
	BULLET = 0x11,
	TANK
} ENGINE_TYPE;

typedef enum _TIME_INTERVAL {
	TIME_INTERVAL_10 = 10,
	TIME_INTERVAL_20 = 20,
	TIME_INTERVAL_40 = 40,
	TIME_INTERVAL_80 = 80,
	TIME_INTERVAL_100 = 100,
	TIME_INTERVAL_120 = 120,
	TIME_INTERVAL_150 = 150,
	TIME_INTERVAL_200 = 200,
	TIME_INTERVAL_800 = 800
};

class CEngine {
public:
	CEngine();
	/* Be Attention, must use virtual, otherwise, we can't use base pointer call children's ~ fun */
	virtual ~CEngine();

public:
	virtual bool Move(DIRECTION direction) = 0;
	virtual void Del() = 0;

public:
	/* Identify bullet or tank */
	ENGINE_TYPE m_type;
	
	/* time interval */
	int m_interval_move;

	int m_interval_move_storage;

	int m_interval_change_dst_dir;

	int m_interval_change_dst_dir_storage;

	int m_interval_auto_bullet;

	bool m_dissapper_flag;

	bool m_user_engine;

	/* Direction will move */
	DIRECTION m_dst_direction;

	CEngine* m_p_record;
};
