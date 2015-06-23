#ifndef _BACKGROUND_H
#define _BACKGROUND_H

typedef void (*task_t)(void);

void add_background_task(task_t);

void run_background_tasks(void);

#endif /* _BACKGROUND_H */
