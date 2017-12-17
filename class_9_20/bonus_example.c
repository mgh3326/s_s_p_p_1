#include<unistd.h>
#include<stdio.h>

int main(){
    int i;
    for (i=0;i<30;i++)
    {
        printf("%d\n",i);
    }
    //printf("tset");
    printf("execl return : %d",execl("/bin/ls","ls","-l",NULL));

    for(i=0;i<300;i++){
        printf("%d\n",i);
    }
    return 0;
}