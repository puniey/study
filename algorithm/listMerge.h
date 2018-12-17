struct LNode
{
	int		m_nValue;
	LNode*	m_pNext;
};
 
void CreateList(LNode** pHead)
{
	int m;
	LNode** pNode = pHead;
	cout << "输入新链表节点：";
	while (cin >> m)
	{
		if (m == 0)
			break;
		LNode* pNewNode = new LNode;
		pNewNode->m_nValue = m;
		pNewNode->m_pNext = NULL;
		*pNode = pNewNode;
		pNode = &(pNewNode->m_pNext);
	}
}

void PrintList(LNode* pHead)
{
	while (pHead != NULL)
	{
		cout << pHead->m_nValue << " ";
		pHead = pHead->m_pNext;
	}
	cout << endl;
}

LNode* Merge(LNode* &pHead1, LNode* &pHead2)
{
	if (!pHead1)
	{
		return pHead2;
	}
	else if (!pHead2)
	{
		return pHead1;
	}

	LNode* pMerge;
	if (pHead1->m_nValue < pHead2->m_nValue)
	{
		pMerge = pHead1;
	}
	else
	{
		pMerge = pHead2;
	}

	while (pHead1 && pHead2)
	{
		if (pHead1->m_nValue < pHead2->m_nValue)
		{
			LNode* pNext = pHead1->m_pNext;
			pHead1->m_pNext = pHead2;
			pHead1 = pNext;
		}
		else
		{
			LNode* pNext = pHead2->m_pNext;
			pHead2->m_pNext = pHead1;
			pHead2 = pNext;
		}
	}
	return pMerge;
}

int main()
{
	LNode *pHead1 = NULL, *pHead2 = NULL;
	CreateList(&pHead1);
	PrintList(pHead1);
	CreateList(&pHead2);
	PrintList(pHead2);
	cout << "合并后：";
	PrintList(Merge(pHead1, pHead2));
	system("pause");
	return 0;
}