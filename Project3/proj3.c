#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/time.h>

//-----------------------------------------------------------------------------

#define MAXENTRIES 6 //compare to BUFFER_SIZE
#define MAXTOPICS 1 //compare to MAXQUEUES
#define MAXNAME 100 //max name of topic queue

#define URLSIZE 100
#define CAPSIZE 100

#define DELTA 999

//-----------------------------------------------------------------------------

struct topicEntry {
  int entryNum;
  struct timeval timeStamp;
  int pubID;
  char photoURL[URLSIZE];
  char photoCaption[CAPSIZE];
};
typedef struct topicEntry topicEntry;

//-----------------------------------------------------------------------------

struct topicQ {
  char *name[MAXNAME];
  topicEntry *const buffer;
  int head;
  int tail;
  const int length;
};
typedef struct topicQ topicQ;

//-----------------------------------------------------------------------------

//Global registry for topic Q's
topicQ *registry[MAXTOPICS];

//-----------------------------------------------------------------------------

//global variable for entry number
int entry_number = 1;

//-----------------------------------------------------------------------------

void printQ(char *QID){
  for (size_t i = 0; i < MAXTOPICS; i++) {
    if (strcmp(*registry[i]->name, QID) == 0){
      printf("\n\n---Entries of Q[%s]---\n[", *registry[i]->name);
      for (size_t j = 0; j <= MAXENTRIES; j++) {
        printf("%d, ", registry[i]->buffer[j].entryNum);
      }
      printf("]\n-------------------------\n\n");
    }
  }
}//end of printQ()

//-----------------------------------------------------------------------------

int enqueue(char *QID, topicEntry *TE){

  for (size_t i = 0; i < MAXTOPICS; i++) {
    if (strcmp(*registry[i]->name, QID) == 0){
      //if tail is NOT located @ null
      if (registry[i]->buffer[registry[i]->tail].entryNum != -1){
        //place TE at tail location
        registry[i]->buffer[registry[i]->tail] = *TE;
        //Set TE entryNum
        registry[i]->buffer[registry[i]->tail].entryNum = entry_number;
        entry_number++;
        //increment tail
        int new_tail = (registry[i]->tail + 1) % (MAXENTRIES+1);
        registry[i]->tail = new_tail;
        return 1;
      }
      else{
        printf("BUFFER FULL\n");
        return 0;
      }
    }//end of if registry is correct
  }//end for loop
  return 0;
}//end of enqueue()

//-----------------------------------------------------------------------------

int getEntry(char *QID, int lastEntry, topicEntry *TE){

  for (size_t i = 0; i < MAXTOPICS; i++) {
    if (strcmp(*registry[i]->name, QID) == 0){

      //Case1: topic queue is empty: return 0
      if (registry[i]->tail == registry[i]->head){
        printf("getEntry: <queue is empty>\n");
        return 0;
      }

      //Case2: lastEntry+1 is in Queue: copy lastEntry+1 data to TE, return 1)
      for (size_t j = 0; j < MAXENTRIES+1; j++) {
        if (registry[i]->buffer[j].entryNum == lastEntry+1){
          TE = registry[i]->buffer[j];
          printf("getEntry: <found lastEntry+1>\n");
          return 1;
        }
      }

      //Case3: Topic queue is NOT empty & lastEntry+1 is NOT in queues
      for (size_t j = 0; j < MAXENTRIES+1; j++) {
        //ii: if there exists entry.entryNum > lastEntry+1:
        if (registry[i]->buffer[j].entryNum > lastEntry+1){
          //copy entry data to TE, return entry.entryNum
          TE = registry[i]->buffer[j];
          printf("getEntry: <found something bigger>\n");
          return registry[i]->buffer[j].entryNum;
        }
      }
      //i: if all entries < lastEntry+1: return 0
      printf("getEntry: <all enties less than lastEntry+1>\n");
      return 0;
    }
  }

  printf("Invalid Queue name!\n");
  return 0;

}//end of getEntry()

//-----------------------------------------------------------------------------

int dequeue(char *QID){

  for (size_t i = 0; i < MAXTOPICS; i++) {
    if (strcmp(*registry[i]->name, QID) == 0){
      //If tail != head
      if (registry[i]->tail != registry[i]->head){
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
        printf("BUFFER EMPTY\n");
        return 0;
      }
    }//end of if registry is correct
  }//end for loop
  return 0;
}//end of dequeue()

//-----------------------------------------------------------------------------

int main(int argc, char const *argv[]) {

  topicEntry buffer1[MAXENTRIES+1];

  topicQ testy = {
    .name = "test",
    .buffer = buffer1,
    .head = 0,
    .tail = 0,
    .length = MAXENTRIES + 1 };

  topicEntry null;
  null.entryNum = -1;
  testy.buffer[MAXENTRIES] = null;

  registry[0] = &testy;

  topicEntry def;
  def.entryNum = 0;

  for (size_t i = 0; i < MAXENTRIES; i++) {
    testy.buffer[i] = def;
  }

  topicEntry tst;

  printf("\nTesting getEntry()\n");
  for (size_t z = 0; z < MAXENTRIES+1; z++) {
    printQ(*testy.name);
    dequeue(*testy.name);
  }

  topicEntry place_hold;
  place_hold.entryNum = -999;

  printf("\n\nWe're now empty, lets try case one (empty queue)...\n");
  printQ(*testy.name);
  printf("\tResult:[%d]\n", getEntry(*testy.name, entry_number, &place_hold));
  printQ(*testy.name);
  printf("place_hold[%d]\n\n", place_hold.entryNum);


  for (size_t z = 0; z < MAXENTRIES+1; z++) {
    //printQ(*testy.name);
    enqueue(*testy.name, &tst);
  }
  printf("We're now full, lets try case two (lastEntry+1 in queue)...\n");
  printQ(*testy.name);
  printf("\tResult:[%d]\n", getEntry(*testy.name, 3, &place_hold));
  printQ(*testy.name);
  printf("place_hold[%d]\n\n", place_hold.entryNum);


  printf("lets try case three.1 (not empty, all are less)...\n");
  printQ(*testy.name);
  printf("\tResult:[%d]\n", getEntry(*testy.name, 999, &place_hold));
  printQ(*testy.name);
  printf("place_hold[%d]\n\n", place_hold.entryNum);

  printf("lets try case three.2 (not empty, found something bigger)...\n");
  printQ(*testy.name);
  printf("\tResult:[%d]\n", getEntry(*testy.name, -420, &place_hold));
  printQ(*testy.name);
  printf("place_hold[%d]\n", place_hold.entryNum);


  // for (size_t z = 0; z < MAXENTRIES+1; z++) {
  //   printQ(*testy.name);
  //   enqueue(*testy.name, &tst);
  // }

  return 0;

}//end of main()

//-----------------------------------------------------------------------------
