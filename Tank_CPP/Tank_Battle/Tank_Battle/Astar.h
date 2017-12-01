#pragma once
#include <vector>
#include <windows.h>
#include "Engine.h"

/* Be Attention: Never Forget */
using namespace std;

typedef struct _STEPNODE {
	/* Every Step, Increase */
	int s_g_value;

	/* Current Distance To End Point */
	int s_h_value;

	/* Calculate f(x) = g(x) + h(x) */
	int s_f_value;

	/* Current Coordinate */
	COORD s_cur;

	/* How to get current place, record parent's coordinate */
	COORD s_parent;

	/* Record move direction, from parent->current */
	DIRECTION s_direction;

	void SetCur_H_F_val(COORD cur, COORD end);

	bool operator==(const _STEPNODE& compare);
} STEPNODE, *PSTEPNODE;

class CAstar {
public:
	CAstar();
	CAstar(const COORD& start, const COORD& end);
	~CAstar();
	void Init(const COORD& start, const COORD& end);
	bool FindPath();
	void GetPath();
	void PrintPath();
	void ClearPath();
	DIRECTION GetDirection();
	bool IsPathEmpty();

private:
	COORD m_start;
	COORD m_end;
	vector<STEPNODE> m_table_open;
	vector<STEPNODE> m_table_close;
	vector<STEPNODE> m_table_min_path;
};
