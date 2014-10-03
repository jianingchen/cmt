
Jianing Chen
j.n.chen@sheffield.ac.uk

==== Simple Cooperative Multitasking ====

 - This is a light-weighted solution for multi-tasking using one thread. 
 - Stack for each process are still in the main stack of the program, but with some offset. 
 - See "cmt/debug.c" for some examples.
 - Build debug.c to test. For example: gcc debug.c

 - Initialize with:
    ```
    cmt_initialize(NULL);// NULL => use default setting
    ```

 - Process function example:

    ```
    void ProcessFunctionA(void*data){
        // no large local variables. (limited by stack size)
        // int data[32]; - OK
        // int buffer[1000]; - Avoid
        // int *buffer = (int*)malloc(sizeof(int)*1000); - OK
        
        //... any other code ...
        
        // delay like this:
        cmt_wait(500);
        
        // polled wait like this:
        while(some_condition()){
            cmt_cooperate();
        }
        
    }
    ```

 - Lauch a process:
    
    ```
    
    cmt_launch_process(ProcessFunctionA,NULL);
    
    ```



 - Main loop example:
    
    ```
    while(1){
        
        //... any other code ...
        
        cmt_main_routine();
        cmt_process_timers(time_between_iteration);
        
        //... any other code ...
        
        time_delay(time_between_iteration);// any delay method (if needed)
    }
    ```
    
