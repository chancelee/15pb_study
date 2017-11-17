#include "AccountOperation.h"
#include <time.h>
#include <conio.h>

void account_add(void) {
	password* note_account = NULL;
	int i = 0;
	char ch = 0;

	printf("%s\n", __FUNCTION__);

	linknode_add_tail(&note_account, g_note_head);

	printf("Pls input account name: ");
	scanf_s("%s", note_account->account_name, _countof(note_account->account_name));
	getchar();

	printf("Pls input account password: ");
// 	scanf_s("%s", note_account->account_password, _countof(note_account->account_password));
// 	getchar();

	while ((ch = _getch()) != 0x0d) {
		putchar('*');
		note_account->account_password[i++] = ch;
	}

	// Storage note_account name && password in file TBD...
	write_note(g_note_head->next);
}

void account_del(void) {
	char del_name[20] = {};
	char account_password[20] = {};
	password* del_node = NULL;
	int i = 0;
	char ch = 0;

	srand((unsigned int)time(NULL));

	printf("%s\n", __FUNCTION__);
	printf("Pls input account name you want delete: ");
	scanf_s("%s", del_name, _countof(del_name));
	getchar();

	del_node = linknode_search_node_name(g_note_head, del_name);
	if (del_node == NULL) {
		printf("Not Found, Ensure you input the right name\n");
		getchar();
		return;
	}

	printf("Pls input account password: ");
// 	scanf_s("%s", account_password, _countof(account_password));
// 	getchar();

	while ((ch = _getch()) != 0x0d) {
		putchar('*');
		account_password[i++] = ch;
	}
	putchar(10);

	if (!strcmp(account_password, del_node->account_password)) {
		linknode_delete(&del_node, g_note_head);
		printf("Already Deleted\n");
	} else {
		if (rand() % 20 > 10) printf("Ensure you input the correct password!\n");
		else printf("YOU WANT HACK MY ACCOUNT!!!\n");
	}

	write_note(g_note_head->next);

	getchar();
}

void account_modify(void) {
	char modify_name[20] = {};
	char old_password[20] = {};
	char new_password[20] = {};
	password* modify_node = NULL;
	int i = 0;
	char ch = 0;

	srand((unsigned int)time(NULL));

	printf("%s\n", __FUNCTION__);
	printf("Pls input account name you want change: ");
	scanf_s("%s", modify_name, _countof(modify_name));
	getchar();

	modify_node = linknode_search_node_name(g_note_head, modify_name);
	if (modify_node == NULL) {
		printf("Not Found, Ensure you input the right name\n");
		getchar();
		return;
	}

	printf("Pls input old password: ");
// 	scanf_s("%s", old_password, _countof(old_password));
// 	getchar();

	while ((ch = _getch()) != 0x0d) {
		putchar('*');
		old_password[i++] = ch;
	}
	putchar(10);

	if (!strcmp(old_password, modify_node->account_password)) {
		printf("Pls input new password: ");
// 		scanf_s("%s", new_password, _countof(new_password));
// 		getchar();
		
		i = 0;

		while ((ch = _getch()) != 0x0d) {
			putchar('*');
			new_password[i++] = ch;
		}
		putchar(10);

		strcpy_s(modify_node->account_password, sizeof(modify_node->account_password), new_password);
		printf("Password Changed Success!\n");
	} else {
		if (rand() % 20 > 10) printf("Ensure you input the correct password!\n");
		else printf("YOU WANT HACK MY ACCOUNT!!!\n");
	}

	write_note(g_note_head->next);

	getchar();
}


void account_search(void) {
	char key_word[20] = {};
	int search_result = 0;

	printf("%s\n", __FUNCTION__);
	printf("Pls input strings you want filter: ");

	scanf_s("%s", key_word, _countof(key_word));
	getchar();

	search_result = linknode_smart_search(g_note_head, key_word);
	if (!search_result) {
		printf("Not Found!!\n");
	}
	getchar();
}

void account_print(void) {
	printf("%s\n", __FUNCTION__);
	linknode_search_all(g_note_head->next);
	getchar();
}