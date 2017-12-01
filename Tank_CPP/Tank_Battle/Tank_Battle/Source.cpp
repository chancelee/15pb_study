#include <iostream>
#include <time.h>
#include "Game.h"
#include "Tank.h"
#include "EngineManage.h"

using namespace std;

int main(int argc, char** argv) {
	CTank tank_obj;
	CGame game_obj;
	CEngineManage engine_manage_obj;

	game_obj.StartGame(tank_obj, engine_manage_obj);
	game_obj.PlayGame(tank_obj, engine_manage_obj);

	return 0;
}
