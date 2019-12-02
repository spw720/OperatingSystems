//
//Project 3 (part1) - Sean Wilson - CIS415 @ UofO F'19
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

  //test entry
  topicEntry tst;

  while(1){
    for (size_t i = 0; i < MAXTOPICS; i++) {
      if(registry[i] != NULL){
        if (strcmp(*registry[i]->name, (char *)input) == 0){

          //lock it down with this topics lock
          printf("*\tpublisher(): Locking up queue[%s]\n", *registry[i]->name);
          pthread_mutex_lock(&lock[i]);

          int result = enqueue(*registry[i]->name, &tst);

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
            result = enqueue(*registry[i]->name, &tst);

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
        }//end of if() Q names match
      }//end of if() Q is null
    }//end of for() topic in topics
  }//end of infinite while() loop
  return NULL;
}//end of publisher()

//------------------------------------------------------------------------------

void *subscriber(void *input){ //getEntry()

  //empty struct to-be filled by getEntry()
  topicEntry place_hold;
  place_hold.entryNum = -999;

  //last entry initially set to 1
  int last_entry = 1;

  while(1){

    for (size_t i = 0; i < MAXTOPICS; i++) {
      if(registry[i] != NULL){
        if (strcmp(*registry[i]->name, (char *)input) == 0){

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
        }//if()
      }//if()
    }//for()

  }//end of infinite while loop

  return NULL;

}//end of subscriber()

//------------------------------------------------------------------------------

