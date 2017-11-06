#include <iostream>
#include "MyMap.h"
#include "MySnake.h"
#include "MyFood.h"
#include "MyDraw.h"
#include "MyWelcome.h"
#include "MyGame.h"
#include <windows.h>

using namespace std;

#pragma comment(lib, "winmm.lib")

int main(int argc, char** argv) {
	CMyMap map_obj;
	CMyFood food_obj;
	CMySnake snake_obj;
	CMyWelcome game_welcome_obj;
	CMyGame game_obj;

	// Disable Chinese input method
	keybd_event(VK_SHIFT, 0, 0, 0);
	Sleep(100);
	keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
	PlaySoundA("fight.wav", NULL, SND_ASYNC | SND_NODEFAULT | SND_LOOP);

	game_obj.StartGame(snake_obj, food_obj, map_obj, game_welcome_obj);
	game_obj.PlayGame(snake_obj, food_obj, map_obj, game_welcome_obj);

	return 0;
}
