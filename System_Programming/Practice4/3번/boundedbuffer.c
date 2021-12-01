#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#define BUFFER_SIZE 100
#define THREAD_COUNT 4
int buffer[BUFFER_SIZE];
int in = -1; // 생산자 변수
int out = -1; // 소비자 변수
int num = 0; // 최종 생산 - 소비 합
sem_t buffer_slot;
sem_t items;
sem_t mutex; // 이진 세마포어를 위한 변수 생성
// Buffer 최대값이 1보다 클 경우에는 Race Condition 발생해 데이터가 덮어씌여질 수 있기 때문에 mutex 생성. (상호 배제 고려)
void *readers(void *arg); // 소비자 함수 선언부
void *writers(void *arg); // 생산자 함수 선언부
int main(void){
	sem_init(&buffer_slot, 0, BUFFER_SIZE);
	sem_init(&items, 0, 0);
	sem_init(&mutex, 0, 1);
	pthread_t threads[THREAD_COUNT];
	for (int i = 0; i < THREAD_COUNT; i++){
		if (i % 2 == 0) //소비자 쓰레드
			pthread_create(&threads[i], NULL, readers, NULL);
		else //생산자 쓰레드
			pthread_create(&threads[i], NULL, writers, NULL);
	}
	for (int i = 0; i < THREAD_COUNT; i++)
		pthread_join(threads[i], NULL); // Join으로 쓰레드를 하나로 합친다.
	printf("\n result : %d \n \n", num);
    // 생산자와 소비자가 경쟁 상태가 발생하지 않았을 경우 결과값은 0
	pthread_exit(NULL);
	sem_destroy(&buffer_slot);
	sem_destroy(&items);
	return 0;
}
void *readers(void *arg){ // 소비자 함수 구현부
	int data;
	for (int i = 0; i < BUFFER_SIZE * 10; i++)   {
		sem_wait(&items); // 대기
		sem_wait(&mutex); // 임계 영역
		out++;
		num--;
		out %= BUFFER_SIZE;
		data = buffer[out];
		sem_post(&mutex); // 임계영역 끝
		sem_post(&buffer_slot);
		printf("Readers consume Index : %d \n", data); // 소비한 idx 출력
	}
}
void *writers(void *arg){ // 생산자 함수 구현부
	int i, data;
	for (int i = 0; i < BUFFER_SIZE * 10; i++){
		printf("writers produce Index : %d \n", i); // 생산된 idx 출력
		sem_wait(&buffer_slot); // 대기
		sem_wait(&mutex); // 임계영역 시작
		num++;
		in++;
		in %= BUFFER_SIZE;
		buffer[in] = i;
		sem_post(&mutex); // 임계영역 끝
		sem_post(&items);
	}
}
