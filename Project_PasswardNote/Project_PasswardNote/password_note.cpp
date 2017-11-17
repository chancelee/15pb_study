#include <stdio.h>
#include <stdlib.h>
#include "password_note.h"
#include "encrypt.h"
#include "LinkNode.h"
#include "AccountOperation.h"

int main(int argc, char** argv) {
	int account_operation = 0;

	g_note_head = linknode_init();

	read_note();

	// Read file content to memory (link_node) TBD..

	// TBD...
	while (1) {
		printf("Pls input cmd number you want execute:\n");
		printf("1. Add    Account\n");
		printf("2. Del    Account\n");
		printf("3. Modify Account\n");
		printf("4. Search Account\n");
		printf("5. Print  Account\n");

		scanf_s("%d", &account_operation);
		getchar();
		system("cls");

		switch (account_operation)
		{
		case ADD_ACCOUNT:
			// Add    Account
			account_add();
			break;
		case DEL_ACCOUNT:
			// Del    Account
			account_del();
			break;
		case MODIFY_ACCOUNT:
			// Modify Account
			account_modify();
			break;
		case SEARCH_ACCOUNT:
			// Search Account
			account_search();
			break;
		case PRINT_ACCOUNT:
			// Print  All Account
			account_print();
			break;
		default:
			break;
		}

		system("cls");
	}

	linknode_delete_all(&g_note_head);

	system("pause");

	return 0;
}
