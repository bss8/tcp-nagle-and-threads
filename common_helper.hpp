#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <chrono>

#include <pthread.h>

#include <netinet/tcp.h> // TCP_

#define PORT 5001
#define BUF_SIZE 4096
#define BACKLOG 10

// ANSII codes for colored text, to improve UI and readability
std::string ANSII_BLUE_START = "\033[0;30;46m";
std::string ANSII_RED_START = "\033[0;31m";
std::string ANSII_RED_HIGH = "\033[9;37;41m";
std::string ANSII_END = "\033[0m";
std::string ANSII_GREEN_START = "\033[0;32m";
std::string ANSII_BLUE_COUT = "\033[0;30;46m";
std::string ANSII_RED_COUT = "\033[41;1m";
std::string ANSII_GREEN_COUT = "\033[0;30;42m";
std::string ANSII_YELLOW_COUT = "\033[30;48;5;3m";



