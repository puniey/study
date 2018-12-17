//斐波那契数列
unsigned long long Fibonacci(unsigned int n)
{
	if (n == 0)
		return 0;
	if (n == 1)
		return 1;
	long long nResult1 = 0;
	long long nResult2 = 1;
	long long result = 0;
	for (unsigned int i = 2; i <= n; ++i)
	{
		result = nResult2 + nResult1;
		nResult1 = nResult2;
		nResult2 = result;
	}
	return result;
}