#include <iostream>
#include <exception>
#include <pthread.h>
#include <semaphore.h>

#ifndef LOCKER_H
#define LOCKER_H

class sem {
private:
	sem_t m_sem;
public:
	sem() {
		if (sem_init(&m_sem, 0, 0) != 0) {
			throw::std::exception();
		}
	}
	sem(int num)//num为资源数
    {
        if (sem_init(&m_sem, 0, num) != 0)
        {
            throw std::exception();
        }
    }
	
	~sem() {
		sem_destroy(&m_sem);
	}

	bool wait() {
		return sem_wait(&m_sem) == 0;
	} 

	bool post() {
		return sem_post(&m_sem) == 0;
	}
};

class locker {
public:
	//创建并销毁互斥锁
	locker() {
		if (pthread_mutex_init(&m_mutex, NULL) != 0) {
			throw std::exception();
		}
	}
	
	//销毁互斥锁
	~locker() {
		pthread_mutex_destroy(&m_mutex);
	}

	//获取互斥锁
	bool lock() {
		return pthread_mutex_lock(&m_mutex) == 0;
	}

	//释放互斥锁
	bool unlock() {
		return pthread_mutex_unlock(&m_mutex) == 0;
	}
private:
	pthread_mutex_t m_mutex;
};


class cond {

public:
	//创建并初始化条件变量
	cond() {
		if (pthread_mutex_init(&m_mutex, NULL) !=0 ) {
			throw std::exception();
		}
		if (pthread_cond_init(&m_cond, NULL) != 0) {
			//构造函数出现问题，释放已经分配的资源
			pthread_mutex_destroy(&m_mutex);
			throw std::exception();
		}
	}

	//销毁条件变量
	~cond() {
		pthread_mutex_destroy(&m_mutex);
		pthread_cond_destroy(&m_cond);
	}

	//等待条件变量
	bool wait() {
		int ret = 0;
		pthread_mutex_lock(&m_mutex);
		ret = pthread_cond_wait(&m_cond, &m_mutex);
		pthread_mutex_unlock(&m_mutex);
		return ret == 0;
	}

	//唤醒等待条件变量的线程
	bool signal() {
		return pthread_cond_signal(&m_cond) == 0;
	}
private:
	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond;
};


// class LockMap {
// 	LockMap()

// private:
// 	unordered_map<int, int> mp; 	//userId -> sockfd
// 	locker lock;
// };

#endif