int main(int argc, char const *argv[]) {

  topicEntry buffer1[MAXENTRIES+1] = {};

  topicEntry buffer2[MAXENTRIES+1] = {};

  topicEntry buffer3[MAXENTRIES+1] = {};

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

  topicQ testy3 = {
    .name = "three",
    .buffer = buffer3,
    .head = 0,
    .tail = 0,
    .length = MAXENTRIES + 1 };

  //NULL entry
  topicEntry null;
  null.entryNum = -1;
  //DEFAULT entry
  topicEntry def;
  def.entryNum = 0;

  //Set last entry to null
  testy.buffer[MAXENTRIES] = null;
  testy2.buffer[MAXENTRIES] = null;
  testy3.buffer[MAXENTRIES] = null;

  //place topicQ in registry
  registry[0] = &testy;
  registry[1] = &testy2;
  registry[2] = &testy3;

  //Set all values besides NULL to 0(default/empty)
  for (size_t i = 0; i < MAXENTRIES; i++) {
    testy.buffer[i] = def;
    testy2.buffer[i] = def;
    testy3.buffer[i] = def;
  }

  printQ(*testy.name);

  pthread_t cleanup_thread;

  pthread_t publisher_thread1_1;
  pthread_t publisher_thread1_2;
  pthread_t publisher_thread1_3;

  pthread_t subscriber_thread1_1;
  pthread_t subscriber_thread1_2;
  pthread_t subscriber_thread1_3;

  pthread_t publisher_thread2_1;
  pthread_t publisher_thread2_2;
  pthread_t publisher_thread2_3;

  pthread_t subscriber_thread2_1;
  pthread_t subscriber_thread2_2;
  pthread_t subscriber_thread2_3;

  pthread_t publisher_thread3_1;
  pthread_t publisher_thread3_2;
  pthread_t publisher_thread3_3;

  pthread_t subscriber_thread3_1;
  pthread_t subscriber_thread3_2;
  pthread_t subscriber_thread3_3;

  pthread_create(&publisher_thread1_1, NULL, publisher, *testy.name);
  pthread_create(&publisher_thread1_2, NULL, publisher, *testy.name);
  pthread_create(&publisher_thread1_3, NULL, publisher, *testy.name);

  pthread_create(&publisher_thread2_1, NULL, publisher, *testy2.name);
  pthread_create(&publisher_thread2_2, NULL, publisher, *testy2.name);
  pthread_create(&publisher_thread2_3, NULL, publisher, *testy2.name);

  pthread_create(&publisher_thread3_1, NULL, publisher, *testy3.name);
  pthread_create(&publisher_thread3_2, NULL, publisher, *testy3.name);
  pthread_create(&publisher_thread3_3, NULL, publisher, *testy3.name);

  //printQ(*testy.name);
  sleep(5);
  //printQ(*testy.name);

  pthread_create(&subscriber_thread1_1, NULL, subscriber, *testy.name);
  pthread_create(&subscriber_thread1_2, NULL, subscriber, *testy.name);
  pthread_create(&subscriber_thread1_3, NULL, subscriber, *testy.name);

  pthread_create(&subscriber_thread2_1, NULL, subscriber, *testy2.name);
  pthread_create(&subscriber_thread2_2, NULL, subscriber, *testy2.name);
  pthread_create(&subscriber_thread2_3, NULL, subscriber, *testy2.name);

  pthread_create(&subscriber_thread3_1, NULL, subscriber, *testy3.name);
  pthread_create(&subscriber_thread3_2, NULL, subscriber, *testy3.name);
  pthread_create(&subscriber_thread3_3, NULL, subscriber, *testy3.name);

  //printQ(*testy.name);
  sleep(5);
  //printQ(*testy.name);

  pthread_create(&cleanup_thread, NULL, cleanup, NULL);

  //printQ(*testy.name);
  sleep(5);
  //printQ(*testy.name);

  pthread_cancel(subscriber_thread1_1);
  pthread_cancel(subscriber_thread1_2);
  pthread_cancel(subscriber_thread1_3);

  pthread_cancel(subscriber_thread2_1);
  pthread_cancel(subscriber_thread2_2);
  pthread_cancel(subscriber_thread2_3);

  pthread_cancel(subscriber_thread3_1);
  pthread_cancel(subscriber_thread3_2);
  pthread_cancel(subscriber_thread3_3);

  sleep(5);

  pthread_cancel(publisher_thread1_1);
  pthread_cancel(publisher_thread1_2);
  pthread_cancel(publisher_thread1_3);

  pthread_cancel(publisher_thread2_1);
  pthread_cancel(publisher_thread2_2);
  pthread_cancel(publisher_thread2_3);

  pthread_cancel(publisher_thread3_1);
  pthread_cancel(publisher_thread3_2);
  pthread_cancel(publisher_thread3_3);

  sleep(10);

  pthread_cancel(cleanup_thread);

  pthread_join(cleanup_thread, NULL);

  pthread_join(publisher_thread1_1, NULL);
  pthread_join(publisher_thread1_2, NULL);
  pthread_join(publisher_thread1_3, NULL);

  pthread_join(publisher_thread2_1, NULL);
  pthread_join(publisher_thread2_2, NULL);
  pthread_join(publisher_thread2_3, NULL);

  pthread_join(publisher_thread3_1, NULL);
  pthread_join(publisher_thread3_2, NULL);
  pthread_join(publisher_thread3_3, NULL);

  pthread_join(subscriber_thread1_1, NULL);
  pthread_join(subscriber_thread1_2, NULL);
  pthread_join(subscriber_thread1_3, NULL);

  pthread_join(subscriber_thread2_1, NULL);
  pthread_join(subscriber_thread2_2, NULL);
  pthread_join(subscriber_thread2_3, NULL);

  pthread_join(subscriber_thread3_1, NULL);
  pthread_join(subscriber_thread3_2, NULL);
  pthread_join(subscriber_thread3_3, NULL);

  printQ(*testy.name);

  //topicEntry tst;
  //topicEntry place_hold
  // //Case 1
  // printf("\nCase 1 le[%d](empty queue)...\n", entry_number);
  // printf(">\tResult:[%d]\n", getEntry(*testy.name, entry_number, &place_hold));
  // printf(">\tplace_hold[%d]\n\n", place_hold.entryNum);
  // //Fill test buffer
  // for (size_t z = 0; z < MAXENTRIES+1; z++) { enqueue(*testy.name, &tst); }
  // printQ(*testy.name);
  // //Case 2
  // printf("Case 2 le[3](lastEntry+1 in queue)...\n");
  // printf(">\tResult:[%d]\n", getEntry(*testy.name, 3, &place_hold));
  // printf(">\tplace_hold[%d]\n\n", place_hold.entryNum);
  // //Case 3.1
  // printf("Case 3.1 le[999](not empty, all are less)...\n");
  // printf(">\tResult:[%d]\n", getEntry(*testy.name, 999, &place_hold));
  // printf(">\tplace_hold[%d]\n\n", place_hold.entryNum);
  // //Case 3.2
  // printf("Case 3.2 le[-420](not empty, found something bigger)...\n");
  // printf(">\tResult:[%d]\n", getEntry(*testy.name, -420, &place_hold));
  // printf(">\tplace_hold[%d]\n\n", place_hold.entryNum);
  // //Case 4 (invalid queue name)
  // printf("Case 4 (invalid queue name)...\n");
  // printf(">\tResult:[%d]\n", getEntry("nonsense", -420, &place_hold));

  return 0;

}//end of main()

//-----------------------------------------------------------------------------
