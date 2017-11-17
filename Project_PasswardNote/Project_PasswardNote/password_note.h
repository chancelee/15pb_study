#ifndef __PASSWORD_NOTE_H__
#define __PASSWORD_NOTE_H__

#include <stdio.h>
#include <stdlib.h>

typedef struct _PASSWORD_NOTE{
	char account_name[20];
	char account_password[20];
	struct _PASSWORD_NOTE* next;
}password, *p_password;

#endif
