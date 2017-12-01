#pragma once
#include <windows.h>

class CDrawOwnMap {
public:
	CDrawOwnMap();
	~CDrawOwnMap();

public:
	bool DrawSelfMap();

private:
	void DrawInterface();
	void OutputPos(int nX, int nY);
	bool MouseMove(MOUSE_EVENT_RECORD &stcMouse);
	bool MouseClick(MOUSE_EVENT_RECORD & stcMouse);
	bool MouseEventProc(MOUSE_EVENT_RECORD stcMouse);

	/* Index of different wall */
	int m_wall_index;
};
