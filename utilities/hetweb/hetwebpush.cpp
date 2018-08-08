#include <ostream>
#include <iostream>
#include <unistd.h>
#include <csignal>
#include <cstring>
#include "CWebMeasure.h"
#include "CCURLUpload.h"

CWebMeasure* measure = 0;

void help(char* appname){

	printf("%s [OPTIONS]\n", appname);
	printf("Starts measurement using libmeasure and sends results as JSON to a URL\n");
	printf("Options:\n");
	printf(" -i interval\t\tMeasurement and result interval for all resources in milliseconds\n");
	printf("            \t\tdefault: 2 seconds\n");
	printf("            \t\tminimal interval: 10 milliseconds\n");
	printf(" -u url\t\t\tURL to send the data to\n");
	printf(" -s interval\t\tInterval for sending the results stored in the buffer to the URL\n");
	printf("            \t\tin milliseconds\n");
	printf("            \t\tdefault: 2 seconds\n");
	printf("            \t\tminimal interval: 500 milliseconds\n");
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
	long intervalSendMs = 2000;
	char* url = 0;
	int bufferSize = 10;

	// check arguments
	int c = 0;
	
	while((c = getopt(argc, argv, "+i:u:s:b:h")) != -1) {
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
			case 's':
				{
					char *endptr = NULL;
					long long int number;
					number = strtoll(optarg, &endptr, 10);
					if (NULL == endptr || (0 != *endptr) || number < 500) {
						fprintf(stderr, "-s invalid interval\n");
						exit(EXIT_FAILURE);
					}
					intervalSendMs = number;
				}
			break;

			case 'u':
				{
					url = optarg;
					if (url == 0 || strlen(url) == 0) {
						fprintf(stderr, "-u invalid URL\n");
						exit(EXIT_FAILURE);
					}
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

	if (url == 0) {
		std::cerr << "-u URL not given " << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "URL: " << url << std::endl;
	std::cout << "Measurement interval: " << intervalMs << " ms" << std::endl;
	std::cout << "Send interval: " << intervalSendMs << " ms" << std::endl;
	std::cout << "Buffer size: " << bufferSize << std::endl;

	measure = new CWebMeasure(intervalMs*1000000, bufferSize);
	measure->startMeasurement();

	std::cout << "Measurement started" << std::endl;

	CCURLUpload* upload = new CCURLUpload(*measure, url, intervalSendMs);
	ret = upload->start();
	if (0 == ret) {
		std::cout << "Upload started" << std::endl;
	} else {
		std::cout << "Starting upload failed" << std::endl;
	}

	if (0 == ret) {
		waitForSignal();
	}

	
	upload->stop();
	std::cout << "Upload stopped" << std::endl;

	delete upload;

	measure->stopMeasurement();
	std::cout << "Measurement stopped" << std::endl;
	
	delete measure;


	return 0;
}
