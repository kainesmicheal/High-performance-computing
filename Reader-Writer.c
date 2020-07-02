#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define NUM_READERS 5
#define NUM_READS 5
#define NUM_WRITERS 5
#define NUM_WRITES 5

unsigned int gSharedValue = 0;
pthread_mutex_t gSharedMemoryLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gReadPhase = PTHREAD_COND_INITIALIZER;
pthread_cond_t gWritePhase = PTHREAD_COND_INITIALIZER;
int gWaitingReaders = 0, gReaders = 0;

void* writerMain(void* threadArgument);
void* readerMain(void* threadArgument);

int main(int argc, char** argv){
    int i;
    int readerNum[NUM_READERS];
    int writerNum[NUM_WRITERS];

    pthread_t readerThreadIDs[NUM_READERS];
    pthread_t writerThreadIDs[NUM_WRITERS];

    srandom((unsigned)time(NULL));

    for(i = 0; i< NUM_READERS;i++){
        readerNum[i] = i;
        pthread_create(&readerThreadIDs[i],NULL,readerMain,&readerNum[i]);
    }
    for(i=0; i<NUM_WRITERS; i++){
        writerNum[i] = i;
        pthread_create(&writerThreadIDs[i],NULL,writerMain,&writerNum[i]);
    }
    for(i=0; i< NUM_READERS; i++){
        pthread_join(readerThreadIDs[i],NULL);
    }
    for(i=0; i<NUM_WRITERS; i++) {
        pthread_join(writerThreadIDs[i],NULL);
    }
    return 0;
}

void* readerMain(void* threadArgument) {
    int id = *((int*)threadArgument);
    int i = 0, numReaders =0;

    for(i = 0; i< NUM_READS; i++){
        usleep(1000 * (random() % NUM_READERS + NUM_WRITERS));

        pthread_mutex_lock(&gSharedMemoryLock);
            gWaitingReaders++;
            while(gReaders == -1){
                pthread_cond_wait(&gReadPhase, &gSharedMemoryLock);
            }
            gWaitingReaders--;
            numReaders = ++gReaders;
        pthread_mutex_unlock(&gSharedMemoryLock);

        fprintf(stdout, "[r%d] reading %u [readers: %2d]\n",id,gSharedValue);

        pthread_mutex_lock(&gSharedMemoryLock);
            gReaders--;
            if(gReaders == 0){
                pthread_cond_signal(&gWritePhase);
            }
        pthread_mutex_unlock(&gSharedMemoryLock);
    }

    pthread_exit(0);
}

void* writerMain(void* threadArgument){
    int id = *((int*)threadArgument);
    int i =0, numReaders =0;

    for(i = 0; i < NUM_WRITES; i++){
        usleep(1000 * (random() % NUM_READERS + NUM_WRITERS));

        pthread_mutex_lock(&gSharedMemoryLock);
            while(gReaders != 0){
                pthread_cond_wait(&gWritePhase, &gSharedMemoryLock);
            }
            gReaders = -1;
            numReaders = gReaders;
        pthread_mutex_lock(&gSharedMemoryLock);

        fprintf(stdout, "[w%d] writing %u [readers: %2d]\n",id, ++gSharedValue);

        pthread_mutex_lock(&gSharedMemoryLock);
            gReaders = 0;
            if(gWaitingReaders > 0){
                pthread_cond_broadcast(&gReadPhase);
            }
            else{
                pthread_cond_broadcast(&gWritePhase);
            }
        pthread_mutex_unlock(&gSharedMemoryLock);
    }
    pthread_exit(0);
}
