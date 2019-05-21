#include <sys/types.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
int main()
{
	sem_t *semaphore;
	printf("\n");
	int test, pid;
	key_t key, key2;
	size_t size = 1024;
	char *virtaddr;
	if ((key = ftok("./key.txt", 'R')) == -1)
	{
		perror("ftok");
		return 1;
	}
	if ((key2 = ftok("./sem.txt", 'R')) == -1)
	{
		perror("ftok");
		return 1;
	}
	int shmid, shmid2;
	if ((shmid2 = shmget(key2, sizeof(sem_t), IPC_CREAT)) == -1)
	{
		perror("shmget");
		return 1;
	}
	if ((shmid = shmget(key, size, IPC_CREAT)) == -1)
	{
		perror("shmget");
		return 1;
	}
	if ((semaphore = shmat(shmid2, NULL, 0)) == -1)
	{
		perror("shmat");
		return 1;
	}
	if ((virtaddr = shmat(shmid, NULL, 0)) == -1)
	{
		perror("shmat");
		return 1;
	}
	if (sem_init(semaphore, 1, 1) == -1)
	{
		perror("sem_init");
		return 1;
	}
	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return 1;
	}
	else if (pid == 0)
	{
		/* READ */
		fork();
		fork();
		if ((semaphore = shmat(shmid2, NULL, 0)) == -1)
		{
			perror("shmat");
			return 1;
		}
		if ((virtaddr = shmat(shmid, NULL, 0)) == -1)
		{
			perror("shmat");
			return 1;
		}
		printf("Parent started\n");
		for (; true;)
		{

			sem_wait(semaphore);
			printf("Read from memory: %s\n", virtaddr);
			usleep(100000);
			sem_post(semaphore);
			sleep(1);
		}
	}
	else
	{
		/* WRITE */
		if ((semaphore = shmat(shmid2, NULL, 0)) == -1)
		{
			perror("shmat");
			return 1;
		}
		printf("Child started\n");
		char b[30];
		if ((virtaddr = shmat(shmid, NULL, 0)) == -1)
		{
			perror("shmat");
			return 1;
		}
		for (; true;)
		{

			sem_wait(semaphore);
			printf("Do something: ");
			scanf("%s", b);
			strncpy(virtaddr, b, size);
			printf("Write in memory: %s\n", b);
			sem_post(semaphore);
			sleep(1);
		}
	}
	sem_destroy(semaphore);
	return 0;
}
