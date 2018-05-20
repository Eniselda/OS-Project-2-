#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>
#include "commondefs.h"
#include <semaphore.h>

struct shared_data *sh_d;

//create a struct that will be passed to the thread
    char filename [128];
         //crating sem arrays
     sem_t* sems_mutex[10];
     sem_t* sems_full[10];
     sem_t* sems_empty[10];
     
    char prefix[128];
	char mutexx[256];
	char emptyy[256];
	char fulll[256];    
    
//thread function
static void *search (void *args){

    //semaphores for RESULT
	 sem_t* sems_mutex[10];
     sem_t* sems_full[10];
     sem_t* sems_empty[10];
     
     
	struct request *p;
	p=(struct request *)malloc(sizeof(struct request));
	p = (struct request * ) args;
	
	int in = p->index;		
	
	   //open the semaphores
     
	  snprintf(mutexx, sizeof(mutexx), "%s%s%d", prefix , "mutex -result",in);       
	  snprintf(fulll, sizeof(fulll), "%s%s%d", prefix , "full -result",in);        
	  snprintf(emptyy, sizeof(emptyy), "%s%s%d", prefix , "empty -result",in); 

	 sems_mutex[in] = sem_open(mutexx, O_RDWR);
	 if (sems_mutex[in] < 0) {
		perror("can not open semaphore\n");
		exit (1);
		}

	 sems_full[in] = sem_open(fulll, O_RDWR);
	 if (sems_full[in] < 0) {
		perror("can not open semaphore\n");
		exit (1);
	}
	
	 sems_empty[in] = sem_open(emptyy, O_RDWR);
	 if (sems_empty[in] < 0) {
		perror("can not open semaphore\n");
		exit (1);
	}

 
	char test[128];
	strcpy(test, p->keyword);
	//Starting to search 

	FILE *fp;
	int line_num = 1;
	int find_result = 0;
	char temp[1024];
	int lines[20000];
	int k =0;
	if((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "could not open file");
	 	exit(1);
	}

	while(fgets(temp, 1024, fp) != NULL) {
		
		if((strstr(temp, test)) != NULL) {
				
		        lines[k] = line_num; 
				k++;
				find_result++;
		}
		line_num++;
	}
	
	
	//Close the file if still open.
	if(fp) {
		fclose(fp);}
		
		 for (int j =0; j<find_result; j++){
			if (SYNCHRONIZED) {
				
                
                    sem_wait(sems_empty[in]);
				    sem_wait(sems_mutex[in]);
				    
					sh_d->r[in].arr[sh_d->r[in].in_r] = lines[j];
					sh_d->r[in].in_r = (sh_d->r[in].in_r + 1) % BUFFER_SIZE;
					
					sem_post(sems_mutex[in]);
					sem_post(sems_full[in]);
				    
					
                    
                      }} 
                    
              /*  else {
					while ( ((sh_d->r[in].in_r + 1) % BUFFER_SIZE)  == sh_d->r[in].out_r)
							;

							sh_d->r[in].arr[j] = lines[j];
							sh_d->r[in].in_r = (sh_d->r[in].in_r + 1) % BUFFER_SIZE;
							
						    //sh_d->r[in].arr[find_result] = -1; 
						    }  */
	
		
        //inserting -1 in the end 
		sem_wait(sems_empty[in]);
		sem_wait(sems_mutex[in]);
			sh_d->r[in].arr[sh_d->r[in].in_r] = -1;
			sh_d->r[in].in_r = (sh_d->r[in].in_r + 1) % BUFFER_SIZE;
		sem_post(sems_mutex[in]);
		sem_post(sems_full[in]);
		
		sem_close(sems_mutex[in]);
		sem_close(sems_empty[in]);
		sem_close(sems_full[in]);
	
		pthread_exit(NULL);	
}
	

//           MAIN             

