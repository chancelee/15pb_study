#pragma once
#include "Engine.h"
#include "Tank.h"
#include "FileOperate.h"
#include <vector>

using namespace std;

class CEngineManage {
public:
	CEngineManage();
	~CEngineManage();

public:
	void ManageEngine();
	void ManageEngine(CTank& tank);
	void ManageAutoEngine();
	bool IsEngineEmpty();
	bool ClearEngine();
	bool AddEngine(CEngine* p_engine);
	bool DelEngine(CEngine* p_engine);
	void IntervalManage();
	void DstDirectionChange();

private:
	vector<CEngine*> m_p_engine_storage;

	friend class CFileOperate;
};
