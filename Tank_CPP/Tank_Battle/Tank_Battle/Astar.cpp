#include "Astar.h"
#include "C_Map.h"
#include "ConsoleOperate.h"

CAstar::CAstar() {
	m_table_open.clear();
	m_table_close.clear();
	m_table_min_path.clear();
}

CAstar::CAstar(const COORD& start, const COORD& end) {
	STEPNODE t_start = {};

	m_start.X = start.X;
	m_start.Y = start.Y;
	m_end.X = end.X;
	m_end.Y = end.Y;

	t_start.s_cur.X = start.X;
	t_start.s_cur.Y = start.Y;
	t_start.s_g_value = 0;
	t_start.SetCur_H_F_val(start, end);

	m_table_open.clear();
	m_table_close.clear();
	m_table_min_path.clear();

	m_table_open.push_back(t_start);
}

CAstar::~CAstar() {
	m_table_open.clear();
	m_table_close.clear();
	m_table_min_path.clear();

	m_table_open.swap(vector<STEPNODE>());
	m_table_close.swap(vector<STEPNODE>());
	m_table_min_path.swap(vector<STEPNODE>());
}

void CAstar::Init(const COORD& start, const COORD& end) {
	STEPNODE t_start = {};

	m_start.X = start.X;
	m_start.Y = start.Y;
	m_end.X = end.X;
	m_end.Y = end.Y;

	t_start.s_cur.X = start.X;
	t_start.s_cur.Y = start.Y;
	t_start.s_g_value = 0;
	t_start.SetCur_H_F_val(start, end);

	m_table_open.clear();
	m_table_close.clear();
	//ClearPath();
	m_table_min_path.clear();

	m_table_open.push_back(t_start);
}

bool CAstar::FindPath() {
	int nIndex = 0;
	int nCount = 0;
	bool fake_point = false;
	STEPNODE surround[4] = {};

	while (!m_table_open.empty()) {
		nCount = 0;
		nIndex = 0;
		for (auto it : m_table_open) {
			if (it.s_f_value < m_table_open[nIndex].s_f_value) {
				nIndex = nCount;
			}
			nCount++;
		}

#if 0
		/* Point--UP */
		for (int i = 0; i < 3; i++) {
			surround[i].s_cur.X = m_table_open[nIndex].s_cur.X - 1 + i;
			surround[i].s_cur.Y = m_table_open[nIndex].s_cur.Y - 2;
			surround[i].s_g_value = m_table_open[nIndex].s_g_value + 1;
			surround[i].SetCur_H_F_val(surround[0].s_cur, m_end);
			surround[i].s_direction = UP;
		}

		/* Point--DOWN */
		for (int i = 0; i < 3; i++) {
			surround[i + 3].s_cur.X = m_table_open[nIndex].s_cur.X - 1 + i;
			surround[i + 3].s_cur.Y = m_table_open[nIndex].s_cur.Y + 2;
			surround[i + 3].s_g_value = m_table_open[nIndex].s_g_value + 1;
			surround[i + 3].SetCur_H_F_val(surround[1].s_cur, m_end);
			surround[i + 3].s_direction = DOWN;
		}

		/* Point--LEFT */
		for (int i = 0; i < 3; i++) {
			surround[i + 6].s_cur.X = m_table_open[nIndex].s_cur.X - 2;
			surround[i + 6].s_cur.Y = m_table_open[nIndex].s_cur.Y - 1 + i;
			surround[i + 6].s_g_value = m_table_open[nIndex].s_g_value + 1;
			surround[i + 6].SetCur_H_F_val(surround[2].s_cur, m_end);
			surround[i + 6].s_direction = LEFT;
		}

		/* Point--RIGHT */
		for (int i = 0; i < 3; i++) {
			surround[i + 9].s_cur.X = m_table_open[nIndex].s_cur.X + 2;
			surround[i + 9].s_cur.Y = m_table_open[nIndex].s_cur.Y - 1 + i;
			surround[i + 9].s_g_value = m_table_open[nIndex].s_g_value + 1;
			surround[i + 9].SetCur_H_F_val(surround[3].s_cur, m_end);
			surround[i + 9].s_direction = RIGHT;
		}
#else
		surround[0].s_cur.X = m_table_open[nIndex].s_cur.X;
		surround[0].s_cur.Y = m_table_open[nIndex].s_cur.Y - 1;
		surround[0].s_g_value = m_table_open[nIndex].s_g_value + 1;
		surround[0].SetCur_H_F_val(surround[0].s_cur, m_end);
		surround[0].s_direction = UP;

		surround[1].s_cur.X = m_table_open[nIndex].s_cur.X;
		surround[1].s_cur.Y = m_table_open[nIndex].s_cur.Y + 1;
		surround[1].s_g_value = m_table_open[nIndex].s_g_value + 1;
		surround[1].SetCur_H_F_val(surround[1].s_cur, m_end);
		surround[1].s_direction = DOWN;

		surround[2].s_cur.X = m_table_open[nIndex].s_cur.X - 1;
		surround[2].s_cur.Y = m_table_open[nIndex].s_cur.Y;
		surround[2].s_g_value = m_table_open[nIndex].s_g_value + 1;
		surround[2].SetCur_H_F_val(surround[2].s_cur, m_end);
		surround[2].s_direction = LEFT;

		surround[3].s_cur.X = m_table_open[nIndex].s_cur.X + 1;
		surround[3].s_cur.Y = m_table_open[nIndex].s_cur.Y;
		surround[3].s_g_value = m_table_open[nIndex].s_g_value + 1;
		surround[3].SetCur_H_F_val(surround[3].s_cur, m_end);
		surround[3].s_direction = RIGHT;
#endif

		for (int i = 0; i < 4; i++) {
			surround[i].s_parent = m_table_open[nIndex].s_cur;
		}

		m_table_close.push_back(m_table_open[nIndex]);
		m_table_open.erase(m_table_open.begin() + nIndex);

		for (int i = 0; i < 4; i++) {
#if 1
			if (abs(surround[i].s_cur.X - m_end.X) <= 1 &&
				abs(surround[i].s_cur.Y - m_end.Y) <= 1) {
				m_table_close.push_back(surround[i]);
				return true;
			}
#else
			if (surround[i].s_cur.X == m_end.X &&
				surround[i].s_cur.Y == m_end.Y) {
				m_table_close.push_back(surround[i]);
				return true;
			}
#endif

			/* In few occasions, A* is not a good algrithom */
			if (C_Map::m_gMap[surround[i].s_cur.Y][surround[i].s_cur.X] || 
				surround[i].s_cur.X < 0 ||
				surround[i].s_cur.X >= MAPCOL ||
				surround[i].s_cur.Y < 0 ||
				surround[i].s_cur.Y >= MAPROW) {
				continue;
			}

// 			for (int nPosY = surround[i].s_cur.Y - 1; nPosY < surround[i].s_cur.Y + 2; nPosY++) {
// 				for (int nPosX = surround[i].s_cur.X - 1; nPosX < surround[i].s_cur.X + 2; nPosX++) {
// 					if (C_Map::m_gMap[nPosY][nPosX]) {
// 						continue;
// 					}
// 				}
// 			}

			for (auto it : m_table_open) {
				if (it == surround[i]) {
					fake_point = true;
					break;
				}
			}

			if (fake_point) {
				fake_point = false;
				continue;
			}

			for (auto it : m_table_close) {
				if (it == surround[i]) {
					fake_point = true;
					break;
				}
			}

			if (fake_point) {
				fake_point = false;
				continue;
			}

			m_table_open.push_back(surround[i]);
		}
	}

	return false;
}

