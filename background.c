#include "background.h"

#include "cc1111.h"

#define MAX_TASKS	8

static task_t tasks[MAX_TASKS];

static int task_count;

void add_background_task(task_t t)
{
	tasks[task_count++] = t;
}

void run_background_tasks(void)
{
	int i;

	for (i = 0; i < task_count; ++i) {
		task_t t = tasks[i];
		(*t)();
	}
	PCON = PCON_IDLE;
}
