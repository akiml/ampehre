#ifndef __CHTTPServer_H__
#define __CHTTPServer_H__
#include <vector>
#include <thread>
#include <mutex>
#include "CHTTPClient.h"

class CHTTPServer {

	private:
		std::thread mThread;
		pid_t mPid;
		static std::vector<CHTTPClient*> mClients;
		static std::mutex mClientMutex;
		int mSocket = -1;
		int mStop = 0;
		int mPort = 0;

	private:
		void serve();

	public:
		CHTTPServer(int port);
		~CHTTPServer();
		int start();
		void stop();
		static void addClient(int socket);
		static void removeClient(CHTTPClient* client);

};

#endif
