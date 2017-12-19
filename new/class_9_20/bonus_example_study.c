#include<unistd.h>
#include<stdio.h>
#include <stdlib.h>

int main(){
    int i;
    for (i=0;i<30;i++)
    {
        printf("%d\n",i);
    }
    printf("tset\n\n");
    //printf("execl return : %d",execl("/bin/ls","ls","-l",NULL));
    if (execlp("/bin/ls", "ls", "-l", (char *)NULL) == -1) {
        perror("execlp");
        exit(1);
    }
    for(i=0;i<30;i++){
        printf("%d\n",i);
    }
    return 0;
}