#pragma once
#include "Tank.h"
#include "EngineManage.h"
#include "Game.h"

class CGame {
public:
	CGame();
	~CGame();

public:
	void StartGame(CTank& ptank, CEngineManage& engine_manage_obj);
	int PlayGame(CTank& ptank, CEngineManage& engine_manage_obj);

private:
	void SaveGame(CTank& ptank, CEngineManage& engine_manage_obj);
	bool ReadGame(CTank& ptank, CEngineManage& engine_manage_obj);
	void RestartGame(CTank& ptank, CEngineManage& engine_manage_obj);

public:
	static bool g_restart_game;
	static char m_choose_level;
	static int m_enermy_count;
	static int m_cur_enrmy_count;
	static int m_recovery_count;
};
