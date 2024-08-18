#include "MEServer.h"
#include <cstring>
int main(int argc, char* argv[])
{
	Server server;
	server.InitServer();
	while(true)
	{
		server.UpdateServer(); 
	}
	return 0;
}