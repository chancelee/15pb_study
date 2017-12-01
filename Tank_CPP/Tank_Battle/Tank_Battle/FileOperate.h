#pragma once
#include "EngineManage.h"
#include "Tank.h"

class CFileOperate {
public:
	CFileOperate();
	~CFileOperate();
	void SaveTankInfo(CEngineManage& engine_manage_obj);
	bool ReadTankInfo(CTank& tank_user, CEngineManage& engine_manage_obj);
};