int main(int argc, char **argv){

    strcpy(filename,argv[2]);

     //Sempahores for request Queueu 
     sem_t* sems_mutexR;
     sem_t* sems_fullR;
     sem_t* sems_emptyR;
     
    //semaphores for the request state queue
	 sem_t* sems_mutexS;

	int fd;
	char sharedmem_name[128];
	void *shm_start;

    strcpy(prefix, argv[3]);
    
	struct stat sbuf;

	strcpy(sharedmem_name, argv[1]);
	shm_unlink (sharedmem_name);
	
	/* create a shared memory segment */
	fd = shm_open(sharedmem_name, O_RDWR | O_CREAT, 0660);

	if (fd < 0) {

		perror("can not create shared memory\n");
		exit (1);
	}

	/* set the size of the shared memory */

	ftruncate(fd, SHAREDMEM_SIZE);
	fstat(fd, &sbuf); /* get info about shared memmory */

	
	// map the shared memory into the address space of the process

	shm_start = mmap(NULL, sbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (shm_start < 0) {

		perror("can not map the shared memory \n");

		exit (1);

	} 
	
	//printf ("mapping ok, start address = %lu\n",(unsigned long) shm_start);
	close(fd);

	sh_d = (struct shared_data*) shm_start;
	

	//INITIALIZING THE DATA IN SHARED MEMORY 
	//request_q
	for(int i=0;i<10;i++){
	
	strcpy(sh_d->req_arr[i].keyword,"");
	sh_d->req_arr[i].index=0; 
	
	}
	
	sh_d->in_q=0;
	sh_d->out_q=0;
	

   //initializing queue state 
	for(int i=0;i<10;i++){
	
		sh_d->queue_state[i]=0; 
	}
	
	sh_d->in_s=0;
	sh_d->out_s=0;
	
	
	//initializing result queues 
	
	for(int i=0;i<10;i++){
	
	for(int j=0;j<100;j++){
	
	sh_d->r[i].arr[j]= 0;}
	
	sh_d->r[i].in_r=0;
	sh_d->r[i].out_r=0;
	
	}
	
	
	//CREATE SEMPAHORES FOR RESULT QUEUES  
	
	
	//for mutex
	for (int i =0; i<10; i++){
	    snprintf(mutexx, sizeof(mutexx), "%s%s%d", prefix , "mutex -result",i);
	    sem_unlink (mutexx);
		sems_mutex[i] = sem_open(mutexx, O_CREAT | O_RDWR, 0660, 1);
		if (sems_mutex[i] < 0) {
			perror("can not create semaphore\n");
			//exit (1);
		}
	
	}
	
	//for full
	for (int i =0; i<10; i++){
	snprintf(fulll, sizeof(fulll), "%s%s%d", prefix , "full -result",i);
	sem_unlink (fulll);
	sems_full[i] = sem_open(fulll, O_CREAT | O_RDWR, 0660, 0);
	if (sems_full[i] < 0) {
		perror("can not create semaphore\n");
		//exit (1);
	}
	
	}
	
	//for empty
	for (int i =0; i<10; i++){
	    snprintf(emptyy, sizeof(emptyy), "%s%s%d", prefix , "empty -result",i);
	    sem_unlink (emptyy);
		sems_empty[i] = sem_open(emptyy, O_CREAT | O_RDWR, 0660, BUFFER_SIZE);
		if (sems_empty[i] < 0) {
			perror("can not create semaphore\n");
			//exit (1);
		}
	
	}
	
	
	
	/* first clean up semaphores with same names for Request */
	
	snprintf(mutexx, sizeof(mutexx), "%s%s", prefix , "mutex -request");
	snprintf(fulll, sizeof(fulll), "%s%s", prefix , "full -request");
	snprintf(emptyy, sizeof(emptyy), "%s%s", prefix , "empty -request");
	
	sem_unlink (mutexx);
	sem_unlink (fulll);
	sem_unlink (emptyy);
	
	
	
	sems_mutexR = sem_open(mutexx, O_CREAT | O_RDWR, 0660, 1);
	if (sems_mutexR < 0) {
		perror("can not create semaphore\n");
		exit (1);
	}
	
	
	
	
	sems_fullR = sem_open(fulll, O_CREAT | O_RDWR, 0660, 0);
	if (sems_fullR < 0) {
		perror("can not create semaphore\n");
		exit (1);
	}
	
	
	
	sems_emptyR = sem_open(emptyy, O_CREAT | O_RDWR, 0660, 10);
	if (sems_emptyR < 0) {
		perror("can not create semaphore\n");
		exit (1);
	}
	
	
	//SEMAPHORES FOR QUEUE STATE
	
	snprintf(mutexx, sizeof(mutexx), "%s%s", prefix , "mutex -request STATE");
    	sem_unlink (mutexx);
	
	sems_mutexS = sem_open(mutexx, O_CREAT | O_RDWR, 0660, 1);
	if (sems_mutexS < 0) {
		perror("can not create semaphore\n");
		exit (1);
	}

	
	sem_close(sems_mutexS);
		
		
	//WAITS for REQUEST queue

	  
	  while(1){
	  
	  pthread_t tid;
	  struct request *p;
	  p= malloc(sizeof(struct request));
	    if (SYNCHRONIZED) {
				sem_wait(sems_fullR);
				sem_wait(sems_mutexR);
	            
	            
	            strcpy(p->keyword,sh_d->req_arr[sh_d->out_q].keyword);
	            p->index = sh_d->req_arr[sh_d->out_q].index;
	            sh_d->out_q = (sh_d->out_q + 1) % 10;
	            
	  
	            sem_post(sems_mutexR);
				sem_post(sems_emptyR);
				
				pthread_create(&tid, NULL, search, (void*) p);
			//	pthread_join(tid,NULL);
				
			} 
			
	/*	else{
			   while (sh_d->in_q == sh_d->out_q)
			   ;
			   
			   parameters[i].index  = sh_d->req_arr[sh_d->out_q].index;
			   strcpy(parameters[i].keyword ,sh_d->req_arr[sh_d->out_q].keyword);

			   
			   thr=pthread_create(&(tid), NULL, &search, (void*) &parameters[i]);
			   sh_d->out_q = (sh_d->out_q + 1) % BUFFER_SIZE;
			   } */ 
		   }
	
		sem_close(sems_mutexR);
		sem_close(sems_emptyR);
		sem_close(sems_fullR);
	
	

	return 0;
}

