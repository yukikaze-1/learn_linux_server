#include "unp.h"

int main(int argc,char** argv)
{
    char * ptr;
    char** pptr;
    char buf[INET_ADDRSTRLEN];
    struct hostent *hptr;

    if(argc<2)  printf("lack of host name!\n");
    //err_sys("lack of host name!");

    while(--argc > 0){
        ptr = *++argv;
        if( (hptr = gethostbyname(ptr) ) == NULL){
            //err_msg("get hostname error for host: %s: %s",ptr,hstrerror(h_errno));
            printf("get hostname error for host: %s: %s\n",ptr,hstrerror(h_errno));
            continue;
        }

        printf("offical name: %s \n",hptr->h_name);
        for(pptr = hptr->h_aliases;*pptr!=NULL;++pptr){
            printf("\talias: %s \n",*pptr);
        }
        
        switch(hptr->h_addrtype){
            case AF_INET:
                pptr  = hptr->h_addr_list;
                while(*pptr!=NULL){
                    printf("\taddress: %s\n",inet_ntop(AF_INET,*pptr,buf,sizeof(buf)));
                    //printf("\taddress: %s\n",Inet_ntop(AF_INET,ptr,buf,MAXLINE));
                    ++pptr;
                }
                break;
                default:
                //err_ret("unknown address type");
                printf("unknown address type");
                break;
        }//end swtich
    }//end while

    return 0;
}