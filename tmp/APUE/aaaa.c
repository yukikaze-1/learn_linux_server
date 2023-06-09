//#define	_GNU_SOURCE
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


typedef void Sigfunc(int) ;

//signal definition
Sigfunc* signal(int signo,Sigfunc* func);

//signal func
static void sig_int(int signo);
static void sig_chld(int signo);

//system func
int system(const char* cmdstring);

int main()
{
    if(signal(SIGINT,sig_int) == SIG_ERR)
        printf("signal(SIGINT) error\n");
    if(signal(SIGCHLD,sig_chld) == SIG_ERR)
        printf("signal(SIGCHLD) error\n");
    if(system("/bin/ed") <0)
        printf("system() error\n");

    return 0;
}
/*
//old system
int system(const char* cmdstring)
{
    pid_t pid;
    int status;
    if(cmdstring == NULL)
        return (1);

    if((pid=fork()) < 0){
        status =-1;
    }else if(pid == 0){//child
        execl("/bin/sh","sh","-c",cmdstring,(char*)0);
        _exit(127);

    }else{//parent
        while( waitpid(pid,&status,0) < 0){
            if(errno == EINTR){
                status =-1;
                break;
            }
        }
    }
}
*/

int system(const char* cmdstring)
{
    pid_t pid;
    int status;
    struct sigaction ignore,saveintr,savequit;
    sigset_t  chldmask,savemask;
    if(cmdstring == NULL)
        return (1);
    
    //set the SIGCHLD and SIGQUIT to ignore
    ignore.sa_handler = SIG_IGN;
    sigemptyset(&ignore.sa_mask);
    ignore.sa_flags =0;
    if(sigaction(SIGINT,&ignore,&saveintr) < 0)
        return (-1);
    if(sigaction(SIGQUIT,&ignore,&savequit) < 0)
        return (-1);

    //block the SIGCHLD
    sigemptyset(&chldmask);
    sigaddset(&chldmask,SIGCHLD);
    if(sigprocmask(SIG_BLOCK,&chldmask,&savemask) < 0)
        return (-1);

    //fork and exec
    if((pid = fork()) <0){
        status =-1;
    }else if(pid == 0){ //child
        //recover the SIGINT and SIGQUIT sigaction
        sigaction(SIGINT,&saveintr,NULL);
        sigaction(SIGQUIT,&savequit,NULL);
        //recover the mask to unblock SIGCHLD
        sigprocmask(SIG_SETMASK,&savemask,NULL);

        execl("/bin/sh","sh","-c",cmdstring,(char*)0);
        _exit(127);

    }else{//parent
        while( waitpid(pid,&status,0) < 0){
            if(errno == EINTR){
                status =-1;
                break;
            }
        }
    }


    //recover the SIGINT and SIGQUIT
    if(sigaction(SIGINT,&ignore,&saveintr) < 0)
        return (-1);
    if(sigaction(SIGQUIT,&ignore,&savequit) < 0)
        return (-1);
    if(sigprocmask(SIG_SETMASK,&savemask,NULL) < 0)
        return (-1);

    return (status);

}//end system


Sigfunc* signal(int signo,Sigfunc* func)
{
    struct sigaction act,oact;

    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags =0;
    if(signo == SIGALRM){
        #ifdef SA_INTERRUPT
                act.sa_flags |= SA_INTERRUPT;
        #endif
    }else{
        act.sa_flags |= SA_RESTART;
    }

    if(sigaction(signo,&act,&oact) < 0)
        return (SIG_ERR);
    return (oact.sa_handler);

}

static void sig_int(int signo)
{
    printf("caught SIGINT\n");
}
static void sig_chld(int signo)
{
    printf("caught SIGCHLD\n");
}
