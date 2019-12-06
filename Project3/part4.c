#define _GNU_SOURCE
//
//Project 3 (part4) - Sean Wilson - CIS415 @ UofO F'19
//

//------------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <sys/time.h>
#include <time.h>

//------------------------------------------------------------------------------

#define MAXENTRIES 10 //compare to BUFFER_SIZE
#define MAXTOPICS 10 //compare to MAXQUEUES
#define MAXNAME 100 //max name of topic queue

#define URLSIZE 100 //max URL size for an entry
#define CAPSIZE 100 //max caption size for an entry

double DELTA = 2; //time (in seconds) until a topic is removed from queue

#define NUMPROXIES 5 //Amount of subs/pubs allowed at any given time

//------------------------------------------------------------------------------

struct topicEntry {
  int entryNum;
  struct timeval timeStamp;
  int pubID;
  char photoURL[URLSIZE];
  char photoCaption[CAPSIZE];
};
typedef struct topicEntry topicEntry;

//------------------------------------------------------------------------------

struct topicQ {
  int topicID;
  char *name[MAXNAME];
  topicEntry * buffer;
  int head;
  int tail;
  int length;
};
typedef struct topicQ topicQ;

//------------------------------------------------------------------------------

//struct to be sent to pub/sub threads
struct thread_args {
    char *file_name;
    int thread_ID;
};
typedef struct thread_args thread_args;

//------------------------------------------------------------------------------

//Global registry for topic Q's
topicQ *registry[MAXTOPICS];

//------------------------------------------------------------------------------

//Global array of locks for respective Q's
pthread_mutex_t lock[MAXTOPICS] = {};

//------------------------------------------------------------------------------

//global variable for entry number (to-be incremented on each enqueue() op)
int entry_number = 1;

//========================================

//**************************************************
//TODO - MOVED ALL THESE OUTSIDE MAIN TO MAKE GLOBAL
//**************************************************

//initialize thread pools
pthread_t sub_pool[NUMPROXIES] = {};
pthread_t pub_pool[NUMPROXIES] = {};

pthread_t cleanup_thread;

//array of filenames
char sub_file_names[NUMPROXIES][MAXNAME];
char pub_file_names[NUMPROXIES][MAXNAME];

//arrays to-be used to decide if thread is taken
int sub_avail[NUMPROXIES] = {0};
int pub_avail[NUMPROXIES] = {0};

//structs of arguments to-be sent to pthread_create
thread_args sub_thread_args[NUMPROXIES] = {};
thread_args pub_thread_args[NUMPROXIES] = {};

//========================================


//------------------------------------------------------------------------------
//Functions
//------------------------------------------------------------------------------


//Print contents of a queue given name
void printQ(char *QID){
  for (size_t i = 0; i < MAXTOPICS; i++) {
    if(registry[i] != NULL){
      if (strcmp(*registry[i]->name, QID) == 0){
        printf("\n---Entries of Q[%s]---\n[", *registry[i]->name);
        for (size_t j = 0; j <= registry[i]->length; j++) {
          printf("%d, ", registry[i]->buffer[j].entryNum);
        }
        printf("]\n\n");
      }//end of if name = QID
    }//end of if registry[i] != NULL
  }//end of for-loop
}//end of printQ()

//------------------------------------------------------------------------------

int enqueue(char *QID, topicEntry *TE){
  struct timeval time;
  //for topic in registry
  for (size_t i = 0; i < MAXTOPICS; i++) {
    //if topic in registry not NULL
    if(registry[i] != NULL){
      //if name of topic is target
      if (strcmp(*registry[i]->name, QID) == 0){
        //if tail is NOT located @ null
        if (registry[i]->buffer[registry[i]->tail].entryNum != -1){
          //place TE at tail location
          registry[i]->buffer[registry[i]->tail] = *TE;
          //Set TE entryNum
          registry[i]->buffer[registry[i]->tail].entryNum = entry_number;
          //increment entry_number for next enqueue() op
          entry_number++;
          //Set timeStamp
          //struct timeval time;
          gettimeofday(&time, NULL);
          registry[i]->buffer[registry[i]->tail].timeStamp = time;
          //increment tail
          int new_tail = (registry[i]->tail + 1) % (registry[i]->length+1);
          registry[i]->tail = new_tail;
          return 1;
        }
        else{
          printf(">\t\tenqueue(): *BUFFER FULL*\n");
          return 0;
        }
      }//end of if registry is correct
    }//end of if registry[i] != NULL
  }//end for loop
  printf(">\t\tenqueue(): Invalid Queue name!\n");
  return 0;
}//end of enqueue()

