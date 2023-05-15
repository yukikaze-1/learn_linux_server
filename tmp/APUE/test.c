#include <sys/resource.h>
#include <stdlib.h>
# include <sys/acct.h>

int main(int argc,char ** argv)
{
    struct rlimit a;

    printf("WRYYY!!!!:%lu\n",RLIM_INFINITY);

    getrlimit(RLIMIT_AS,&a);
    printf("RLIMIT_AS: \nsoft:%lu\nhard:%lu\n",a.rlim_cur,a.rlim_max);

    getrlimit(RLIMIT_CPU,&a);
    printf("RLIMIT_CPU: \nsoft:%lu\nhard:%lu\n",a.rlim_cur,a.rlim_max);
    
    getrlimit(RLIMIT_NICE,&a);
    printf("RLIMIT_NICE: \nsoft:%lu\nhard:%lu\n",a.rlim_cur,a.rlim_max);

    getrlimit(RLIMIT_NOFILE,&a);
    printf("RLIMIT_NOFILE: \nsoft:%lu\nhard:%lu\n",a.rlim_cur,a.rlim_max);

    getrlimit(RLIMIT_RSS,&a);
    printf("RLIMIT_RSS: \nsoft:%lu\nhard:%lu\n",a.rlim_cur,a.rlim_max);

    getrlimit(RLIMIT_SIGPENDING,&a);
    printf("RLIMIT_SIGPENDING: \nsoft:%lu\nhard:%lu\n",a.rlim_cur,a.rlim_max);

    getrlimit(RLIMIT_STACK,&a);
    printf("RLIMIT_STACK: \nsoft:%lu\nhard:%lu\n",a.rlim_cur,a.rlim_max);
    return 0;
}