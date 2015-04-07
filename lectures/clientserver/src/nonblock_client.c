#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <fcntl.h>
#include <sys/epoll.h>


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

main( void)
{

//char * host = "192.168.101.99";
char * host = "127.0.0.1";
char * service = "2525";
char * proto = "tcp";
//char * proto = "sctp";
struct sockaddr_in sin, remote;
struct tm *tp;
time_t t;
int i, sd, rsd, rlen, readed, pid, rc;
char buf[513], t_str[512];

int flags;
int epfd;
struct epoll_event sd_ev;

    if ( (sd = mksock( host, service, proto, &sin)) == -1)
    {
		perror( "Ошибка при создании сокета");
		return 1;
    }

    printf( "Адрес порта сервера %s = %X\n", service, sin.sin_port);

    flags = fcntl( sd, F_GETFL);
    fcntl( sd, F_SETFL, flags | O_NONBLOCK);

    rc = connect( sd, (struct sockaddr *)&sin, sizeof( sin));

    epfd = epoll_create1(0);
    if ( epfd == -1)
    {
        perror("Can't create polling interface");
        exit(1);
    }
    sd_ev.events = EPOLLOUT | EPOLLONESHOT;
    sd_ev.data.fd = sd;

    if ( epoll_ctl( epfd, EPOLL_CTL_ADD, sd, &sd_ev) == -1)
    {
        perror("Can't add socket to polling");
        exit(1);
    }

    rc = epoll_wait(epfd, & sd_ev, 1, 300*1000);

    switch ( rc)
    {
        case 0:
                printf("Connection timeout\n");
                break;
        case -1:
                /* even for EINTR -- original code works in such way */
                perror("error in connection");
                exit(1);
                break;
        default:
                printf("Events detected: %d\n", rc);

                if ( sd_ev.events & ( EPOLLHUP | EPOLLRDHUP | EPOLLERR))
                    printf("error in connection\n");

                break;
    }


    fcntl( sd, F_SETFL, flags);

    close( sd);

    return 0;
}
