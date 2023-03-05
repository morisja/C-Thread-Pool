/* 
 * WHAT THIS EXAMPLE DOES
 * 
 * We create a pool of 4 threads and then add 40 tasks to the pool(20 task1 
 * functions and 20 task2 functions). task1 and task2 simply print which thread is running them.
 * 
 * As soon as we add the tasks to the pool, the threads will run them. It can happen that 
 * you see a single thread running all the tasks (highly unlikely). It is up the OS to
 * decide which thread will run what. So it is not an error of the thread pool but rather
 * a decision of the OS.
 * 
 * */

#include <stdio.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include "thpool.h"
#include <sys/stat.h>
#include <string.h>

#define THREADCOUNT 8
#define FILESIZE (1024*1024*3)

typedef struct
{
  char dirname[50];
  char file_path[50];
  int  marker;
  char *buffer;
}command_struct;

void generate_fixed_file(char* file_path);



void generate(char* file_path, size_t size) {
    FILE *myFile = fopen(file_path, "w");
    FILE *randomiser = fopen("/dev/zero", "r");

    char bufor[size];
    size_t result = fread(bufor, 1, size, randomiser);
    fwrite(bufor, 1, result, myFile);

    fclose(myFile);
    fclose(randomiser);
}


void generate_fixed_file(char* file_path)
{
	//printf("%s\n",file_path);
    FILE* fp = fopen(file_path, "wb");

    if (fp == NULL) {
        printf("Error: could not open file for writing.");
        exit(1);
    }

    char* buffer2 = (char*)malloc(FILESIZE);

    for (long int i = 0; i < FILESIZE; i++) {
        buffer2[i] = (char)(0);
    }

	for (int n=0; n<1024*3;n++){
	    fwrite(buffer2, 1, FILESIZE, fp);
	}
	free(buffer2);
    fclose(fp);
}


void write_file(char *file_path, char *buffer){
    FILE* fp = fopen(file_path, "wb");

    if (fp == NULL) {
        printf("Error: could not open file for writing.");
        exit(1);
    }
    fwrite(buffer, 1, FILESIZE, fp);
	fclose(fp);
}

void task(void *param){
	command_struct test = *((command_struct *)param);
	//generate_fixed_file(test.file_path);
	//generate(test.file_path,(1024*1024*3));
	write_file(test.file_path,test.buffer);
	if (test.marker == 1){
		printf("Thread #%u working on %s\n", (int)pthread_self(),test.file_path);
	}
	//free(param);
}


int main(){
	char hex[]="0123456789abcdef";


	puts("Making threadpool with 4 threads");
	threadpool thpool = thpool_init(THREADCOUNT);

	command_struct* test = malloc(sizeof(command_struct));
	int i;
	int s=16*16*16*16;
	command_struct *instructions[s];

    char dirname[50];
    char file_path[50];


	int file_size2=1024*1024*3;
    char* buffer = (char*)malloc(file_size2);

    for (long int i = 0; i < file_size2; i++) {
        buffer[i] = (char)(0);
    }

	unsigned long int t_start=(unsigned long)time(NULL);

	s=0;
    for (int i=0;i<=15;i++){
        for (int j=0;j<=15;j++){
            sprintf(dirname,"./out/%c%c",hex[i],hex[j]);
            mkdir("./out",0755);
            mkdir(dirname,0755);
            for (int p=0;p<=5;p++){
                for (int q=0;q<=5;q++){
					instructions[s]= malloc(sizeof(command_struct));
					strcpy(instructions[s]->dirname,dirname);
                    sprintf(instructions[s]->file_path,"./out/%c%c/%c%c%c%c",hex[i],hex[j],hex[i],hex[j],hex[p],hex[q]);
					instructions[s]->marker=0;
					instructions[s]->buffer=buffer;
					if (s%(16*16)==0){
						instructions[s]->marker=1;
					}		
					thpool_add_work(thpool, task,instructions[s]);
					s++;
	                //generate_fixed_file(file_path, buffer,file_size);
                }
            }
        }
    }
	printf("waiting\n");

	thpool_wait(thpool);
	unsigned long int t_end=(unsigned long)time(NULL);
	unsigned long int duration=t_end-t_start;

	printf("job took %lu seconds\n",duration);
	//read from /dev/zero
	//1 thread, 25 files per - 33s
	//2 thread, 25 files per - 37s
	//4 thread, 25 files per - 28s
	//8 thread, 25 files per - 28s
	//generate fixed file
	//1 thread, 25 files per - 40s
	//2 thread, 25 files per - 31s
	//4 thread, 25 files per - 30s
	//8 thread, 25 files per - 30s
	//sharedbuffer
	//1 thread, 25 files per - 33s
	//2 thread, 25 files per - 27s
	//4 thread, 25 files per - 28s
	//8 thread, 25 files per - 27s
	puts("Killing threadpool");
	thpool_destroy(thpool);
	for(int i=0;i<=s;i++){
		free(instructions[s]);
	}
	
	return 0;
}
