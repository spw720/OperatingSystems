//
//Project 3 (part2) - Sean Wilson - CIS415 @ UofO F'19
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

#define MAXENTRIES 6 //compare to BUFFER_SIZE
#define MAXTOPICS 100 //compare to MAXQUEUES
#define MAXNAME 100 //max name of topic queue

#define URLSIZE 100 //max URL size for an entry
#define CAPSIZE 100 //max caption size for an entry

#define DELTA 5 //time (in seconds) until a topic is removed from queue

#define NUMPROXIES 10 //Amount of subs/pubs allowed at any given time

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
    topicEntry tobe_pub[MAXENTRIES];
};
typedef struct pub_args pub_args;

//------------------------------------------------------------------------------

//struct to be sent to subscriber threads
struct sub_args {
    char tobe_sub[MAXTOPICS][MAXNAME];
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
          printf(">\tenqueue(): *BUFFER FULL*\n");
          return 0;
        }
      }//end of if registry is correct
    }//end of if registry[i] != NULL
  }//end for loop
  printf(">\tenqueue(): Invalid Queue name!\n");
  return 0;
}//end of enqueue()

//------------------------------------------------------------------------------

int getEntry(char *QID, int lastEntry, topicEntry *TE){

  for (size_t i = 0; i < MAXTOPICS; i++) {
    if(registry[i] != NULL){
      if (strcmp(*registry[i]->name, QID) == 0){
        //Case1: topic queue is empty: return 0
        if (registry[i]->tail == registry[i]->head){
          printf(">\tgetEntry(): <queue is empty>\n");
          return 0;
        }
        //Case2: lastEntry+1 is in Queue: copy lastEntry+1 data to TE, return 1)
        for (size_t j = 0; j < MAXENTRIES+1; j++) {
          if (registry[i]->buffer[j].entryNum == lastEntry+1){
            //copy data over
            *TE = registry[i]->buffer[j];
            printf(">\tgetEntry(): <found lastEntry+1>\n");
            return 1;
          }
        }
        //Case3: Topic queue is NOT empty & lastEntry+1 is NOT in queues
        for (size_t j = 0; j < MAXENTRIES+1; j++) {
          //ii: if there exists entry.entryNum > lastEntry+1:
          if (registry[i]->buffer[j].entryNum > lastEntry+1){
            //copy entry data to TE, return entry.entryNum
            *TE = registry[i]->buffer[j];
            printf(">\tgetEntry(): <found something bigger>\n");
            return registry[i]->buffer[j].entryNum;
          }
        }
        //i: if all entries < lastEntry+1: return 0
        printf(">\tgetEntry(): <all enties less than lastEntry+1>\n");
        return 0;
      }
    }//end of if registry[i] != NULL
  }
  printf(">\tgetEntry(): Invalid Queue name!\n");
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
            printf(">\tdequeue(): Operation rejected due to DELTA\n");
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
          printf(">\tdequeue(): *BUFFER EMPTY*\n");
          return 0;
        }
      }//end of if registry is correct
    }//end of if registry[i] != NULL
  }//end for loop
  printf(">\tdequeue(): Invalid Queue name!\n");
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

  for (size_t i = 0; i < MAXTOPICS; i++) {
    if(registry[i] != NULL){
      if (strcmp(*registry[i]->name, ((struct pub_args*)input)->queue_name) == 0){

        //USED IN WHILE LOOP TO REFERENCE ARGUMENT IN PASSED_IN ARRAY
        int z = 0;

        //TODO Do I need to check that the next entry to be published is not NULL???
        //while( ((pub_args*)input)->tobe_pub[z] != NULL ){





        //while( z < MAXENTRIES){
        while(z < 5) {





          //lock it down with this topics lock
          printf("*\tpublisher(): Locking up queue[%s]\n", *registry[i]->name);
          pthread_mutex_lock(&lock[i]);

          int result = enqueue(*registry[i]->name, &((struct pub_args*)input)->tobe_pub[z]);
          printQ(*registry[i]->name);

          //unlock it with this topics lock
          printf("*\tpublisher(): Unlocking queue[%s]\n", *registry[i]->name);
          pthread_mutex_unlock(&lock[i]);
          //While enqueue returns 0 (either from full queue or wrong Q name)
          while(result == 0){
            printf("*\tpublisher(): enqueue on [%s] failed. Full buffer?\n", *registry[i]->name);
            //lock it down with this topics lock
            printf("*\tpublisher(): Locking up queue[%s]\n", *registry[i]->name);
            pthread_mutex_lock(&lock[i]);

            //try to enqueue again
            result = enqueue(*registry[i]->name, &((struct pub_args*)input)->tobe_pub[z]);
            printQ(*registry[i]->name);

            //unlock it with this topics lock
            printf("*\tpublisher(): Unlocking queue[%s]\n", *registry[i]->name);
            pthread_mutex_unlock(&lock[i]);
            //Sleep to help make print statements print before thread yields
            sleep(1);
            //Yield CPU and put thread into ready queue
            sched_yield();
          }//end of while enqueue() returns 0
          printf("*\tpublisher(): enqueue on [%s] succeeded\n", *registry[i]->name);
          //sleep as to make print statements more readable
          sleep(1);

          //DONT FORGET ABOUT ME!
          z++;
          //DONT FORGET ABOUT ME!

        }
      }
    }
  }
  return NULL;


  // while(1){
  //   for (size_t i = 0; i < MAXTOPICS; i++) {
  //     if(registry[i] != NULL){
  //       if (strcmp(*registry[i]->name, ((struct pub_args*)input)->queue_name) == 0){
  //         printf("*\tpublisher(): FOUND MATCH[%s]\n", ((struct pub_args*)input)->queue_name);
  //
  //         //((struct pub_args*)input)->queue_name
  //
  //         //lock it down with this topics lock
  //         printf("*\tpublisher(): Locking up queue[%s]\n", *registry[i]->name);
  //         pthread_mutex_lock(&lock[i]);
  //
  //         int result = enqueue(*registry[i]->name, &tst);
  //
  //         //unlock it with this topics lock
  //         printf("*\tpublisher(): Unlocking queue[%s]\n", *registry[i]->name);
  //         pthread_mutex_unlock(&lock[i]);
  //
  //         //While enqueue returns 0 (either from full queue or wrong Q name)
  //         while(result == 0){
  //           printf("*\tpublisher(): enqueue on [%s] failed. Full buffer?\n", *registry[i]->name);
  //
  //           //lock it down with this topics lock
  //           printf("*\tpublisher(): Locking up queue[%s]\n", *registry[i]->name);
  //           pthread_mutex_lock(&lock[i]);
  //
  //           //try to enqueue again
  //           result = enqueue(*registry[i]->name, &tst);
  //
  //           //unlock it with this topics lock
  //           printf("*\tpublisher(): Unlocking queue[%s]\n", *registry[i]->name);
  //           pthread_mutex_unlock(&lock[i]);
  //
  //           //Sleep to help make print statements print before thread yields
  //           sleep(1);
  //           //Yield CPU and put thread into ready queue
  //           sched_yield();
  //         }//end of while enqueue() returns 0
  //
  //         printf("*\tpublisher(): enqueue on [%s] succeeded\n", *registry[i]->name);
  //         //sleep as to make print statements more readable
  //         sleep(1);
  //       }//end of if() Q names match
  //     }//end of if() Q is null
  //   }//end of for() topic in topics
  // }//end of infinite while() loop
  // return NULL;


}//end of publisher()

