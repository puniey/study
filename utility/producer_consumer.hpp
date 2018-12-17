//生产者和消费者模式的后进先出型缓冲区
#include <boost/thread.hpp>
#include <stack>

boost::mutex gMutex; //全局互斥量 用于同步IO
class CBuffer
{
public:
	CBuffer(size_t n) : nUnread(0), nCapacity(n) {}
	void put(const int &x)
	{
		{
			boost::mutex::scoped_lock lock(mu);
			while (is_full())
			{
				{
					boost::mutex::scoped_lock io_lock(gMutex);
					std::cout << "buffer is full,waiting....." << std::endl;
				}
				put_cond.wait(mu);
			}
			buf.push(x);
			++nUnread;
			{
				boost::mutex::scoped_lock io_lock(gMutex);
				std::cout << "put " << x << std::endl;
			}
		}
		get_cond.notify_one();
	}

	void get(int *x)
	{
		{
			boost::mutex::scoped_lock lock(mu);
			while (is_empty())
			{
				{
					boost::mutex::scoped_lock io_lock(gMutex);
					std::cout << "buffer is empty,waiting..." << std::endl;
				}
				get_cond.wait(mu);
			}
			*x = buf.top();
			buf.pop();
			--nUnread;
			{
				boost::mutex::scoped_lock lock(gMutex);
				std::cout << "get " << *x << std::endl;
			}
		}
		put_cond.notify_one();
	}

private:
	bool is_full() { return nUnread == nCapacity; }
	bool is_empty() { return nUnread == 0; }

private:
	boost::mutex mu;                        //互斥量,配合条件变量使用
	boost::condition_variable_any put_cond; //写入条件变量
	boost::condition_variable_any get_cond; //读取条件变量
	std::stack<int> buf;                    //缓冲区对象
	int nUnread;
	int nCapacity;

};