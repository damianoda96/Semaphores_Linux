#include<stdio.h> 
#include<sys/types.h> 
#include<semaphore.h>
#include<pthread.h>
#include<unistd.h>

int is_tobacco = 0, is_paper = 0, is_match = 0;

int smoke_counter = 0;

sem_t mutex, agent_sem, tobacco, paper, match, to_sem, pa_sem, ma_sem;

// ######_____________ AGENT THREADS _______________#####

void *agent0(void *arg)
{
	while(1)
	{
		printf("Agent0 puts TOBACCO and PAPER on table...\n");
		sleep(3);
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
		sleep(3);
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
		sleep(3);
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

void *tobacco_smoker(void *arg)
{
	while(1)
	{
		sem_wait(&to_sem);
		printf("Smoker %d is MAKING a cigarette...\n", (int)(size_t)arg);
		sleep(2);
		sem_post(&agent_sem);
		printf("Smoker %d is SMOKING a cigarette...\n", (int)(size_t)arg);
		sleep(2);
		smoke_counter+=1;
	}
}

void *paper_smoker(void *arg)
{
	while(1)
	{
		sem_wait(&pa_sem);
		printf("Smoker %d is MAKING a cigarette...\n", (int)(size_t)arg);
		sleep(2);
		sem_post(&agent_sem);
		printf("Smoker %d is SMOKING a cigarette...\n", (int)(size_t)arg);
		sleep(2);
		smoke_counter+=1;
	}
}

void *match_smoker(void *arg)
{
	while(1)
	{
		sem_wait(&ma_sem);
		printf("Smoker %d is MAKING a cigarette...\n", (int)(size_t)arg);
		sleep(2);
		sem_post(&agent_sem);
		printf("Smoker %d is SMOKING a cigarette...\n", (int)(size_t)arg);
		sleep(2);
		smoke_counter+=1;
	}
}

// ######________________ MAIN __________________######

int main() 
{ 
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

	pthread_create(&t_smoker1, 0, tobacco_smoker, 1);
	pthread_create(&t_smoker2, 0, tobacco_smoker, 2);

	pthread_create(&p_smoker1, 0, paper_smoker, 3);
	pthread_create(&p_smoker2, 0, paper_smoker, 4);

	pthread_create(&m_smoker1, 0, match_smoker, 5);
	pthread_create(&m_smoker2, 0, match_smoker, 6);

	while(smoke_counter < 18);

	return 0;
}
