#include <stdio.h>
#include <time.h>
#include <string.h>

#define DOMAINLEN  16
#define TLD ".proteansec"

/*
 * Generate a domain with DGA algorithm based on year/month/day variables.
 */
char* generate_domain(unsigned long year, unsigned long month, unsigned long day) {
  char domain[DOMAINLEN + sizeof(TLD)];
  unsigned short i;
  for(i=0; i<DOMAINLEN; i++) {
    year  = ((year ^ 8 * year) >> 11) ^ ((year & 0xFFFFFFF0) << 17);
    month = ((month ^ 4 * month) >> 25) ^ 16 * (month & 0xFFFFFFF8);
    day   = ((day ^ (day << 13)) >> 19) ^ ((day & 0xFFFFFFFE) << 12);
    domain[i] = (unsigned char)(((year ^ month ^ day) % 25) + 97);
  }

  // add TLD to the domain
  strncpy(domain+DOMAINLEN, ".infosec", sizeof(TLD));

  // finish string representation with NULL character
  domain[DOMAINLEN + sizeof(TLD)] = '\0' ;

  // return the domain
  return domain; 
}


/*
 * Main function that generates a daily DGA domain and outputs it to stdout.
 */
long main() {
  char* domain;
  time_t ctime = time(NULL);
  struct tm* ltime = localtime(&ctime);
  unsigned long year  = ltime->tm_year + 1900;
  unsigned long month  = ltime->tm_mon  + 1;
  unsigned long day    = ltime->tm_mday;
  printf("Time   : %lu:%lu:%lu\n", year, month, day);
  
  domain = generate_domain(year, month, day);
  printf("Domain : %s\n", domain);

  return 0;
}
