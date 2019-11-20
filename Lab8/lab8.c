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

MTQ *registry[MAXQUEUES];

//-----------------------------------------------------------------------------

int main(){

  //initialize all MTQ structs
  MTQ breakfast;
  *breakfast.name = "breakfast";
  mealTicket buffer1[BUFFER_SIZE];
  *breakfast.buffer = &buffer1;

  MTQ lunch;
  *lunch.name = "lunch";
  mealTicket buffer2[BUFFER_SIZE];
  *lunch.buffer = &buffer2;

  MTQ dinner;
  *dinner.name = "dinner";
  mealTicket buffer3[BUFFER_SIZE];
  *dinner.buffer = &buffer3;

  MTQ bar;
  *bar.name = "bar";
  mealTicket buffer4[BUFFER_SIZE];
  *bar.buffer = &buffer4;

  printf("BFAST: %s\n", *breakfast.name);
  printf("LUNCH: %s\n", *lunch.name);
  printf("DINNER: %s\n", *dinner.name);
  printf("BAR: %s\n", *bar.name);

  //push all MTQ's onto register
  registry[0] = &breakfast;
  registry[1] = &lunch;
  registry[2] = &dinner;
  registry[3] = &bar;

  //create and initialize 3 meal-tickets
  mealTicket m1;
  m1.ticketNum = 0;
  m1.dish = "one";

  mealTicket m2;
  m2.ticketNum = 1;
  m2.dish = "two";

  mealTicket m3;
  m3.ticketNum = 2;
  m3.dish = "three";

  //push meal tickets into MTQ's
  // enqueue(&breakfast.name, &m1);
  // enqueue(&breakfast.name, &m2);
  // enqueue(&breakfast.name, &m3);
  //
  // enqueue(&lunch.name, &m1);
  // enqueue(&lunch.name, &m2);
  // enqueue(&lunch.name, &m3);
  //
  // enqueue(&dinner.name, &m1);
  // enqueue(&dinner.name, &m2);
  // enqueue(&dinner.name, &m3);
  //
  // enqueue(&bar.name, &m1);
  // enqueue(&bar.name, &m2);
  // enqueue(&bar.name, &m3);



}//end of main()

//-----------------------------------------------------------------------------
