#define BUFFER_SIZE 100 
#define SEMNAME_MUTEX       "/name_sem_mutex"
#define SEMNAME_FULL        "/name_sem_fullcount"
#define SEMNAME_EMPTY       "/name_sem_emptycount"

#define SEMNAME_MUTEXR       "/name_sem_mutexR"
#define SEMNAME_FULLR       "/name_sem_fullcountR"
#define SEMNAME_EMPTYR       "/name_sem_emptycountR"

#define SEMNAME_MUTEXS      "/name_sem_mutexS"


#define SYNCHRONIZED 1

struct request{
 
    char keyword[128];
    int index;
};
 struct res{
     int in_r;
	 int out_r;
	 int arr[BUFFER_SIZE];
};    


struct shared_data{
  
   struct request req_arr[10];
   int in_q;
   int out_q;
   int queue_state[10];
   int in_s;
   int out_s;
   struct res r[10];

   
};
  
  
  
#define SHAREDMEM_SIZE 7000

