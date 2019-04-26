#include<stdio.h> 
#include<sys/types.h> 
#include<semaphore.h>
#include<pthread.h>
#include<unistd.h>

int mutex_used = 1, generated_item[2], generated = 0 , running = 1;

int is_tobacco = 0, is_paper = 0, is_match = 0;

char *item[] = {"tobacco","paper","matches"}; 

sem_t mutex, agent_sem, tobacco, paper, match, to_sem, pa_sem, ma_sem;

// ######_____________ AGENT THREADS _______________#####

void *agent0(void *arg)
{
	while(1)
	{
		printf("Agent0 puts TOBACCO and PAPER on table...\n");
		sleep(2);
		sem_wait(&agent_sem);
		sem_post(&tobacco);
		sem_post(&paper);
	}
}

void *agent1(void *arg)
{
	while(1)
	{
		printf("Agent1 puts TOBACCO and MATCH on table...\n");
		sleep(2);
		sem_wait(&agent_sem);
		sem_post(&tobacco);
		sem_post(&match);
	}
}

void *agent2(void *arg)
{
	while(1)
	{
		printf("Agent2 puts PAPER and MATCH on table...\n");
		sleep(2);
		sem_wait(&agent_sem);
		sem_post(&paper);
		sem_post(&match);
	}
}

// #######_______________ PUSHERS ___________________#######

void *tobacco_pusher(void *arg)
{
	while(1)
	{
		printf("Pushin Tabacci...\n");
		//sleep(2);
		sem_wait(&tobacco);
		sem_wait(&mutex);

		if(is_paper)
		{
			is_paper = 0;
			sem_post(&ma_sem);
		}
		else if(is_match)
		{
			is_match = 0;
			sem_post(&pa_sem);
		}
		else
		{
			is_tobacco = 1;
		}
		sem_post(&mutex);
	}
}

void *paper_pusher(void *arg)
{
	while(1)
	{
		printf("Pushin papa...\n");
		//sleep(2);
		sem_wait(&paper);
		sem_wait(&mutex);

		if(is_tobacco)
		{
			is_tobacco = 0;
			sem_post(&ma_sem);
		}
		else if(is_match)
		{
			is_match = 0;
			sem_post(&to_sem);
		}
		else
		{
			is_paper = 1;
		}
		sem_post(&mutex);
	}
}

void *match_pusher(void *arg)
{
	while(1)
	{
		printf("Pushin matchy...\n");
		//sleep(2);
		sem_wait(&match);
		sem_wait(&mutex);

		if(is_tobacco)
		{
			is_tobacco = 0;
			sem_post(&pa_sem);
		}
		else if(is_paper)
		{
			is_paper = 0;
			sem_post(&to_sem);
		}
		else
		{
			is_match = 1;
		}
		sem_post(&mutex);
	}
}

// ######_____________ SMOKERS _________ ############

void *tobacco_smoker()
{
	while(1)
	{
		sem_wait(&to_sem);
		printf("Making cigarette...\n");
		sleep(2);
		sem_post(&agent_sem);
		printf("Smoking cigarette...\n");
		sleep(2);
	}
}

void *paper_smoker()
{
	while(1)
	{
		sem_wait(&pa_sem);
		printf("Making cigarette...\n");
		sleep(2);
		sem_post(&agent_sem);
		printf("Smoking cigarette...\n");
		sleep(2);
	}
}

void *match_smoker()
{
	while(1)
	{
		sem_wait(&ma_sem);
		printf("Making cigarette...\n");
		sleep(2);
		sem_post(&agent_sem);
		printf("Smoking cigarette...\n");
		sleep(2);
	}
}

// ######_____________ STARTER STUFF ________________######

void *agent(void *arg) 
{ 
	int i, j, k=0;

	while(1)       
	{ 
		sleep(2); 
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

void *smoker(void *i)
{ 
	while(1)       
	{
		sleep(2); 
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

// _________________________________________

int main() 
{ 

	// TEST:

	pthread_t agnt0, agnt1, agnt2, t_pusher, p_pusher, m_pusher;
	pthread_t t_smoker1, t_smoker2, p_smoker1, p_smoker2, m_smoker1, m_smoker2;

	sem_init(&mutex,0,1);
	sem_init(&agent_sem,0,1);

	// Agents

	pthread_create(&agnt0, 0, agent0, 0);
	pthread_create(&agnt1, 0, agent1, 0);
	pthread_create(&agnt2, 0, agent2, 0);

	// Pushers

	pthread_create(&t_pusher, 0, tobacco_pusher, 0);
	pthread_create(&p_pusher, 0, paper_pusher, 0);
	pthread_create(&m_pusher, 0, match_pusher, 0);

	// Smokers

	pthread_create(&t_smoker1, 0, tobacco_smoker, 0);
	pthread_create(&t_smoker2, 0, tobacco_smoker, 0);

	pthread_create(&p_smoker1, 0, paper_smoker, 0);
	pthread_create(&p_smoker2, 0, paper_smoker, 0);

	pthread_create(&m_smoker1, 0, match_smoker, 0);
	pthread_create(&m_smoker2, 0, match_smoker, 0);

	while(running);

	// WORKING:

	/*pthread_t smkr0,smkr1,smkr2,agnt;

	sem_init(&mutex,0,1);

	pthread_create(&agnt, 0, agent, 0); 
	pthread_create(&smkr0, 0,smoker, 0); 
	pthread_create(&smkr1, 0,smoker, 1); 
	pthread_create(&smkr2, 0,smoker, 2);

	while(running);*/

	return 0;
}
