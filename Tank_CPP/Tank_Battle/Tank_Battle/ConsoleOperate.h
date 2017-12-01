#pragma once
class CConsoleOperate {
public:
	CConsoleOperate();
	~CConsoleOperate();

public:
	void PrintChar(int Wide, int High, char* pszChar, char color);
	void SetWindowSize(char* pTitle, int nWid, int nHeight);
};
