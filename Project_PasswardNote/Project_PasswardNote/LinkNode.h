#ifndef __LINKNODE_H__
#define __LINKNODE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "password_note.h"

/*
 * LinkNode operation function
 */
extern password* linknode_init(void);
extern void linknode_add_tail(p_password* note, p_password note_head);
extern void linknode_delete(p_password* note, p_password note_head);
extern void linknode_delete_all(p_password* note_head);
extern void linknode_search_all(p_password note_head);
extern p_password linknode_search_node_name(p_password note_head, const char* src);
extern int linknode_smart_search(p_password note_head, const char* src);

/*
 * LinkNode global head note
 */
extern password* g_note_head;

#endif
