#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//-----------------------------------------------------------------------------

#define MAXNAME 100 //max name of mealTicket queue
#define MAXQUEUES 10 //max number of queues

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

int enqueue(char *MTQ_ID, mealTicket *MT){
  return 0;
}//end of enqueue()

//-----------------------------------------------------------------------------

int dequeue(char *MTQ_ID, int ticketNum, mealTicket *MT){
  return 0;
}//end of dequeue()

//-----------------------------------------------------------------------------

//Global registry
MTQ *registry[MAXQUEUES];

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

  printf("Name: %s\n", *brk.name);
  printf("Buffer[0]: %d\n", *brk.buffer[0]);
  printf("Head: %d\n", brk.head);
  printf("Tail: %d\n", brk.tail);
  printf("Length: %d\n", brk.length);

  // printf("BFAST: %s\n", *brk.name);
  // printf("LUNCH: %s\n", *lun.name);
  // printf("DINNER: %s\n", *din.name);
  // printf("BAR: %s\n", *bar.name);

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
  m2.ticketNum = 1;
  m2.dish = "d_two";

  mealTicket m3;
  m3.ticketNum = 2;
  m3.dish = "d_three";

  //push meal tickets into MTQ's
  // enqueue(&brk.name, &m1);
  // enqueue(&brk.name, &m2);
  // enqueue(&brk.name, &m3);
  //
  // enqueue(&lun.name, &m1);
  // enqueue(&lun.name, &m2);
  // enqueue(&lun.name, &m3);
  //
  // enqueue(&din.name, &m1);
  // enqueue(&din.name, &m2);
  // enqueue(&dins.name, &m3);
  //
  // enqueue(&bar.name, &m1);
  // enqueue(&bar.name, &m2);
  // enqueue(&bar.name, &m3);



}//end of main()

//-----------------------------------------------------------------------------
