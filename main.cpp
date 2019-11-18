#include <iostream>
#include <thread>
#include <map>
#include <chrono>
#include <cmath>
#include <string>
#include <random>


#define NUMBER_THREAD 10
typedef std::thread::id PIDType;

typedef class TestTimer {
private:
	typedef struct TestTimerItem {
		long begin_time = 0;							
		long total_time = 0;
		long total_buffered_time = 0;
		long total_frame_num = 0;
		long total_sequence_num = 0;
	} TestTimerItem;
private:
	std::map<PIDType, TestTimerItem> timers;
	int thread_count;
	long total_time;
	long total_frame_count;
	long total_sequence_count;
	float total_buffered_time;
	float avg_buffered_time;
	float rtf_frame;
	float rtf_sequence;
public:
	TestTimer() = default;									// 构造函数
	~TestTimer() = default;									// 析构函数
	
	void init();											// 初始化Timer
	void clear();											// 清除所有timeritem
	bool creteTimer(PIDType pid);							// 针对某个pid创建timeritem
	void total_time_begin(PIDType pid);						// 某个timerbegin
	void total_time_acc(PIDType pid);						// 某个timer计数
	void total_buffer_acc(PIDType pid);						// 某个timer计数
	void report(PIDType pid);								// 针对某个thread进行汇报
	void report();											// 总体汇报
	void total_seq_num_add(PIDType pid,long number);			// 
	void total_frame_num_add(PIDType pid, long number);			//
}TestTimer, *pTestTimer;

void func(int i, pTestTimer timer, PIDType pid) {
	for (int j = 0; j < i; j++) {
		timer->total_time_begin(pid);
		for (int x = 1; x < 100000; x++) {
			std::log10(x);
		}
		timer->total_time_acc(pid);
		if (j == i - 1) {
			timer->total_buffer_acc(pid);
		}
	}
}

int get_random_number() {
	std::random_device rd;
	std::default_random_engine e(rd());
	std::uniform_int_distribution<> u(100, 1000);
	return u(e);
}

long get_time_now() {
	std::chrono::steady_clock::duration d = std::chrono::steady_clock::now().time_since_epoch();
	std::chrono::milliseconds milli = std::chrono::duration_cast<std::chrono::milliseconds>(d);
	return (long)milli.count();
}
	
void thread_function(pTestTimer timer) {
	 PIDType pid = std::this_thread::get_id();
	 timer->creteTimer(pid);
	 int x = get_random_number();
	 timer->total_frame_num_add(pid, (long)x);
	 timer->total_seq_num_add(pid, (long)x);
	 func(get_random_number(), timer, pid);
}


/* ---------------- Main Function ----------------*/
int main() {
	
	// Set the Timer
	TestTimer timer;
	timer.init();
	std::thread threads[NUMBER_THREAD];
	for (int i = 0; i < NUMBER_THREAD; i++) {
		threads[i] = std::thread(thread_function, &timer);
	}
	for (int i = 0; i < NUMBER_THREAD; i++)
	{
		threads[i].join();
	}
	timer.report();
	timer.clear();
	timer.~TestTimer();
	int x; 
	std::cin >> x;
	return 0;
}

/* ---------------- TestTimer Implementation ----------------*/
void TestTimer::init() {
	thread_count = 0;
	total_time = 0;
	total_frame_count = 0;
	total_sequence_count = 0;
	total_buffered_time = 0;
	avg_buffered_time = 0;
	rtf_frame = 0;
	rtf_sequence = 0;
}

void TestTimer::clear() {
	timers.clear();
}


void TestTimer::total_frame_num_add(PIDType pid, long number) {
	timers[pid].total_frame_num += number;
}

void TestTimer::total_seq_num_add(PIDType pid, long number) {
	timers[pid].total_sequence_num += number;
}

bool TestTimer::creteTimer(PIDType pid) {
	TestTimerItem item;
	timers.insert(std::pair<PIDType, TestTimerItem>(pid, item));
	thread_count++;
	return true;
}

void TestTimer::total_time_begin(PIDType pid) {
	timers[pid].begin_time = get_time_now();
}

void TestTimer::total_time_acc(PIDType pid) {
	timers[pid].total_time += get_time_now() - timers[pid].begin_time;
}

void TestTimer::total_buffer_acc(PIDType pid) {
	timers[pid].total_buffered_time += get_time_now() - timers[pid].begin_time;
}

void TestTimer::report(PIDType pid) {
	std::cout << "*******report******\n";
	std::cout << "thread ID:" << pid << std::endl;
}

void TestTimer::report() {
	std::cout << "*********************report********************\n";
	for (auto i : timers) {
		std::cout << "thread id:\t" << i.first << "\t"<< "time:\t" << i.second.total_time << std::endl;
		total_time += i.second.total_time;
		total_buffered_time += i.second.total_buffered_time;
		total_frame_count += i.second.total_frame_num;
		total_sequence_count += i.second.total_sequence_num;
	}
	std::cout << "\n--------sum--------" << std::endl;
	rtf_frame = (float)total_frame_count / (float)total_time;
	rtf_sequence = (float)total_sequence_count / (float)total_time;
	std::cout << "total_time:\t\t" << total_time << std::endl;
	std::cout << "total_buffered_time:\t"<< total_buffered_time<< std::endl;
	std::cout << "total_frame_count:\t" << total_frame_count<< std::endl;
	std::cout << "total_sequence_count:\t" << total_sequence_count<< std::endl;
	std::cout << "rtf_frame:\t\t" << rtf_frame<< std::endl;
	std::cout << "rtf_sequence:\t\t" << rtf_sequence << std::endl;
}