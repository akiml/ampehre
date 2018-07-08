#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <cerrno>
#include <iostream>
#include <ostream>
#include <csignal>
#include "CHTTPServer.h"

std::vector<CHTTPClient*> CHTTPServer::mClients;
std::mutex CHTTPServer::mClientMutex;

CHTTPServer::CHTTPServer(int port){
	mPort = port;
}

CHTTPServer::~CHTTPServer(){
	mClientMutex.lock();
	for (unsigned int i=0; i<mClients.size(); i++) {
		delete mClients[i];
	}
	mClients.clear();
	mClientMutex.unlock();
}

int CHTTPServer::start(){

	struct sockaddr_in6 addr = {};
	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons(mPort);
	addr.sin6_addr = in6addr_any;
	this->mSocket = socket(AF_INET6, SOCK_STREAM, 0);

	if(bind(this->mSocket, (struct sockaddr*)&addr, sizeof( addr)) < 0) {
		return -1;
	}
	
	if (listen(this->mSocket, 0)) {
		close(this->mSocket);
		mSocket = -1;
		return -1;
	}

	this->mStop = 0;

	this->mThread = std::thread(&CHTTPServer::serve, this);

	std::cout << "Listening on port " << mPort << std::endl;

	return 0;
}

void CHTTPServer::stop(){

	int ret;
	mStop = 1;
	if (-1 != mSocket) {
		close(mSocket);
		mSocket = -1;
	}
	ret = kill(mPid, SIGINT);
	if (ret == -1) {
	}

	if (mThread.joinable()) {
		mThread.join();
	}
}

void CHTTPServer::serve(){

	mPid = syscall(SYS_gettid);

	int error;

	struct sockaddr_in6 addr = {};
	socklen_t addrlen;
	addrlen = sizeof(addr);
	int socket;
	
	while (0 == mStop) {

		socket = accept(mSocket, (struct sockaddr*) &addr, &addrlen);
		if (socket == -1) {
			error = errno;

			if (error == EINTR || error == EBADF) {
				break;
			}
			break;
		}
		addClient(socket);
	}

}

void CHTTPServer::addClient(int socket){

	mClientMutex.lock();
	CHTTPClient* client = new CHTTPClient(socket);

	// remove done clients
	for (int i=mClients.size()-1; i>=0; i--) {
		if (mClients[i]->mDone == 1) {
			delete mClients[i];
			mClients.erase(mClients.begin()+i);
		}
	}

	// add new client
	mClients.push_back(client);	

	mClientMutex.unlock();

}

void CHTTPServer::removeClient(CHTTPClient* client){

	mClientMutex.lock();
	int ix = -1;
	for (unsigned int i = 0; i<mClients.size(); i++) {
		if (mClients[i] == client) {
			ix = i;
			break;
		}
	}
	if (-1 != ix) {
		mClients.erase(mClients.begin()+ix);
	}
	mClientMutex.unlock();

}