//------------------------------------------------------------------------------

int getEntry(char *QID, int lastEntry, topicEntry *TE){

  for (size_t i = 0; i < MAXTOPICS; i++) {
    if(registry[i] != NULL){
      if (strcmp(*registry[i]->name, QID) == 0){
        //Case1: topic queue is empty: return 0
        if (registry[i]->tail == registry[i]->head){
          printf(">\t\tgetEntry(): <queue is empty>\n");
          return 0;
        }
        //Case2: lastEntry+1 is in Queue: copy lastEntry+1 data to TE, return 1)
        for (size_t j = 0; j < registry[i]->length+1; j++) {
          if (registry[i]->buffer[j].entryNum == lastEntry+1){
            //copy data over
            *TE = registry[i]->buffer[j];
            printf(">\t\tgetEntry(): <found lastEntry+1> time[%d] eNum[%d]\n", TE->timeStamp.tv_sec, TE->entryNum);
            return 1;
          }
        }
        //Case3: Topic queue is NOT empty & lastEntry+1 is NOT in queues
        for (size_t j = 0; j < registry[i]->length+1; j++) {
          //ii: if there exists entry.entryNum > lastEntry+1:
          if (registry[i]->buffer[j].entryNum > lastEntry+1){
            //copy entry data to TE, return entry.entryNum
            *TE = registry[i]->buffer[j];
            printf(">\t\tgetEntry(): <found something bigger> time[%d] eNum[%d]\n", TE->timeStamp.tv_sec, TE->entryNum);
            return registry[i]->buffer[j].entryNum;
          }
        }
        //i: if all entries < lastEntry+1: return 0
        printf(">\t\tgetEntry(): <all enties less than lastEntry+1>\n");
        return 0;
      }
    }//end of if registry[i] != NULL
  }
  printf(">\t\tgetEntry(): Invalid Queue name!\n");
  return 0;
}//end of getEntry()

//------------------------------------------------------------------------------

int dequeue(char *QID){

  struct timeval new;
  struct timeval old;
  double diff;

  for (size_t i = 0; i < MAXTOPICS; i++) {
    if(registry[i] != NULL){
      if (strcmp(*registry[i]->name, QID) == 0){
        //If tail != head
        if (registry[i]->tail != registry[i]->head){

          //Check if oldest entry has passed the time DELTA
          //get current time
          gettimeofday(&new, NULL);
          //compute time difference
          old = registry[i]->buffer[registry[i]->head].timeStamp;
          diff = (new.tv_sec - old.tv_sec) * 1e6;
          diff = (diff + (new.tv_usec - old.tv_usec)) * 1e-6;
          //printf("DIFFERENCE: {%f}\n", diff);
          if(diff < DELTA) {
            printf(">\t\tdequeue(): Operation rejected due to DELTA\n");
            return 0;
          }
          //if tail null:
          if (registry[i]->buffer[registry[i]->tail].entryNum == -1){
            //set head entryNum to -1 (null)
            registry[i]->buffer[registry[i]->head].entryNum = -1;

            int head_minus1 = (registry[i]->head - 1) % (registry[i]->length+1);

            if (head_minus1 == -1) { head_minus1 = registry[i]->length; }
            //set head-1 entryNum to 0 (empty)
            registry[i]->buffer[head_minus1].entryNum = 0;
            //Increment head

            int new_head = (registry[i]->head + 1) % (registry[i]->length+1);

            registry[i]->head = new_head;
          }
          else {

            int head_minus2 = (registry[i]->head - 1) % (registry[i]->length+1);

            if (head_minus2 == -1) { head_minus2 = registry[i]->length; }

            //set head-1 entryNum to 0 (empty)
            registry[i]->buffer[head_minus2].entryNum = 0;
            //set head entryNum to -1 (null)
            registry[i]->buffer[registry[i]->head].entryNum = -1;
            //increment head
            int new_head = (registry[i]->head + 1) % (registry[i]->length+1);

            registry[i]->head = new_head;
          }
          return 1;
        }
        else {
          printf(">\t\tdequeue(): *BUFFER EMPTY*\n");
          return 0;
        }
      }//end of if registry is correct
    }//end of if registry[i] != NULL
  }//end for loop
  printf(">\t\tdequeue(): Invalid Queue name!\n");
  return 0;
}//end of dequeue()


