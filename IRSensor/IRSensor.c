#include <libpynq.h>
#include <unistd.h> // Include the usleep function
#include <time.h>
#include <stdint.h>
#include <sys/time.h>

#define IR_SENSOR_PIN ADC0 
#define NUMBER_IR_VALUES 10
#define BLACK 0
#define WHITE 1
// Delay in seconds
#define DELAY 1.0 

typedef struct ir_values{
    int value;
    struct ir_values *next;
}ir_val;

ir_val *appendNode(ir_val *head, int value){
  //building new link
  ir_val * newNode = (ir_val *) malloc(sizeof(ir_val));
  newNode->value = value;
  newNode->next = NULL;
  //check if new link is start of the chain
  if(head == NULL){
    return newNode;
  }
  ir_val * searchLast = head;
  while(searchLast->next!=NULL){
    searchLast = searchLast->next;
  }
  searchLast->next = newNode;
  return head;
}

ir_val *freeNode(ir_val *head){
  //check if a chain exists
  if(head==NULL){return NULL;}
  //delete chain of lenght 1
  if(head->next==NULL){free(head); return NULL;}

  ir_val *current = head;
  ir_val *next = head->next;
  printf("remove curr\n\n");
  free(current);
  //current=NULL;

  return next;
}


// Initialize PYNQ board and switchbox
void setup() {
    printf("Entered setup");
    pynq_init();    
    adc_init(); 
}

//testing the ir sensor
void test(){
    printf("Entered loop");

    while (true){

        int ir_sensor_input = adc_read_channel_raw(IR_SENSOR_PIN);
        //float pVolt = ir_sensor_input *5 / 65535 ;
        printf("ir_value = %d\n", ir_sensor_input); 

        if (ir_sensor_input>250){    //>260 longer wires sensor  >245 short-wired sensor
            printf("Black\n\n");
        }else{printf("White\n\n");}
        
        sleep_msec(1000);
    }
}

//calculating AVG value for IR sensor with link list of n elements
//doesn't work
void running_AVG() {
    printf("Entered loop");

    ir_val * ir_val_head =NULL;

    int ir_avg=0;
    int index_ir_value=0;

    while (true){
        
        if(index_ir_value==NUMBER_IR_VALUES){

            ir_val * currNode = ir_val_head;
            int count=0;
            while(currNode!=NULL){
                ir_avg+=currNode->value;
                currNode = currNode->next;
                count++;
            }
            printf("count= %d   ",count);
            if(ir_avg < NUMBER_IR_VALUES/2){
                printf("AVERAGE = BLACK %d\n\n",ir_avg);
            }else{
                printf("AVERAGE = WHITE %d\n\n",ir_avg);
            }

            ir_avg=0;
            ir_val_head = freeNode(ir_val_head);
            index_ir_value=0;
        } 

        //reading value from sensor
        int ir_sensor_input = adc_read_channel_raw(IR_SENSOR_PIN);
        if (ir_sensor_input>250){    //>260 longer wires sensor  >245 short-wired sensor
            printf("Black %d\n",ir_sensor_input);
            ir_val_head=appendNode(ir_val_head,BLACK);
        }else{
            printf("White %d\n",ir_sensor_input);
            ir_val_head=appendNode(ir_val_head,WHITE);
        } 
        sleep_msec(500);  //maybe not using sleep but smth more efficient
        
        index_ir_value++;
    }

    while(ir_val_head!=NULL){
        ir_val_head = freeNode(ir_val_head);
    }
}

//calculating AVG value for IR sensor with array of n elements - uses sleep_msec()
//works
void running_AVG2() {
    printf("Entered loop");

    int ir_avg=0;

    while (true){
       

        for(int i=0; i< NUMBER_IR_VALUES;i++){
            int ir_sensor_input = adc_read_channel_raw(IR_SENSOR_PIN);
            if (ir_sensor_input>250){    //>260 longer wires sensor  >245 short-wired sensor
                printf("Black %d\n",ir_sensor_input);
                ir_avg+=BLACK;
            }else{
                printf("White %d\n",ir_sensor_input);
                ir_avg+=WHITE;
            } 
            //depending on the time for one move of the robot
            sleep_msec(200);  //maybe not using sleep but smth more efficient
        
        }

        if(ir_avg < NUMBER_IR_VALUES/2){
            printf("AVERAGE = BLACK %d\n\n",ir_avg);
        }else{
            printf("AVERAGE = WHITE %d\n\n",ir_avg);
        }

        ir_avg=0;
    } 
}

//calculating AVG value for IR sensor with array of n elements - every d-seconds ("DELAY") gets value from sensor 
//works
void running_AVG3() {
    printf("Entered loop");

    int curr_index_IR_VALUES=0;
    int ir_avg=0;
    struct timeval start_time, current_time;
    double elapsed_time;
    //saves initial time when we enter the loop
    gettimeofday(&start_time, NULL);

    while (true){
        gettimeofday(&current_time, NULL);
        elapsed_time = (current_time.tv_sec - start_time.tv_sec) +
                       (current_time.tv_usec - start_time.tv_usec) / 1000000.0;

        //if the amount of seconds, stored in "DELAY", has passed - take a measurment from the sensor
        if (elapsed_time >= DELAY) {
            if(curr_index_IR_VALUES < NUMBER_IR_VALUES){
                int ir_sensor_input = adc_read_channel_raw(IR_SENSOR_PIN);
                if (ir_sensor_input>250){    //>260 longer wires sensor  >245 short-wired sensor
                    printf("Black %d\n",ir_sensor_input);
                    ir_avg+=BLACK;
                }else{
                    printf("White %d\n",ir_sensor_input);
                    ir_avg+=WHITE;
                } 

                curr_index_IR_VALUES++;
            }
           
            // Reset the start time
            gettimeofday(&start_time, NULL); 
        }
        //after saving n values from the sensor display the average
        if(curr_index_IR_VALUES >= NUMBER_IR_VALUES){
            if(ir_avg < NUMBER_IR_VALUES/2){
                printf("AVERAGE = BLACK %d\n\n",ir_avg);
            }else{
                printf("AVERAGE = WHITE %d\n\n",ir_avg);
            }

            ir_avg=0;
            curr_index_IR_VALUES=0;
        }        
    } 
}


// Run the code:
int main(void) {
    printf("entered main");
    setup();


    //test();
    //running_AVG();
    //running_AVG2();
    running_AVG3();

    adc_destroy();
    pynq_destroy();
    return EXIT_SUCCESS;
}
