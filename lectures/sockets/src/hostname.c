#include <stdio.h>
#include <netdb.h>

int main (void) {
 struct hostent *pht;
 char *pct;
 int i, j;

 sethostent (1);

 while ((pht = gethostent ()) != NULL) {
   printf ("Официальное имя хоста: %s\n", pht->h_name);
   printf ("Альтернативные имена:\n");
   for (i = 0; (pct = pht->h_aliases [i]) != NULL; i++) {
     printf ("  %s\n", pct);
   }
   printf ("Тип адреса хоста: %d\n", pht->h_addrtype);
   printf ("Длина адреса хоста: %d\n", pht->h_length);
   printf ("Сетевые адреса хоста:\n");
   for (i = 0; (pct = pht->h_addr_list [i]) != NULL; i++) {
     for (j = 0; j < pht->h_length; j++) {
   printf (" %d", (unsigned char) pct [j]);
     }
     printf ("\n");
   }
 }

 endhostent ();

 return 0;
}
