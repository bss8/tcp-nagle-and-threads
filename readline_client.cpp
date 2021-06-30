#include "common_helper.hpp"
#define MAXLINE 4096

void *copyto(void *);

typedef struct
{
	int read_fd;		/* caller's descriptor to read from */
	char *read_ptr;		/* caller's buffer to read into */
	size_t read_maxlen; /* max #bytes to read */
						/* next three are used internally by the function */
	int rl_cnt;			/* initialize to 0 */
	char *rl_bufptr;	/* initialize to rl_buf */
	char rl_buf[MAXLINE];
} Rline;

void readline_rinit(int, void *, size_t, Rline *);
ssize_t readline_r(Rline *);

static int sockfd; /* global for both threads to access */
static FILE *fp;
static pthread_key_t rl_key;
static pthread_once_t rl_once = PTHREAD_ONCE_INIT;

void Writen(int, void *, size_t);
ssize_t writen(int, const void *, size_t);

void str_cli(FILE *fp_arg, int sockfd_arg);
int Tcp_connect(const char *host, const char *serv);
void *copyto(void *arg);

static void readline_destructor(void *ptr);
static void readline_once(void);
ssize_t my_read(Rline *tsd, int fd, char *ptr);
ssize_t readline(int fd, void *vptr, size_t maxlen);
ssize_t Readline(int fd, void *ptr, size_t maxlen);
void *Calloc(size_t n, size_t size);

int main(int argc, char **argv)
{
	int sockfd;

	if (argc != 3)
	{
		printf("usage: tcpcli <hostname> <service>");
		return EXIT_FAILURE;
	}

	sockfd = Tcp_connect(argv[1], argv[2]);

	str_cli(stdin, sockfd); /* do it all */

	exit(0);
}

int tcp_connect(const char *host, const char *serv)
{
	int sockfd, n;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
	{
		printf("tcp_connect error for %s, %s: %s", host, serv, gai_strerror(n));
		exit(1);
	}
	ressave = res;

	do
	{
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0)
			continue; /* ignore this one */

		if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break; /* success */

		close(sockfd); /* ignore this one */
	} while ((res = res->ai_next) != NULL);

	if (res == NULL) /* errno set from final connect() */
	{
		printf("tcp_connect error for %s, %s", host, serv);
		exit(1);
	}

	freeaddrinfo(ressave);

	return (sockfd);
}
/* end tcp_connect */

/*
 * We place the wrapper function here, not in wraplib.c, because some
 * XTI programs need to include wraplib.c, and it also defines
 * a Tcp_connect() function.
 */

int Tcp_connect(const char *host, const char *serv)
{
	return (tcp_connect(host, serv));
}

void str_cli(FILE *fp_arg, int sockfd_arg)
{
	char recvline[MAXLINE];
	pthread_t tid;

	sockfd = sockfd_arg; /* copy arguments to externals */
	fp = fp_arg;

	pthread_create(&tid, NULL, copyto, NULL);

	while (Readline(sockfd, recvline, MAXLINE) > 0)
		fputs(recvline, stdout);
}

void *copyto(void *arg)
{
	char sendline[MAXLINE];

	while (fgets(sendline, MAXLINE, fp) != NULL)
		Writen(sockfd, sendline, strlen(sendline));

	shutdown(sockfd, SHUT_WR); /* EOF on stdin, send FIN */

	return (NULL);
	/* 4return (i.e., thread terminates) when EOF on stdin */
}

ssize_t /* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = (const char *)vptr;
	nleft = n;
	while (nleft > 0)
	{
		if ((nwritten = write(fd, ptr, nleft)) <= 0)
		{
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0; /* and call write() again */
			else
				return (-1); /* error */
		}

		nleft -= nwritten;
		ptr += nwritten;
	}
	return (n);
}
/* end writen */

void Writen(int fd, void *ptr, size_t nbytes)
{
	if (writen(fd, ptr, nbytes) != nbytes)
	{
		std::cerr << "Writen error" << std::endl;
		exit(1);
	}
}

static void readline_destructor(void *ptr)
{
	free(ptr);
}

static void readline_once(void)
{
	std::cout << "rl_key before pthread_key_create call: " << &rl_key << std::endl;
	pthread_key_create(&rl_key, readline_destructor);
	std::cout << "rl_key: " << &rl_key << std::endl;
	std::cout << "rl_key after pthread_key_create call: " << &rl_key << std::endl;
}

// typedef struct {
//   int	 rl_cnt;			/* initialize to 0 */
//   char	*rl_bufptr;			/* initialize to rl_buf */
//   char	 rl_buf[MAXLINE];
// } Rline;
/* end readline1 */

/* include readline2 */
// remove static here and in the signature up above to test without static declaration
ssize_t my_read(Rline *tsd, int fd, char *ptr)
{
	if (tsd->rl_cnt <= 0)
	{
	again:
		if ((tsd->rl_cnt = read(fd, tsd->rl_buf, MAXLINE)) < 0)
		{
			if (errno == EINTR)
				goto again;
			return (-1);
		}
		else if (tsd->rl_cnt == 0)
			return (0);
		tsd->rl_bufptr = tsd->rl_buf;
	}

	tsd->rl_cnt--;
	*ptr = *tsd->rl_bufptr++;
	return (1);
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
	size_t n, rc;
	char c, *ptr;
	Rline *tsd;

	// To test out #4 part a, comment out the below line
	//
	//pthread_once(&rl_once, readline_once);
	if ((tsd = (Rline *)pthread_getspecific(rl_key)) == NULL)
	{
		tsd = (Rline *)Calloc(1, sizeof(Rline)); /* init to 0 */
		std::cout << "rl_key before pthread_setspecific call: " << &rl_key << std::endl;
		pthread_setspecific(rl_key, tsd);
		std::cout << "rl_key after pthread_setspecific call: " << &rl_key << std::endl;
	}

	ptr = (char *)vptr;
	for (n = 1; n < maxlen; n++)
	{
		if ((rc = my_read(tsd, fd, &c)) == 1)
		{
			*ptr++ = c;
			if (c == '\n')
				break;
		}
		else if (rc == 0)
		{
			*ptr = 0;
			return (n - 1); /* EOF, n - 1 bytes read */
		}
		else
			return (-1); /* error, errno set by read() */
	}

	*ptr = 0;
	return (n);
}
/* end readline2 */

ssize_t Readline(int fd, void *ptr, size_t maxlen)
{
	ssize_t n;

	if ((n = readline(fd, ptr, maxlen)) < 0)
	{
		std::cerr << "readline error" << std::endl;
		exit(1);
	}
	return (n);
}

void *Calloc(size_t n, size_t size)
{
	void *ptr;

	if ((ptr = calloc(n, size)) == NULL)
	{
		std::cerr << "Calloc error" << std::endl;
		exit(1);
	}
	return (ptr);
}
