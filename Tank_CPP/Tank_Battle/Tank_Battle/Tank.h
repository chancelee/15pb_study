#pragma once
#include "Engine.h"
#include <windows.h>

typedef enum _TANK_TYPE {
	ENERMY_Z01 = 0x01,
	ENERMY_Z02,
	ENERMY_Z03,
	ENERMY_Z04
};

class CTank : public CEngine {
public:
	CTank();
	~CTank();

public:
	void InitTank(int center_nX, int center_nY, DIRECTION directions, char color);
	bool ShouldTankAlive();
	void DestroyTank();
	bool GenerateTank();
	void PrintTank();
	bool m_is_tank_alive;
	bool m_tank_enermy_flag;
	char m_color;
	virtual bool Move(DIRECTION direction);
	bool TankCanMove(DIRECTION direction);
	virtual void Del();
	COORD GetCurCoord();
	DIRECTION GetCurDir();
	void SetTimeInterval(int interval_move, int interval_change_dir);
	/* Life circle */
	static int m_tank_blood;
	/* Destroy one enemy, got score */
	static int m_tank_score;

private:
	void EraseTank();
	bool CanGenerateTank(int nX, int nY);
	/* Coordinate */
	int m_tank_nX;
	int m_tank_nY;

	/* Current Direction */
	DIRECTION m_tank_direction;

	/* Move Speed */
	int m_tank_speed;

	int m_tank_type;
};