//------------------------------------------------------------------------------

void *subscriber(void *input){ //getEntry()

  //empty struct to-be filled by getEntry()
  topicEntry place_hold;
  place_hold.entryNum = -999;

  //last entry initially set to 1
  int last_entry = 1;

  //for topic in registry
  for (size_t i = 0; i < MAXTOPICS; i++) {
    if(registry[i] != NULL){
      //for topic in passed in struct
      for (size_t j = 0; j < MAXTOPICS; j++) {
        //if name of topic in registry hits with a name in passed in topic array
        if (strcmp(*registry[i]->name, ((struct sub_args*)input)->tobe_sub[j]) == 0){
          printf("*\tsubscriber(): HIT[%s]\n", ((struct sub_args*)input)->tobe_sub[j]);


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
            //sleep so print shows up
            sleep(1);
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
          sleep(1);


        }
      }
    }
  }




  // while(1){
  //
  //   for (size_t i = 0; i < MAXTOPICS; i++) {
  //     if(registry[i] != NULL){
  //       if (strcmp(*registry[i]->name, (char *)input) == 0){
  //
  //         //try to getEntry
  //
  //         //lock it down with this topics lock
  //         printf("*\tsubscriber(): Locking up queue[%s]\n", *registry[i]->name);
  //         pthread_mutex_lock(&lock[i]);
  //
  //         int result = getEntry(*registry[i]->name, last_entry, &place_hold);
  //
  //         //unlock it with this topics lock
  //         printf("*\tsubscriber(): Unlocking queue[%s]\n", *registry[i]->name);
  //         pthread_mutex_unlock(&lock[i]);
  //
  //         //if getEntry() returns 0 (all entries < lastEntry+1 <or> Q is empty)
  //         if(result == 0){
  //           printf("*\tsubscriber(): getEntry on [%s] failed\n", *registry[i]->name);
  //           //sleep so print shows up
  //           sleep(1);
  //           //yield CPU and put back on ready Q
  //           sched_yield();
  //         }
  //         //if getEntry() returns 1 (found lastEntry+1)
  //         else if(result == 1){
  //           printf("*\tsubscriber(): getEntry on [%s] found entry:[%d]\n", *registry[i]->name, place_hold.entryNum);
  //           last_entry++;
  //         }
  //         else{
  //           printf("*\tsubscriber(): getEntry on [%s] found entry:[%d]", *registry[i]->name, place_hold.entryNum);
  //           printf(" ... lastEntry is now:[%d]\n", result);
  //           last_entry = result;
  //         }
  //         sleep(1);
  //       }//if()
  //     }//if()
  //   }//for()
  //
  // }//end of infinite while loop

  return NULL;

}//end of subscriber()

