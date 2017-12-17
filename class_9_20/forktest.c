#define N			100000000	// for 문으로 1부터 세어올라갈 숫자
#define TARGET_NUM	8			// 8개의 프로세스

int main() {
  for (processes = 1; processes <= TARGET_NUM; processes++) {
      pid = fork();
      if (pid < 0) {
          fprintf(stderr, "Fork failed! Aborting..\n");
          return -1;
      } else if (pid == 0) {
          // 자식 프로세스
          printf("Process %ld (%d) started.\n", processes, getpid());
          fflush(stdout);

          // 타이머 시작, 연산, 타이머 끝, 타이머 계산
          gettimeofday(&start_time[processes-1],NULL);
          for (i = 0, x = 0; i < N/input; i++) {
              x = x + 1;
          }
          gettimeofday(&stop_time[processes-1],NULL);
          timersub(&stop_time[processes-1], 
          	&start_time[processes-1], 
          	&elapsed_time[processes-1]);

          // 이해하기 쉬운 메세지를 출력한다.
          printf("Process %ld (%d) total time was %f seconds. x = %lld.\n",
          			processes,
          			getpid(),
          			elapsed_time[processes-1].tv_sec+elapsed_time[processes-1].tv_usec/1000000.0, x);
          fflush(stdout);

          // 자식 프로세스는 여기까지만 하면 된다. break 문을 사용하지 않으면 자기 혼자 for 문을 반복하게 된다.
          break;
      }

  }
  // 부모 프로세스는 자식 프로세스들이 연산과 보고를 마칠 때 까지 기다린다.
  wait(NULL);
  return 0;
}