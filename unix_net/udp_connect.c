#include "unp.h"

int udp_connect(const char *host, const char *serv)
{
    int sockfd, n;
    struct addrinfo hints, *res, *ressave;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family     = AF_INET;
    hints.ai_socktype   = SOCK_DGRAM;
    
    if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
    {
         printf("udp_connect getaddrinfo error for %s %s\n", host, serv);                           
         return -1;                                                                                 
     }                                                                                              
                                                                                                    
     ressave = res;                                                                                 
     while (res != NULL)                                                                            
     {                                                                                              
         sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);                       
         if (sockfd < 0)                                                                            
             continue;                                                                              
         if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)                                   
             break;                                                                                 
         res = res->ai_next;                                                                        
         close(sockfd);                                                                             
     }                                                                                              
                                                                                                    
     if (res == NULL)                                                                               
     {                                                                                              
         freeaddrinfo(ressave);                                                                     
         printf("udp_connect error for %s %s\n", host, serv);                                       
         return -1;                                                                                 
     }                                                                                              
     else                                                                                           
     {                                                                                              
         freeaddrinfo(ressave);                                                                     
         return sockfd;
	 }

}
