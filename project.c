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
void parse_redirect(char *Arr[]);
int tab();
int double_tab();
static char temp[500];
static int AMPERSAND_count;
int count;
struct sigaction act;
static int status;
void execute_cmd(char *Arr[], int ohoh);
void execute_cmdgrp(char *Arr[]);
void ls_function();
void zombie_handler(int signo)
{
	pid_t pid;
	int stat;

	while ((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("\n자식 프로세스 (%d)가 정상적으로 종료되었습니다.\nUser Shall >>", pid);
	fflush(stdout);
}

void signal_handler(int signo)
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
	sigset_t set;

	sigfillset(&set);
	sigdelset(&set, SIGCHLD);
	//sigprocmask(SIG_SETMASK, &set, NULL);

	act.sa_flags = SA_RESTART;
	sigemptyset(&act.sa_mask);
	act.sa_handler = zombie_handler;
	sigaction(SIGCHLD, &act, 0);
	int c;
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	while (1)
	{
		count = 0;
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
				c = '\0';
				c = getch();
				if (c == '\t')
				{
					c = '\0';
					double_tab();
					for (int i = 0; i < count; i++)
					{
						printf("%c", temp[i]);
					}
				}

				else
				{

					if (c == 127)
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
			command_index++;				  // 인덱스 증가
			ptr = strtok(NULL, " ");		  // 다음 문자열을 잘라서 포인터를 반환
		}

		int pipe_count = 0;
		AMPERSAND_count = 0;

		if (strcmp(command_Arr[0], "q") == 0) //‘q’ 명령어를 입력받으면 Shell 종료하기
			exit(0);						  //종료
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
				command_Arr[i] = NULL;
				command_index--;
				//printf("test\n");
			}
			//printf("%s ", command_Arr[i]); // 문자열 포인터 배열에 인덱스로 접근하여 각 문자열 출력
		}
		//printf("\npipe_count=(%d)\n", pipe_count);

		no_pipe_run_command(command_Arr);
		fflush(stdin);
		free(input);
		input = NULL;
	}
	//fflush(stdin); // 키보드 버퍼를 모두 비워주는 함수
}

