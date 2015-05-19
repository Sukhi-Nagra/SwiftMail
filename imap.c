#include <stdbool.h>
#include <string.h>
#include "tcp.h"

#define IMAP_GREETING "* SwiftMail \r\n"
#define IMAP_GREETING_LEN 14
#define ISTERMINAL(c) (c != ' ' && c != '\r' && c != '\n')

enum imap_command_type
{
	// Any State
	C_CAPABILITY,
	C_NOOP,
	C_LOGOUT,

	// Not Authenticated State
	C_STARTTLS,
	C_AUTHENTICATE,
	C_LOGIN,

	// Authenticated State
	C_SELECT,
	C_EXAMINE,
	C_CREATE,
	C_DELETE,
	C_RENAME,
	C_SUBSCRIBE,
	C_UNSUBSCRIBE,
	C_LIST,
	C_LSUB,
	C_STATUS,
	C_APPEND,

	// Select States
	C_CHECK,
	C_CLOSE,
	C_EXPUNGE,
	C_SEARCH,
	C_FETCH,
	C_STORE,
	C_COPY,
	C_UID,

	// Experimental
	C_X
};

enum imap_arg_type
{
	A_ATOM,
	A_NUMBER,
	A_STRING,
	A_BSTRING,
	A_LIST,
	A_NIL,
	A_RANGE,
	A_SET
};

struct imap_arg_list
{
	union {
		char *atom;
		int number;
		char *string;
		char *bstring;
		// ...
	} *data;
	enum imap_arg_type type;
	struct imap_arg_list *next;
};

struct imap_command
{
	char *id;
	enum imap_command_type type;
	struct imap_arg_list *args;
};

enum imap_command_type _parse_command(char *command_string)
{
	if (strcmp(command_string, "CAPABILITY") == 0)
		return C_CAPABILITY;
	else if (strcmp(command_string, "NOOP") == 0)
		return C_NOOP;
	else if (strcmp(command_string, "LOGOUT") == 0)
		return C_LOGOUT;
	else if (strcmp(command_string, "STARTTLS") == 0)
		return C_STARTTLS;
	else if (strcmp(command_string, "AUTHENTICATE") == 0)
		return C_AUTHENTICATE;
	return C_X;
}

bool imap_send_greeting(struct tcpConnection *conn)
{
	return tcp_write(conn, IMAP_GREETING, IMAP_GREETING_LEN) == IMAP_GREETING_LEN;
}

struct imap_command *imap_read_command(struct tcpConnection *conn)
{
	int pos = 0;
	char buff[100];
	char curr_char[1];
	char *id, *command;
	struct imap_command *imap_command = NULL;

	// read id
	do
	{
		tcp_read(conn, curr_char, 1);
		buff[pos++] = curr_char[0];
	} while (ISTERMINAL(curr_char[0]));
	buff[pos - 1] = '\0';

	// copy id from buff
	id = (char *) malloc(pos);
	strncpy(id, buff, pos);

	// reset buff and counter
	pos = 0;
	buff[0] = '\0';

	// read command imap_command
	do
	{
		tcp_read(conn, curr_char, 1);
		buff[pos++] = curr_char[0];
	} while (ISTERMINAL(curr_char[0]));
	buff[pos - 1] = '\0';

	// allocate imap_command
	imap_command = (struct imap_command *) malloc(sizeof(struct imap_command));
	imap_command->id = id;
	imap_command->type = _parse_command(buff);

	// imap_command->args = _read_arg_list(conn);

	return imap_command;
}

#include <stdio.h>
#include <signal.h>
int main()
{
	signal(SIGCHLD, SIG_IGN);
	struct tcpConnection *conn = tcp_listen("localhost", "143");
	if (conn == NULL)
		perror("WTF");

	while (1)
	{
		struct tcpConnection *client = tcp_accept(conn);

		if (client == NULL)
			perror("WTF2");

		if (fork() == 0)
		{
			tcp_close(conn);
			imap_send_greeting(client);
			struct imap_command *cmd = imap_read_command(client);
			printf("imap_command.id = %s\nimap_command.type = %d\n",
					cmd->id,
					cmd->type);

			exit(0);
		}

		tcp_close(client);
	}

	return 0;
}
