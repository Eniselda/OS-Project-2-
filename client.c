#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "commondefs.h"
#include <pthread.h>
#include <semaphore.h>

int main(int argc, char **argv){
	
    
    
    
    
    char prefix[128];
	char mutexx[256];
	char emptyy[256];
	char fulll[256];
	strcpy(prefix, argv[3]);
	
	//semaphores for the result queues
	 sem_t* sems_mutex[10];
     sem_t* sems_full[10];
	 sem_t* sems_empty[10];
	 
	 //semaphores for the request queue
	 sem_t* sems_mutexR;
     sem_t* sems_fullR;
     sem_t* sems_emptyR;
     
     //semaphores for the request state queue
	 sem_t* sems_mutexS;

 //start a new request and initialize it 
	
    struct request *qq;	
    qq=(struct request *)malloc(sizeof(struct request));
    strcpy(qq->keyword,argv[2]);
	qq->index = 5;
	int fd;

	char sharedmem_name[128];

	void *shm_start;

	struct shared_data *sh_d; /* pointer to the shared data for request */

	struct stat sbuf;

	strcpy(sharedmem_name, argv[1]);
	

	/* open a shared memory segment */

	fd = shm_open(sharedmem_name, O_RDWR, 0660);

	if (fd < 0) {

		perror("can not create shared memory\n");
		exit (1);
	} else {

		//printf("sharedmem create success, fd = %d\n", fd);

	}
	
	
	fstat(fd, &sbuf); /* get info about shared memmory */

	/* check the size */

	
	// map the shared memory into the address space of the process

	shm_start = mmap(NULL, sbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (shm_start < 0) {

		perror("can not map the shared memory \n");

		exit (1);

	} 
	
		//printf ("mapping ok, start address = %lu\n",(unsigned long) shm_start);

	close(fd);
	

	sh_d = (struct shared_data *) shm_start;

	//CREATING SEMAPHORES FOR STATE QUEUE 
	
	snprintf(mutexx, sizeof(mutexx), "%s%s", prefix , "mutex -request STATE");
		
	 sems_mutexS = sem_open(mutexx, O_RDWR);
	 if (sems_mutexS < 0) {
		perror("can not open semaphore\n");
		exit (1);
		}
	
	
	//checking queue_state for available index
	int free_index =0; 
	int freeIndex =0;
	int found = 0;
	
	sem_wait(sems_mutexS);
	while ( found == 0 && freeIndex<10){
		if(sh_d->queue_state[freeIndex] ==0){
			free_index = freeIndex;
			found = 1;
		}
		freeIndex++;
	}
	if(found==0){
	printf("There are to many clients running. Please wait!");
	exit(1);
	}
	
	
    //each client updates the queue state and takes the index 
	qq->index = free_index;
	sh_d->queue_state[free_index]=1;	
	
	sem_post(sems_mutexS);
	
	//OPEN SEMPAHORES FOR REQUEST QUEUES  
