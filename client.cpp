#include "common_helper.hpp"

void disable_nagle_alg(int sockfd);

int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[BUF_SIZE];

    if (argc < 2)
    {
        fprintf(stderr, "usage %s hostname\n", argv[0]);
        return EXIT_FAILURE;
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        std::cerr << "ERROR opening socket" << std::endl;

    disable_nagle_alg(sockfd);

    server = gethostbyname(argv[1]);

    if (server == NULL)
    {
        std::cerr << stderr << "  ERROR, no such host" << std::endl;
        return EXIT_FAILURE;
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        std::cerr << "ERROR connecting" << std::endl;

    printf("Enter the message to send the server: ");
    bzero(buffer, BUF_SIZE);
    //fgets(buffer, BUF_SIZE - 1, stdin);  // Do NOT use fgets or fputs as per assignment instructions

    std::cin >> buffer;

    n = write(sockfd, buffer, strlen(buffer));

    if (n < 0)
        std::cerr << "ERROR writing to socket" << std::endl;

    bzero(buffer, BUF_SIZE);
    n = read(sockfd, buffer, BUF_SIZE - 1);

    if (n < 0)
        std::cerr << "ERROR reading from socket" << std::endl;

    std::cout << buffer << std::endl;

    close(sockfd);

    return EXIT_SUCCESS;
}

/**
 * disables Nagle's algorithm, although we really only need to worry about this on the sender (client end). 
 * It is done here for practice and convenience. 
 * if TCP_NODELAY is set (on), the algorithm is considered turned off.  
 */
void disable_nagle_alg(int sockfd)
{
    int isEnabled = 1;
    int set_res = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&isEnabled, sizeof(isEnabled));
    if (set_res < 0)
        std::cerr << "Error disabling Nagle's algorithm via setting socket option for TCP_NODELAY!"
                  << strerror(errno) << std::endl;
}
