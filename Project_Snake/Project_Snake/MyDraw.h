#pragma once
#include <windows.h>
#include <iostream>

class CMyDraw {
public:
	CMyDraw();
	~CMyDraw();
	void WriteChar(int nX, int nY, char* pChar, WORD wAttr);
};
