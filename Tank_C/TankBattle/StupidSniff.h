#ifndef _STUPID_SNIFF_H__
#define _STUPID_SNIFF_H__

#include "Tank.h"

#define TEND_UP		1
#define TEND_DOWN	2
#define TEND_LEFT	4
#define TEND_RIGHT	8

extern int StupidSniff(PTANK ptank_enermy, int sensibility);

#endif
