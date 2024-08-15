#include "Server.h"
#include <cstring>
int main(int argc, char* argv[])
{
	Server server;
	server.InitServer(argc, argv);
	WSACleanup();
	return 0;
}