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
        registry[i]->tail += 1; //TODO make this wraparound

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

  for (size_t i = 0; i < MAXQUEUES; i++) {

    if (strcmp(*registry[i]->name, MTQ_ID) == 0){

      //If tail != head
      if (registry[i]->tail != registry[i]->head){

        //Set MT with data @ head
        MT->ticketNum = registry[i]->buffer[registry[i]->head].ticketNum;
        MT->dish = registry[i]->buffer[registry[i]->head].dish;

        //Set data @ tail to blank data entry
        registry[i]->buffer[registry[i]->tail].ticketNum = 0;
        registry[i]->buffer[registry[i]->tail].dish = "default";

        //Set where head is to NULL
        registry[i]->buffer[registry[i]->head].ticketNum = -1;
        registry[i]->buffer[registry[i]->head].dish = "null";

        //Increment head
        registry[i]->head += 1; //TODO make this wraparound

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

  enqueue(*lun.name, &m1);
  enqueue(*lun.name, &m2);
  enqueue(*lun.name, &m3);

  enqueue(*din.name, &m1);
  enqueue(*din.name, &m2);
  enqueue(*din.name, &m3);

  enqueue(*bar.name, &m1);
  enqueue(*bar.name, &m2);
  enqueue(*bar.name, &m3);

  int is_empty = MAXQUEUES;

  mealTicket test;

  while(is_empty > 0){

    printf("\n");
    for (size_t i = 0; i <= BUFFER_SIZE; i++) {
      printf("%s, ", registry[0]->buffer[i].dish);
    }
    printf("\n");

    if (dequeue(*brk.name, 1, &test) == 0) {is_empty--;}
    else {printf("Queue: <%s> - Ticket Number: <%d> - Dish: <%s>\n", *brk.name, test.ticketNum, test.dish);}
    if (dequeue(*lun.name, 1, &test) == 0) {is_empty--;}
    else {printf("Queue: <%s> - Ticket Number: <%d> - Dish: <%s>\n", *lun.name, test.ticketNum, test.dish);}
    if (dequeue(*din.name, 1, &test) == 0) {is_empty--;}
    else {printf("Queue: <%s> - Ticket Number: <%d> - Dish: <%s>\n", *din.name, test.ticketNum, test.dish);}
    if (dequeue(*bar.name, 1, &test) == 0) {is_empty--;}
    else {printf("Queue: <%s> - Ticket Number: <%d> - Dish: <%s>\n", *bar.name, test.ticketNum, test.dish);}

  }//end of while(not all queues are empty)

  // //test A.
  // printf("\n***TEST [A]***\n");
  // printf("Test Case: <A> - Result: <%d>\n", dequeue(*brk.name, 1, &test));
  //
  // //test B.
  // printf("\n***TEST [B]***\n");
  // enqueue(*lun.name, &m1);
  // enqueue(*lun.name, &m2);
  // enqueue(*lun.name, &m3);
  // enqueue(*lun.name, &m1);
  // enqueue(*lun.name, &m2);
  // enqueue(*lun.name, &m3);
  // enqueue(*lun.name, &m1);
  // enqueue(*lun.name, &m2);
  // enqueue(*lun.name, &m3);
  // printf("Test Case: <B> - Result: <%d>\n", dequeue(*lun.name, 1, &test));
  //
  // //test C.
  // printf("\n***TEST [C]***\n");
  // enqueue(*din.name, &m1);
  // enqueue(*din.name, &m2);
  // enqueue(*din.name, &m3);
  // enqueue(*din.name, &m1);
  // enqueue(*din.name, &m2);
  // enqueue(*din.name, &m3);
  // enqueue(*din.name, &m1);
  // enqueue(*din.name, &m2);
  // enqueue(*din.name, &m2);
  // printf("Test Case: <C> - Result: <%d>\n", enqueue(*din.name, &m3));
  //
  // //test D.
  // printf("\n***TEST [D]***\n");
  // printf("Test Case: <D> - Result: <%d>\n", enqueue(*bar.name, &m1));

  //More testing.........

  // for (size_t i = 0; i <= BUFFER_SIZE; i++) {
  //   printf("%d, \n", registry[0]->buffer[i].ticketNum);
  // }
  //
  // printf("\n\n");
  //
  // for (size_t i = 0; i < BUFFER_SIZE+1; i++) {
  //   printf("deQ - Result: <%d>\n", dequeue(*brk.name, 1, &test));
  // }
  //
  // printf("\n\n");
  //
  // for (size_t i = 0; i <= BUFFER_SIZE; i++) {
  //   printf("%d, \n", registry[0]->buffer[i].ticketNum);
  // }
  //
  // for (size_t i = 0; i < BUFFER_SIZE+1; i++) {
  //   printf("enQ - Result: <%d>\n", enqueue(*brk.name, &test));
  // }
  //
  // printf("\n\n");


  return 0;

}//end of main()

//-----------------------------------------------------------------------------
