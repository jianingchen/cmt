
Jianing Chen
j.n.chen@sheffield.ac.uk

==== Simple Cooperative Multitasking ====

 - This is a light-weighted solution for multi-tasking in one thread.
 - See "cmt/debug.c" for some examples.
 - Build debug.c to test. For example: gcc debug.c

 - Process function example:

    ...
    void ProcessFunctionA(void*data){
        // no large local variables. (limited by stack size)
        // int data[32]; - OK
        // int buffer[1000]; - Avoid
        // int *buffer = (int*)malloc(sizeof(int)*1000); - OK
        
        //... code here ...
        
        // delay like this:
        cmt_wait(500);
        
        // polled wait like this:
        while(some_condition()){
            cmt_cooperate();
        }
        
    }
    ...

 - Lauch a process:
    
    ...
    cmt_launch_process(ProcessFunctionA,NULL);
    ...



 - Main loop example:
    
    ...
    while(1){
        
        // (other code)
        
        cmt_main_routine();
        cmt_process_timers(time_between_iteration);
        
        // (other code)
        
        time_delay(time_between_iteration);// any delay method (if needed)
    }
    ...
    
