#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>

int main()
{
    int pid;
    int status;
    int spid;
    pid = fork();

    if(pid==0)
    {
        sleep(5);
        printf("I will be back %d\n",getpid());
        return 1;
    }
    else if(pid>0)
    {
        printf("I'm parent %d\n",getpid());
        printf("Press any key and wait\n");
        getchar();

        spid=wait(&status);
        printf("child process wait success\n");

        printf("PID     : %dn",spid);
        printf("Exit Value : %d\n",WEXITSTATUS(status));
        //WEXITSTATUS(status)
        // :exit()를 호출하기 위한 인자나 return 값이
        // 설정되고 종료된 자식의 반환 코드의 최하위
        // 8비트를 평가 (정상 종료 여부 판단)
        printf("Exit stat   :%d\n",WIFEXITED(status));
        // WIFEXITED(status)
        // : 자식이 정상적으로 종료되었다면 non-zero
    }
    else
    {
        perror("fork error :");
    }
}