//²âÊÔIO½Ó¿Ú
#include "unp.h"

int main(void)
{
    char buf[BUFSIZE];
    //while (fgets(buf, 10, stdin) != NULL)
        //fgets(buf, 10, stdin);
    while (gets(buf) != NULL)
        printf("%s", buf);
    return 0;
}