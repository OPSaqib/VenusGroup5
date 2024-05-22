#include <arm_shared_memory_system.h>
#include <json-c/json.h>
#include <json-c/json_object.h>
#include <libpynq.h>
#include <platform.h>
#include <stdint.h>
#include <stepper.h>

int main(void) {
  pynq_init();
  // Initialize the stepper driver.
  stepper_init();
  // Apply power to the stepper motors.
  stepper_enable();
  
  //RUN THE ROBOT
  forward();
  right();
  left();
  //RUN THE ROBOT

  stepper_destroy();
  pynq_destroy();

  return EXIT_SUCCESS;
}

//move the robot forwards
void forward() {
    stepper_set_speed(6144, 6144);
    stepper_steps(200, 200); //CAN BE MODIFIED
    while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
}

//turn the robot right
void right() {
    stepper_set_speed(3072, 6144);
    stepper_steps(100, 200); //CAN BE MODIFIED
    while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
}

//turn the robot left 
void left() {
    stepper_set_speed(6144, 3072);
    stepper_steps(200, 100); //CAN BE MODIFIED
    while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
}

void fullrotation() {
    //this situation shouldn't ever happen as the algorithm should be as such that the robot never encounters a deadlock situation
}
