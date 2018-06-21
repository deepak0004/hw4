#include <iostream>
#include <memory>
#include <unistd.h>
using namespace std;

// compile: g++ -std=c++1y -O3 unique.cpp -o unique
static int numinserts = 1000;

struct node 
{
	int val;
	unique_ptr<struct node> next;
};

static unsigned long long rdtsc()
{
	unsigned hi, lo;

	asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
	return (((unsigned long long)hi) << 32) | lo;
}

int delete_node(unique_ptr<struct node>& root, int idx)
{
	if (root == NULL || idx < 0)
	{
		return 0;
	}
	if (idx == 0) 
	{
		root = move(root->next);
		return 1;
	}

	unique_ptr<struct node> *rootp = &root;
		
	while (*rootp) 
	{
		if (idx == 1) 
		{
			unique_ptr<struct node> todel = move((*rootp)->next);
			if (todel)
				(*rootp)->next = move(todel->next);
			return 1;
		}
		rootp = &(*rootp)->next;
		idx--;
	}
	return 0;
}

int iterate_list(unique_ptr<struct node> &root)
{
	if (root == NULL)
		return 0;

	unique_ptr<struct node> *rootp = &root;
	int ret = 0;

	while(*rootp) 
	{
		ret++;
		rootp = &(*rootp)->next;
	}

	return ret;
}

int insert_nodes(unique_ptr<struct node> &root)
{
	unique_ptr<struct node> elem;
	elem = unique_ptr<struct node>(new(struct node));
	if (elem == NULL)
		return 0;
	elem->val = 0;
	elem->next = move(root);
	root = move(elem);

	return 1;
}

int main(int argc, const char *argv[])
{
	unique_ptr<struct node> head = NULL;
	int numelems;
	unsigned long long time;

	if( argc==2 ) 
	{
		numinserts = atoi(argv[1]);
	}
	printf("numinserts : %d\n", numinserts);
	if( numinserts<0 )
	{
		return 0;
	}

	time = rdtsc();
	for( int i=0; i<numinserts; i++) 
	{
		if( !insert_nodes(head) ) 
		{
			cout << "failed to insert node: " << i << endl;
			return 0;
		}
	}
	printf("time taken during insert: %lld ms\n", (rdtsc() - time)/2530000);

	numelems = iterate_list(head);
	if( numelems!=numinserts ) 
	{
		printf("test failed! %d %d\n", numelems, numinserts);
	}

	time = rdtsc();
	for( int i=0; i<numinserts/20; i++) 
	{
		int idx = rand()%((numinserts/20) - i);
		if( !delete_node(head, idx) ) 
		{
		   	cout << "failed to delete node : " << idx << endl;
			return 0;
		}
	}
	printf("time taken during delete: %lld ms\n", (rdtsc() - time)/2530000);

	numelems = iterate_list(head);
	if (numelems != numinserts - (numinserts/20)) 
	{
		printf("test failed! %d %d\n", numelems, numinserts);
	}

	time = rdtsc();
	head = NULL;
	printf("time taken during cleanup: %lld ms\n", (rdtsc() - time)/2530000);

	return 0;
}