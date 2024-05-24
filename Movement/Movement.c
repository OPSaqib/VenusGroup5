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
  enable_stepper();
  stepper_set_speed(6144, 3072);
  stepper_steps(-1400, -200); //CAN BE MODIFIED
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
  disable_stepper();
}

//TODO:
//turn the robot left by: left speed X, steps Y, then right steps Y/2, speed X*2 (so goes 1/2 as fast)
void left() {
  enable_stepper();
  stepper_set_speed(3072, 6144);
  stepper_steps(-200, -1350); //CAN BE MODIFIED
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
  disable_stepper();
}

//NOT REQUIRED:
void make_left_slightly() {
  enable_stepper();
  stepper_set_speed(3072, 6144);
  stepper_steps(-200, -400); //CAN BE MODIFIED
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
  disable_stepper();
}

//NOT REQUIRED:
void make_right_slightly() {
  enable_stepper();
  stepper_set_speed(6144, 3072);
  stepper_steps(-150, -50); //CAN BE MODIFIED
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
  disable_stepper();
}

void forwards() {
  enable_stepper();
  stepper_set_speed(3072, 3072);
  stepper_steps(-200, -200); //CAN BE MODIFIED
  while (!stepper_steps_done()) {}; //Wait for stepper steps to finish
  disable_stepper();
}

int main(void) {
  pynq_init();
  // Initialize the stepper driver.

  // Now do the movements required
  forwards();
  sleep_msec(2000);
  forwards();
  sleep_msec(2000);
  left();
  //sleep_msec(2000);
  //make_right_slightly();
  sleep_msec(2000);
  forwards();
  sleep_msec(2000);
  forwards();
  sleep_msec(2000);
  forwards();
  //sleep_msec(2000);
  //left();
  //sleep_msec(2000);
  //forwards();
  pynq_destroy();

  return EXIT_SUCCESS;
}
