#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXNAME 100
#define MAXQUEUES 10

struct mealTicket {
  int ticketNum;
  char *dish;
};
typedef struct mealTicket mealTicket;

struct MTQ {
  char *name[MAXNAME];
  mealTicket *const buffer;
  int head;
  int tail;
  const int length;
};
typedef struct MTQ MTQ;

int enqueue(char *MTQ_ID, mealTicket *MT){

}//end of enqueue()

int dequeue(char *MTQ_ID, int ticketNum, mealTicket *MT){

}//end of dequeue()


MTQ *registry[MAXQUEUES];

int main(){

  //initialize all MTQ structs
  struct MTQ breakfast;
  *breakfast.name = "breakfast";

  struct MTQ lunch;
  *lunch.name = "lunch";

  struct MTQ dinner;
  *dinner.name = "dinner";

  struct MTQ bar;
  *bar.name = "bar";

  //push all MTQ's onto register
  registry[0] = *breakfast;
  registry[1] = *lunch;
  registry[2] = *dinner;
  registry[3] = *bar;

  //create and initialize 3 meal-tickets
  struct mealTicket m1;
  struct mealTicket m2;
  struct mealTicket m3;

  m1.ticketNum = 0;
  m1.dish = "one";

  m2.ticketNum = 1;
  m2.dish = "two";

  m3.ticketNum = 2;
  m3.dish = "three";

  //push meal tickets into MTQ's
  //breakfast.buffer[0] =



}//end of main()
