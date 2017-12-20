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
int run_command(char *Arr[]);
int tab(char *pwd);
int one_tab();
int double_tab();
static char temp[500];
static int AMPERSAND_count;
int count;
void redirection_run(char *Arr[], int ohoh);
void pipe_run(char *Arr[]);
void ls_function();
void zombie_handler(int signo)//SIGCHLD 핸들러 입니다. 좀비 프로세스가 생기는것을 막아줍니다.
{
	pid_t pid;
	while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
		{
			printf("\n자식 프로세스 (%d)가 정상적으로 종료되었습니다.\nUser Shall >>", pid);
			for (int i = 0; i < count; i++)
			printf("%c", temp[i]);
		}
	fflush(stdout);
}

void signal_handler(int signo)//SIGINT와 SIGQUIT 핸들러 함수입니다.
{
	if (signo == SIGINT)
	{
		printf("\nSIGINT 시그널은 무시되었습니다.\n");
	}
	else if (signo == SIGQUIT)
	{
		printf("\nSIGQUIT 시그널은 무시되었습니다.\n");
	}
	printf("User Shall >>");

	for (int i = 0; i < count; i++)
		printf("%c", temp[i]);
	fflush(stdout);
}
static int command_index;
int main(void)
{
	struct sigaction act;
	sigset_t set;
	sigfillset(&set);
	sigdelset(&set, SIGCHLD);//SIGCHLD 좀비 프로세스가 생기는것을 막아줍니다.
	act.sa_flags = SA_RESTART;//Background 프로세스 종료시에 사용됩니다.
	sigemptyset(&act.sa_mask);
	act.sa_handler = zombie_handler;
	sigaction(SIGCHLD, &act, 0);
	int c;
	signal(SIGINT, signal_handler);//SIGINT(Ctrl + C) 시그널로 Shell 종료를 방지하기
	signal(SIGQUIT, signal_handler);//SIGQUIT(Ctrl + \) 시그널로 Shell 종료를 방지하기
	while (1)//입력받은 명령어 실행 후 정상적으로 Shell로 복귀
	{
		count = 0;
		for (int i = 0; i < 500; i++)//초기화 해줍니다.
			temp[i] = '\0';
		char *input;
		printf("User Shall >>");
		while (1)
		{//사용자로부터 입력받은 명령어 실행하기 (총 3점)
			c = getch();//사용자로부터 명령어 입력받기 (1점)			
			//getch를 통해 사용자로부터 입력을 받습니다.
			if (c == '\t')
			{
				one_tab();
				
				c = '\0';
				c = getch();//한번 탭이 눌린 상태에서의 getch입니다.
				if (c == '\t')
				{
					c = '\0';
					double_tab();
					for (int i = 0; i < count; i++)
					{
						printf("%c", temp[i]);
					}
				}

				else//탭키 한번 입력후 바로 키를 입력 받기 위함입니다.
				{

					if (c == 127)//백스페이스바를 과제완느 무관하지만 구현했습니다.
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
					}

					if (c == '\n')//엔터 입력
					{
						if (count == 1)//아무것도 입력이 되지 않았다면 다시 입력을 받도록 하였습니다.
						{
							temp[--count] = '\0';
							printf("User Shall >>");
							continue;
						}
						input = (char *)malloc(sizeof(char) * (count));
						for (int i = 0; i < count; i++)
							input[i] = '\0';
						for (int i = 0; i < count - 1; i++)
							input[i] = temp[i];
						break;
					}
					continue;
				}
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
			}

			if (c == '\n')////입력받은 명령어 실행하기 (1.5점)
			//엔터값이 입력되면 
			{
				if (count == 1)//아무것도 입력이 되지 않았다면 다시 입력을 받도록 하였습니다.
				{
					temp[--count] = '\0';
					printf("User Shall >>");
					continue;
				}
				input = (char *)malloc(sizeof(char) * (count));
				for (int i = 0; i < count; i++)
				{
					input[i] = '\0';
				}
				for (int i = 0; i < count - 1; i++)
				{
					input[i] = temp[i];
				}
				break;
			}
		}
		char *ptr = NULL;
		char *command_Arr[100] = {
			NULL,
		}; // 크기가 100인 문자열 포인터 배열을 선언하고 NULL로 초기화
		command_index = 0;
		fflush(stdout);
		ptr = strtok(input, " ");
		while (ptr != NULL) // 자른 문자열이 나오지 않을 때까지 반복
		{
			command_Arr[command_index] = ptr; // 문자열을 자른 뒤 메모리 주소를 문자열 포인터 배열에 저장
			command_index++;				  // 인덱스 증가
			ptr = strtok(NULL, " ");		  // 다음 문자열을 잘라서 포인터를 반환
		}
		AMPERSAND_count = 0;
		if (strcmp(command_Arr[0], "q") == 0) //‘q’ 명령어를 입력받으면 Shell 종료하기(0.5점)
			exit(0);						  //종료
		for (int i = 0; i < command_index; i++)
		{
			if (strcmp(command_Arr[i], "&") == 0) // 문자열 배열 중에 & 가 있는지 확인하고
			{
				AMPERSAND_count++;//있으면 인덱스를 늘려주고
				command_Arr[i] = NULL;//널값을 추가시켜주고
				command_index--;//명령어의 인덱스를 줄여줍니다.
			}
		}
		run_command(command_Arr);//
		fflush(stdin);
		free(input);//동적할당 해준것을 반환하여 줍니다.
		input = NULL;
	}
}
int one_tab(){//
char *d;

	d = NULL;
	int temp_count = 0;
	int temp_index = 0;
	char *oh;
	for (int i = 0; i < count+1; i++)//?
	{
		if (temp[count - i] == ' ')
		{
			temp_index = count - i;
			break;
		}
		
		temp_count++;
	}
	
	if(temp_count>1)
	{

	
	oh = (char *)malloc(sizeof(char) * (temp_count));
	
	for (int i = 0; i < temp_count; i++)
	{
		oh[i] = '\0';
	}
	for (int i = 0; i < temp_count - 1; i++)
	{
		oh[i] = temp[temp_index + i + 1];
	}
	int wow_index=0;
	for (int i = 0; i < temp_count - 1; i++)
	{
		if(oh[temp_count-i-2]=='/')
		{
			wow_index=temp_count-i-2;
			break;
		}
		if(i ==temp_count-2)
		{
			wow_index=-1;
		}
	}
	char *temp_oh;
	if(wow_index>0)
	{
		temp_oh = (char *)malloc(sizeof(char) * (wow_index+2));
		for(int i=0;i<wow_index+2;i++)
		{
			temp_oh[i]='\0';
		}
		for(int i=0;i<wow_index+1;i++)
		{
			temp_oh[i]=oh[i];
		}
		if(strcmp(temp_oh,"./")==0)
		tab(NULL);
		else
		{
			tab(temp_oh);
		}
		
	
		free(oh);
		free(temp_oh);
	}
	else
	{
		tab(NULL);
	}
	}
	fflush(stdout);
	
	return 0;
}
int tab(char *pwd)//탭 1번 함수
{
	char *d;

	d = NULL;
	int temp_count = 0;
	int temp_index = 0;
	char *oh;
	for (int i = 0; i < count+1; i++)
	{
		if(pwd==NULL)
		{
			if (temp[count - i] == '/' || temp[count - i] == ' ')
			{
				temp_index = count - i;
				break;
			}
		}
		else
		{
			if (temp[count - i] == '/')
			{
				temp_index = count - i;
				break;
			}
		}
		
		temp_count++;
	}
	
	if(temp_count>1)
	{

	
	oh = (char *)malloc(sizeof(char) * (temp_count));
	for (int i = 0; i < temp_count; i++)
	{
		oh[i] = '\0';
	}
	for (int i = 0; i < temp_count - 1; i++)
	{
		oh[i] = temp[temp_index + i + 1];
	}
	
	int index = 0;
	int oh_count = 0;
	char *Arr[100] = {
		NULL,
	}; // 크기가 10인 문자열 포인터 배열을 선언하고 NULL로 초기화
	DIR *dir_info;
	struct dirent *dir_entry;
	if(pwd==NULL)
	{
		dir_info = opendir("."); // 현재 디렉토리를 열기
	}
	else
	{
		dir_info = opendir(pwd); // pwd 디렉토리를 열기
	}
	if (NULL != dir_info)
	{
		while (dir_entry = readdir(dir_info)) // 디렉토리 안에 있는 모든 파일과 디렉토리 출력
		{
			if (strncmp(dir_entry->d_name, ".",1) == 0)
			continue; //.으로 시작하는 . 과 ..과 숨긴파을은 안 나오도록 하였습니다.
			Arr[index++] = dir_entry->d_name;
		}
		closedir(dir_info);
	}

	for (int i = 0; i < index; i++)
	{
		if (strcmp(oh, Arr[i]) == 0)
		{
			return 0;
		}
	}
	for (int i = 0; i < index; i++)
	{
		if (strncmp(oh, Arr[i], temp_count - 1) == 0)
		{
			oh_count++;
		}
	}

	int num = oh_count;
	int oh_index = 0;
	int *equal_index;
	equal_index = (int *)malloc(sizeof(int) * index);
	if (oh_count > 1) //여러개 있을 때
	{
		for (int i = 0; i < index; i++)
		{
			if (strncmp(oh, Arr[i], temp_count - 1) == 0)
			{
				equal_index[oh_index] = i;
				oh_index++;
			}
		}
		equal_index[oh_index] = '\0';
		int k = 0;
		int t = 0;

		while (1)
		{

			for (int i = 1; i < oh_index; i++)
			{
				if (strncmp(Arr[equal_index[0]], Arr[equal_index[i]], k) != 0)
				{
					t = 1;
					break;
				}
				if (i == oh_index - 1)
				{

					k++;
				}
			}
			if (t == 1)
				break;
		}
		for (int i = strlen(oh); i < k - 1; i++)
		{
			printf("%c", Arr[equal_index[0]][i]);
			temp[count++] = Arr[equal_index[0]][i];
		}
		
	}
	else if (oh_count == 1)
	{
		int i_one = 0;
		for (i_one = 0; i_one < index; i_one++)
		{
			if (strncmp(oh, Arr[i_one], temp_count - 1) == 0)
			{
				break;
			}
		}
		for (int i = temp_count - 1; i < strlen(Arr[i_one]); i++)
		{
			printf("%c", Arr[i_one][i]);
			temp[count++] = Arr[i_one][i];
		}
	}
	else
	{
		return 0;
	}

	free(oh);
	free(equal_index);
	}
	fflush(stdout);
	return 0;
}
int double_tab()//탭을 2번 누를 경우 호출되는 함수
{
	int oh = count - 1;
	int temp_count = 0;
	int temp_index = 0;
	
	for(int i=0;i<count;i++)
	{
		if (temp[oh] == '/')
		{
			break;
		}
		oh--;
	}
	if (temp[oh] == '/')
	{
		for (int i = 1; i < oh; i++)
		{
			if (temp[oh - i] == ' ')
			{
				temp_index = oh - i;
				break;
			}
			temp_count++;
		}
	}
	char *dir;
	
	if(temp[temp_index + 1]=='.')
	{
		dir = (char *)malloc(sizeof(char) * (temp_count + 1));
		for (int i = 0; i < temp_count + 1; i++)
		{
			dir[i] = '\0';
		}
		for (int i = 0; i < temp_count; i++)
		{
			dir[i] = temp[temp_index + i + 1];
		}
	}
	else
	{
		dir = (char *)malloc(sizeof(char) * (temp_count + 3));
		for (int i = 0; i < temp_count + 3; i++)
		{
			dir[i] = '\0';
		}
		dir[0]='.';
		dir[1]='/';
		for (int i = 0; i < temp_count; i++)
		{
			dir[i+2] = temp[temp_index + i + 1];
		}
	}
	DIR *dir_info;
	struct dirent *dir_entry;
	// printf("\ntest\n");
	// printf("\n(%s)\n",dir);
	// fflush(stdout);
	if (temp_count > 0)
	{
		
		dir_info = opendir(dir);
	} // dir 디렉토리를 열기
	else
	{
		dir_info = opendir(".");
	} // 현재 디렉토리를 열기
	printf("\n");
	if (NULL != dir_info)
	{
		while (dir_entry = readdir(dir_info)) // 디렉토리 안에 있는 모든 파일과 디렉토리 출력
		{
			if (strncmp(dir_entry->d_name, ".",1) == 0)
			continue; //.으로 시작하는 . 과 ..과 숨긴파을은 안 나오도록 하였습니다.
			printf("%s  ", dir_entry->d_name);
		}
		closedir(dir_info);
	}
	printf("\nUser Shall >>");
	return 0;
}
int run_command(char *Arr[])
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
		pipe_run(Arr);
		return 1;
	}
	if (!AMPERSAND_count)//백그라운드 실행이 아닌 경우입니다.
	{
		waitpid(pid, NULL, 0);
	}
	fflush(stdout);
	return 0;
}

