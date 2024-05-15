#include <Arduino.h>
#include <pynq.h> // Include the PYNQ library

// I2C address of the VL53L0X sensor
#define VL53L0X_ADDR 0x29

void setup() {
  // Initialize the PYNQ board and switchbox
  pynq_init();
  switchbox_init();

  // Initialize the I2C communication
  i2c_init();

  // Set up the VL53L0X sensor
  vl53l0x_init();
  
  //Serial.begin(9600);
}

void loop() {
  // Read distance measurement from the sensor

  for (int i = 0; i < 100; i++) {
    uint16_t distance = vl53l0x_read_distance();

    printf("Distance: ");
    printf(distance);
    printf(" mm");

    delay(1000); // Delay between measurements
  }
}

void vl53l0x_init() {
  // Initialize the VL53L0X sensor
  i2c_begin_transmission(VL53L0X_ADDR);
  i2c_write(0x00); // Register address to configure the sensor
  // Send configuration data (refer to the sensor datasheet)
  i2c_write(0x01); // Example configuration data
  i2c_end_transmission();
}

uint16_t vl53l0x_read_distance() {
  // Request distance measurement from the VL53L0X sensor
  i2c_begin_transmission(VL53L0X_ADDR);
  i2c_write(0x10); // Register address for distance measurement
  i2c_end_transmission();

  // Read distance data (2 bytes)
  i2c_request_from(VL53L0X_ADDR, 2);
  uint8_t byte1 = i2c_read();
  uint8_t byte2 = i2c_read();

  // Convert byte data to distance (16-bit value)
  uint16_t distance = (byte1 << 8) | byte2;

  return distance;
}

// Run the code:
int main() {
  printf("entered main");
  setup();
  loop();
  return 0;
}
