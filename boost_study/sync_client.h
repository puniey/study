#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::asio;
using namespace std;

void sync_client(io_service &ios)
{
	try
	{
		cout << "client start." << endl;

		ip::tcp::socket sock(ios);
		ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 6688);
		sock.connect(ep);
		cout << "conncet to " << sock.remote_endpoint().address() << ":" << sock.remote_endpoint().port() << endl;
		boost::this_thread::sleep_for(boost::chrono::milliseconds(3000));
		cout << "begin read" << endl;
		vector<char> str(100, 0);
		sock.read_some(buffer(str));
		cout << "finish read" << endl;
		cout << &str[0] << endl;
	}
	catch (std::exception &e)
	{
		cout << e.what() << endl;
	}
}