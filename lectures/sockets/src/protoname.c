#include <stdio.h>
#include <netdb.h>

int main (void) {
 struct protoent *pht;
 char *pct;
 int i;

 setprotoent (1);

 while ((pht = getprotoent ()) != NULL) {
   printf ("Официальное имя протокола: %s\n", pht->p_name);
   printf ("Альтернативные имена:\n");
   for (i = 0; (pct = pht->p_aliases [i]) != NULL; i++) {
     printf ("  %s\n", pct);
   }
   printf ("Номер протокола: %d\n\n", pht->p_proto);
 }

 if ((pht = getprotobyname ("ipv6")) != NULL) {
   printf ("Номер протокола ipv6: %d\n\n", pht->p_proto);
 } else {
   fprintf (stderr, "Протокол ip в базе не найден\n");
 }

 if ((pht = getprotobyname ("IPV6")) != NULL) {
   printf ("Номер протокола IPV6: %d\n\n", pht->p_proto);
 } else {
   fprintf (stderr, "Протокол IPV6 в базе не найден\n");
 }

 endprotoent ();

 return 0;
}
