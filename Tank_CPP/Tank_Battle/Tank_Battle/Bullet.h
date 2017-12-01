#pragma once
#include "Tank.h"
#include "EngineManage.h"

class CBullet : public CEngine {
public:
	CBullet();
	~CBullet();
	void GenerateBullet(CTank& tank, CEngineManage& engine_manage);
	void PrintBullet();
	void BulletDisapper();
	void DelAllBullet();
	virtual bool Move(DIRECTION direction);
	virtual void Del();
	CTank* m_owner_tank;

private:
	int m_bullet_nX;
	int m_bullet_nY;
	DIRECTION m_bullet_direction;
	int m_bullet_speed;
};
