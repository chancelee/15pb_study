#include "FileOperate.h"
#include "Tank.h"
#include "bullet.h"
#include "C_Map.h"
#include "Game.h"
#include <fstream>
#include <windows.h>
#include <stdio.h>

using namespace std;

CFileOperate::CFileOperate() {
}

CFileOperate::~CFileOperate() {
}

void CFileOperate::SaveTankInfo(CEngineManage& engine_manage_obj) {
	int* bullet_num_arr = nullptr;
	int user_engine = 0;
	int auto_engine = 0;
	int tank_num = 0;
	int tank_count = 0;
	CTank** p_tank_obj = nullptr;

	fstream fout("tank_info", ios::out | ios::binary);
	fstream fout_map("map_info", ios::out | ios::binary);
	fstream fout_game("game_info", ios::out | ios::binary);

	//fout.write((char*)ptank, sizeof(TANK));

	/* 1. Got Bullet Number, And Need to Know it's owner tank 
	 * 2. Record Tank's Number, need distinguish enermy or not;
	 * 3. Write
	 */
	for (auto it : engine_manage_obj.m_p_engine_storage) {
		if (it->m_type == TANK) {
			tank_num++;
		}

		if (it->m_user_engine && it->m_type == TANK) {
			user_engine++;
		}
	}

	p_tank_obj = new CTank*[tank_num]();
	bullet_num_arr = new int[tank_num]();
	auto_engine = tank_num - user_engine;

	for (auto it : engine_manage_obj.m_p_engine_storage) {
		if (it->m_type == TANK) {
			p_tank_obj[tank_count++] = (CTank*)it;
		}
	}

	for (auto it : engine_manage_obj.m_p_engine_storage) {
		if (it->m_type == BULLET) {
			for (int i = 0; i < tank_num; i++) {
				if (((CBullet*)it)->m_owner_tank == p_tank_obj[i]) {
					bullet_num_arr[i]++;
				}
			}
		}
	}

	fout.write((char*)&user_engine, sizeof(int));
	for (int i = 0; i < tank_num; i++) {
		if (p_tank_obj[i]->m_user_engine) {
			fout.write((char*)&bullet_num_arr[i], sizeof(int));
			if (bullet_num_arr[i]) {
				for (auto it : engine_manage_obj.m_p_engine_storage) {
					if (it->m_type == BULLET && ((CBullet*)it)->m_owner_tank == p_tank_obj[i]) {
						/* Be Attention, because we use virtual fun, we have a virtual fun table at the head, but we shouldn't storage it !!! */
						fout.write((char*)it + sizeof(int*), sizeof(CBullet) - sizeof(int*));
					}
				}
			}
			fout.write((char*)p_tank_obj[i] + sizeof(int*), sizeof(CTank) - sizeof(int*));
		}
	}

	fout.write((char*)&auto_engine, sizeof(int));
	for (int i = 0; i < tank_num; i++) {
		if (!p_tank_obj[i]->m_user_engine) {
			fout.write((char*)&bullet_num_arr[i], sizeof(int));
			if (bullet_num_arr[i]) {
				for (auto it : engine_manage_obj.m_p_engine_storage) {
					if (it->m_type == BULLET && ((CBullet*)it)->m_owner_tank == p_tank_obj[i]) {
						fout.write((char*)it + sizeof(int*), sizeof(CBullet) - sizeof(int*));
					}
				}
			}
			fout.write((char*)p_tank_obj[i] + sizeof(int*), sizeof(CTank) - sizeof(int*));
		}
	}

	fout.write((char*)&CTank::m_tank_blood, sizeof(int));
	fout.write((char*)&CTank::m_tank_score, sizeof(int));

	delete[] p_tank_obj;
	p_tank_obj = nullptr;

	delete[] bullet_num_arr;
	bullet_num_arr = nullptr;

	fout.close();

	fout_map.write((char*)C_Map::m_gMap, sizeof(C_Map::m_gMap));
	fout_map.close();

	fout_game.write((char*)&(CGame::m_choose_level), sizeof(char));
	fout_game.write((char*)&(CGame::m_enermy_count), sizeof(int));
	fout_game.write((char*)&(CGame::m_cur_enrmy_count), sizeof(int));
	fout_game.write((char*)&(CGame::m_recovery_count), sizeof(int));
	fout_game.close();


	// The third para can't use 0, otherwise, occur error in the view framework
	MessageBox(0, L"Save Success", L" ", MB_OK);
}

bool CFileOperate::ReadTankInfo(CTank& tank_user, CEngineManage& engine_manage_obj) {
	int* bullet_num_arr = nullptr;
	int user_engine = 0;
	int auto_engine = 0;
	int tank_num = 0;
	int tank_count = 0;
	CTank* p_tank = nullptr;
	CBullet* p_bullet = nullptr;

	// Must add ios::binary, otherwise, sometimes, read large buffer can't work
	fstream fin("tank_info", ios::in | ios::binary);
	fstream fin_map("map_info", ios::in | ios::binary);
	fstream fin_game("game_info", ios::in | ios::binary);
	
	if (!fin || !fin_map || !fin_game) {
		return false;
	}

	fin.read((char*)&user_engine, sizeof(int));
	bullet_num_arr = new int[user_engine]();
	
	for (int i = 0; i < user_engine; i++) {
		//p_tank = new CTank();
		fin.read((char*)&bullet_num_arr[i], sizeof(int));
		if (bullet_num_arr[i]) {
			for (int j = 0; j < bullet_num_arr[i]; j++) {
				p_bullet = new CBullet();
				fin.read((char*)p_bullet + sizeof(int*), sizeof(CBullet) - sizeof(int*));
				p_bullet->m_p_record = p_bullet;
				p_bullet->m_owner_tank = &tank_user;
				engine_manage_obj.AddEngine(p_bullet);
			}
		}
		fin.read((char*)&tank_user + sizeof(int*), sizeof(CTank) - sizeof(int*));
		tank_user.m_p_record = &tank_user;
		engine_manage_obj.AddEngine(&tank_user);
	}
	delete[] bullet_num_arr;

	fin.read((char*)&auto_engine, sizeof(int));
	bullet_num_arr = new int[auto_engine]();

	for (int i = 0; i < auto_engine; i++) {
		p_tank = new CTank();
		fin.read((char*)&bullet_num_arr[i], sizeof(int));
		if (bullet_num_arr[i]) {
			for (int j = 0; j < bullet_num_arr[i]; j++) {
				p_bullet = new CBullet();
				fin.read((char*)p_bullet + sizeof(int*), sizeof(CBullet) - sizeof(int*));
				p_bullet->m_p_record = p_bullet;
				p_bullet->m_owner_tank = p_tank;
				engine_manage_obj.AddEngine(p_bullet);
			}
		}
		fin.read((char*)p_tank + sizeof(int*), sizeof(CTank) - sizeof(int*));
		p_tank->m_p_record = p_tank;
		engine_manage_obj.AddEngine(p_tank);
	}

	fin.read((char*)&CTank::m_tank_blood, sizeof(int));
	fin.read((char*)&CTank::m_tank_score, sizeof(int));

	fin.close();

	fin_map.read((char*)C_Map::m_gMap, sizeof(C_Map::m_gMap));
	fin_map.close();

	fin_game.read((char*)&(CGame::m_choose_level), sizeof(char));
	fin_game.read((char*)&(CGame::m_enermy_count), sizeof(int));
	fin_game.read((char*)&(CGame::m_cur_enrmy_count), sizeof(int));
	fin_game.read((char*)&(CGame::m_recovery_count), sizeof(int));
	fin_game.close();

	return true;
}
