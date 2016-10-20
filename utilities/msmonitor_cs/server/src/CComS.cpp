#include "CComS.hpp"

CComS::CComS(){}

CComS::~CComS(){}

int CComS::initSocket(){
	int sockfd;  // listen on sock_fd, new connection on new_fd
	struct sockaddr_in host_addr;	// my address information
	int yes=1;

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1){
		std::cout <<"error while creating socket!"<<std::endl;
		return -1;
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
		std::cout << "error setting socket options" << std::endl;
		return -1;
	}
	
	host_addr.sin_family = AF_INET;		 // host byte order
	host_addr.sin_port = htons(PORT);	 // short, network byte order
	host_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
	memset(&(host_addr.sin_zero), '\0', 8); // zero the rest of the struct

	if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr_in)) == -1){
		std::cout << "error while binding socket" << std::endl;
		return -1;
	}

	if (listen(sockfd, 5) == -1){
		std::cout << "error listening to socket" << std::endl;
		return -1;
	}
	
	std::cout << "listening on socket " << sockfd << std::endl;
	return sockfd;
}

void CComS::acceptSocket(int sockfd, int* recv_length, char* buffer, int& new_socket) {
	socklen_t sin_size;
	int nsockfd;
	struct sockaddr_in client_addr;

	std::cout << "waiting for client" << std::endl;
	sin_size = sizeof(struct sockaddr_in);
	nsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
	if(nsockfd == -1){
		std::cout << "error while accepting connection" << std::endl;
		exit(-1);
	}
	std::cout << "server: got connection from " << inet_ntoa(client_addr.sin_addr)<< " port " << ntohs(client_addr.sin_port)<< std::endl;
	*recv_length = recv(nsockfd, buffer, 1024, 0);
	dump(buffer, *recv_length);
	
	new_socket = nsockfd;	
}

// dumps raw memory in strings
void CComS::dump(char *data_buffer, const unsigned int length) {
	unsigned int i;
	for(i=0; i < length; i++) {
		printf("%c", data_buffer[i]);
	}
}