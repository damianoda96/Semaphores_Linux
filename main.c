#include<stdio.h> 
#include<sys/types.h> 
#include<semaphore.h>
#include<pthread.h>

int mutex_used=1,generated_item[2],generated=0, running = 1; 

char *item[]={"tobacco","paper","matches"}; 

sem_t mutex;

void *agent(void *arg) 
{ 
	int i,j,k=0; 
	while(1)       
	{ 
		sleep(1); 
		sem_wait(&mutex); 
		if(mutex_used==1)             
		{ 
			i=k; 
			j=i+1; 
			if(j==3) 
				j=0; 
			k=j; 
			generated_item[0]=i; 
			generated_item[1]=j; 
			printf("agent is produced %s,%s\n",item[i],item[j]); 
			generated=1; 
			mutex_used=0;             
		}

		

		sem_post(&mutex);       
	} 
} 
void *smokeri(void *i)
{ 
	while(1)       
	{
		sleep(1); 
		sem_wait(&mutex); 
		if(mutex_used==0)            
		{ 
			if(generated && generated_item[0]!= (int)(size_t)i && generated_item[1]!= (int)(size_t)i) 
			{ 
				printf("smoker%d completed his smoking\n",(int)(size_t)i); 
				mutex_used=1; 
				generated=0;                     
			}             
		} 
		sem_post(&mutex);       
	} 
} 

int main() 
{ 
	pthread_t smkr0,smkr1,smkr2,agnt; 
	sem_init(&mutex,0,1); 
	pthread_create(&agnt,0,agent,0); 
	pthread_create(&smkr0,0,smokeri,0); 
	pthread_create(&smkr1,0,smokeri,1); 
	pthread_create(&smkr2,0,smokeri,2); 
	while(running);
	return 0;
}
