//一个数二进制中1的个数
int NumberOf1(int num)
{
	int count = 0;
	int n = 1;
	while (n)
	{
		if (num & n)
		{
			++count;
		}
		n = n << 1;
	}
	return count;
}

int NumberOf1_1(int num)
{
	int count = 0;
	while (num)
	{
		num = num & (num - 1);
		++count;
	}
	return count;
}