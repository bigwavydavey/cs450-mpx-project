
struct queue
{
  int count;
  struct pcb *head;
  struct pcb *tail;
};

struct pcb
{
	char name[100];
	int class;
	int priority;
	// 0- ready not suspended, 1- ready suspended, 2- blocked non suspended, 3- blcoked suspended, 5- running
	int state;
	unsigned char stack[1024];
	unsigned char *top;
	unsigned char *base;
	struct pcb *next;
	struct pcb *prev;
};