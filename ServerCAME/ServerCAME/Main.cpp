#include "Server.h"
#include <cstring>
int main(int argc, char* argv[])
{
	Server server;
	server.UnregisterPlayer();
	server.InitServer(argc, argv);
	return 0;
}