#include <CComS.hpp>

CComS::CComS(){}

CComS::~CComS(){}

int CComS::initSocket(){
	int sockfd;  // listen on sock_fd, new connection on new_fd
	struct sockaddr_in host_addr;	// my address information
	int yes=1;

	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
		fatal("in socket");

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		fatal("setting socket option SO_REUSEADDR");
	
	host_addr.sin_family = AF_INET;		 // host byte order
	host_addr.sin_port = htons(PORT);	 // short, network byte order
	host_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
	memset(&(host_addr.sin_zero), '\0', 8); // zero the rest of the struct

	if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1)
		fatal("binding to socket");

	if (listen(sockfd, 5) == -1)
		fatal("listening on socket");
	
	printf("listening on socket %d\n", sockfd);
	return sockfd;
}

void CComS::acceptSocket(int sockfd) {
	socklen_t sin_size;
	int nsockfd;
	int recv_length = 1;
	struct sockaddr_in client_addr;
	char buffer[1024];
	
	while(1) {    // Accept loop
		printf("waiting for client\n");
		sin_size = sizeof(struct sockaddr_in);
		nsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
		if(nsockfd == -1)
			fatal("accepting connection");
		printf("server: got connection from %s port %d\n",inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		recv_length = recv(nsockfd, &buffer, 1024, 0);
		dump(buffer, recv_length);
		
		close(nsockfd);
	}
}


void CComS::fatal(char *message) {
   char error_message[100];

   strcpy(error_message, "[!] Fatal Error ");
   strncat(error_message, message, 83);
   perror(error_message);
   exit(-1);
}

// dumps raw memory in strings
void CComS::dump(char *data_buffer, const unsigned int length) {
	unsigned int i;
	for(i=0; i < length; i++) {
		printf("%c", data_buffer[i]);
	}
}