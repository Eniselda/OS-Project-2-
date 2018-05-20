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
	
