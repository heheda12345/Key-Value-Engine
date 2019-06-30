#ifndef __RW_LOCK__
#define __RW_LOCK__
 
#include <mutex>
#include <condition_variable>
 
class RWLock
{
public:
	RWLock() = default;
	~RWLock() = default;
public:
	void lock_read() {
		std::unique_lock<std::mutex> ulk(counter_mutex);
		cond_r.wait(ulk, [=]()->bool {return write_cnt == 0; });
		++read_cnt;
	}
	void lock_write() {
		std::unique_lock<std::mutex> ulk(counter_mutex);
		++write_cnt;
		cond_w.wait(ulk, [=]()->bool {return read_cnt == 0; });
		inwriteflag = true;
	}
	void release_read() {
		std::unique_lock<std::mutex> ulk(counter_mutex);
		if (--read_cnt == 0) {
			cond_w.notify_all();
		}
	}
	void release_write() {
		std::unique_lock<std::mutex> ulk(counter_mutex);
		if (--write_cnt == 0) {
			cond_r.notify_all();
		}
	}
	
private:
	volatile size_t read_cnt{ 0 };
	volatile size_t write_cnt{ 0 };
	volatile bool inwriteflag{ false };
	std::mutex counter_mutex;
	std::condition_variable cond_w;
	std::condition_variable cond_r;
};
 
 #endif