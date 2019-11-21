#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//-----------------------------------------------------------------------------

#define MAXNAME 100 //max name of mealTicket queue
#define MAXQUEUES 4 //max number of queues

#define BUFFER_SIZE 8 //max entries in each MTQ Buffer

//-----------------------------------------------------------------------------

struct mealTicket {
  int ticketNum;
  char *dish;
};
typedef struct mealTicket mealTicket;

//-----------------------------------------------------------------------------

struct MTQ {
  char *name[MAXNAME];
  mealTicket *const buffer;
  int head;
  int tail;
  const int length;
};
typedef struct MTQ MTQ;

//-----------------------------------------------------------------------------

//Global registry
MTQ *registry[MAXQUEUES];

//-----------------------------------------------------------------------------

int global_ticket = 1;

int enqueue(char *MTQ_ID, mealTicket *MT){

  printf("\n--------------------------------\n\n");

  for (size_t i = 0; i < MAXQUEUES; i++) {

    if (strcmp(*registry[i]->name, MTQ_ID) == 0){

      //if tail is NOT located @ null
      if (registry[i]->buffer[registry[i]->tail].ticketNum != -1){

        //place MT at tail location
        registry[i]->buffer[registry[i]->tail] = *MT;

        //Set MT ticketNum
        registry[i]->buffer[registry[i]->tail].ticketNum = global_ticket;
        global_ticket++;

        printf("placed ticket[%s, %d] @ index[%d]\n",
        registry[i]->buffer[registry[i]->tail].dish,
        registry[i]->buffer[registry[i]->tail].ticketNum,
        registry[i]->tail);

        //increment tail
        printf("tail was: %d\n", registry[i]->tail);
        registry[i]->tail += 1;
        printf("tail is now: %d\n", registry[i]->tail);

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

int dequeue(char *MTQ_ID, int ticketNum, mealTicket *MT){

  printf("\n--------------------------------\n\n");

  for (size_t i = 0; i < MAXQUEUES; i++) {

    if (strcmp(*registry[i]->name, MTQ_ID) == 0){

      //If tail != head
      if (registry[i]->tail != registry[i]->head){

        //Set MT with data @ head
        MT->ticketNum = registry[i]->buffer[registry[i]->head].ticketNum;
        MT->dish = registry[i]->buffer[registry[i]->head].dish;

        printf("MT now has: [%d, %s]\n", MT->ticketNum, MT->dish);

        //Set where head is to NULL
        registry[i]->buffer[registry[i]->head].ticketNum = -1;
        registry[i]->buffer[registry[i]->head].dish = "null";

        printf("HEAD now has: [%d, %s]\n",
        registry[i]->buffer[registry[i]->head].ticketNum,
        registry[i]->buffer[registry[i]->head].dish);

        //Set where NULL ~WAS~ to blank data entry
        registry[i]->buffer[registry[i]->head-1].ticketNum = 0;
        registry[i]->buffer[registry[i]->head-1].dish = "default";

        printf("HEAD-1 now has: [%d, %s]\n",
        registry[i]->buffer[registry[i]->head-1].ticketNum,
        registry[i]->buffer[registry[i]->head-1].dish);


        printf("HEAD was: %d\n", registry[i]->head);
        //Increment head
        registry[i]->head += 1;
        printf("HEAD is now: %d\n", registry[i]->head);

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

int main(){

  //initialize all MTQ structs

  mealTicket buffer1[BUFFER_SIZE + 1];
  mealTicket buffer2[BUFFER_SIZE + 1];
  mealTicket buffer3[BUFFER_SIZE + 1];
  mealTicket buffer4[BUFFER_SIZE + 1];

  MTQ brk = {
    .name = "Breakfast",
    .buffer = buffer1,
    .head = 0,
    .tail = 0,
    .length = BUFFER_SIZE + 1 };
  MTQ lun = {
    .name = "Lunch",
    .buffer = buffer2,
    .head = 0,
    .tail = 0,
    .length = BUFFER_SIZE + 1 };
  MTQ din = {
    .name = "Dinner",
    .buffer = buffer3,
    .head = 0,
    .tail = 0,
    .length = BUFFER_SIZE + 1 };
  MTQ bar = {
    .name = "Bar",
    .buffer = buffer4,
    .head = 0,
    .tail = 0,
    .length = BUFFER_SIZE + 1 };

  //Set last element of buffer to NULL
  mealTicket null;
  null.ticketNum = -1;
  null.dish = "null";
  brk.buffer[BUFFER_SIZE] = null;
  lun.buffer[BUFFER_SIZE] = null;
  din.buffer[BUFFER_SIZE] = null;
  bar.buffer[BUFFER_SIZE] = null;

  //push all MTQ's onto register
  registry[0] = &brk;
  registry[1] = &lun;
  registry[2] = &din;
  registry[3] = &bar;

  //create and initialize 3 meal-tickets
  mealTicket m1;
  m1.ticketNum = 0;
  m1.dish = "d_one";

  mealTicket m2;
  m2.ticketNum = 0;
  m2.dish = "d_two";

  mealTicket m3;
  m3.ticketNum = 0;
  m3.dish = "d_three";

  //push meal tickets into MTQ's
  enqueue(*brk.name, &m1);
  enqueue(*brk.name, &m2);
  enqueue(*brk.name, &m3);

  mealTicket test;
  dequeue(*brk.name, 1, &test);
  printf("\n\ntst: %s : %d\n", test.dish, test.ticketNum);
  dequeue(*brk.name, 1, &test);
  printf("\n\ntst: %s : %d\n", test.dish, test.ticketNum);
  dequeue(*brk.name, 1, &test);
  printf("\n\ntst: %s : %d\n", test.dish, test.ticketNum);
  dequeue(*brk.name, 1, &test);
  printf("\n\ntst: %s : %d\n", test.dish, test.ticketNum);
  dequeue(*brk.name, 1, &test);
  printf("\n\ntst: %s : %d\n", test.dish, test.ticketNum);
  dequeue(*brk.name, 1, &test);
  printf("\n\ntst: %s : %d\n", test.dish, test.ticketNum);

  // enqueue(*lun.name, &m1);
  // enqueue(*lun.name, &m2);
  // enqueue(*lun.name, &m3);
  //
  // enqueue(*din.name, &m1);
  // enqueue(*din.name, &m2);
  // enqueue(*din.name, &m3);
  //
  // enqueue(*bar.name, &m1);
  // enqueue(*bar.name, &m2);
  // enqueue(*bar.name, &m3);

  return 0;

}//end of main()

//-----------------------------------------------------------------------------
