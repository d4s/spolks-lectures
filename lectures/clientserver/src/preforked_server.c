#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>

#define CHILDS 10

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

char * host = "0.0.0.0";
char * service = "2525";
char * proto = "tcp";
//char * proto = "sctp";
struct sockaddr_in sin, remote;
struct tm *tp;
time_t t;
int i, sd, rsd, rlen, readed, pid;
char buf[513], t_str[512];


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
		pid = fork();
		switch ( pid)
		{
			case -1:
				perror( "Не создается дочерний процесс");
				break;
			case 0:
				pid = i+1;
				i=CHILDS+1;
			break;
			default:
				pid=0;
		}
	}

    if ( listen( sd, 0) == -1)
    {
	perror( "Ошибка при переводе сокета в пассивный режим");
        return 1;
	}

    while(1)
    {
		rlen = sizeof( remote);
		rsd = accept( sd, (struct sockaddr *)&remote, &rlen);
		fprintf( stderr, "Process %d accepted connection\n", pid);
		t = time( NULL);
		tp = localtime( &t);
		strftime( t_str, 512, "%a, %d %b %Y %T %z", tp);
		snprintf( buf, 512, "Server [%d]: %s\n", pid, t_str );
		send(rsd, buf, strlen(buf), 0);
		close( rsd);
    }

    return 0;
}
