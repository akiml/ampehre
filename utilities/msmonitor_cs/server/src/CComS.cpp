#include "CComS.hpp"

CComS::CComS(){}

CComS::~CComS(){}

int CComS::initSocket(){
	int sockfd;  // listen on sock_fd, new connection on new_fd
	struct sockaddr_in host_addr;	// my address information
	int yes=1;

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1){
		printf("error while creating socket!");
		return -1;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
		printf("error setting socket options");
		return -1;
	}
	
	host_addr.sin_family = AF_INET;		 // host byte order
	host_addr.sin_port = htons(PORT);	 // short, network byte order
	host_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
	memset(&(host_addr.sin_zero), '\0', 8); // zero the rest of the struct

	if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr_in)) == -1){
		printf("error while binding socket");
		return -1;
	}

	if (listen(sockfd, 5) == -1){
		printf("error listening to socket");
		return -1;
	}
	
	printf("listening on socket %d\n", sockfd);
	return sockfd;
}

void CComS::acceptSocket(int sockfd) {
	socklen_t sin_size;
	int nsockfd;
	int recv_length = 1;
	struct sockaddr_in client_addr;
	char buffer[1024];
	
	while(1) {    // Accept loop - later threads
		printf("waiting for client\n");
		sin_size = sizeof(struct sockaddr_in);
		nsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
		if(nsockfd == -1){
			printf("error while accepting connection");
			exit(-1);
		}
		printf("server: got connection from %s port %d\n",inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		recv_length = recv(nsockfd, &buffer, 1024, 0);
		dump(buffer, recv_length);
		
		close(nsockfd);
	}
}

void CComS::registerClient(int socket) {
	mRegClients.push_back(socket);
}

void CComS::unregisterClient(int socket) {	
	mRegClients.remove(socket);
}


// dumps raw memory in strings
void CComS::dump(char *data_buffer, const unsigned int length) {
	unsigned int i;
	for(i=0; i < length; i++) {
		printf("%c", data_buffer[i]);
	}
}