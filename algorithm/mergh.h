//两个有序序列合并 到第一个序列 时间复杂度O(n)
void merge(vector<int>& v1, vector<int>& v2)
{
	int size1 = v1.size();
	int size2 = v2.size();
	int totalsize = size1 + size2;
	v1.resize(totalsize);
	int i = size1 - 1, j = size2 - 1;
	for ( ; i >= 0 && j >= 0 && totalsize >= 0;)
	{
		if (v1[i] > v2[j])
		{
			v1[--totalsize] = v1[i];
			--i;
		}
		else
		{
			v1[--totalsize] = v2[j];
			--j;
		}
	}
	if (j >= 0 && totalsize > 0)
	{
		while (j >= 0 && totalsize >= 0)
		{
			v1[--totalsize] = v2[j--];
		}
	}
}