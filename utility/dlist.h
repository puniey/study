#ifndef _H_D_LIST_H_
#define _H_D_LIST_H_

#include <stddef.h>
#include <assert.h> 
#include <stdint.h>

struct dlist final
{
	struct dlist *next;
	struct dlist *prev;
	int num;
	dlist(const dlist&) = delete;
	dlist& operator=(const dlist&) = delete; 
	dlist()
	{
		num = 0;
		next = prev = this;
#ifndef NDEBUG
		dlist_addr = (uintptr_t)this;
#endif
	}

	~dlist(void)
	{
		next = prev = NULL;
#ifndef NDEBUG
		dlist_addr = (uintptr_t)0;
#endif
	}
#ifndef NDEBUG 
	uintptr_t dlist_addr; 
#endif
};

#ifndef NDEBUG
#define debug_list_init(a)				((a)->dlist_addr = (uintptr_t)(a))
#define debug_list_test(a, node)		(assert((uintptr_t)(a) == (node)->dlist_addr))
#define debug_list_value(node, value)	((node)->dlist_addr = (uintptr_t)(value))
#else
#define debug_list_init(a)				((void) 0)
#define debug_list_test(a, node)		((void) 0)
#define debug_list_value(node, value)	((void) 0)
#endif

/*ptr is mem address*/
#define list_enter(ptr, type, mem)		((type *)((char *)ptr - ((uintptr_t) &((type *)0)->mem)))

static inline void dlist_pushback(struct dlist *head, struct dlist *newnode) {
	assert(newnode != NULL);
	newnode->prev = head->prev;
	newnode->next = head;
	head->prev->next = newnode;
	head->prev = newnode;
	head->num++;
	debug_list_value(newnode, head);
}

static inline void dlist_pushfront(struct dlist *head, struct dlist *newnode) {
	assert(newnode != NULL);
	newnode->prev = head;
	newnode->next = head->next;
	head->next->prev = newnode;
	head->next = newnode;
	head->num++;
	debug_list_value(newnode, head);
}

static inline void dlist_remove(struct dlist *head, struct dlist *node) {
	assert(node != NULL);
	debug_list_test(head, node);
	node->prev->next = node->next;
	node->next->prev = node->prev;
	head->num--;

	node->next = node;
	node->prev = node;
	debug_list_value(node, 0);
}

static inline struct dlist *dlist_popfront(struct dlist *head) {
	struct dlist *node;
	if (head == head->next)
		return NULL;
	node = head->next;
	dlist_remove(head, node);
	return node;
}

static inline struct dlist *dlist_popback(struct dlist *head) {
	struct dlist *node;
	if (head == head->prev)
		return NULL;
	node = head->prev;
	dlist_remove(head, node);
	return node;
}

static inline bool dlist_empty(struct dlist *head) {
	assert(head != NULL);
	assert(head->next != NULL);
	assert(head->prev != NULL);
	return (head == head->next);
}

static inline bool dlist_isvalid(struct dlist *head, struct dlist *node) {
	assert(head != NULL);
	assert(node != NULL);
	debug_list_test(head, node);
	return (head != node);
}
#endif