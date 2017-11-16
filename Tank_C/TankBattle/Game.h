#ifndef _GAME_H__
#define _GAME_H__

#include "Tank.h"

extern void StartGame(PTANK ptank, PTANK ptank_enermy);
extern int PlayGame(PTANK ptank, PTANK ptank_enermy);
extern void SaveGame();
extern bool ReadGame();

#endif
