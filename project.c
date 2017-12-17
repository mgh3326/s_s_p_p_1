#include <stdio.h>
#include <termio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h>
int getch(void);
int runcommand(char **cline);
int no_pipe_run_command(char *Arr[]);
int one_pipe_run_command(char *Arr[], int command_index);
void parse_redirect(char* Arr[]);
int tab();
int double_tab();
static char temp[500];
int count;
void signal_handler(int signo) {
	if (signo == SIGINT)
	{
		printf("^C");
		temp[count++] = '^';
		temp[count++] = 'C';
		fflush(stdout);
	}
};
static int command_index;
int main(void)
{
	int c;
	//signal

	// sigset_t blockset;             // 막아놓을 시그널 목록
	// sigemptyset(&blockset);        //시그널 입력 자체를 무시하도록 하였습니다.
	// sigaddset(&blockset, SIGINT);  //목록에 해당 ^C시그널 추가
	// sigaddset(&blockset, SIGQUIT); //^\시그널 추가
	// sigprocmask(SIG_BLOCK, &blockset, NULL);
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	while (1)
	{
		count = 0;
		int blank_count = 0;
		for (int i = 0; i < 500; i++)
		{
			temp[i] = '\0';
		}
		char *input;
		printf("User Shall >>");
		while (1)
		{

			c = getch();
			if (c == '\t')
			{
				tab();
				//c='\0';
				continue;
				c = getch();
				// if (c == '\t')
				// {
				//    double_tab();
				//    //c='\0';
				//    continue;
				// }
				// else
				// {
				//     printf("%c", c);
				//     temp[count] = c;
				//     count++;
				// }

				//continue;
			}
			else if (c == 127)
			{
				if (count > 0)
				{
					printf("\b");
					temp[--count] = '\0';
					printf(" ");
					printf("\b");
					continue;
				}

			}
			else
			{
				printf("%c", c);
				temp[count] = c;
				count++;
				//continue;
			}


			if (c == '\n')
			{
				if (count == 1)
				{
					temp[--count] = '\0';
					printf("User Shall >>");
					continue;
				}
				//temp[count]='\0';
				//for(int i=0;i<count-1;i++)
				//printf("%c",temp[i]);
				input = (char *)malloc(sizeof(char) * (count));
				for (int i = 0; i < count; i++)
				{
					input[i] = '\0';
				}
				for (int i = 0; i < count - 1; i++)
				{
					input[i] = temp[i];
				}
				//count++;
				break;

			}
		}
		char *ptr = NULL;
		char *command_Arr[100] = {
			NULL,
		}; // 크기가 10인 문자열 포인터 배열을 선언하고 NULL로 초기화

		command_index = 0;
		//for(int i=0;i<count-1;i++)

		fflush(stdout);
		ptr = strtok(input, " ");

		while (ptr != NULL) // 자른 문자열이 나오지 않을 때까지 반복
		{
			command_Arr[command_index] = ptr; // 문자열을 자른 뒤 메모리 주소를 문자열 포인터 배열에 저장
			command_index++;           // 인덱스 증가
			ptr = strtok(NULL, " "); // 다음 문자열을 잘라서 포인터를 반환
		}


		int pipe_count = 0;
		int AMPERSAND_count = 0;

		if (strcmp(command_Arr[0], "q") == 0)
		{
			printf("쉘을 종료합니다.\n");
			exit(0); //종료
		}
		for (int i = 0; i < command_index; i++)
		{

			if (strcmp(command_Arr[i], "|") == 0) // 문자열 포인터 배열의 요소가 NULL이 아닐 때만
			{
				pipe_count++;
				//printf("test\n");
			}
			if (strcmp(command_Arr[i], "&") == 0) // 문자열 포인터 배열의 요소가 NULL이 아닐 때만
			{
				AMPERSAND_count++;
				//printf("test\n");
			}
			//printf("%s ", command_Arr[i]); // 문자열 포인터 배열에 인덱스로 접근하여 각 문자열 출력
		}
		//printf("\npipe_count=(%d)\n", pipe_count);


		no_pipe_run_command(command_Arr);

		// if (pipe_count == 0)
		// {
		//     no_pipe_run_command(command_Arr);
		// }
		// else if (pipe_count == 1)
		// {
		//     one_pipe_run_command(command_Arr, command_index);
		// }
		// for(int i=0;i<strlen(input);i++)
		// {
		//     if(input[i]=' ')
		//     {
		//         blank_count++;
		//     }
		// printf("%c",input[i]);

		// }
		// printf("\nblank_count :(%d)\n",blank_count);
		//printf("%s\n", input);

		//runcommand(&input);
		//no_pipe_run_command();

		//fflush(stdout);
		fflush(stdin);

		free(input);
		input = NULL;
	}
	//fflush(stdin); // 키보드 버퍼를 모두 비워주는 함수

}
int tab() {
	char * d;

	d = NULL;
	//printf("\ntest(%s)\n",d);
	int temp_count = 0;
	int temp_index = 0;
	char *oh;
	// char char_array[50];
	// char_array[0]='\0';
	for (int i = 0; i < count; i++)
	{
		if (temp[count - i] == '/')
		{
			temp_index = count - i;
			break;
		}
		temp_count++;
	}
	oh = (char *)malloc(sizeof(char) * (temp_count));
	for (int i = 0; i < temp_count; i++)
	{
		oh[i] = '\0';
	}
	for (int i = 0; i < temp_count - 1; i++)
	{
		oh[i] = temp[temp_index + i + 1];
	}
	//printf("\ntest (%d) (%d)\n",temp_index,temp_count);

	//printf("\n%s\n",oh);


	// for(int i=temp_index+1;i<count-temp_count+1;i++)
	// {
	//     char_array[i]=temp[i];
	//     printf("%c",temp[i]);
	// }
	// printf("test (%d)-(%d) = (%d)\n",count,temp_count,count-temp_count);
	// for(int i=0;i<count;i++)
	// {
	//     printf("%c",temp[i]);
	// }
	// printf("\n");
	int index = 0;
	int oh_count=0;
	char *Arr[100] = {
		NULL,
	}; // 크기가 10인 문자열 포인터 배열을 선언하고 NULL로 초기화
	DIR            *dir_info;
	struct dirent  *dir_entry;
	dir_info = opendir(".");              // 현재 디렉토리를 열기
	if (NULL != dir_info)
	{
		while (dir_entry = readdir(dir_info))  // 디렉토리 안에 있는 모든 파일과 디렉토리 출력
		{
			if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0)
			{
				continue;
			}
			Arr[index++] = dir_entry->d_name;

		}
		closedir(dir_info);
	}
	
	for (int i = 0; i < index; i++)
	{
		// printf("%s\n",Arr[i]);
		if (strcmp(oh, Arr[i]) == 0)
		{
			return 0;
			//break;//탈출
		}
	}
		for (int i = 0; i < index; i++)
		{
		if(strncmp(oh,Arr[i],temp_count-1)==0)
			{
				//printf("\n%s\n",Arr[i]);
				oh_count++;
			}
		}
		//printf("(%d)\n",oh_count);
		if(oh_count>1)//여러개 있을 때
		{
			printf("ohoh222222222222\n");

		}
		else if(oh_count==1)
		{
			//printf("1개야 1개\n");
			int i_one=0;
		for (i_one = 0; i_one < index; i_one++)
		{
		if(strncmp(oh,Arr[i_one],temp_count-1)==0)
			{
				//printf("ohoh\n");
				break;
			}
		}
					//printf("test\n%s\n",Arr[i_one]);
					for(int i=temp_count-1;i<strlen(Arr[i_one]);i++)
					{
						printf("%c",Arr[i_one][i]);
						temp[count++]=Arr[i_one][i];
					}
					
		}
		else{
			printf("리턴이야 리턴\n");
			return 0;
		}
	fflush(stdout);
	return 0;

}
int double_tab() {
	DIR            *dir_info;
	struct dirent  *dir_entry;
	dir_info = opendir(".");              // 현재 디렉토리를 열기
	if (NULL != dir_info)
	{
		while (dir_entry = readdir(dir_info))  // 디렉토리 안에 있는 모든 파일과 디렉토리 출력
		{
			if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0)
			{
				continue;
			}
			printf("%s\t", dir_entry->d_name);
		}
		closedir(dir_info);
	}
	printf("\nUser Shall >>");
	return 0;

}
int no_pipe_run_command(char *Arr[])
{

	pid_t pid;

	pid = fork();
	if (pid == -1)
	{
		perror("Failed to fork");
		return 1;
	}
	if (pid == 0)
	{
		parse_redirect(Arr);
		//execl("/bin/ls","ls","-l",NULL);
		execvp(Arr[0], Arr);
		//execlp("ls", "ls -l", NULL);
		printf("command not found\n");
		//perror("Child failed to exec ls");
		return 1;
	}
	//if(AMPERSAND_count) break;
	waitpid(pid, NULL, 0);
	tcsetpgrp(STDIN_FILENO, getpgid(0)); //쉘을 초기화 할때 사용합니다.
	fflush(stdout);
	return 0;
}
void parse_redirect(char* Arr[])
{

	int fd;

	for (int i = command_index - 1; i >= 0; i--)
	{
		if (strcmp(Arr[i], "<") == 0)
		{
			if ((fd = open(Arr[i + 1], O_RDONLY | O_CREAT, 0644)) < 0)
				perror("file open error");
			dup2(fd, STDIN_FILENO);
			close(fd);
			Arr[i] = NULL;
			Arr[i + 1] = NULL;
			command_index = command_index - 2;
		}



		else if (strcmp(Arr[i], ">") == 0)
		{
			if ((fd = open(Arr[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
				perror("file open error");
			dup2(fd, STDOUT_FILENO);
			close(fd);
			Arr[i] = NULL;
			Arr[i + 1] = NULL;
			command_index = command_index - 2;

		}





	}

}
int one_pipe_run_command(char *Arr[], int command_index)
{
	pid_t childpid;
	int fd[2];
	int pipe_index = 0;
	for (int i = 0; i < command_index; i++)
	{
		if (strcmp(Arr[i], "|") == 0) // 문자열 포인터 배열의 요소가 NULL이 아닐 때만
		{
			pipe_index = i;
			break;
			//printf("test\n");
		}
	}
	char *first_Arr[10] = {
		NULL,
	};
	char *second_Arr[10] = {
		NULL,
	};
	for (int i = 0; i < pipe_index; i++)
	{
		first_Arr[i] = Arr[i];
	}
	for (int i = pipe_index + 1; i < command_index; i++)
	{
		second_Arr[i] = Arr[i];
	}
	for (int i = 0; i < pipe_index; i++)
	{
		printf("%s ", first_Arr[i]);
	}
	printf("\n");
	for (int i = pipe_index + 1; i < command_index; i++)
	{
		printf("%s ", second_Arr[i]);
	}
	printf("\n");
	if ((pipe(fd) == -1) || ((childpid = fork()) == -1)) //파이프 생성 및 자식 프로세스 생성
	{
		perror("Failed to setup pipeline");
		return 1;
	}
	if (childpid == 0) //자식인 경우
	{
		if (dup2(fd[1], STDOUT_FILENO) == -1) //표준 출력
			perror("Failed to redirect stdout of ls");
		else if ((close(fd[0]) == -1) || (close(fd[1]) == -1)) //
			perror("Failed to close extra pipe descriptors on ls");
		else
		{
			//execl("/bin/ls", "ls", "-l", NULL);
			execvp(first_Arr[0], first_Arr);
			perror("Failed to exec ls");
		}
		return 1;
	}
	if (dup2(fd[0], STDIN_FILENO) == -1)
		perror("Failed to redirect stdin of sort");
	else if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
		perror("Failed to close extra pipe file descriptors on sort");
	else
	{
		// printf("\ntest\n");

		//execl("/usr/bin/sort", "sort", "-k", "+4", NULL);
		execvp(second_Arr[0], second_Arr);
		perror("Failed to exec sort");
	}
	return 1;
}
int runcommand(char **cline)
{
	int pid;
	int status;
	switch (pid = fork())
	{
	case -1:
		perror("smallsh");
		return (-1);
	case 0:
		//execvp(*cline, cline);
		execl("bin/ls", "ls", "-l", NULL);
		//perror(*cline);
		exit(1);
	}
	if (waitpid(pid, &status, 0) == -1)
		return (-1);
	else
		return (status);
}

int getch(void)
{
	int ch;
	struct termios buf, save;
	tcgetattr(0, &save);
	buf = save;
	buf.c_lflag &= ~(ICANON | ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;
	tcsetattr(0, TCSAFLUSH, &buf);
	ch = getchar();
	tcsetattr(0, TCSAFLUSH, &save);
	return ch;
}
