#include "FileOperation.h"
#include "encrypt.h"
#include <string.h>
#include <errno.h>

int read_note(void) {
	FILE* p_file = NULL;
	char err_reason[20] = {};
	int ret = 0;
	int read_length = 0;
	char* cipher_text = (char*)malloc(sizeof(g_note_head->account_name));
	memset(cipher_text, 0, sizeof(g_note_head->account_name));

	errno_t err = fopen_s(&p_file, "PasswordNote_FILE", "rb");
	if (err) {
		strerror_s(err_reason, sizeof(err_reason), errno);
		printf("%s\n", err_reason);
		ret = -1;
		return ret;
	}

	while (!feof(p_file)) {
		password* note_record = NULL;
		linknode_add_tail(&note_record, g_note_head);
		read_length = fread(cipher_text, sizeof(note_record->account_name), 1, p_file);
		decrypt(cipher_text, note_record->account_name, sizeof(note_record->account_name));
		if (read_length != 1) {
			// Control LOG LEVEL TBD...
			// printf("read from file failed!\n");
			linknode_delete(&note_record, g_note_head);
			ret = -2;
			break;
		}
		fgetc(p_file);
		read_length = fread(cipher_text, sizeof(note_record->account_name), 1, p_file);
		decrypt(cipher_text, note_record->account_password, sizeof(note_record->account_password));
		if (read_length != 1) {
			// Control LOG LEVEL TBD...
			// printf("read from file failed!\n");
			linknode_delete(&note_record, g_note_head);
			ret = -2;
			break;
		}
		fgetc(p_file);
	}

	fclose(p_file);
	free(cipher_text);

	return ret;
}

int write_note(password* note_head) {
	FILE* p_file = NULL;
	char err_reason[20] = {};
	int ret = 0;
	size_t write_length = 0;
	char* cipher_text = (char*)malloc(sizeof(note_head->account_name));
	memset(cipher_text, 0, sizeof(note_head->account_name));

	errno_t err = fopen_s(&p_file, "PasswordNote_FILE", "wb");
	if (err) {
		strerror_s(err_reason, sizeof(err_reason), errno);
		printf("%s\n", err_reason);
		ret = -1;
		return ret;
	}

	while (note_head) {
		encrypt(note_head->account_name, cipher_text, sizeof(note_head->account_name));
		write_length = fwrite(cipher_text, sizeof(note_head->account_name), 1, p_file);
		if (write_length != 1) {
			printf("write to file failed!\n");
			ret = -2;
			break;
		}
		fputc(':', p_file);
		encrypt(note_head->account_password, cipher_text, sizeof(note_head->account_password));
		write_length = fwrite(cipher_text, sizeof(note_head->account_name), 1, p_file);
		if (write_length != 1) {
			printf("write to file failed!\n");
			ret = -2;
			break;
		}
		fputc('\n', p_file);

		note_head = note_head->next;
	}

	fclose(p_file);
	free(cipher_text);

	return ret;
}