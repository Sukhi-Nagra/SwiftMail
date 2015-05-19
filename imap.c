#include "tcp.h"

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
	void *data;
	enum imap_arg_type type;
	struct imap_arg_list *next;
};

struct imap_command
{
	char *id;
	enum imap_command_type type;
	struct imap_arg_list *args;
};

struct imap_command *imap_send_greeting(struct tcpConnection *conn)
{
	char greeting[14] = "* SwiftMail \r\n";
	tcp_write(conn, greeting, 14);
}

struct imap_command *imap_read_command(struct tcpConnection *conn)
{
	int pos = 0;
	char cbuff[500];
	char buff[1000];
	tcp_read(conn, cbuff, 500);
	printf("COMMAND: %s\n", cbuff);
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
			imap_read_command(client);
			exit(0);
		}

		tcp_close(client);
	}
	return 0;
}
