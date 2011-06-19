#include <stdio.h>
#include <netdb.h>

int main (void) {
 struct servent *pht;
 char *pct;
 int i;

 setservent (1);

 while ((pht = getservent ()) != NULL) {
   printf ("Официальное имя сервиса: %s\n", pht->s_name);
   printf ("Альтернативные имена:\n");
   for (i = 0; (pct = pht->s_aliases [i]) != NULL; i++) {
     printf ("  %s\n", pct);
   }
   printf ("Номер порта: %d\n", ntohs ((in_port_t) pht->s_port));
   printf ("Имя протокола: %s\n\n", pht->s_proto);
 }

 if ((pht = getservbyport (htons ((in_port_t) 21), "udp")) != NULL) {
   printf ("Официальное имя сервиса: %s\n", pht->s_name);
   printf ("Альтернативные имена:\n");
   for (i = 0; (pct = pht->s_aliases [i]) != NULL; i++) {
     printf ("  %s\n", pct);
   }
   printf ("Номер порта: %d\n", ntohs ((in_port_t) pht->s_port));
   printf ("Имя протокола: %s\n\n", pht->s_proto);
 } else {
   perror ("GETSERVBYPORT");
 }

 if ((pht = getservbyport (htons ((in_port_t) 21), (char *) NULL)) != NULL) {
   printf ("Официальное имя сервиса: %s\n", pht->s_name);
   printf ("Альтернативные имена:\n");
   for (i = 0; (pct = pht->s_aliases [i]) != NULL; i++) {
     printf ("  %s\n", pct);
   }
   printf ("Номер порта: %d\n", ntohs ((in_port_t) pht->s_port));
   printf ("Имя протокола: %s\n\n", pht->s_proto);
 } else {
   perror ("GETSERVBYPORT");
 }

 endservent ();

 return 0;
}
