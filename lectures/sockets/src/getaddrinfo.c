#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

int main (void) {
    struct addrinfo hints = {AI_CANONNAME, AF_INET, SOCK_STREAM, IPPROTO_TCP,0, NULL, NULL, NULL};
    struct addrinfo *addr_res;
    int res=0;

	if (res=getaddrinfo ("google.by", "http", &hints, &addr_res) != 0) {
	    perror ("GETADDRINFO");
	    printf ("GETADDRINFO: %s\n", gai_strerror(res));
	} else {
    	    printf ("Результаты для сервиса http\n"); 
	    /* Пройдем по списку возвращенных структур */
    	    do {
	        printf ("Адрес сокета: Порт: %d IP-адрес: %s\n",
	    	    ntohs (((struct sockaddr_in *) addr_res->ai_addr)->sin_port),
		    inet_ntoa (((struct sockaddr_in *) addr_res->ai_addr)->sin_addr));
		printf ("Официальное имя хоста: %s\n", addr_res->ai_canonname);
	    } while ((addr_res = addr_res->ai_next) != NULL);
	}

    return 0;
}