void redirection_run(char *Arr[], int ohoh)
{
	int wow = 0;
	if (ohoh == 0)
		wow = command_index;
	else
	{
		wow = ohoh;
	}
	//printf("\n(%d)\n",ohoh);
	int fd; //리다이렉션 부분

	for (int i = wow - 1; i >= 0; i--)//리다이렉션을 확인합니다.
	{
		if (strcmp(Arr[i], "<") == 0)//<발견의 경우
		{
			if ((fd = open(Arr[i + 1], O_RDONLY | O_CREAT, 0644)) < 0)
				perror("file open error");
			dup2(fd, STDIN_FILENO);
			close(fd);
			Arr[i] = NULL;
			Arr[i + 1] = NULL;
			wow = wow - 2;
		}

		else if (strcmp(Arr[i], ">") == 0)//> 발견의 경우
		{
			if ((fd = open(Arr[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0)
				perror("file open error");
			dup2(fd, STDOUT_FILENO);
			close(fd);
			Arr[i] = NULL;
			Arr[i + 1] = NULL;
			wow = wow - 2;
		}
	}
	if (wow == 1 && (strcmp(Arr[0], "ls") == 0))//명령어가 ls 함수를 호출하는 경우입니다.
	{

		ls_function();//ls 함수입니다.
	}
	else
	{
		execvp(Arr[0], Arr);
		printf("command not found\n"); //잘못 입력했을 경우
		exit(0);
	}
}

void pipe_run(char *Arr[])//파이프의 갯수를 확인하고 명령어를 반복해서 실행하려고 리다이렉션 후에 실행을 하는 함수로로 보내주는 함수입니다.
{
	int i = 0;
	int g_count = 0;
	int pfd[2];

	for (int i = 0; i < command_index; i++)
	{
		if (strcmp(Arr[i], "|") == 0)
			g_count++;
	}
	int *equal_index;
	equal_index = (int *)malloc(sizeof(int) * g_count);
	int *pipe_count_index;
	pipe_count_index = (int *)malloc(sizeof(int) * g_count);
	for (int i = 0; i < g_count; i++)
		pipe_count_index[i] = 0;
	int oh_index = 0;
	for (int i = 0; i < command_index; i++)//파이프 갯수를 확인하고 파이프를 문자열 배열에서 없애줍니다.
	{
		if (strcmp(Arr[i], "|") == 0)
		{
			Arr[i] = NULL;
			equal_index[oh_index] = i;
			oh_index++;
		}
	}
	equal_index[oh_index] = command_index;
	oh_index++;
	char *pipe_Arr[100][100] = {
		NULL,
	};
	for (int i = 0; i < 100; i++)
		for (int j = 0; j < 100; j++)
			pipe_Arr[i][j] = NULL;
	for (int i = 0; i < oh_index; i++)
	{
		if (i == 0)
		{
			for (int j = 0; j < equal_index[0]; j++)
			{
				pipe_Arr[i][j] = Arr[j];
				pipe_count_index[0]++;
			}
		}
		else
		{
			for (int j = 0; j < equal_index[i] - equal_index[i - 1] - 1; j++)
			{
				pipe_Arr[i][j] = Arr[j + equal_index[i - 1] + 1];
				pipe_count_index[i]++;
			}
		}
	}
	int k = 0;
	free(equal_index);
	int yes_i = 0;
	if (g_count == 0)
	{
		redirection_run(&Arr[0], 0);
	}
	else
	{
		//printf("\ntest\n");
		for (yes_i = 0; yes_i < oh_index - 1; yes_i++)
		{
			pipe(pfd);
			switch (fork())
			{
			case -1:
				perror("fork error");
			case 0:
				close(pfd[0]);
				dup2(pfd[1], STDOUT_FILENO);
				redirection_run(pipe_Arr[yes_i], pipe_count_index[yes_i]);
				break;
			default:
				close(pfd[1]);
				dup2(pfd[0], STDIN_FILENO);
			}
		}
		redirection_run(pipe_Arr[yes_i], pipe_count_index[yes_i]);
	}
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
void ls_function()//ls 함수 구현하기(1점)
{//opendir 함수를 통해서 현재 경로의 파일 및 디렉토리 목록 출력 하도록 하였습니다.
	DIR *dir_info;
	struct dirent *dir_entry;
	dir_info = opendir("."); // 현재 디렉토리를 열기
	if (NULL != dir_info)
	{
		while (dir_entry = readdir(dir_info)) // 디렉토리 안에 있는 모든 파일과 디렉토리 출력
		{
			if (strncmp(dir_entry->d_name, ".",1) == 0)
				continue; //.으로 시작하는 . 과 ..과 숨긴파을은 안 나오도록 하였습니다.

			printf("%s  ", dir_entry->d_name);
		}
		closedir(dir_info);
	}
	printf("\n");
	exit(0);
}