#include <iostream>
#include <atomic>
#include <thread>
#include <condition_variable>

class PingPong
{
public:
	static constexpr std::size_t MAX = 3;

	void ping()
	{
		std::unique_lock<std::mutex> lock(m_);
		while (count_.load() < MAX)
		{
			cv_.wait(lock, [&pingPrinted = pingPrinted]
					 { return !pingPrinted; });
			std::cout << "Ping" << std::endl;
			count_++;
			pingPrinted = true;
			cv_.notify_one();
		}
	}

	void pong()
	{
		std::unique_lock<std::mutex> lock(m_);
		while (count_.load() < MAX)
		{
			cv_.wait(lock, [&pingPrinted = pingPrinted]
					 { return pingPrinted; });
			std::cout << "Pong" << std::endl;
			count_++;
			pingPrinted = false;
			cv_.notify_one();
		}
	}

private:
	bool pingPrinted = false;
	std::atomic<std::size_t> count_;
	std::mutex m_;
	std::condition_variable cv_;
};

int main()
{
	PingPong p;
	std::thread pingThread(&PingPong::ping, &p);
	std::thread pongThread(&PingPong::pong, &p);

	pingThread.join();
	pongThread.join();
}