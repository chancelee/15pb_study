#include "Tank.h"
#include "Game.h"

int g_enermy_count = 2;

int main(int argc, char** argv) {
	TANK tank;
	TANK tank_enermy[10] = {};

	StartGame(&tank, tank_enermy);
	PlayGame(&tank, tank_enermy);

	return 0;
}
