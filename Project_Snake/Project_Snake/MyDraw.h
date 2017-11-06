#pragma once
#include <windows.h>
#include <iostream>

class CMyDraw {
public:
	CMyDraw();
	~CMyDraw();
	template<typename T>
	void WriteChar(int Wide, int High, T pszChar, WORD wArr);
	bool DrawSelfMap();

public:
	int m_wall_index;

private:
	void DrawInterface();
	void OutputPos(int nX, int nY);
	bool MouseEventProc(MOUSE_EVENT_RECORD stcMouse);
	bool MouseMove(MOUSE_EVENT_RECORD &stcMouse);
	bool MouseClick(MOUSE_EVENT_RECORD & stcMouse);
};
