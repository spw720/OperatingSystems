#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//-----------------------------------------------------------------------------

#define MAXNAME 100
#define MAXQUEUES 10

#define BUFFER_SIZE 8

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
  mealTicket buffer1[BUFFER_SIZE];
  MTQ brk = {
    .name = "Breakfast",
    .buffer = buffer1,
    .head = 0,
    .tail = 0,
    .length = BUFFER_SIZE
  }

  //*brk.name = "Breakfast";

  //*brk.buffer = buffer1;

  // MTQ lun;
  // *lun.name = "Lunch";
  // mealTicket buffer2[BUFFER_SIZE];
  // *lun.buffer = buffer2;
  //
  // MTQ din;
  // *din.name = "Dinner";
  // mealTicket buffer3[BUFFER_SIZE];
  // *din.buffer = buffer3;
  //
  // MTQ bar;
  // *bar.name = "Bar";
  // mealTicket buffer4[BUFFER_SIZE];
  // *bar.buffer = buffer4;


  // printf("BFAST: %s\n", *brk.name);
  // printf("LUNCH: %s\n", *lun.name);
  // printf("DINNER: %s\n", *din.name);
  // printf("BAR: %s\n", *bar.name);

  //push all MTQ's onto register
  // registry[0] = &brk;
  // registry[1] = &lun;
  // registry[2] = &din;
  // registry[3] = &bar;

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
