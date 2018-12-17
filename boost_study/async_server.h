#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::asio;
using namespace std;
class async_server
{
	typedef boost::shared_ptr<ip::tcp::socket> sock_sp;
public:
	async_server(io_service &io) : ios(io),
		acceptor(ios, ip::tcp::endpoint(ip::tcp::v4(), 6688))
	{
		start();
	}

	void start()
	{
		std::cout << "begin accept." << std::endl;
		sock_sp sock(new ip::tcp::socket(ios));
		acceptor.async_accept(*sock, boost::bind(&async_server::accept_handler, this, boost::asio::placeholders::error, sock));
		cout << "finish accept." << endl;
	}
	
	void accept_handler(const boost::system::error_code &e, sock_sp sock)
	{
		if (e)
		{
			cout << e.message() << endl;
			return;
		}
		cout << "client:" << endl;
		cout << sock->remote_endpoint().address() << ":" << sock->remote_endpoint().port() << endl;
		cout << "begin write" << endl;
		sock->async_write_some(buffer("hello asio"), bind(&async_server::write_handler, this, boost::asio::placeholders::error));
		cout << "Finish write" << endl;

		start();
	}

	void write_handler(const boost::system::error_code &)
	{
		cout << "send msg complete." << endl;
	}
private:
	io_service &ios;
	ip::tcp::acceptor acceptor;
	
};

int main()
{
	try
	{
		cout << "server start." << endl;
		io_service ios;
		async_server serv(ios);
		ios.run();
	}
	catch (const std::exception& e)
	{
		cout << e.what() << endl;
	}
	return 0;
}