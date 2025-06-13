#include <Servo.h>

Servo esc1;
Servo esc2;
Servo esc3;
Servo esc4;

void setup() {
  // Attach each ESC to a digital PWM pin
  esc1.attach(10);  // ESC 1 signal wire
  esc2.attach(11);  // ESC 2 signal wire
  esc3.attach(12);  // ESC 3 signal wire
  esc4.attach(13);  // ESC 4 signal wire

  Serial.begin(9600);
  Serial.println("Arming ESCs...");

  // Send the minimum throttle signal to all ESCs (for arming)
  esc1.writeMicroseconds(1000);
  esc2.writeMicroseconds(1000);
  esc3.writeMicroseconds(1000);
  esc4.writeMicroseconds(1000);

  delay(3000);  // Wait 3 seconds for ESCs to arm
  Serial.println("ESCs armed. Ready to spin motors!");
}

void loop() {
  // Example: spin motors at 1300us (slow speed)
  int speed = 1300;  // Range: 1000 (stop) to 2000 (full throttle)

  esc1.writeMicroseconds(speed);
  esc2.writeMicroseconds(speed);
  esc3.writeMicroseconds(speed);
  esc4.writeMicroseconds(speed);

  delay(5000);  // Run motors for 5 seconds

  // Then stop
  esc1.writeMicroseconds(1000);
  esc2.writeMicroseconds(1000);
  esc3.writeMicroseconds(1000);
  esc4.writeMicroseconds(1000);

  delay(5000);  // Wait 5 seconds
}
