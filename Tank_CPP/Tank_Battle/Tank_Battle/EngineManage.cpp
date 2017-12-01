#include "EngineManage.h"
#include "Bullet.h"
#include "Game.h"

CEngineManage::CEngineManage() {
	m_p_engine_storage.clear();
}


CEngineManage::~CEngineManage() {
	m_p_engine_storage.clear();
	m_p_engine_storage.swap(vector<CEngine*>());
}

void CEngineManage::ManageEngine() {
#if 0
	/* Be Attention: can't work, either use reference, the vector was changed */
	for (auto it : m_p_engine_storage) {
#else
	for (unsigned int i = 0; i < m_p_engine_storage.size();) {
		auto it = m_p_engine_storage[i];
#endif
		if (!it->m_user_engine && !it->m_interval_move) {
			/* Before move tank, move his bullet first */
			if (it->m_type == TANK) {
				ManageEngine(*(CTank*)it);
			}

			it->Move(it->m_dst_direction);
			if (it->m_dissapper_flag) {
				if (it->m_type == TANK) {
					CTank::m_tank_score += 2;
					CGame::m_cur_enrmy_count--;
					if (CGame::m_recovery_count)
						CGame::m_recovery_count--;
				}
				it->Del();
				DelEngine(it);
				delete[] (it->m_p_record);
				continue;
			}

			it->m_interval_move = (it->m_type == TANK) ? it->m_interval_move_storage : TIME_INTERVAL_10;
		}

		if (!it->m_user_engine && !it->m_interval_auto_bullet && (it->m_type == TANK)) {
			ManageAutoEngine();
			it->m_interval_auto_bullet = TIME_INTERVAL_80;
		}

		if (it->m_user_engine && it->m_type == TANK) {
			if (!((CTank*)it)->ShouldTankAlive()){
				((CTank*)it)->DestroyTank();
			}
		}

		/* Be Attention: Following scentence should be put here rather than in for(xxx;xxx;here) */
		i++;
	}
}

void CEngineManage::ManageEngine(CTank& tank) {
	for (unsigned int j = 0; j < m_p_engine_storage.size();) {
		auto tank_bullet = m_p_engine_storage[j];
		if (tank_bullet->m_type == BULLET && ((CBullet*)tank_bullet)->m_owner_tank == &tank) {
			tank_bullet->Move(tank_bullet->m_dst_direction);
			if (tank_bullet->m_dissapper_flag) {
				tank_bullet->Del();
				DelEngine(tank_bullet);
				delete[](tank_bullet->m_p_record);
				continue;
			}
		}
		j++;
	}
}

void CEngineManage::ManageAutoEngine() {
#if 0
	/* Be Attention: can't work, either use reference, the vector was changed */
	for (auto it : m_p_engine_storage) {
#else
	for (unsigned int i = 0; i < m_p_engine_storage.size(); i++) {
		auto it = m_p_engine_storage[i];
#endif
		if (!it->m_user_engine && it->m_type == TANK) {
			CBullet* bullet_obj = new CBullet();
			bullet_obj->m_p_record = bullet_obj;
			bullet_obj->GenerateBullet(*(CTank*)it, *this);
		}
	}
}

bool CEngineManage::IsEngineEmpty() {
	return m_p_engine_storage.empty();
}

bool CEngineManage::ClearEngine() {
	if (IsEngineEmpty())
		return true;

	for (unsigned int i = 0; i < m_p_engine_storage.size();) {
		auto it = m_p_engine_storage[i];
		it->Del();
		DelEngine(it);

		if (!it->m_user_engine) {
			delete[](it->m_p_record);
		}
	}

	return true;
}

bool CEngineManage::AddEngine(CEngine* p_engine) {
	m_p_engine_storage.push_back(p_engine);

	return true;
}

bool CEngineManage::DelEngine(CEngine* p_engine) {
	for (unsigned int i = 0; i < m_p_engine_storage.size(); i++) {
		if (m_p_engine_storage[i] == p_engine) {
			m_p_engine_storage.erase(m_p_engine_storage.begin() + i);
			break;
		}
	}

	return true;
}

void CEngineManage::IntervalManage() {
	for (auto it : m_p_engine_storage) {
		if (!it->m_user_engine) {
			it->m_interval_move--;
			it->m_interval_change_dst_dir--;
		}

		if (!it->m_user_engine && it->m_type == TANK) {
			it->m_interval_auto_bullet--;
		}
	}
}

void CEngineManage::DstDirectionChange() {
	for (auto it : m_p_engine_storage) {
		if (!it->m_user_engine && it->m_type == TANK && (!it->m_interval_change_dst_dir || !((CTank*)it)->TankCanMove(((CTank*)it)->GetCurDir()))) {
			it->m_dst_direction = DIRECTION(rand() % 4);
			it->m_interval_change_dst_dir = it->m_interval_change_dst_dir_storage;
		}
	}
}
