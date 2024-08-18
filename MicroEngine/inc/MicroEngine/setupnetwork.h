namespace me {
	class SetupNetwork {
	public:
		bool EstablishConnection();
		//void SetupNetwork::CreateServer();
		void SetupNetwork::BeepBeep();
	};
}


#include <windows.h>
#include "server.h"
//#include "setupnetwork.h"

namespace me {
	bool SetupNetwork::EstablishConnection() {
		// Attempt to connect
		// create server if no connection can be build
		// Return if connection was successful
		return false;
	}

	void SetupNetwork::BeepBeep() {
		Beep(750, 300);
	}
}