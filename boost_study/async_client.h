#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::asio;
using namespace std;

class async_client
{
	typedef boost::shared_ptr<ip::tcp::socket> sock_sp;
public:
	async_client(io_service &io) : ios(io),
		ep(ip::address::from_string("127.0.0.1"), 6688)
	{
		start();
	}

	void start()
	{
		sock_sp sock(new ip::tcp::socket(ios));
		cout << "begin connect" << endl;
		sock->async_connect(ep, boost::bind(&async_client::conn_handler, this, boost::asio::placeholders::error, sock));
	}

	void conn_handler(const boost::system::error_code& ec, sock_sp sock)
	{
		if (ec)
		{
			cout << ec.message() << endl;
			return;
		}
		cout << "receive from " << sock->remote_endpoint().address() << ":" << sock->remote_endpoint().port() << endl;
		boost::shared_ptr<vector<char> > str(new vector<char>(100, 0));

		sock->async_read_some(buffer(*str), boost::bind(&async_client::read_handler, this, boost::asio::placeholders::error, str));
		boost::this_thread::sleep_for(boost::chrono::seconds(2));
		start();
	}

	void read_handler(const boost::system::error_code &ec, boost::shared_ptr<vector<char> > str)
	{
		if (ec)
		{
			cout << ec.message() << endl;
			return;
		}
		cout << &(*str)[0] << endl;
	}
	
private:
	io_service &ios;
	ip::tcp::endpoint ep;
};

int main()
{
	cout << "client start" << endl;
	io_service ios;
	async_client cli(ios);
	ios.run();
}