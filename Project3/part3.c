#define _GNU_SOURCE
//
//Project 3 (part3) - Sean Wilson - CIS415 @ UofO F'19
//

//------------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <sys/time.h>

//------------------------------------------------------------------------------

#define MAXENTRIES 10 //compare to BUFFER_SIZE
#define MAXTOPICS 100 //compare to MAXQUEUES
#define MAXNAME 100 //max name of topic queue

#define URLSIZE 100 //max URL size for an entry
#define CAPSIZE 100 //max caption size for an entry

#define DELTA 2 //time (in seconds) until a topic is removed from queue

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
  char *name[MAXNAME];
  topicEntry *const buffer;
  int head;
  int tail;
  const int length;
};
typedef struct topicQ topicQ;

//------------------------------------------------------------------------------

//struct to be sent to publisher threads
struct pub_args {
    char* queue_name;
    topicEntry *tobe_pub[MAXENTRIES];
};
typedef struct pub_args pub_args;

//------------------------------------------------------------------------------

//struct to be sent to subscriber threads
struct sub_args {
    //char tobe_sub[MAXTOPICS][MAXNAME];
    topicQ *tobe_sub[MAXTOPICS];
};
typedef struct sub_args sub_args;

//------------------------------------------------------------------------------

//Global registry for topic Q's
topicQ *registry[MAXTOPICS];

//------------------------------------------------------------------------------

//Global array of locks for respective Q's
pthread_mutex_t lock[MAXTOPICS] = {};

//------------------------------------------------------------------------------

//global variable for entry number (to-be incremented on each enqueue() op)
int entry_number = 1;

//------------------------------------------------------------------------------

