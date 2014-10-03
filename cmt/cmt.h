/**

Jianing Chen
j.n.chen@sheffield.ac.uk

==== Simple Cooperative Multitasking ====

**/
#ifndef SIMPLE_COOPERATIVE_MULTITASKING
#define SIMPLE_COOPERATIVE_MULTITASKING

#include <stdlib.h>
#include <setjmp.h>

#define CMT_STATE_VACANT    0
#define CMT_STATE_LAUNCH    1
#define CMT_STATE_PAUSE     2
#define CMT_STATE_RUNNING   3

#ifdef	__cplusplus
extern "C" {
#endif



typedef struct CMT_INI{
    unsigned int MaxNumberOfProcess;
    unsigned int ProcessStackSzie;
    unsigned int ProcessStackOffset;
} cmt_ini;

typedef void (*cmt_function)(void*);
typedef signed long cmt_timer;// must be signed, do not change.
typedef long cmt_time;


void cmt_initialize(const cmt_ini* setting);
int cmt_launch_process(cmt_function f,void*f_data);// return -1 if failed
int cmt_launch_process_delay(cmt_function f,void*f_data,cmt_time time);
void cmt_cooperate();
void cmt_wait(cmt_time time);
void cmt_main_routine();
void cmt_process_timers(cmt_time time_elapsed);
int cmt_get_current_process_index();// return -1 if not in a process
int cmt_get_number_of_process();
void cmt_terminate();




#ifdef	__cplusplus
}
#endif

#endif
