struct LNode
{
	int		nVal;
	LNode*	next;
};

void delNode(LNode* &pHead, LNode* pDelNode)
{
	if (pHead == NULL || pDelNode == NULL)
	{
		return;
	}

	if (pDelNode->next != NULL)
	{
		LNode* pNode = pDelNode->next;
		pDelNode->nVal = pNode->nVal;
		pDelNode->next = pNode->next;
		delete pNode;
		pNode = NULL;
	}
	else if (pDelNode == pHead)
	{
		delete pDelNode;
		pDelNode = NULL;
		pHead = NULL;
	}
	else
	{
		LNode* pPreNode = pHead;
		while (pPreNode->next != pDelNode)
		{
			pPreNode = pPreNode->next;
		}
		pPreNode->next = NULL;
		delete pDelNode;
		pDelNode = NULL;
	}
}

void RemoveNode(LNode** pHead, int value)
{
	if (pHead == NULL || *pHead == NULL)
	{
		return;
	}
	LNode* pDelNode = NULL;
	if ((*pHead)->nVal== value)
	{
		pDelNode = *pHead;
		*pHead = (*pHead)->next;
	}
	else
	{
		LNode* pNode = *pHead;
		while (pNode->next != NULL && pNode->next->nVal != value)
			pNode = pNode->next;
		if (pNode->next != NULL && pNode->next->nVal == value)
		{
			pDelNode = pNode->next;
			pNode->next = pNode->next->next;
		}
	}
	if (pDelNode != NULL)
	{
		delete pDelNode;
		pDelNode = NULL;
	}
}

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
		pNewNode->nVal = m;
		pNewNode->next = NULL;
		*pNode = pNewNode;
		pNode = &(pNewNode->next);
	}
}

void PrintList(LNode* pHead)
{
	while (pHead != NULL)
	{
		cout << pHead->nVal << " ";
		pHead = pHead->next;
	}
	cout << endl;
}

int main()
{
	LNode* pHead;
	CreateList(&pHead);
	PrintList(pHead);
	delNode(pHead, pHead);
	PrintList(pHead);
	return 0;
}