int tab()
{
	char *d;

	d = NULL;
	//printf("\ntest(%s)\n",d);
	int temp_count = 0;
	int temp_index = 0;
	char *oh;
	// char char_array[50];
	// char_array[0]='\0';
	for (int i = 0; i < count+1; i++)//?
	{
		if (temp[count - i] == '/' || temp[count - i] == ' ')
		{
			temp_index = count - i;
			break;
		}
		
		temp_count++;
	}
	//if(temp_count==0)
	//printf("ohohohohoh(%d)\n",temp_count);
	// printf("tabtest\n");
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
	int oh_count = 0;
	char *Arr[100] = {
		NULL,
	}; // 크기가 10인 문자열 포인터 배열을 선언하고 NULL로 초기화
	DIR *dir_info;
	struct dirent *dir_entry;
	dir_info = opendir("."); // 현재 디렉토리를 열기
	if (NULL != dir_info)
	{
		while (dir_entry = readdir(dir_info)) // 디렉토리 안에 있는 모든 파일과 디렉토리 출력
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
		if (strncmp(oh, Arr[i], temp_count - 1) == 0)
		{
			//printf("\n%s\n",Arr[i]);
			oh_count++;
		}
	}
	//printf("(%d)\n",oh_count);
	// char ** temp_Arr;
	// temp_Arr = (char **)malloc(oh_count * sizeof(char *));

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
				//printf("(%s)(%s)\n",oh,Arr[i]);
				// temp_Arr[oh_index] = (char *)malloc((strlen(Arr[i])+1) * sizeof(char));
				// temp_Arr[oh_index][strlen(Arr[i])+1]='\0';
				// temp_Arr[oh_index]=Arr[i];
				equal_index[oh_index] = i;
				oh_index++;
				//printf("ohoh\n");
				//break;
			}
		}
		equal_index[oh_index] = '\0';
		int k = 0;
		int t = 0;

		while (1)
		{

			for (int i = 1; i < oh_index; i++)
			{
				//printf("\n(%s)",Arr[equal_index[i]]);

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
		//같은놈들 모아둔거
		//printf("test\n");
		// int yes_index=0;
		// int j=0;
		// int r=0;
		// while(1)
		// {
		// 	for(int i=1;i<oh_count;i++)
		// 	{
		// 		printf("\n(%s)(%s)\n",temp_Arr[0],temp_Arr[i]);
		// 		if(strncmp(temp_Arr[0],temp_Arr[i],++j)!=0)
		// 		{r=1;
		// 			break;}
		// 		if(i==oh_count-1)
		// 		yes_index++;

		// 		//free(temp_Arr[i]); 왜 free가 안되지
		// 	}
		// 	if(r==1)
		// 	break;
		// }

		// printf("test(%d)\n",yes_index);
		// for(int i=strlen(oh);i<yes_index+1;i++)
		// {
		// 	printf("%c",temp_Arr[0][i]);
		// 	temp[count++]=temp_Arr[0][i];
		// }
	}
	else if (oh_count == 1)
	{
		//printf("1개야 1개\n");
		int i_one = 0;
		for (i_one = 0; i_one < index; i_one++)
		{
			if (strncmp(oh, Arr[i_one], temp_count - 1) == 0)
			{
				//printf("ohoh\n");
				break;
			}
		}
		//printf("test\n%s\n",Arr[i_one]);
		for (int i = temp_count - 1; i < strlen(Arr[i_one]); i++)
		{
			printf("%c", Arr[i_one][i]);
			temp[count++] = Arr[i_one][i];
		}
	}
	else
	{
		//printf("리턴이야 리턴\n");
		return 0;
	}

	//free(temp_Arr);
	free(equal_index);
	}
	fflush(stdout);
	return 0;
}
int double_tab()
{
	int oh = count - 1;
	int temp_count = 0;
	int temp_index = 0;
	//printf("(%c)",temp[oh]);
	if (temp[oh] == ' ')
	{
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
	dir = (char *)malloc(sizeof(char) * (temp_count + 1));
	for (int i = 0; i < temp_count + 1; i++)
	{
		dir[i] = '\0';
	}
	for (int i = 0; i < temp_count; i++)
	{
		dir[i] = temp[temp_index + i + 1];
	}
	//printf("\n%s\n",dir);

	DIR *dir_info;
	struct dirent *dir_entry;
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
			if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0)
			{
				continue;
			}
			printf("%s  ", dir_entry->d_name);
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
		// if(!AMPERSAND_count)
		//     {
		//         setpgid(pid, 0);
		//     }
		execute_cmdgrp(Arr);
		// parse_redirect(Arr);

		// execvp(Arr[0], Arr);
		// printf("command not found\n");
		//perror("Child failed to exec ls");
		return 1;
	}
	if (!AMPERSAND_count)
	{
		waitpid(pid, NULL, 0);
	}

	//tcsetpgrp(STDIN_FILENO, getpgid(0)); //쉘을 초기화 할때 사용합니다.
	fflush(stdout);
	return 0;
}
void parse_redirect(char *Arr[])
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
void execute_cmd(char *Arr[], int ohoh)
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

	for (int i = wow - 1; i >= 0; i--)
	{
		if (strcmp(Arr[i], "<") == 0)
		{
			if ((fd = open(Arr[i + 1], O_RDONLY | O_CREAT, 0644)) < 0)
				perror("file open error");
			dup2(fd, STDIN_FILENO);
			close(fd);
			Arr[i] = NULL;
			Arr[i + 1] = NULL;
			wow = wow - 2;
		}

		else if (strcmp(Arr[i], ">") == 0)
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

	//printf("\n(%d)\n",wow);
	if (wow == 1 && (strcmp(Arr[0], "ls") == 0))
	{
		ls_function();
	}
	else
	{

		//printf("쉘을 종료합니다.\n");

		execvp(Arr[0], Arr);
		printf("command not found\n"); //잘못 입력했을 경우
		exit(0);
	}
}

void execute_cmdgrp(char *Arr[])
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
	for (int i = 0; i < command_index; i++)
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
		execute_cmd(&Arr[0], 0);
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
				//execvp(pipe_Arr[yes_i][0], pipe_Arr[yes_i]);
				execute_cmd(pipe_Arr[yes_i], pipe_count_index[yes_i]);
				break;
			default:
				close(pfd[1]);
				dup2(pfd[0], STDIN_FILENO);
			}
		}

		//execvp(pipe_Arr[yes_i][0], pipe_Arr[yes_i]);

		execute_cmd(pipe_Arr[yes_i], pipe_count_index[yes_i]);
	}
	//printf("(%d)\n", oh_index);
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
void ls_function()
{
	DIR *dir_info;
	struct dirent *dir_entry;
	dir_info = opendir("."); // 현재 디렉토리를 열기
	if (NULL != dir_info)
	{
		while (dir_entry = readdir(dir_info)) // 디렉토리 안에 있는 모든 파일과 디렉토리 출력
		{
			if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0)
				continue; //.과 ..은 안 나오도록 하였습니다.

			printf("%s  ", dir_entry->d_name);
		}
		closedir(dir_info);
	}
	printf("\n");
	exit(0);
}