void CAstar::GetPath() {
	int nIndex = m_table_close.size() - 1;
	int nCount = 0;

	m_table_min_path.clear();

	while (1) {
		nCount = 0;

		for (auto it : m_table_close) {
			// Should use parent to determination, otherwise, will include start point
			if (m_table_close[nIndex].s_parent.X == m_start.X &&
				m_table_close[nIndex].s_parent.Y == m_start.Y) {
				return;
			}

			if (it.s_cur.X == m_table_close[nIndex].s_parent.X &&
				it.s_cur.Y == m_table_close[nIndex].s_parent.Y) {
				m_table_min_path.push_back(it);
				nIndex = nCount;
				break;
			}
			nCount++;
		}
	}
}

void CAstar::PrintPath() {
	CConsoleOperate console_obj;

	if (IsPathEmpty())
		return;

	for (int i = m_table_min_path.size() - 1; i >= 0; i--) {
		console_obj.PrintChar(m_table_min_path[i].s_cur.X, m_table_min_path[i].s_cur.Y, "&", 0x09);
	}
}

void CAstar::ClearPath() {
	CConsoleOperate console_obj;

	if (IsPathEmpty())
		return;

	for (int i = m_table_min_path.size() - 1; i >= 0; i--) {
		console_obj.PrintChar(m_table_min_path[i].s_cur.X, m_table_min_path[i].s_cur.Y, " ", 0x09);
	}
}

bool CAstar::IsPathEmpty() {
	return m_table_min_path.empty();
}

DIRECTION CAstar::GetDirection() {
	if (!m_table_min_path.size()) {
		return UP;
	}

	DIRECTION dst_dir = m_table_min_path[m_table_min_path.size() - 1].s_direction;
	/*m_table_min_path.pop_back();*/
	m_table_min_path.erase(m_table_min_path.begin() + m_table_min_path.size() - 1);

	return dst_dir;
}

void _STEPNODE::SetCur_H_F_val(COORD cur, COORD end) {
	s_h_value = abs(end.X - cur.X) + abs(end.Y - cur.Y);
	s_f_value = s_g_value + s_h_value;
}

bool _STEPNODE::operator==(const STEPNODE& compare) {
	if (compare.s_cur.X == s_cur.X &&
		compare.s_cur.Y == s_cur.Y) {
		return true;
	}

	return false;
}