//Print contents of a queue given name
void printQ(char *QID){
  for (size_t i = 0; i < MAXTOPICS; i++) {
    if(registry[i] != NULL){
      if (strcmp(*registry[i]->name, QID) == 0){
        printf("\n---Entries of Q[%s]---\n[", *registry[i]->name);
        for (size_t j = 0; j <= MAXENTRIES; j++) {
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
          int new_tail = (registry[i]->tail + 1) % (MAXENTRIES+1);
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
        for (size_t j = 0; j < MAXENTRIES+1; j++) {
          if (registry[i]->buffer[j].entryNum == lastEntry+1){
            //copy data over
            *TE = registry[i]->buffer[j];
            printf(">\t\tgetEntry(): <found lastEntry+1> time[%d] eNum[%d]\n", TE->timeStamp.tv_sec, TE->entryNum);
            return 1;
          }
        }
        //Case3: Topic queue is NOT empty & lastEntry+1 is NOT in queues
        for (size_t j = 0; j < MAXENTRIES+1; j++) {
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
            int head_minus1 = (registry[i]->head - 1) % (MAXENTRIES+1);
            if (head_minus1 == -1) { head_minus1 = MAXENTRIES; }
            //set head-1 entryNum to 0 (empty)
            registry[i]->buffer[head_minus1].entryNum = 0;
            //Increment head
            int new_head = (registry[i]->head + 1) % (MAXENTRIES+1);
            registry[i]->head = new_head;
          }
          else {
            int head_minus2 = (registry[i]->head - 1) % (MAXENTRIES+1);
            if (head_minus2 == -1) { head_minus2 = MAXENTRIES; }
            //set head-1 entryNum to 0 (empty)
            registry[i]->buffer[head_minus2].entryNum = 0;
            //set head entryNum to -1 (null)
            registry[i]->buffer[registry[i]->head].entryNum = -1;
            //increment head
            int new_head = (registry[i]->head + 1) % (MAXENTRIES+1);
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

void *publisher(void *input){ //enqueue()

  // pub_args *inp = input;
  //
  // for (size_t i = 0; i < MAXTOPICS; i++) {
  //   if(registry[i] != NULL){
  //     if (strcmp(*registry[i]->name, inp->queue_name) == 0){
  //       //USED IN WHILE LOOP TO REFERENCE ARGUMENT IN PASSED_IN ARRAY
  //       int z = 0;
  //       while( inp->tobe_pub[z] != NULL ){
  //         //lock it down with this topics lock
  //         printf("*\tpublisher(): Locking up queue[%s]\n", *registry[i]->name);
  //         pthread_mutex_lock(&lock[i]);
  //         int result = enqueue(*registry[i]->name, inp->tobe_pub[z]);
  //         printQ(*registry[i]->name);
  //         //unlock it with this topics lock
  //         printf("*\tpublisher(): Unlocking queue[%s]\n", *registry[i]->name);
  //         pthread_mutex_unlock(&lock[i]);
  //         //While enqueue returns 0 (either from full queue or wrong Q name)
  //         while(result == 0){
  //           printf("*\tpublisher(): enqueue on [%s] failed, trying again after yield. Full buffer?\n", *registry[i]->name);
  //           //lock it down with this topics lock
  //           printf("*\tpublisher(): Locking up queue[%s]\n", *registry[i]->name);
  //           pthread_mutex_lock(&lock[i]);
  //           //try to enqueue again
  //           result = enqueue(*registry[i]->name, inp->tobe_pub[z]);
  //           printQ(*registry[i]->name);
  //           //unlock it with this topics lock
  //           printf("*\tpublisher(): Unlocking queue[%s]\n", *registry[i]->name);
  //           pthread_mutex_unlock(&lock[i]);
  //           //Sleep to help make print statements print before thread yields
  //           sleep(1);
  //           //Yield CPU and put thread into ready queue
  //           sched_yield();
  //         }//end of while enqueue() returns 0
  //
  //         printf("*\tpublisher(): enqueue on [%s] succeeded\n", *registry[i]->name);
  //         //sleep as to make print statements more readable
  //         sleep(1);
  //         //DONT FORGET ABOUT ME!
  //         z++;
  //         //DONT FORGET ABOUT ME!
  //       }//end of while()
  //     }//end of if()
  //   }//end of if()
  // }//end of for()

  int thread_id = 0;
  printf("Proxy thread <%d> - type: <Publisher>​\n", thread_id);

  return NULL;


}//end of publisher()

//------------------------------------------------------------------------------

void *subscriber(void *input){ //getEntry()

  // sub_args *inp = input;
  //
  // //empty struct to-be filled by getEntry()
  // topicEntry place_hold;
  // place_hold.entryNum = -999;
  //
  // //last entry initially set to 1
  // int last_entry = 1;
  //
  // //for topic in registry
  // for (size_t i = 0; i < MAXTOPICS; i++) {
  //   if(registry[i] != NULL){
  //     //for topic in passed in struct
  //     for (size_t j = 0; j < MAXTOPICS; j++) {
  //       if(inp->tobe_sub[j] != NULL && *registry[i]->name != NULL){
  //         if (strcmp(*registry[i]->name, *inp->tobe_sub[j]->name) == 0){
  //           printf("*\tsubscriber(): HIT[%s]\n", *inp->tobe_sub[j]->name);
  //           //try to getEntry
  //           //lock it down with this topics lock
  //           printf("*\tsubscriber(): Locking up queue[%s]\n", *registry[i]->name);
  //           pthread_mutex_lock(&lock[i]);
  //           int result = getEntry(*registry[i]->name, last_entry, &place_hold);
  //           //unlock it with this topics lock
  //           printf("*\tsubscriber(): Unlocking queue[%s]\n", *registry[i]->name);
  //           pthread_mutex_unlock(&lock[i]);
  //           //if getEntry() returns 0 (all entries < lastEntry+1 <or> Q is empty)
  //           if(result == 0){
  //             printf("*\tsubscriber(): getEntry on [%s] failed\n", *registry[i]->name);
  //             //sleep so print shows up
  //             sleep(1);
  //             //yield CPU and put back on ready Q
  //             sched_yield();
  //           }
  //           //if getEntry() returns 1 (found lastEntry+1)
  //           else if(result == 1){
  //             printf("*\tsubscriber(): getEntry on [%s] found entry:[%d]\n", *registry[i]->name, place_hold.entryNum);
  //             last_entry++;
  //           }
  //           else{
  //             printf("*\tsubscriber(): getEntry on [%s] found entry:[%d]", *registry[i]->name, place_hold.entryNum);
  //             printf(" ... lastEntry is now:[%d]\n", result);
  //             last_entry = result;
  //           }
  //           sleep(1);
  //
  //         }
  //       }
  //     }
  //   }
  // }

  int thread_id = 0;
  printf("Proxy thread <%d> - type: <Subscriber>​\n", thread_id);

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
		input = fopen(argv[1], "r");
    printf("File Mode\n");
	}

  buffy = (char *)malloc(bufferSize * sizeof(char));
  if(buffy == NULL){printf("Error! Unable to allocate input buffer. \n");exit(1);}

  while((file_size = getline(&buffy, &bufferSize, input) ) != -1){

    int spaces = 0;
    int tokens = 0;
    int arguments = 0;

    //file_size = getline(&buffy, &bufferSize, input);

    for (int i = 0; i < file_size; i++) {
      if (buffy[i] == ' '){spaces += 1;}
    }

    tokens = spaces + 1;
    arguments = tokens - 1;

    char *args[tokens+1];
    args[tokens] = NULL;

    printf("Number of arguments is: %d\n", arguments);

    int index = 0;
    int exit = 0;

    token = strtok(buffy, " ");

    while(token != NULL) {

      int length = strlen(token);
      if (length > 0 && token[length - 1] == '\n') token[length-1] = '\0';

      args[index] = token;

      printf("TOKEN[%s]\n", token);
      if (strcmp(token, "exit")==0){
        exit = 1;
      }

      index += 1;

      token = strtok(NULL, " ");

    }//end of while()

    if(exit == 1){
      break;
    }

    if (input == stdin){
      printf(">>> ");
    }

  }//end of while()

  // Close the file
  free(buffy);
  fclose(input);

  return 0;

}//end of main()

//-----------------------------------------------------------------------------
