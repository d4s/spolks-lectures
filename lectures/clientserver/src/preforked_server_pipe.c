#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>

#define CHILDS 10

typedef struct{
	pid_t cpid; //ID дочернего процесса
	int cfd; // канал для связи с дочерним процессом
	int status; // свободен/занят
} child;

child cptr[CHILDS];

int mksock( char *host, char * service, char * proto, struct sockaddr_in *sin)
{
struct hostent *hptr;
struct servent *sptr;
struct protoent *pptr;
int sd=0, type;

int ret, len;
struct linger sling;

    memset( sin, 0, sizeof( *sin));
    sin->sin_family = AF_INET;
    
    if( hptr = gethostbyname( host))
	memcpy( & sin->sin_addr, hptr->h_addr, hptr->h_length);
    else
	return -1;

    if ( ! ( pptr = getprotobyname( proto)) )
	return -1;

    if( sptr = getservbyname( service, proto))
	sin->sin_port = sptr->s_port;
    else 
	if( (sin->sin_port = htons(( unsigned short) atoi (service))) == 0)
	    return -1;

    if ( strcmp( proto, "udp") == 0)
	type = SOCK_DGRAM;
    else if (strcmp( proto, "sctp") == 0)
	type = SOCK_SEQPACKET;
    else
	type = SOCK_STREAM;


    if ( (sd = socket( PF_INET, type, pptr->p_proto)) < 0)
    {
	perror( "Ошибка при распределении сокета");
	return -1;
    }

	return sd;
}

int signal_handler(int sig, siginfo_t *info,void *args)
{
int i;
	fprintf(stderr, "Signal catched from %d\n", info->si_pid);
	for(i=0; i<CHILDS; i++)
		if( info->si_pid == cptr[i].cpid)
			cptr[i].status=0;
	return 1;
}

int read_fd(int recvfd)
{
struct msghdr    msg;
struct cmsghdr  *cmsg;
union {
	struct cmsghdr hdr;
	unsigned char    buf[CMSG_SPACE(sizeof(int))];
} cmsgbuf;
int fd;

struct iovec iov;

char ptr[]={0};
		memset(&msg, 0, sizeof(msg));
		msg.msg_control = &cmsgbuf.buf;
		msg.msg_controllen = sizeof(cmsgbuf.buf);
	
		msg.msg_name=NULL;
		msg.msg_namelen = 0;

		iov.iov_base = ptr;
		iov.iov_len = 1;
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;

		recvmsg(recvfd, &msg, 0);
		fprintf( stderr, "Received message\n");
		for (cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL;
			cmsg = CMSG_NXTHDR(&msg, cmsg)) {
		if (cmsg->cmsg_len == CMSG_LEN(sizeof(int)) &&
			cmsg->cmsg_level == SOL_SOCKET &&
			cmsg->cmsg_type == SCM_RIGHTS) {
				fd = *(int *)CMSG_DATA(cmsg);
				return(fd);
		}
	}
	fprintf( stderr, "Received message - end???\n");
}

void send_fd(int connfd, int sendfd)
{
struct msghdr    msg;
struct cmsghdr  *cmsg;
struct iovec iov;

char ptr[]={0};


union {
	struct cmsghdr hdr;
	unsigned char    buf[CMSG_SPACE(sizeof(int))];
	} cmsgbuf;

		memset(&msg, 0, sizeof(msg));
		msg.msg_control = &cmsgbuf.buf;
		msg.msg_controllen = sizeof(cmsgbuf.buf);

		msg.msg_name=NULL;
		msg.msg_namelen = 0;

		iov.iov_base = ptr;
		iov.iov_len = 1;
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;

		cmsg = CMSG_FIRSTHDR(&msg);
		cmsg->cmsg_len = CMSG_LEN(sizeof(int));
		cmsg->cmsg_level = SOL_SOCKET;
		cmsg->cmsg_type = SCM_RIGHTS;
		*(int *)CMSG_DATA(cmsg) = sendfd;
		
		sendmsg(connfd, &msg, 0);
}


main( void)
{

char * host = "0.0.0.0";
char * service = "2525";
char * proto = "tcp";
//char * proto = "sctp";
struct sockaddr_in sin, remote;
struct tm *tp;
time_t t;
int i, sd, rsd, rlen, readed, pid, ppid;
int sockfd[2];
char buf[513], t_str[512];

struct sigaction action;

    if ( (sd = mksock( host, service, proto, &sin)) == -1)
    {
		perror( "Ошибка при создании сокета");
		return 1;
    }

    printf( "Адрес сервера %s = %s\n", host, (char *) (inet_ntoa( sin.sin_addr)));
    printf( "Адрес порта сервера %s = %X\n", service, sin.sin_port);

	
	i = 1;
	i = setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, &i, sizeof( &i));
	if( i != 0) perror("Опция сокета (SOL_SOCKET, SO_REUSEADDR))");

	if( bind( sd, (struct sockaddr *) &sin, sizeof( sin)) < 0)
	{
		perror( "Ошибка при привязке сокета");
		return 1;
	}

	for ( i=0; i<CHILDS; i++)
	{
		socketpair( AF_LOCAL, SOCK_STREAM, 0, sockfd);
		pid = fork();
		switch ( pid)
		{
			case -1:
				perror( "Не создается дочерний процесс");
				break;
			case 0: //дочерний
				pid = i+1;
				i=CHILDS+1;
				close( sd);
				close( sockfd[0]);
				ppid=getppid();

				break;
			default: // родительский
				close( sockfd[1]);
				cptr[i].cpid = pid;
				cptr[i].cfd = sockfd[0];
				cptr[i].status = 0;
				
				pid=0;

				action.sa_handler = signal_handler;
				sigemptyset (&action.sa_mask);
				action.sa_flags = SA_SIGINFO;
				sigaction (SIGUSR1, &action, NULL);


		}
	}


	if( pid == 0)
	{ // Управляющий сервер
		if ( listen( sd, 0) == -1)
		{
			perror( "Ошибка при переводе сокета в пассивный режим");
			return 1;
		}

		while(1)
		{
			rlen = sizeof( remote);
			while (	(rsd = accept( sd, (struct sockaddr *)&remote, &rlen)) == -1);
			i=0;
			while(cptr[i].status != 0){
				if ( i == CHILDS){
					i=0;
					fprintf( stderr, "Main process: all childs are busy\n");
					sleep(1);
				}
				i++;
			}
			fprintf( stderr, "Main process accepted connection and pass it to [%d]\n", cptr[i].cpid);
			cptr[i].status=1;
			send_fd( cptr[i].cfd, rsd);

			close( rsd);
		}
	} else { // Управляемый сервер
		while(1)
		{
			rsd=read_fd( sockfd[1]);
			t = time( NULL);
			tp = localtime( &t);
			strftime( t_str, 512, "%a, %d %b %Y %T %z", tp);
			snprintf( buf, 512, "Server [%d]: %s\n", pid, t_str );
			send(rsd, buf, strlen(buf), 0);
			close( rsd);
			sleep( 3);
			kill( ppid, SIGUSR1);
		}

	}

    return 0;
}
