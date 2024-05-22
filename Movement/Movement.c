#include <arm_shared_memory_system.h>
#include <json-c/json.h>
#include <json-c/json_object.h>
#include <libpynq.h>
#include <platform.h>
#include <stdint.h>
#include <stepper.h>

//NOTE: stepper_set_speed(left, right) and stepper_steps(left, right) ie left tire is left tuple and 
//right tire is the right tuple

//turn the robot right by: right speed X, steps Y, then left steps Y/2, speed X*2 (so goes 1/2 as fast)
void right() {
  stepper_set_speed(3072, 6144);
  stepper_steps(-400, -1600); //CAN BE MODIFIED
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
}

//turn the robot left by: left speed X, steps Y, then right steps Y/2, speed X*2 (so goes 1/2 as fast)
void left() {
  stepper_set_speed(6144, 3072);
  stepper_steps(-1600, -400); //CAN BE MODIFIED
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
}

void forwards() {
  stepper_set_speed(6144, 6144);
  stepper_steps(-200, -200); //CAN BE MODIFIED
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
}

int main(void) {
  pynq_init();
  // Initialize the stepper driver.
  stepper_init();
  // Apply power to the stepper motors.
  stepper_enable();
  
  //RUN THE ROBOT
  //right();
  //sleep_msec(100);
  //forwards();
  //left();
  //RUN THE ROBOT

  //right();

  //left();
  //sleep_msec(100);
  forwards();
  sleep_msec(5000);
  left();

  stepper_destroy();
  pynq_destroy();

  return EXIT_SUCCESS;
}
