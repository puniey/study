#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::asio;
using namespace std;

void sync_server(io_service &ios)
{
	try
	{
		cout << "server start" << endl;
		ip::tcp::acceptor acceptor(ios, ip::tcp::endpoint(ip::tcp::v4(), 6688));
		cout << acceptor.local_endpoint().address() << endl;
		cout << acceptor.local_endpoint().port() << endl;

		while (true)
		{
			ip::tcp::socket sock(ios);
			acceptor.accept(sock);

			cout << "client:";
			cout << sock.remote_endpoint().address() << ":" << sock.remote_endpoint().port() << endl;
			sock.write_some(buffer("hello asio"));
		}
	}
	catch (std::exception &e)
	{
		cout << e.what() << endl;
	}
}