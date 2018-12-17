//各类排序算法总结
//1.简单选择排序
//时间复杂度 O(n^2) 空间复杂度 O(1) 不稳定
template <typename T>
void SimpleSelectSort(vector<T> &data)
{
	if (data.empty() || 1 == data.size())
		return;

	for (size_t i = 0; i < data.size() - 1; ++i)
	{
		size_t min_index = i;
		for (size_t j = i + 1; j < data.size(); ++j)
		{
			if (data[j] < data[min_index])
				min_index = j;
		}

		if (min_index != i)
		{
			std::swap(data[i], data[min_index]);
		}
	}	
}
//直接插入排序
//时间复杂度 O(n^2) 空间复杂度 O(1) 稳定
//在部分有序的数据中排序效果效好 优化版本为希尔排序
template <typename T>
void DirectInsertSort(vector<T> &data)
{
	if (data.empty() || 1 == data.size())
		return;

	for (size_t i = 1; i < data.size(); ++i)
	{
		if (data[i] >= data[i - 1])
			continue;

		T v = data[i];
		int j;
		for (j = i - 1; j >= 0; --j)
		{
			if (v < data[j])
			{
				data[j + 1] = data[j];
			}
			else
			{
				break;
			}
		}
		//if (j != i - 1)
			data[j + 1] = v;
	}
}
//冒泡排序
//时间复杂度 O(n^2) 空间复杂度 O(1) 稳定
template <typename T>
void BubbleSort(vector<T> &data)
{
	if (data.empty() || 1 == data.size())
		return;
	bool flag = false;
	for (size_t i = 0; i < data.size() - 1; ++i)
	{
		flag = false;
		for (size_t j = 0; j < data.size() - i - 1; ++j)
		{
			if (data[j] > data[j + 1])
			{
				std::swap(data[j], data[j + 1]);
				flag = true;
			}
		}
		if (flag == false) //本次循环没有发生交换说明已经排序完毕
			break;
	}
}
//希尔排序
//时间复杂度 O(nlogn) 空间复杂度 O(1) 不稳定
//缩小增量排序 直接插入排序的优化 增量会影响排序效果 减少了直接插入排序的比较数据对 减少无意义比较
template <typename T>
void ShellSort(vector<T> &data)
{
	if (data.empty() || 1 == data.size())
		return;

	for (size_t gap = data.size() / 2; gap > 0; gap /= 2)
	{
		for (size_t j  = gap; j < data.size(); ++j)
		{
			if (data[j] >= data[j - gap])
				continue;
				
			T v = data[j];
			int k;
			for (k = j - gap; k >= 0; k -= gap)
			{
				if (v < data[k])
					data[k + gap] = data[k];
				else
					break;
			}
			//if (k != j - gap)
				data[k + gap] = v;
		}
	}
}
//快速排序
//时间复杂度 O(nlogn) 空间复杂度 O(logn) 不稳定
//冒泡排序的一种改进
template <typename T>
void QuickSort(vector<T> &data, int low, int high)
{
	if (data.empty() || 1 == data.size())
		return;

	if (low >= high)
		return;

	int first = low;
	int last = high;
	const T &key = data[first];

	while (first < last)
	{
		while (first < last && data[last] >= key)
			--last;
		data[first] = data[last];
		while (first < last && data[first] < key)
			++first;
		data[last] = data[first];
	}

	data[first] = key;
	QuickSort(data, low, first - 1);
	QuickSort(data, first + 1, high);
}
//堆排序
//时间复杂度 O(nlogn) 空间复杂度 O(1) 不稳定
//堆调整
template <typename T>
void HeapAdjust(vector<T> &data, int i, int length)
{
	int nChild;
	for (; 2 * i + 1 < length; i = nChild)
	{
		nChild = 2 * i + 1;
		if (nChild < length - 1 && data[nChild + 1] > data[nChild]) //存在右子树
		{
			++nChild;
		}
		if (data[nChild] > data[i])
		{
			std::swap(data[nChild], data[i]);
		}
		else
		{
			break;
		}
	}
}
//堆排序
template <typename T>
void HeapSort(vector<T> &data)
{
	if (data.empty() || 1 == data.size())
		return;

	for (int i = data.size() / 2 - 1; i >= 0; --i)
	{
		HeapAdjust(data, i, data.size());
	}

	for (int i = data.size() - 1; i > 0; --i)
	{
		std::swap(data[0], data[i]);
		HeapAdjust(data, 0, i);
	}
}