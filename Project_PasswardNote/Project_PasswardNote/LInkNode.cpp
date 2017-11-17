#include "LinkNode.h"

password* g_note_head = NULL;

password* linknode_init(void) {
	password* note_head = (p_password)malloc(sizeof(password));

	memset(note_head, 0, sizeof(password));
	note_head->next = NULL;

	return note_head;
}

void linknode_add_tail(p_password* note, p_password note_head) {
	if (*note == NULL) {
		*note = (p_password)malloc(sizeof(password));
		memset(*note, 0, sizeof(password));
		(*note)->next = NULL;
	}

	while (note_head->next) {
		note_head = note_head->next;
	}

	note_head->next = *note;
}

void linknode_delete(p_password* note, p_password note_head) {
	while (note_head->next) {
		if (note_head->next == *note) {
			note_head->next = (*note)->next;
			(*note)->next = NULL;
			free(*note);
			*note = NULL;
			break;
		}

		note_head = note_head->next;
	}

	if (*note == note_head) {
		free(note);
		note = NULL;
	}
}

void linknode_delete_all(p_password* note_head) {
	p_password* p_note_head = note_head;
	while (*note_head) {
		if (((*p_note_head)->next != NULL)) {
			while ((*p_note_head)->next->next) {
				*p_note_head = (*p_note_head)->next;
			}

			free((*p_note_head)->next);
			(*p_note_head)->next = NULL;
		} else {
			free(*note_head);
			(*note_head)->next = NULL;
			(*note_head) = NULL;
			break;
		}

		*note_head = (*note_head)->next;

		if (*note_head) {
			*p_note_head = *note_head;
		}
	}
}

void linknode_search_all(p_password note_head) {
	if (note_head == NULL) {
		printf("NULL Account!\n");
		return;
	}

	while (note_head) {
		printf("account_name(%s), account_password(%s)\n", 
			note_head->account_name, note_head->account_password);

		note_head = note_head->next;
	}
}

// If Name repeat, TBD...
p_password linknode_search_node_name(p_password note_head, const char* src) {
	while (note_head) {
		if (!strcmp(note_head->account_name, src)) {
			break;
		}

		note_head = note_head->next;
	}

	return note_head;
}

int linknode_smart_search(p_password note_head, const char* src) {
	int result = 0;

	while (note_head) {
		if (strstr(note_head->account_name, src)) {
			printf("%s, %s\n", note_head->account_name, note_head->account_password);
			result++;
		}

		note_head = note_head->next;
	}

	return result;
}