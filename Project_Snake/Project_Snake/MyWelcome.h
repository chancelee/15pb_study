#pragma once
class CMyWelcome {
public:
	CMyWelcome();
	~CMyWelcome();
	void NewGame();
	void SetGame();
	void GameOver();
	bool StartNewGame();

public:
	bool restart_game;
};
