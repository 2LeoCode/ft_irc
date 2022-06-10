Compilation:
	make			==>	Compile normally
	make DEBUG=true	==>	Compile with a debug interface
	make run		==>	Compile and run using the default port/password
						(PORT=6667 PASSWORD=123)
	make run PORT=<port> PASSWORD=<password>
					==> Compile and run using a custom port/password

	Note:	If you already compiled the project, you need to `make clean`
			in order to enable or disable the debug interface.

Connection using netcat:
	nc <address> <port>
	(It's recommended to set a large ping timeout (PING_TIMEOUT macro in Server.hpp)
	if you want to connect this way because the server will send a PING to the
	client upon registration)

Our reference IRC client:
	irssi

Connection using client:
	/connect <address> <port> <password>

Our reference IRC server (that we used to compare incoming requests):
	Freenode (chat.freenode.net:6667)

Our reference IRC manual:
	RFC 1459 (https://datatracker.ietf.org/doc/html/rfc1459)
