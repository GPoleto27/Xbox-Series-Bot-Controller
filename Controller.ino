#include <XboxSeriesXControllerESP32_asukiaaa.hpp>

// LEFT MOTOR FORWARD
#define LF 5
// LEFT MOTOR BACKWARD
#define LB 18
// RIGHT MOTOR FORWARD
#define RF 19
// RIGHT MOTOR BACKWARD
#define RB 21

// For any controller
// XboxSeriesXControllerESP32_asukiaaa::Core xboxController();

// Specify MAC ADDRESS
XboxSeriesXControllerESP32_asukiaaa::Core xboxController("f4:6a:d7:07:b8:2a");

void setup() {
  Serial.begin(115200);
  xboxController.begin();
}

// < ---------- MOTOR FUNCTIONS ----------- >
void left_forward(float lm) {
  analogWrite(LF, 0);
  // Writes PWM speed from [0 to 1] to [0 to 255](integer)
  analogWrite(LB, (int)(lm * 255));
}

void left_backward(float lm) {
  analogWrite(LB, 0);
  // Same as before, but uses the absolute value to the opposite pin
  analogWrite(LF, (int)(-lm * 255));
}

void right_forward(float rm) {
  analogWrite(RB, 0);
  analogWrite(RF, (int)(rm * 255));
}

void right_backward(float rm) {
  analogWrite(RF, 0);
  analogWrite(RB, (int)(-rm * 255));
}
// < -------- END MOTOR FUNCTIONS --------- >

void loop() {
  xboxController.onLoop();
  
  // Left stick x-axis value normalized to [-1 to 1] (left to right)
  float x_axis = (float)(xboxController.xboxNotif.joyLHori / 32767.0) - 1;
  // Total acceleration is the right trigger value [0 to 1023] minus the left trigger value
  // Normalized to [0 to 1]
  float total_acc = (float)(xboxController.xboxNotif.trigRT - xboxController.xboxNotif.trigLT) / 1023.0;
  // Right motor speed, left motor speed
  float rm = 0, lm = 0;

  // < ---------- CONTROLLER LOGIC ---------- >
  if (xboxController.isConnected()) {
    digitalWrite(LED_BUILTIN, HIGH);
    
    // STOPPED
    if (total_acc == 0) {
      // Rotate on it's own axis
      rm = x_axis;
      lm = -x_axis;
    }
    
    // FORWARD
    else if (total_acc > 0) {
      // Turning right
      if (x_axis > 0.1) {
        // The right motor speed should decrease for a right turn
        lm = total_acc;
        rm = total_acc * (1 - x_axis);
      }
      // Turning left
      else if (x_axis < -0.1) {
        // The left motor speed should decrease for a left turn
        // I must sum because x_axis is negative
        lm = total_acc * (1 + x_axis);
        rm = total_acc;
      }
      else {
        // Just moves forward
        lm = rm = total_acc;
      }
    }
    
    //BACKWARD
    else {
      // Same as FORWARD...
      if (x_axis > 0.1) {
        // But now I made a design choice that if I'm moving backward and tap the stick to the right
        // the robot will turn it's front side to right, instead of what normaly happens, for example, in a car

        // If you prefer the latter behaviour:
        // lm = total_acc;
        // rm = total_acc * (1 - x_axis);
        
        lm = total_acc * (1 - x_axis);
        rm = total_acc;
      }
      else if (x_axis < -0.1) {
        // And again:
        // lm = total_acc * (1 + x_axis);
        // rm = total_acc;
        
        lm = total_acc;
        rm = total_acc * (1 + x_axis);
      }
      else {
        lm = rm = total_acc;
      }
    }
  }

  // If not connected to the controller
  else {
    digitalWrite(LED_BUILTIN, LOW);
    lm = rm = 0;
  }
  // < -------- END CONTROLLER LOGIC -------- >

  // Call motor functions
  if (lm > 0)
    left_forward(lm);
  else
    left_backward(lm);

  if (rm > 0)
    right_forward(rm);
  else
    right_backward(rm);
}
