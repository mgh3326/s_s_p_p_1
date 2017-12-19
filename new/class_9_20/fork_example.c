#include<stdio.h>
#include<unistd.h>

int main()
{

    //printf("I am process %ld\n",(long)getpid());    
    fork();

    printf("I am process %ld\n",(long)getpid());    
    fork();

    printf("I am process %ld\n",(long)getpid());    
    fork();

    printf("I am process %ld\n",(long)getpid());
    
    fork();

    printf("I am process %ld\n",(long)getpid());
    //printf("rr");
    return 0;
}