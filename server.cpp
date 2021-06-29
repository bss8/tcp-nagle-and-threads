#include "common_helper.hpp"

#define _BSD_SOURCE
#include <arpa/inet.h>

void *thread_function_one(void *arg);
static void *doit(void *arg);
void disable_nagle_alg(int sockfd);
void setup_tcp_serv(int sockfd, sockaddr_in serv_addr);
void accept_and_process(int sockfd, socklen_t clilen, sockaddr_in cli_addr);
void look_sir_droids();

int main(int argc, char *argv[])
{

    std::cout << ANSII_BLUE_COUT << "Question 1 - program regarding Nagle algorithm." << ANSII_END << std::endl;

    int sockfd;
    socklen_t clilen;

    struct sockaddr_in serv_addr, cli_addr;

    // create a socket
    // socket(int domain, int type, int protocol)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "ERROR opening socket";
        return EXIT_FAILURE;
    }

    disable_nagle_alg(sockfd);
    setup_tcp_serv(sockfd, serv_addr);
    accept_and_process(sockfd, clilen, cli_addr);

    std::cout << ANSII_BLUE_COUT << "Question 2 - program regarding pthreads." << ANSII_END << std::endl;

    pthread_t thread_id;

    int thread_ret = pthread_create(&thread_id, NULL, &thread_function_one, NULL);

    // pthread_exit(NULL);
    // exit(0);

    if (thread_ret)
        std::cerr << ANSII_RED_START << "Thread creation failed : " << strerror(thread_ret) << ANSII_END << std::endl;
    else
    {
        std::cout << "Thread Created with ID : " << thread_id << std::endl;
        pthread_detach(thread_id);
    }

    for (int i = 0; i < 10; i++)
        std::cout << "non detached, main thread increment: " << i << std::endl;

    int listenfd, *iptr;
    pthread_t tid;
    socklen_t addrlen, len;
    struct sockaddr *cliaddr;

    for (;;)
    {
        len = addrlen;
        iptr = (int *)malloc(sizeof(int));
        //*iptr = accept(listenfd, cliaddr, &len);
        accept_and_process(sockfd, clilen, cli_addr);
        pthread_create(&tid, NULL, &doit, iptr);
    }

    close(sockfd);

    return EXIT_SUCCESS;
}

/**
 * 
 * 
 */ 
void *thread_function_one(void *arg)
{
    std::cout << "Thread Function :: Start" << std::endl;

    // pthread_exit(NULL);
    //exit(0);

    // Sleep for 2 seconds using the portable, standard C++ function sleep_for()
    // which takes a crono argument
    std::this_thread::sleep_for(std::chrono::seconds(2));

    for (int i = 0; i < 10; i++)
        std::cout << "non detached, non-main thread increment: " << i << std::endl;

    std::cout << "Thread Function :: End" << std::endl;
    return NULL;
}

/**
 * 
 * 
 */ 
static void *doit(void *arg)
{
    int connfd;

    connfd = *((int *)arg);
    free(arg);
    look_sir_droids();
    pthread_detach(pthread_self());
    std::cout << "Chancelor Palpatine says: DO IT ANAKIN!" << std::endl;
    close(connfd); /* done with connected socket */
    return (NULL);
}

/**
 * 
 * 
 */ 
void disable_nagle_alg(int sockfd)
{
    int isEnabled = 1;
    int set_res = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&isEnabled, sizeof(isEnabled));
    if (set_res < 0)
        std::cerr << "Error disabling Nagle's algorithm via setting socket option for TCP_NODELAY!"
                  << strerror(errno) << std::endl;
}

/**
 * 
 * 
 */ 
void setup_tcp_serv(int sockfd, sockaddr_in serv_addr)
{
    // clear address structure
    bzero((char *)&serv_addr, sizeof(serv_addr));

    /* setup the host_addr structure for use in bind call */
    // server byte order
    serv_addr.sin_family = AF_INET;

    // automatically be filled with current host's IP address
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // convert short integer value for port must be converted into network byte order
    serv_addr.sin_port = htons(PORT);

    // bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
    // bind() passes file descriptor, the address structure,
    // and the length of the address structure
    // This bind() call will bind  the socket to the current IP address on port
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        std::cerr << "ERROR on binding" << std::endl;
        exit(1);
    }
}

void accept_and_process(int sockfd, socklen_t clilen, sockaddr_in cli_addr)
{
    char buffer[BUF_SIZE];
    int newsockfd;
    int n;

    // This listen() call tells the socket to listen to the incoming connections.
    // The listen() function places all incoming connection into a backlog queue
    // until accept() call accepts the connection.
    // Here, we set the maximum size for the backlog queue to BACKLOG (should be 10 by default).
    listen(sockfd, BACKLOG);

    // The accept() call actually accepts an incoming connection
    clilen = sizeof(cli_addr);

    // This accept() function will write the connecting client's address info
    // into the the address structure and the size of that structure is clilen.
    // The accept() returns a new socket file descriptor for the accepted connection.
    // So, the original socket file descriptor can continue to be used
    // for accepting new connections while the new socker file descriptor is used for
    // communicating with the connected client.
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
    {
        std::cout << "ERROR on accept" << std::endl;
        exit(1);
    }

    std::cout << "server: got connection from " << inet_ntoa(cli_addr.sin_addr)
              << " on port " << ntohs(cli_addr.sin_port) << std::endl;

    send(newsockfd, "Hello world!\n", 12, 0);

    bzero(buffer, BUF_SIZE);

    for (int i = 0; i < 5; i++)
        send(newsockfd, " Repeat String! \n", 17, 0);

    n = read(newsockfd, buffer, BUF_SIZE - 1);
    if (n < 0)
    {
        std::cerr << "ERROR reading from socket" << std::endl;
        exit(1);
    }

    std::cout << ANSII_GREEN_START << "Client msg: " << buffer << ANSII_END << std::endl;

    close(newsockfd);
    
}

void look_sir_droids()
{
std::cout << "                                     /~\\                           " << std::endl; 
std::cout << "                                    |oo )      We're doomed!       " << std::endl; 
std::cout << "                                    _\\=/_                          " << std::endl; 
std::cout << "                    ___        #   /  _  \\   #                     " << std::endl; 
std::cout << "                   /() \\        \\\\//|/.\\|\\\\//                      " << std::endl; 
std::cout << "                 _|_____|_       \\/  \\_/  \\/                       " << std::endl; 
std::cout << "                | | === | |         |\\ /|                          " << std::endl; 
std::cout << "                |_|  O  |_|         \\_ _/                          " << std::endl; 
std::cout << "                 ||  O  ||          | | |                          " << std::endl; 
std::cout << "                 ||__*__||          | | |                          " << std::endl; 
std::cout << "                |~ \\___/ ~|         []|[]                          " << std::endl; 
std::cout << "                /=\\ /=\\ /=\\         | | |                          " << std::endl; 
std::cout << "________________[_]_[_]_[_]________/_]_[_\\_________________________" << std::endl;  
}