#ifndef _BULLET_H__
#define _BULLET_H__
#include "Tank.h"

extern void GenerateBullet(PTANK ptank);
extern void BulletMove(PTANK ptank);
extern void DelAllBullet(PTANK ptank);
extern int BulletNum(PTANK ptank);
extern void BulletAdd(PTANK ptank, PBULLET pbullet);

#endif
