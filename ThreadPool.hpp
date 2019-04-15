#ifndef _THREAD_POOL_HPP_
#define _THREAD_POOL_HPP_

#include <iostream>
#include <queue>
#include <pthread.h>
#include "Log.hpp"

typedef int (*handler_t)(int);	//定义一个函数指针
class Task	//任务
{
	private:
		int sock;
		handler_t handler;	//处理socket
	public:
		Task()
		{
			sock = -1;
			handler = NULL;
		}	
		void SetTask(int sock_,handler_t handler_)
		{
			sock = sock_;
			handler = handler_;
		}
		void Run()
		{
			handler(sock);	
		}
		~Task()
		{}
};

#define NUM 5

class ThreadPool
{
	private:
		int thread_total_num;	//线程总数
		int thread_idle_num;	//任务队列中没有任务，代表正在休眠的任务
		std::queue<Task> task_queue;	//任务队列：一个一个的任务task
		pthread_mutex_t lock;	//保护任务队列的锁
		pthread_cond_t cond;	//条件变量
		volatile bool is_quit;
	public:
		void LockQueue()
		{
			pthread_mutex_lock(&lock);
		}		
		void UnlockQueue()
		{
			pthread_mutex_unlock(&lock);
		}
		bool IsEmpty()
		{
			return task_queue.size() == 0;
		}
		void ThreadIdle()
		{
			if(is_quit)
			{
				UnlockQueue();
				thread_total_num--;
				LOG(INFO,"thread quit...");
				pthread_exit((void *)0);
			}
			thread_idle_num++;
			pthread_cond_wait(&cond,&lock);
			thread_idle_num--;
		}
		void WakeupOneThread()
		{
			pthread_cond_signal(&cond);
		}
		void WakeAllThread()
		{	
			pthread_cond_broadcast(&cond);
		}
		static void *thread_routine(void *arg)
		{
			ThreadPool *tp = (ThreadPool*)arg;
			pthread_detach(pthread_self());
			
			for(;;)
			{
				tp->LockQueue();
				while(tp->IsEmpty())
				{
					tp->ThreadIdle();
				}
				Task t;
				tp->PopTask(t);
				tp->UnlockQueue();
				LOG(INFO,"task has be taken,handler...");
				std::cout << "thread id is : " <<pthread_self() << std::endl;
				t.Run();
			}	
		}
		public:
			ThreadPool(int num_ = NUM):
				thread_total_num(num_),thread_idle_num(0),is_quit(false)
			{
				pthread_mutex_init(&lock,NULL);
				pthread_cond_init(&cond,NULL);
			}
			void initThreadPool()
			{
				int i_ = 0;
				for(;i_ < thread_total_num;i_++)
				{
					pthread_t tid;	
					pthread_create(&tid,NULL,thread_routine,this);
				}
			}
			void PushTask(Task &t_)
			{
				LockQueue();	
				if(is_quit)
				{
					UnlockQueue();	//线程退出了，所有人不能往队列里添加任务
					return;
				}
				task_queue.push(t_);
				WakeupOneThread();
				UnlockQueue();
			}	
			void PopTask(Task &t_)
			{
				t_ = task_queue.front();
				task_queue.pop();
			}
			void Stop()
			{
				LockQueue();
				is_quit = true;
				UnlockQueue();
				
				while(thread_idle_num > 0)
				{
					WakeAllThread();
				}
			}
			
			~ThreadPool()
			{
				pthread_mutex_destroy(&lock);
				pthread_cond_destroy(&cond);
			}
};


#endif
