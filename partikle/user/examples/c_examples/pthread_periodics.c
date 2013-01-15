/*
** pthread_periodics.c
** 
** Made by (root)
** Login   <root@box1.disca.upv.es>
** 
** Started on  Tue Dec 19 18:19:17 2006 root
** Last update Tue Dec 19 19:20:16 2006 root
*/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define NTASKS 2

pthread_t tasks[NTASKS];
int finish=0;
struct timespec start;


void *task(void *data){

  struct timespec delay = {0,500000000}, now;
  while (!finish){
    nanosleep(&delay, NULL);
    clock_gettime(CLOCK_REALTIME,&now);
    printf("[%d] Wakes up at %04d:%d\n", (int) data, now.tv_sec-start.tv_sec, now.tv_nsec);
  }  

  pthread_exit(0);
  return 0;
} 

int main (){
  struct timespec delay,now;
  int loop;
  
  clock_gettime(CLOCK_REALTIME,&start);

  for (loop=0; loop < NTASKS; loop++) {
    pthread_create(&tasks[loop], NULL, task, (void *)loop);
  }

  delay.tv_sec =0;
  delay.tv_nsec=500000000L;
  printf("===== Main nanosleep for %d:%d\n", delay.tv_sec, delay.tv_nsec);
  for (loop=0; loop < 20; loop++) {
    nanosleep(&delay,NULL);
    clock_gettime(CLOCK_REALTIME,&now);
    printf("\n\n\n Main at %04d:%d\n", now.tv_sec-start.tv_sec, now.tv_nsec);
  }


  delay.tv_sec =5;
  delay.tv_nsec=0L;
  printf("===== Main nanosleep for %d:%d\n", delay.tv_sec, delay.tv_nsec);
  for (loop=0; loop < 4; loop++) {
    nanosleep(&delay,NULL);
    clock_gettime(CLOCK_REALTIME,&now);
    printf("\n\n\n Main at %04d:%d\n", now.tv_sec-start.tv_sec, now.tv_nsec);

  }

  delay.tv_sec =20;
  delay.tv_nsec=0L;
  printf("===== Main nanosleep for %d:%d\n", delay.tv_sec, delay.tv_nsec);
  nanosleep(&delay,NULL);
  clock_gettime(CLOCK_REALTIME,&now);
  printf("\n\n\n Main at %04d:%d\n", now.tv_sec-start.tv_sec, now.tv_nsec);


  return 0;
  
}
