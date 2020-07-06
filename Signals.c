#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void DieWithError(char* errorMessage);
void InterruptSignalHandler(int signalType);

int main(int argc, char* argv[]){
    struct sigaction handler;
    handler.sa_handler = InterruptSignalHandler;
    if(sigfillset(&handler.sa_mask) < 0){
        DieWithError("Sigfillset() failed");
    }
    handler.sa_flags = 0;
    if(sigaction(SIGINT, &handler, 0) < 0){
        DieWithError("Sigaction() failed");
    }
    for(;;) pause();
    exit(0);
}

void InterruptHandler (int signalType){
    fprintf(stderr, "Interrup received. Exiting program.\n");
    exit(1);
}
void DieWithError(char* errorMessage){
    printf("%s \n",errorMessage);
    exit(1);
}