//------------------------------------------------------------------------------

int main(int argc, char const *argv[]) {

  //initialize thread pools
  pthread_t sub_pool[NUMPROXIES] = {};
  pthread_t pub_pool[NUMPROXIES] = {};

  //arrays to-be used to decide if thread is taken
  int sub_avail[NUMPROXIES] = {};
  int pub_avail[NUMPROXIES] = {};

  //set all entries to 0 to indicate threads are all free
  for (size_t i = 0; i < NUMPROXIES; i++) {
    sub_avail[i] = 0;
    pub_avail[i] = 0;
  }

  //Initialize a topic Queue***************
  topicEntry buffer1[MAXENTRIES+1] = {};
  topicEntry buffer2[MAXENTRIES+1] = {};
  topicQ testy = {
    .name = "one",
    .buffer = buffer1,
    .head = 0,
    .tail = 0,
    .length = MAXENTRIES + 1 };
  topicQ testy2 = {
    .name = "two",
    .buffer = buffer2,
    .head = 0,
    .tail = 0,
    .length = MAXENTRIES + 1 };
  //NULL entry
  topicEntry null;
  null.entryNum = -1;
  testy.buffer[MAXENTRIES] = null;
  testy2.buffer[MAXENTRIES] = null;
  //DEFAULT entry
  topicEntry def;
  def.entryNum = 0;
  //Set all values besides NULL to 0(default/empty)
  for (size_t i = 0; i < MAXENTRIES; i++) {
    testy.buffer[i] = def;
    testy2.buffer[i] = def;
  }
  //place topicQ in registry
  registry[0] = &testy;
  registry[1] = &testy2;
  //****************************************

  //Entries to-be published
  topicEntry one;
  topicEntry two;
  topicEntry three;
  topicEntry four;
  topicEntry five;

  //filling struct to-be sent to pub thread
  pub_args *trial1 = (pub_args *)malloc(sizeof(pub_args));
  //name of queue to be published to
  char name[] = "one";
  trial1->queue_name = name;
  //filling struct array with entries to be published
  trial1->tobe_pub[0] = one;
  trial1->tobe_pub[1] = two;
  trial1->tobe_pub[2] = three;
  trial1->tobe_pub[3] = four;
  trial1->tobe_pub[4] = five;

  //list of topics to read entries from
  //char to_be_sub[2] = {*testy.name, *testy2.name};
  sub_args *trial2 = (sub_args *)malloc(sizeof(sub_args));
  strcpy(trial2->tobe_sub[0], *testy.name);
  strcpy(trial2->tobe_sub[1], *testy2.name);


  //iterate through pub thread pool
  for (size_t i = 0; i < NUMPROXIES; i++) {
    //if we find an available thread
    if(pub_avail[i] == 0){
      //set that thread to unavailable
      pub_avail[i] = 1;


      //create it with struct we made as param
      //pthread_create(&pub_pool[i], NULL, publisher, (void *)trial1);

      if (0 == pthread_create(&pub_pool[i], NULL, publisher, (void *)trial1)){
       pthread_cancel(pub_pool[i]);
      }


    }
  }

  //iterate through pub thread pool
  for (size_t i = 0; i < NUMPROXIES; i++) {
    //if we find an available thread
    if(sub_avail[i] == 0){
      //set that thread to unavailable
      sub_avail[i] = 1;


      //create it with struct we made as param
      //pthread_create(&sub_pool[i], NULL, subscriber, (void *)trial2);

      if (0 == pthread_create(&sub_pool[i], NULL, subscriber, (void *)trial2)){
        pthread_cancel(sub_pool[i]);
      }

    }
  }

  pthread_t cleanup_thread;
  if (0 == pthread_create(&cleanup_thread, NULL, cleanup, NULL)){
    pthread_cancel(cleanup_thread);
  }

  sleep(10);

  pthread_cancel(cleanup_thread, NULL);

  //cancel all active threads
  for (size_t i = 0; i < NUMPROXIES; i++) {
    if(pub_avail[i] == 1){
      pthread_cancel(pub_pool[i]);
      //set thread to available
      pub_avail[i] = 0;
    }
  }

  //sleep(5);

  // for (size_t i = 0; i < NUMPROXIES; i++) {
  //   if(sub_avail[i] == 1){
  //     pthread_cancel(sub_pool[i], NULL);
  //     //set thread to available
  //     sub_avail[i] = 0;
  //   }
  // }
  //
  // for (size_t i = 0; i < NUMPROXIES; i++) {
  //   if(pub_avail[i] == 1){
  //     pthread_detach(pub_pool[i]);
  //   }
  // }
  // for (size_t i = 0; i < NUMPROXIES; i++) {
  //   if(sub_avail[i] == 1){
  //     pthread_detach(sub_pool[i]);
  //   }
  // }
  //
  // pthread_detach(cleanup_thread);

  free(trial1);
  free(trial2);


  return 0;

}//end of main()

//-----------------------------------------------------------------------------
