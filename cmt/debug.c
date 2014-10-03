
#include "cmt.c"

#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int stack_overflow(){
    int waste[8000];
    waste[7999] = 0;
    return waste[7999];
}

void routineA(void*data){
    int i;
    
    printf("<A: %p>",&i);
    
    for(i=0;i<10;i++){
        
        printf("[A: %d]",i);
        
        //stack_overflow();
        
        cmt_wait(500);
        
    }
    
    printf("[A: end]");
    
}


void routineB(void*data){
    int i;
    
    printf("<B: %p>",&i);
    
    for(i=0;i<30;i++){
        
        printf("[B: %d]",i);
        
        cmt_wait(300);
        
    }
    
    printf("[B: end]");
    
}

void routineD(void*data){
    int i;
    
    printf("<D: %p>",&i);
    
    for(i=0;i<10;i++){
        
        printf("[D: %d]",i);
        
        if(i==4){
            cmt_launch_process(routineA,NULL);
        }
        
        
        cmt_wait(1000);
        
    }
    
    printf("[D: end]");
    
}

void routineC(void*data){
    int i;
    
    printf("<C: %p>",&i);
    
    for(i=0;i<100;i++){
        
        printf("[C: %d]",i);
        
        if(i==89){
            cmt_launch_process(routineD,NULL);
        }
        
        cmt_wait(100);
        
    }
    
    printf("[C: end]");
    
}

void routineW(void*data){
    int i;
    
    printf("<W: %p>",&i);
    
    while(1){
        
        if(data){
            printf("[%s]",(char*)data);
        }
        
        cmt_cooperate();
    }
    
    printf("[W: end]");
    
}


void routineX(void*data){
    int i;
    
    printf("<X: %p>",&i);
    
    i = 0;
    while(1){
        
        printf("[X: %d]",i++);
        
        cmt_wait(100);
        
    }
    
    printf("[X: end]");
    
}
// "i" in X and Y should be increased in a ratio of 3:2
void routineY(void*data){
    int i;
    
    printf("<Y: %p>",&i);
    
    i = 0;
    while(1){
        
        printf("[Y: %d]",i++);
        
        cmt_wait(150);
        
    }
    
    printf("[Y: end]");
    
}

void routineZ(void*data){
    int i;
    
    printf("<Z: %p>",&i);
    
    printf("[Z: end]");
    
}

int main(int argc,char*argv[]){
    const unsigned int STEP_SIZE_MS = 100;
    char strA[] = "Hello";
    char strB[] = "World";
    int i;
    cmt_ini typical_setting;
    
    typical_setting.MaxNumberOfProcess = 32;
    typical_setting.ProcessStackSzie = 2048;
    typical_setting.ProcessStackOffset = 65536;
    
    cmt_initialize(&typical_setting);
    
    printf("\nCMT Initialized");
    printf("\n%d",sizeof(jmp_buf));
    
    cmt_launch_process(routineW,strA);
    cmt_launch_process(routineW,strB);
    
    cmt_launch_process(routineX,NULL);
    cmt_launch_process(routineY,NULL);
    cmt_launch_process_delay(routineZ,NULL,5000);
    
    cmt_launch_process(routineA,NULL);
    cmt_launch_process(routineB,NULL);
    cmt_launch_process(routineC,NULL);
    
    i = 0;
    while(i<500){
        
        printf("\n%5.5d\t%6.3lf\t",i,(double)i*STEP_SIZE_MS/1000.0);
        
        cmt_main_routine();
        cmt_process_timers(STEP_SIZE_MS);
        
        i++;
        
        Sleep(STEP_SIZE_MS);
    }
    
    cmt_terminate();
    
    printf("\nCMT Terminated");
    
    if(cmt_get_number_of_process()){
        printf("\nCMT Warning: %d process leaked !!!",cmt_get_number_of_process());
    }
    
    printf("\n");
    
    system("pause");
    
    return 0;
}
