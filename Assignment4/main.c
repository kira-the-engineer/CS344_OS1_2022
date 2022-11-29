#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

int myCount = 0; //counter that is incremented by threads
bool startCount; //bool keeping producer wait loop

pthread_mutex_t myMutex = PTHREAD_MUTEX_INITIALIZER; //create mutex
pthread_cond_t myCond1, myCond2; //create conditions to be toggled by threads

//plan below here
//in producer increment count
//once count is incremented, unlock mutex and set flag

//in consumer print count
//once count is printed send condition that count can be incrmented again

void *consumer() {
    if(pthread_mutex_lock(&myMutex) == 0){ //lock before printing
        printf("CONSUMER: myMutex locked \n");
    }

    printf("CONSUMER: Waiting on myCond1 \n");
    while(startCount) //wait until we increment   
    pthread_cond_wait(&myCond1, &myMutex); //wait until producer increments

    printf("myCount %d -> %d \n", myCount - 1, myCount); //print count
    startCount = true;


    if(pthread_mutex_unlock(&myMutex) == 0){ //if we successfully unlock
        printf("CONSUMER: unlock myMutex \n");
    }

    if(pthread_cond_signal(&myCond2) == 0){ //if we successfuly signal
        printf("CONSUMER: signaling myCond2 \n"); //signal that we're done printing
    }
}

int main(){
     printf("PROGRAM START \n");
     pthread_t c;
     startCount = true;

     if(pthread_create(&c, NULL, consumer, NULL) == 0){
	printf("CONSUMER CREATED \n");
     }


     while(1){
        if(pthread_mutex_lock(&myMutex) == 0){ //lock before incrementing counter
	   printf("PRODUCER: myMutex locked \n"); //if mutex_lock returns 0, lock successful
	   //only print if lock is successful
        }

        printf("PRODUCER: Waiting on myCond2 \n");
        while(!startCount) //wait until consumer is done printing
        pthread_cond_wait(&myCond2, &myMutex); //wait until consumer is done printing
    
        myCount += 1; //increment myCount by 1 once consumer has printed
	startCount = false;


        if(pthread_mutex_unlock(&myMutex) == 0){ //if we successfully unlock
	   printf("PRODUCER: unlock myMutex \n");
        }

        if(pthread_cond_signal(&myCond1) == 0){ //if we successfuly signal
	    printf("PRODUCER: signaling myCond1 \n"); //signal that we're done incrementing
        }

	if(myCount == 10){
	   printf("PROGRAM END \n");
	   return 0;
	}

     }

}