//------------------------------------------------------------------------------
//Thread functions
//------------------------------------------------------------------------------


void *cleanup(void *arg){
  //Spin forever! (until thread is cancelled elsewhere)
  while(1){
    //for topic in topicQ
    for (size_t i = 0; i < MAXTOPICS; i++) {
      if(registry[i] != NULL){
        //lock it down with this topics lock
        printf("*\tcleanup(): Locking up queue[%s]\n", *registry[i]->name);
        pthread_mutex_lock(&lock[i]);
        //While dequeue keeps finding entries past DELTA to dequeue, keep going!
        while(dequeue(*registry[i]->name)){}
        //unlock it with this topics respective lock
        printf("*\tcleanup(): Unlocking queue[%s]\n", *registry[i]->name);
        pthread_mutex_unlock(&lock[i]);
      }//end of if registry==NULL
    }//end of for(topics)
    //sleep as to make print statements more readable
    sleep(1);
  }//end of main infinite loop
  return NULL;
}//end of cleanup()

//------------------------------------------------------------------------------

void *publisher(void *inp){ //enqueue()

  printf("*******PUBLISHER**********\n");

  thread_args *thread_args = inp;

  //FILE *input = NULL;
  char *buffy = NULL;
  size_t bufferSize = 2048;
	size_t file_size = 0;
  char *token = NULL;

  // printf("ID: %d\n", args->thread_ID);
  printf("FILE: %s\n", thread_args->file_name);

  int i, len = strlen(thread_args->file_name);
  if(thread_args->file_name[0] == '\"'){
  	for(i=1; i<len-1; i++){
  	   thread_args->file_name[i-1] = thread_args->file_name[i];
  	}
  	thread_args->file_name[i-1] = '\0';
  }

	printf("NEW FILE: %s\n", thread_args->file_name);

  //input = fopen(thread_args->file_name, "r");
  FILE *input = fopen(thread_args->file_name, "r");

  if (input == NULL){
    printf("PUB-FILE NOT OPENED!\n");
    free(buffy);
    return NULL;
  }

  buffy = (char *)malloc(bufferSize * sizeof(char));
  if(buffy == NULL){printf("Error! Unable to allocate input buffer. \n");exit(1);}

  char* rest = buffy;

  printf("*******PUB4**********\n");

  while((file_size = getline(&buffy, &bufferSize, input) ) != -1){
    int spaces = 0;
    int tokens = 0;
    for (int i = 0; i < file_size; i++) {if (buffy[i] == ' '){spaces += 1;}}
    tokens = spaces + 1;
    char *args[tokens+1];
    args[tokens] = NULL;
    int index = 0;

    //**************************************************
    //TODO use strtok_r
    //**************************************************

    printf("BEFORE PUB TOKEN: [%s]\n", token);
    //Testing strtok_r
    // char* rest = buffy;
    token = strtok_r(buffy, " ", &rest);
    printf("AFTER PUB TOKEN: [%s]\n", token);
    //token = strtok(buffy, " ");
    //Testing strtok_r


    while(token != NULL) {
      int length = strlen(token);
      if (length > 0 && token[length - 1] == '\n'){ token[length-1] = '\0';}
      args[index] = token;
      index += 1;

      //**************************************************
      //TODO use strtok_r
      //**************************************************

      //Testing strtok_r
      //char* rest2 = NULL;
      token = strtok_r(NULL, " ", &rest);
      printf("PUB TOKEN: [%s]\n", token);

      //token = strtok(NULL, " ");
      //Testing strtok_r

    }//end of while()

    //if there are arguments to be parsed
    if(args[0] != NULL){

      //get id
      if (strcmp(args[0], "put")==0){
        if (args[1] != NULL){
          if (args[2] != NULL){
            if (args[3] != NULL){

              //----------------------------------

              printf("Proxy thread <%d> - type: <Publisher> - Executed command: <Put>\n", thread_args->thread_ID);

              topicEntry to_be_enq;

              to_be_enq.pubID = thread_args->thread_ID;

              strcpy(to_be_enq.photoURL, args[2]);
              //to_be_enq.photoURL = args[2];

              //**************************************************
              //TODO make caption able to take caption with spaces
              //**************************************************
              strcpy(to_be_enq.photoCaption, args[3]);
              //to_be_enq.photoCaption = args[3];


              for (size_t i = 0; i < MAXTOPICS; i++) {
                if(registry[i] != NULL){
                  if(registry[i]->topicID == atoi(args[1])){

                    //lock it down with this topics lock
                    printf("*\tpublisher(): Locking up queue[%s]\n", *registry[i]->name);
                    pthread_mutex_lock(&lock[i]);

                    int result = enqueue(*registry[i]->name, &to_be_enq);
                    printQ(*registry[i]->name);

                    //unlock it with this topics lock
                    printf("*\tpublisher(): Unlocking queue[%s]\n", *registry[i]->name);
                    pthread_mutex_unlock(&lock[i]);

                    //While enqueue returns 0 (either from full queue or wrong Q name)
                    if(result == 0){

                      printf("*\tpublisher(): enqueue on [%s] failed. Full buffer?\n", *registry[i]->name);

                      //Yield CPU and put thread into ready queue
                      sched_yield();
                    }//end of while enqueue() returns 0
                    else{printf("*\tpublisher(): enqueue on [%s] succeeded\n", *registry[i]->name);}

                  }//if()
                }//if()
              }//end of for()
              //----------------------------------

            }
          }
        }
      }
      //sleep milli
      else if (strcmp(args[0], "sleep")==0){
        if (args[1] != NULL){

          //----------------------------------
          int milli = atoi(args[1]); // length of time to sleep, in miliseconds
          struct timespec tim = {0};
          tim.tv_sec = 0;
          tim.tv_nsec = milli * 1000000L;
          nanosleep(&tim, (struct timespec *)NULL);

          printf("Proxy thread <%d> - type: <Publisher> - Executed command: <Sleep>\n", thread_args->thread_ID);
          //----------------------------------

        }
      }
      //stop
      else if (strcmp(args[0], "stop")==0){

        //----------------------------------
        printf("Proxy thread <%d> - type: <Publisher> - Executed command: <Stop>\n", thread_args->thread_ID);
        break;

        free(buffy);
        fclose(input);

        return NULL;

        //----------------------------------

      }
      else{printf("[pub] Invalid Command <%s>\n", args[0]);}
    }//end of if(args[0] != Null)
  }//end of main while()

  printf("PUB ERREO?\n");

  free(buffy);
  fclose(input);
  return NULL;

}//end of publisher()

