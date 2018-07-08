#include <ostream>
#include <iostream>
#include <unistd.h>
#include <csignal>
#include "CHTTPServer.h"
#include "CWebMeasure.h"

CWebMeasure* measure = 0;

void help(char* appname){

	printf("%s [OPTIONS]\n", appname);
	printf("Starts measurement using libmeasure and serves results as CSV using HTTP\n");
	printf("Options:\n");
	printf(" -i interval\t\tMeasurement and result interval for all resources in milliseconds\n");
	printf("            \t\tdefault: 2 seconds\n");
	printf("            \t\tminimal interval: 10 milliseconds\n");
	printf(" -p port\t\tHTTP server port\n");
	printf("        \t\tdefault: 9999\n");
	printf(" -b buffer size\t\tResult buffer size\n");
	printf("               \t\tdefault: 10\n");
	printf("               \t\tminimal buffer size: 1\n");

}

void serveSigaction(int sig){
}

void waitForSignal(){

	int ret = 0;
	struct sigaction siga = {};
	siga.sa_handler = (void(*)(int)) (&serveSigaction);
	ret = sigaction(SIGINT, &siga, 0);
	if (ret == -1) {

	}

	sigset_t set = {};
	siginfo_t signal = {};
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGTERM);
	ret = sigwaitinfo(&set, &signal);
	int error = 0;
    if (ret == -1) {
        error = errno;
        std::cout << "Signal error" <<  error << std::endl;
    } else {
        std::cout << "Received signal to shutdown" << std::endl;
    }
}

int main(int argc, char **argv){

	long intervalMs = 2000;
	int port = 9999;
	int bufferSize = 10;

	// check arguments
	int c = 0;
	
	while((c = getopt(argc, argv, "+i:p:b:h")) != -1) {
		switch (c) {
			case 'i':
				{
					char *endptr = NULL;
					long long int number;
					number = strtoll(optarg, &endptr, 10);
					if (NULL == endptr || (0 != *endptr) || number < 10) {
						fprintf(stderr, "-i invalid interval\n");
						exit(EXIT_FAILURE);
					}
					intervalMs = number;
				}
			break;
			case 'p':
				{
					char *endptr = NULL;
					long long int number;
					number = strtoll(optarg, &endptr, 10);
					if (NULL == endptr || (0 != *endptr) || number < 1 || number > 65536) {
						fprintf(stderr, "-p invalid port\n");
						exit(EXIT_FAILURE);
					}
					port = number;
				}		
			break;
			case 'b':
				{
					char *endptr = NULL;
					long long int number;
					number = strtoll(optarg, &endptr, 10);
					if (NULL == endptr || (0 != *endptr) || number < 1 || number > 100000) {
						fprintf(stderr, "-b invalid buffer size\n");
						exit(EXIT_FAILURE);
					}
					bufferSize = number;
				}		
			break;
			case 'h':
				help(argv[0]);
				exit(EXIT_SUCCESS);
			break;
			default:
				help(argv[0]);
				exit(EXIT_FAILURE);
			break;
		}
	}

	int ret = 0;

	measure = new CWebMeasure(intervalMs*1000000, bufferSize);
	measure->startMeasurement();

	CHTTPServer* server = new CHTTPServer(port);
	ret = server->start();
	if (0 == ret) {
		waitForSignal();
		server->stop();
	} else {
		std::cerr << "Failed to start server on port " << port << std::endl;
	}

	measure->stopMeasurement();
	delete server;
	
	delete measure;

	return 0;
}
