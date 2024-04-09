#pragma once
#ifndef THREADPOOL_H
#define THREADPOOL_H


#ifdef POOL_WORKER_SIZE
	#if ( POOL_WORKER_SIZE >= 1024 || POOL_WORKER_SIZE <= 0)
		#undef POOL_WORKER_SIZE
		#define POOL_WORKER_SIZE (int)std::thread::hardware_concurrency() * 8
	#endif
#else
	#define POOL_WORKER_SIZE (int)std::thread::hardware_concurrency() * 8
#endif

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define FORCE_INLINE __forceinline
#elif __linux__
	#define FORCE_INLINE inline __attribute__((always_inline))
#elif __APPLE__
	#define FORCE_INLINE inline __attribute__((always_inline))
#endif



#include <atomic>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <vector>
#include <future>
#include <set>
#include <memory>


class thread_pool {
public:
	inline thread_pool();
	inline ~thread_pool();

	template<class Fn, class ...Args>
	inline void add_work_to_queue(Fn&& fn, Args&&... args);
	inline void stop();
	inline int free_worker_count();
	inline int busy_worker_count();
	inline std::mutex* get_queue_submission_mutex();
	inline void join();
private:
	std::atomic<bool> _is_working = true;
	std::atomic<bool> _joined = false;
	std::atomic<int> _free_worker_count = POOL_WORKER_SIZE;
	std::condition_variable _work_available;
	std::mutex _queue_submission_mutex;
	std::mutex _worker_assignment_mutex;
	std::vector<std::thread> _worker_threads;
	std::queue<std::unique_ptr<std::function<void()>>> _work_queue;
};


thread_pool::thread_pool() {
	_worker_threads.reserve(POOL_WORKER_SIZE);
	
	// create the worker threads and set them to wait until work is ready to be done
	for (int i = 0; i < POOL_WORKER_SIZE; i++) {
		_worker_threads.push_back(
			std::thread([this] {
					while (true) {
						std::unique_ptr<std::function<void()>> task;
						{
							std::unique_lock<std::mutex> queue_lock(_queue_submission_mutex);

							// Wait until there is work in the queue or the thread should no longer be working.
							_work_available.wait(queue_lock, [this] {
								return !this->_work_queue.empty() || !this->_is_working.load();
								});

							if (!this->_is_working.load() && this->_work_queue.empty()) {
								break; // Exit loop if no longer working and queue is empty.
							}

							if (!this->_work_queue.empty()) {
								task = std::move(_work_queue.front());
								_work_queue.pop();
							}
						}

						// Now that we have a task and have released the lock, we can perform the task.
						if (task)
						{
							// decrement the free worker count
							_free_worker_count.fetch_sub(1);
							
							// run the task
							(*task)();
							
							// increment the free worker count
							_free_worker_count.fetch_add(1);
						}

						// if the worker shouldn't be working anymore, break out of the loop
						if (!this->_is_working.load() || this->_work_queue.empty())
							break;
					}
				}
		));
	}
}


thread_pool::~thread_pool() {
	if (!_joined.load()) {
		join();
	}
}

void thread_pool::join() {
	while(!_work_queue.empty() && _free_worker_count.load() > 0) {
		_work_available.notify_all();
	}

	if (_work_queue.empty()) {
		_is_working.store(false);
	}

	// join all the workers in the pool
	for (int i = 0; i < POOL_WORKER_SIZE; i++) {
		_worker_threads[i].join();
	}

	// set the joined status to true so we don't try to join again
	_joined.store(true);
}

template<class Fn, class ...Args>
void thread_pool::add_work_to_queue(Fn&& fn, Args&&... args) {
	{
		std::lock_guard<std::mutex> lock(_queue_submission_mutex);
		_work_queue.push(std::make_unique<std::function<void()>>([fn = std::forward<Fn>(fn), ...args = std::forward<Args>(args)]() mutable {
			std::invoke(fn, std::move(args)...);
		}));
	}
	_work_available.notify_all();
}

void thread_pool::stop() {
	join();
}

int thread_pool::free_worker_count() {
	return _free_worker_count.load();
};

int thread_pool::busy_worker_count(){
	return POOL_WORKER_SIZE - _free_worker_count.load();
};

std::mutex* thread_pool::get_queue_submission_mutex(){
	return &_queue_submission_mutex;
};

#endif // !THREADPOOL_H