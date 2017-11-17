#ifndef __ACCOUNT_OPERATION_H__
#define __ACCOUNT_OPERATION_H__

#include <stdio.h>
#include "LinkNode.h"
#include "FileOperation.h"

enum {
	ADD_ACCOUNT = 1,
	DEL_ACCOUNT,
	MODIFY_ACCOUNT,
	SEARCH_ACCOUNT,
	PRINT_ACCOUNT
};

// extern function
extern void account_add(void);
extern void account_del(void);
extern void account_modify(void);
extern void account_search(void);
extern void account_print(void);

#endif
