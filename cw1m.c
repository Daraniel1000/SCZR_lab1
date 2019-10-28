#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#define PORT 10000 


int sock = 0; 
struct sockaddr_in serv_addr;

int koniec = 0;
pthread_t pid1, pid2;

char bufor[256];
int gl;

int wait;

pthread_mutex_t mutex; // muteks
pthread_cond_t ready,sent;


void * zczytuj(void * arg)
{
    char tempbuf[256];
    int l;

    while(1) {
    for(int i=0; i<256; ++i) tempbuf[i] =  0;
    printf("Enter a message up to 256 characters: ");
    scanf("%s",tempbuf);
    for(l=0; l<256; ++l)
    {
        if(tempbuf[l]==0) break;
        //printf("%d",tempbuf[l]);
    }
    //printf("tempbuf size = %d\n",l);
    pthread_mutex_lock(&mutex);
    for(int i=0; i<l; ++i)
    {
        bufor[i] = tempbuf[i];
    }
    gl = l;
    pthread_cond_signal(&ready);
    pthread_mutex_unlock(&mutex);
    //usleep(1000);
    }
}

void * wysylaj(void * arg)
{
    while(1) {
        pthread_mutex_lock(&mutex);
        //printf("sender got past mutex\n");
        //pthread_cond_signal(&sent);
        pthread_cond_wait(&ready, &mutex);
        //printf("sender got past cond\n");
        sleep(wait);
        //for(int i=0; i<gl; ++i) printf("%c",bufor[i]);
        //printf("\n");
        //printf("%d\n",gl);
        send(sock , bufor , gl, 0); 
        //printf("message sent\n"); 
        read(sock, bufor, 256);
        //close(sock);
        for(int i=0; i<256; ++i) bufor[i] =  0;
        gl = 0;
        pthread_mutex_unlock(&mutex);
    }
}


int main(int argc, char *argv[])
{ 
    
    if (argc < 3) {
    wait = 0;
    }
    else {
    wait = atoi(argv[2]);
    }
    if(argc < 2) {
    printf("Usage: %s address (optional wait time)...", argv[0]);
    return -1;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    if(inet_pton(AF_INET,argv[1], &serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        { 
        printf("Connection failed");
        return -1;
    }

    gl = 0;

    pthread_create(&pid1,NULL,zczytuj,NULL);
    pthread_create(&pid2,NULL,wysylaj,NULL);
    pthread_join(pid1,NULL);
    pthread_join(pid2,NULL);
    return 0;
}

