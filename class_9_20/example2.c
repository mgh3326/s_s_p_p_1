#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
int main(void){
pid_t flag; // 부모 내에서 프로세스 식별번호 저장
printf ("Just one process so far\n");
printf ("Calling for ...\n");
flag = fork(); // 새로운 프로세스를 생성
if(flag == -1){ //fork 실패
    printf ("Fork returned error code. no child\n");
    exit(1);
    }
    else if(flag == 0){ // 자식 프로세스의 경우
    printf ("I`m the child. I have d pid %d",
    (int)getpid());
    }
    else{ // 부모 프로세스의 경우
    printf ("I`m the parent. I have pid %d\n", (int) getpid());
    }
    return 0;
    }