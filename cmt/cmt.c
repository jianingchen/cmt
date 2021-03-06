/**

Jianing Chen
j.n.chen@sheffield.ac.uk

==== Simple Cooperative Multitasking ====

**/

#include "cmt.h"

#define CMT_STATE_VACANT    0
#define CMT_STATE_LAUNCH    1
#define CMT_STATE_PAUSE     2
#define CMT_STATE_RUNNING   3

typedef struct CMTS{
    int state;
    cmt_timer wait_timer;
    void* data_pointer;
    cmt_function function;
    jmp_buf function_context;
} cmts;

// cmt system context
unsigned int cmt_max_number_of_process;
unsigned int cmt_process_stack_size;
unsigned int cmt_process_stack_offset;
jmp_buf cmt_main_buffer;
int cmt_number_of_process;
cmts *cmt_process_list;
cmts *cmt_current_process;
int cmt_current_process_index;

void cmt_initialize(const cmt_ini*setting){
    int i;
    cmts *p;
    
    if(setting){
        cmt_max_number_of_process = setting->MaxNumberOfProcess;
        cmt_process_stack_size = setting->ProcessStackSzie;
        cmt_process_stack_offset = setting->ProcessStackOffset;
    }else{
        cmt_max_number_of_process = 32;
        cmt_process_stack_size = 8192;
        cmt_process_stack_offset = 32768;
    }
    
    cmt_process_list = (cmts*)malloc(sizeof(cmts)*cmt_max_number_of_process);
    
    for(i=0;i<cmt_max_number_of_process;i++){
        p = cmt_process_list + i;
        p->state = CMT_STATE_VACANT;
        p->wait_timer = 0;
        p->data_pointer = NULL;
        p->function = NULL;
    }
    cmt_current_process_index = -1;
    cmt_current_process = NULL;
    cmt_number_of_process = 0;
}

int cmt_launch_process_delay(cmt_function f,void*f_data,cmt_time time){
    int i;
    cmts *p;
    
    if(f==NULL){
        return -1;
    }
    
    for(i=0;i<cmt_max_number_of_process;i++){
        p = cmt_process_list + i;
        if(p->state==CMT_STATE_VACANT){
            p->state = CMT_STATE_LAUNCH;
            p->wait_timer = time;
            p->data_pointer = f_data;
            p->function = f;
            return i;
        }
    }
    
    return -1;
}

int cmt_launch_process(cmt_function f,void*f_data){
    return cmt_launch_process_delay(f,f_data,0);
}

void cmt_cooperate(){
    if(!setjmp(cmt_current_process->function_context)){
        longjmp(cmt_main_buffer,1);
    }
}

void cmt_wait(cmt_time time){
    cmt_current_process->wait_timer += time;// use plus to merge the residue of last countdown
    cmt_cooperate();
}

static void cmt_shift_current_process(const int stack_offset){
    #ifdef _MSC_VER
    char *pad = (char*)_alloca(stack_offset);// for vc compiler
    #else
    char pad[stack_offset];// use dynamic amount of stack
    #endif
    
    pad[stack_offset - 1] = 0;// prevent it from getting optimized by compiler
    
    cmt_current_process->function(cmt_current_process->data_pointer);
    
    cmt_current_process->state = CMT_STATE_VACANT;
    cmt_current_process->function = NULL;
    cmt_current_process->data_pointer = NULL;
    cmt_number_of_process--;
    longjmp(cmt_main_buffer,1);
}

void cmt_main_routine(){
    int i;
    int stack_offset;
    cmts *p;
    
    for(i=0;i<cmt_max_number_of_process;i++){
        
        cmt_current_process_index = i;
        p = cmt_process_list + i;
        
        if(p->wait_timer>0){
            continue;
        }
        
        if(p->state==CMT_STATE_LAUNCH){
            
            if(!setjmp(cmt_main_buffer)){
                
                p->state = CMT_STATE_RUNNING;
                cmt_number_of_process++;
                
                cmt_current_process = p;
                stack_offset = cmt_process_stack_offset + cmt_process_stack_size*i;
                cmt_shift_current_process(stack_offset);
                
            }
            
        }else
        if(p->state==CMT_STATE_RUNNING){
            
            if(!setjmp(cmt_main_buffer)){
                cmt_current_process = p;
                longjmp(p->function_context,1);
            }
            
        }
        
    }
    
    cmt_current_process_index = -1;
}

void cmt_process_timers(cmt_time time_elapsed){
    int i;
    for(i=0;i<cmt_max_number_of_process;i++){
        if(cmt_process_list[i].wait_timer > 0){
            cmt_process_list[i].wait_timer -= time_elapsed;
        }
    }
}

int cmt_get_current_process_index(){
    return cmt_current_process_index;
}

int cmt_get_number_of_process(){
    return cmt_number_of_process;
}

void cmt_terminate(void){
    if(cmt_process_list){
        free(cmt_process_list);
        cmt_process_list = NULL;
    }
}
