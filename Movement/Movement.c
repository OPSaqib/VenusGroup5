#include <arm_shared_memory_system.h>
#include <json-c/json.h>
#include <json-c/json_object.h>
#include <libpynq.h>
#include <platform.h>
#include <stdint.h>
#include <stepper.h>

void enable_stepper() {
  stepper_init();
  stepper_enable();
}

void disable_stepper() {
  stepper_destroy();
}

//NOTE: stepper_set_speed(left, right) and stepper_steps(left, right) ie left tire is left tuple and 
//right tire is the right tuple

//turn the robot right by: right speed X, steps Y, then left steps Y/2, speed X*2 (so goes 1/2 as fast)
void right() {
  //enable_stepper();
  stepper_set_speed(-6144, -3072);
  stepper_steps(-1450, -200); //CAN BE MODIFIED
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
  //disable_stepper();
}

//TODO:
//turn the robot left by: left speed X, steps Y, then right steps Y/2, speed X*2 (so goes 1/2 as fast)
void left() {
  //enable_stepper();
  stepper_set_speed(-3072, -6144);
  stepper_steps(-200, -1400); //CAN BE MODIFIED
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
  //disable_stepper();
}

void forwards() {
  //enable_stepper();
  stepper_set_speed(-3072, -3072);
  stepper_steps(-200, -200); //CAN BE MODIFIED
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
  //disable_stepper();
}

void backwards() {
  //enable_stepper();
  stepper_set_speed(3072, 3072);
  stepper_steps(400, 400); //CAN BE MODIFIED
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
  //disable_stepper();
}

int main(void) {
  pynq_init();
  // Initialize the stepper driver.
  enable_stepper();
  forwards();
  sleep_msec(20);
  forwards();
  sleep_msec(20);
  forwards();
  sleep_msec(20);
  forwards();
  sleep_msec(20);
  forwards();
  sleep_msec(20);
  left();
  sleep_msec(20);
  forwards();
  sleep_msec(20);
  forwards();
  disable_stepper();
  return EXIT_SUCCESS;
}