//------------------------------------------------------------------------------

void *subscriber(void *inp){ //getEntry()

  printf("*******SUBSCRIBER**********\n");

  thread_args *thread_args = inp;

  //FILE *input = NULL;
  char *buffy = NULL;
  size_t bufferSize = 2048;
	size_t file_size = 0;
  char *token = NULL;

  // printf("ID: %d\n", args->thread_ID);
  printf("FILE: %s\n", thread_args->file_name);

	int i, len = strlen(thread_args->file_name);

  if(thread_args->file_name[0] == '\"'){
  	for(i = 1; i<len-1; i++){
  	   thread_args->file_name[i-1] = thread_args->file_name[i];
  	}
  	thread_args->file_name[i-1]='\0';
  }

	printf("NEW FILE: %s\n", thread_args->file_name);

	FILE *input = fopen(thread_args->file_name, "r");

  if (input == NULL){
    printf("SUB-FILE NOT OPENED!\n");
    free(buffy);
    return NULL;
  }


  buffy = (char *)malloc(bufferSize * sizeof(char));
  if(buffy == NULL){printf("Error! Unable to allocate input buffer. \n");exit(1);}

  char* rest = buffy;

  while((file_size = getline(&buffy, &bufferSize, input) ) != -1){

    int spaces = 0;
    int tokens = 0;
    for (int i = 0; i < file_size; i++) {if (buffy[i] == ' '){spaces += 1;}}
    tokens = spaces + 1;
    char *args[tokens+1];
    args[tokens] = NULL;
    int index = 0;

    //**************************************************
    //TODO make caption able to take caption with spaces
    //**************************************************

    printf("BEFORE SUB TOKEN: [%s]\n", token);
    //Testing strtok_r
    //char* rest = buffy;
    token = strtok_r(buffy, " ", &rest);
    //token = strtok(buffy, " ");

    printf("AFTER SUB TOKEN: [%s]\n", token);

    //Testing strtok_r

    while(token != NULL) {
      int length = strlen(token);
      if (length > 0 && token[length - 1] == '\n'){ token[length-1] = '\0';}
      args[index] = token;
      index += 1;

      //**************************************************
      //TODO make caption able to take caption with spaces
      //**************************************************
      //Testing strtok_r
      token = strtok_r(NULL, " ", &rest);
      //token = strtok(NULL, " ");

      printf("SUB TOKEN: [%s]\n", token);


      //Testing strtok_r

    }//end of while()

    //if there are arguments to be parsed
    if(args[0] != NULL){

      //get id
      if (strcmp(args[0], "get")==0){
        if (args[1] != NULL){
          //----------------------------------

          printf("Proxy thread <%d> - type: <Subscriber> - Executed command: <Get>\n", thread_args->thread_ID);

          //empty struct to-be filled by getEntry()
          topicEntry place_hold;
          place_hold.entryNum = -999;

          //last entry initially set to 1
          int last_entry = 1;

          for (size_t i = 0; i < MAXTOPICS; i++) {
            if(registry[i] != NULL){
              if(registry[i]->topicID == atoi(args[1])){

                //try to getEntry

                //lock it down with this topics lock
                printf("*\tsubscriber(): Locking up queue[%s]\n", *registry[i]->name);
                pthread_mutex_lock(&lock[i]);

                int result = getEntry(*registry[i]->name, last_entry, &place_hold);

                //unlock it with this topics lock
                printf("*\tsubscriber(): Unlocking queue[%s]\n", *registry[i]->name);
                pthread_mutex_unlock(&lock[i]);

                //if getEntry() returns 0 (all entries < lastEntry+1 <or> Q is empty)
                if(result == 0){
                  printf("*\tsubscriber(): getEntry on [%s] failed\n", *registry[i]->name);
                  //yield CPU and put back on ready Q
                  sched_yield();
                }
                //if getEntry() returns 1 (found lastEntry+1)
                else if(result == 1){
                  printf("*\tsubscriber(): getEntry on [%s] found entry:[%d]\n", *registry[i]->name, place_hold.entryNum);
                  last_entry++;
                }
                else{
                  printf("*\tsubscriber(): getEntry on [%s] found entry:[%d]", *registry[i]->name, place_hold.entryNum);
                  printf(" ... lastEntry is now:[%d]\n", result);
                  last_entry = result;
                }
              }
            }
          }
          //----------------------------------

        }
      }
      //sleep milli
      else if (strcmp(args[0], "sleep")==0){
        if (args[1] != NULL){

          //----------------------------------
          int milli = atoi(args[1]); // length of time to sleep, in miliseconds
          struct timespec tim = {0};
          tim.tv_sec = 0;
          tim.tv_nsec = milli * 1000000L;
          nanosleep(&tim, (struct timespec *)NULL);

          printf("Proxy thread <%d> - type: <Subscriber> - Executed command: <Sleep>\n", thread_args->thread_ID);
          //----------------------------------

        }
      }
      //stop
      else if (strcmp(args[0], "stop")==0){

        //----------------------------------
        printf("Proxy thread <%d> - type: <Subscriber> - Executed command: <Stop>\n", thread_args->thread_ID);


        free(buffy);
        fclose(input);
        return NULL;

        //----------------------------------

      }
      else{printf("[sub] Invalid Command <%s>\n", args[0]);}

    }//end of if args[0]!=NULL

  }//end of main while()

  printf("SUB ERREO?\n");

  free(buffy);
  fclose(input);
  return NULL;

}//end of subscriber()

