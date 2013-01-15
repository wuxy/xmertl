/*
** world.c
** 
*/

#include <stdio.h>
#include <pthread.h>


void *foo (void  *str){
  printf("%s",str);
  pthread_exit(0);
  return NULL;
}
