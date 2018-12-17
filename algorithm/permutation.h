void Permutation(char* str, char* pBegin);
void Permutation(char* str)
{
	if (str == NULL)
		return;
	Permutation(str, str);
}

void Permutation(char* str, char* pBegin)
{
	if (*pBegin == '\0')
	{
		cout << str << endl;
	}
	else
	{
		for (char* pCh = pBegin; *pCh != '\0'; ++pCh)
		{
			std::swap(*pCh, *pBegin);
			Permutation(str, pBegin + 1);
			std::swap(*pCh, *pBegin);
		}
	}
}

int main()
{
	char buf[10] = "abc";
	Permutation(buf);
	return 0;
}