//------------------------------------------------------------------------------


int main(int argc, char const *argv[]) {

  FILE *input = NULL;
  char *buffy = NULL;
  size_t bufferSize = 2048;
	size_t file_size;
  char *token;

  //check for command mode
  if (argc == 1){
    input = stdin;
    printf("Command Mode\n");
    printf(">>> ");
  }
  //check for file mode
  if (argc == 2){
    printf("File Mode\n");
		input = fopen(argv[1], "r");
	}
  buffy = (char *)malloc(bufferSize * sizeof(char));
  if(buffy == NULL){printf("Error! Unable to allocate input buffer. \n");exit(1);}


  //========================================

  //array of topics initialized to NULL
  topicQ queues[MAXTOPICS] = {};

  //2-d array of buffers to-be assigned to queues on create
  topicEntry buffer_store[MAXTOPICS][MAXENTRIES+1] = {};

  //array of queue names
  char topic_names[MAXTOPICS][MAXNAME];


  topicEntry null;
  null.entryNum = -1;
  topicEntry def;
  def.entryNum = 0;

  //set rest of values to default
  for (size_t j = 0; j < MAXTOPICS; j++) {
    for (size_t i = 0; i < MAXENTRIES; i++) {
      buffer_store[j][i] = def;
    }
  }

  //queue locater
  int queue_loc = 0;

  while((file_size = getline(&buffy, &bufferSize, input) ) != -1){
    int spaces = 0;
    int tokens = 0;
    int arguments = 0;
    for (int i = 0; i < file_size; i++) {if (buffy[i] == ' '){spaces += 1;}}
    tokens = spaces + 1;
    arguments = tokens - 1;
    char *args[tokens+1];
    args[tokens] = NULL;
    int index = 0;
    int exit = 0;
    token = strtok(buffy, " ");
    while(token != NULL) {
      int length = strlen(token);
      if (length > 0 && token[length - 1] == '\n'){ token[length-1] = '\0';}
      args[index] = token;
      if (strcmp(token, "exit")==0){exit = 1;}
      index += 1;
      token = strtok(NULL, " ");
    }//end of while()

    if(exit == 1){
      break;
    }

    //if there are arguments to be parsed
    if(args[0] != NULL){

      if (strcmp(args[0], "create")==0){
        if (args[1] != NULL){
          if (args[2] != NULL){
            if (args[3] != NULL){
              if (args[4] != NULL){

                //========================================
                if(queue_loc >= MAXTOPICS){
                  printf("MAX NUMBER OF QUEUES REACHED\n");
                }
                else{
                  //place initilaized topicQ into registry
                  registry[queue_loc] = &queues[queue_loc];
                  //set ID
                  registry[queue_loc]->topicID = atoi(args[2]);
                  //set name
                  strcpy(topic_names[queue_loc], args[3]);
                  *registry[queue_loc]->name = topic_names[queue_loc];
                  //set length
                  int len = MAXENTRIES;
                  if (atoi(args[4]) > len){
                    registry[queue_loc]->length = len;
                  }
                  else{
                    registry[queue_loc]->length = atoi(args[4]);
                  }
                  //set last entry of topicQ buffer to NULL
                  buffer_store[queue_loc][registry[queue_loc]->length] = null;
                  //set buffer of regustry to repective initialized buffer from buffer store
                  registry[queue_loc]->buffer = buffer_store[queue_loc];

                  printf("\tSuccesfully created topic[%s]\n", args[3]);

                  //increment topic locater
                  queue_loc++;

                }
                //========================================

              }else{printf("MISSING VALUE!\n");}
            }else{printf("MISSING VALUE!\n");}
          }else{printf("MISSING VALUE!\n");}
        }else{printf("MISSING VALUE!\n");}
      }
      //-----------------------------------
      else if (strcmp(args[0], "delta")==0){
        if (args[1] != NULL){

          //========================================
          double val = atof(args[1]);
          DELTA = val;
          printf("\tDELTA is now %f\n", DELTA);
          //========================================

        }
        else{printf("MISSING VALUE!\n");}
      }
      //-----------------------------------
      else if (strcmp(args[0], "add")==0){
        if (args[1] != NULL){
          if (strcmp(args[1], "publisher")==0){
            if (args[2] != NULL){

              //========================================
              int check_availp = 0;
              for (size_t i = 0; i < NUMPROXIES; i++) {
                if(pub_avail[i] == 0){

                  printf("\tFound available publisher thread[%d]\n", i);
                  pub_avail[i] = 1;
                  strcpy(pub_file_names[i], args[2]);
                  break;

                }
                else{
                  check_availp++;
                }
              }
              if(check_availp==NUMPROXIES){
                printf("\tNo more available publisher threads\n");
              }
              //========================================

            }
            else {printf("MISSING VALUE!\n");}
          }
          else if (strcmp(args[1], "subscriber")==0){
            if (args[2] != NULL){

              //========================================
              int check_avails = 0;
              for (size_t i = 0; i < NUMPROXIES; i++) {
                if(sub_avail[i] == 0){

                  printf("\tFound available subsriber thread[%d]\n", i);
                  sub_avail[i] = 1;
                  strcpy(sub_file_names[i], args[2]);
                  break;

                }
                else{
                  check_avails++;
                }
              }
              if(check_avails==NUMPROXIES){
                printf("\tNo more available subscriber threads\n");
              }
              //========================================

            }
            else {printf("MISSING VALUE!\n");}
          }
          else{printf("UNKNOWN VALUE AFTER <add>\n");}
        }
        else{printf("MISSING VALUE!\n");}
      }
      //-----------------------------------
      else if (strcmp(args[0], "query")==0){

        if(args[1] != NULL){
          if (strcmp(args[1], "topics")==0){

            //========================================
            int checkT = 0;
            for (size_t i = 0; i < MAXTOPICS; i++) {
              if(registry[i] != NULL){
                printf("'\tTopic[%d]: ID:%d Name:%s Length:%d\n", i, registry[i]->topicID, *registry[i]->name, registry[i]->length);
              }
              else{checkT ++;}
            }
            if(checkT == MAXTOPICS){
              printf("\tThere are no topics you fool!\n");
            }
            //========================================

          }
          else if (strcmp(args[1], "publishers")==0){


            //========================================
            int checkP = 0;
            for (size_t i = 0; i < NUMPROXIES; i++) {
              if(pub_avail[i] == 1){
                printf("\tPublisher[%d]: File:%s\n", i, pub_file_names[i]);
              }
              else{checkP++;}
            }
            if(checkP == NUMPROXIES){
              printf("\tThere are no publishers you fool!\n");
            }
            //========================================

          }
          else if (strcmp(args[1], "subscribers")==0){

            //========================================
            int checkS = 0;
            for (size_t i = 0; i < NUMPROXIES; i++) {
              if(sub_avail[i] == 1){
                printf("\tSubscriber[%d]: File:%s\n", i, sub_file_names[i]);
              }
              else{checkS++;}
            }
            if(checkS == NUMPROXIES){
              printf("\tThere are no subscribers you fool!\n");
            }
            //========================================

          }
          else{printf("UNKNOWN VALUE AFTER <query>\n");}
        }
        else{printf("MISSING VALUE!\n");}
      }
      //-----------------------------------
      else if (strcmp(args[0], "start")==0){

        //========================================
        for (size_t i = 0; i < NUMPROXIES; i++) {

          if(sub_avail[i] == 1){
            printf("\tStarting subscriber[%d]\n", i);

            sub_thread_args[i].file_name = sub_file_names[i];
            sub_thread_args[i].thread_ID = i;

            pthread_create(&sub_pool[i], NULL, subscriber, (void *)&sub_thread_args[i]);
          }

          if(pub_avail[i] == 1){
            printf("\tStarting publisher[%d]\n", i);

            pub_thread_args[i].file_name = pub_file_names[i];
            pub_thread_args[i].thread_ID = i;

            pthread_create(&pub_pool[i], NULL, publisher, (void *)&pub_thread_args[i]);
          }

        }

        //start up the cleanup thread
        pthread_create(&cleanup_thread, NULL, cleanup, NULL);

        //========================================

      }
      else{printf("[main] UNKNOWN COMMAND! <%s>\n", args[0]);}

    }
    else{printf("MISSING VALUE!\n");}

    if (input == stdin){
      printf(">>> ");
    }

  }//end of while()

  sleep(10);

  pthread_cancel(cleanup_thread);

  //cancel all active threads
  for (size_t i = 0; i < NUMPROXIES; i++) {
    if(pub_avail[i] == 1){
      pthread_join(pub_pool[i], NULL);
      //set thread to available
      pub_avail[i] = 0;
    }
  }

  for (size_t i = 0; i < NUMPROXIES; i++) {
    if(sub_avail[i] == 1){
      pthread_join(sub_pool[i], NULL);
      //set thread to available
      sub_avail[i] = 0;
    }
  }

  pthread_join(cleanup_thread, NULL);

  // Close the file
  free(buffy);
  fclose(input);

  return 0;

}//end of main()

//-----------------------------------------------------------------------------
