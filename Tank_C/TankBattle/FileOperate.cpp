#include "Tank.h"
#include "bullet.h"
#include "map.h"
#include <fstream>

using namespace std;

void SaveTankInfo(PTANK ptank, PTANK ptank_enermy) {
	int bullet_num = 0;
	int enermy_tank_num = 0;

	fstream fout("tank_info", ios::out | ios::binary);
	fstream fout_map("map_info", ios::out | ios::binary);

	fout.write((char*)ptank, sizeof(TANK));

	for (int i = 0; i < 2; i++) {
		if (ptank_enermy[i].m_is_tank_alive) {
			enermy_tank_num++;
		}
	}
	fout.write((char*)&enermy_tank_num, sizeof(int));

	for (int i = 0; i < 2; i++) {
		if (ptank_enermy[i].m_is_tank_alive) {
			fout.write((char*)&i, sizeof(int));
		}
	}

	for (int i = 0; i < 2; i++) {
		if (ptank_enermy[i].m_is_tank_alive) {
			fout.write((char*)&ptank_enermy[i], sizeof(TANK));
		}
	}

	bullet_num = BulletNum(ptank);
	fout.write((char*)&bullet_num, sizeof(int));

	if (bullet_num) {
		while (ptank->m_tank_bullet) {
			fout.write((char*)(ptank->m_tank_bullet), sizeof(BULLET));
			ptank->m_tank_bullet = ptank->m_tank_bullet->m_bullet_next;
		}
	}

	for (int i = 0; i < 2; i++) {
		bullet_num = BulletNum(&ptank_enermy[i]);
		fout.write((char*)&bullet_num, sizeof(int));

		if (bullet_num) {
			while (ptank_enermy[i].m_tank_bullet) {
				fout.write((char*)(ptank_enermy[i].m_tank_bullet), sizeof(BULLET));
				ptank_enermy[i].m_tank_bullet = ptank_enermy[i].m_tank_bullet->m_bullet_next;
			}
		}
	}

	// each element 4B
	//fout.write((char*)m_gMap, sizeof(m_gMap));

	fout.close();

	fout_map.write((char*)m_gMap, sizeof(m_gMap));
	fout_map.close();

	// The third para can't use 0, otherwise, occur error in the view framework
	MessageBox(0, L"Save Success", L" ", MB_OK);
}

bool ReadTankInfo(PTANK ptank, PTANK ptank_enermy) {
	int bullet_num = 0;
	int enermy_tank_num = 0;
	int enermy_index[2] = {0};
	PBULLET pbullet = NULL;

	// Must add ios::binary, otherwise, sometimes, read large buffer can't work
	fstream fin("tank_info", ios::in | ios::binary);
	fstream fin_map("map_info", ios::in | ios::binary);
	if (!fin || !fin_map) {
		return false;
	}

	fin.read((char*)ptank, sizeof(TANK));

	fin.read((char*)&enermy_tank_num, sizeof(int));

	for (int i = 0; i < enermy_tank_num; i++) {
		fin.read((char*)&enermy_index[i], sizeof(int));
	}

	for (int i = 0; i < enermy_tank_num; i++) {
		fin.read((char*)&ptank_enermy[enermy_index[i]], sizeof(TANK));
	}

	ptank->m_tank_bullet = nullptr;
	for (int i = 0; i < 2; i++) {
		ptank_enermy[i].m_tank_bullet = nullptr;
	}

	fin.read((char*)&bullet_num, sizeof(int));
	for (int i = 0; i < bullet_num; i++) {
		pbullet = (PBULLET)malloc(sizeof(BULLET));
		memset(pbullet, 0, sizeof(BULLET));
		fin.read((char*)pbullet, sizeof(BULLET));
		pbullet->m_bullet_next = nullptr;
		BulletAdd(ptank, pbullet);
	}

	for (int i = 0; i < 2; i++) {
		fin.read((char*)&bullet_num, sizeof(int));
		for (int i = 0; i < bullet_num; i++) {
			pbullet = (PBULLET)malloc(sizeof(BULLET));
			memset(pbullet, 0, sizeof(BULLET));
			fin.read((char*)pbullet, sizeof(BULLET));
			pbullet->m_bullet_next = nullptr;
			BulletAdd(&ptank_enermy[i], pbullet);
		}
	}

	// each element 4B
	//fin.read((char*)m_gMap, sizeof(m_gMap));

	fin.close();

	fin_map.read((char*)m_gMap, sizeof(m_gMap));
	fin_map.close();

	return true;
}
