#include <Arduino.h> // Arduino core framework
#include <string.h> // Standard C++ (STL) library
#include "hardware/pio.h" // Raspberry Pi Pico PIO library

#include "manchester_tx.pio.h" // Manchester TX PIO program header


// Put global variables here:

#define LED_PIN 14 // Pin for our external LED
PIO pio = pio0; // We define 'pio' as pio0 hardware block
uint sm = 0;    // We define 'sm' as state machine 0
const char* stringData = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz1!2@3#4$5%6^7&8*9(0)-_=+`~ "; // A C-style char string (*)


// Put function definitions here:
// None

// Put your setup code here, to run once:
void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Pause here until the Serial Monitor is opened
  while (!Serial) delay(1);
  delay(100); // Wait 0.1 seconds to ensure stable connection
  Serial.println("Serial monitor connected! Starting transmission...");

  // Load the PIO program into memory
  uint offset = pio_add_program(pio, &manchester_tx_program);
  
  // Initialize the state machine on Pin 14
  manchester_tx_program_init(pio, sm, offset, LED_PIN);
  
  // Change the clock speed (frequency) to match the target frequency for data transmission
  // 10Hz means 10 bits per second. 
  // At 30fps, the camera sees each bit for exactly 3 frames.
  float target_freq = 10; 
  float div = (float)clock_get_hz(clk_sys) / (target_freq * 16.0f);
  pio_sm_set_clkdiv(pio, sm, div); // Change clock frequency for the state machine

  // Clear the internal registers to be safe
  pio_sm_clear_fifos(pio, sm);
  pio_sm_restart(pio, sm);

  // Enable the state machine
  // Starts the Manchester hardware logic on state machine 0
  pio_sm_set_enabled(pio, sm, true);
  Serial.println("System ready. Sending data...");

  // Print the string data as ASCII and Base-16 UINT8_T Decimal to the serial monitor
  Serial.println("String data (ASCII): ");
  Serial.println(stringData); // Print as ASCII
  Serial.println("String data (UINT8_T): ");
  for (int i = 0; i < strlen(stringData); i++) { // Print as UINT8_T
    uint8_t byteValue = (uint8_t)stringData[i];
    Serial.print(byteValue); // Print decimal byte value without newline
    Serial.print(" "); // Space between bytes
  }
  Serial.println(); // New line and carriage return after printing all bytes (\n + \r)
  Serial.println("Transmission complete!");
}


// Put your main code here, to run repeatedly:
void loop() {
  // Blink the external LED to the manchester encoded data continuously every 3 seconds

  // Send sync / preamble data to indicate start of transmission

  Serial.println("Sending sync / preamble data...");

  // Send 'U' (0x55) five times
  uint8_t preambleByte = 0x55;
  for (int i = 0; i < 5; i++) {
    pio_sm_put_blocking(pio, sm, preambleByte);
  }

  // Send '#' (0x23) once
  uint8_t syncByte = 0x23;
  pio_sm_put_blocking(pio, sm, syncByte);

  Serial.println("Sync / preamble data sent!");

  // Send each byte of the string data to the PIO state machine
  Serial.println("Blinking LED with data manchester encoded data...");
  for (int i = 0; i < strlen(stringData); i++) {
    uint8_t byteValue = (uint8_t)stringData[i];
    // Push it to the PIO hardware
    // This blocks C++ until there is room in the FIFO
    pio_sm_put_blocking(pio, sm, byteValue);
  }
  Serial.println("LED transmission complete!");

  delay(5000); // Wait for 5 seconds before repeating
}