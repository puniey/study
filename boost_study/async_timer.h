#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class async_timer
{
public:
	template <typename F>
	async_timer(boost::asio::io_service &ios, int x, F func) : 
		count(0), max_count(x), f(func), t(ios, boost::posix_time::milliseconds(500))
	{
		t.async_wait(boost::bind(&async_timer::call_func, this, boost::asio::placeholders::error));
	}

	void call_func(const boost::system::error_code&)
	{
		if (count >= max_count)
		{
			std::cout << "max call_func" << std::endl;
			return;
		}
		++count;
		f();
		t.expires_at(t.expires_at() + boost::posix_time::milliseconds(500));
		t.async_wait(boost::bind(&async_timer::call_func, this, boost::asio::placeholders::error));
	}
private:
	int count, max_count;
	boost::function<void()> f;
	boost::asio::deadline_timer t;
};