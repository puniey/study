//共享读 独占写
class CRWData
{
public:
	CRWData() : m_nData(0) {};
	void write()
	{
		boost::unique_lock<boost::shared_mutex> lock(rw_mu);
		++m_nData;
		{
			boost::mutex::scoped_lock io_lock(gMutex);
			std::cout << "thread " << boost::this_thread::get_id() << " write data " << m_nData << std::endl;
		}
	}
	void read(int *x)
	{
		boost::shared_lock<boost::shared_mutex> lock(rw_mu);
		*x = m_nData;
		{
			boost::mutex::scoped_lock io_lock(gMutex);
			io_lock.lock();
			std::cout << "thread " << boost::this_thread::get_id() << " read data " << m_nData << std::endl;
		}
	}
private:
	int					m_nData;
	boost::shared_mutex	rw_mu;
};