/*
PS2XInput enables the use of gamepads for the PlayStation series via XInput.

Copyright (C) 2019-2020 by SukkoPera <software@sukkology.net>
Copyright (C) 2023 by Lay31415 <lay31415@bm5keys-forever.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include <XInput.h>

// PsxNewLib HwSpi Config
// DAT CMD CLK must be connected to HWSPI pin
////////////////////

#include <PsxControllerHwSpi.h>
#define PIN_PS2_ATT 10
PsxControllerHwSpi<PIN_PS2_ATT> psx;

// PsxNewLib BitBang Config
// Specify any GPIO
////////////////////

// #include <PsxControllerBitBang.h>
// #define PIN_PS2_CLK 13
// #define PIN_PS2_DAT 12
// #define PIN_PS2_CMD 11
// #define PIN_PS2_ATT 10
// PsxControllerBitBang<PIN_PS2_ATT, PIN_PS2_CMD, PIN_PS2_DAT, PIN_PS2_CLK> psx;

// Function Analog stick value
////////////////////
#define JOY_DEADZONE 32   // Disabled at 0
#define ANALOG_MIN 0
#define ANALOG_MAX 255
const byte ANALOG_CENTER = (ANALOG_MAX - ANALOG_MIN) / 2;

void setup() {
  psx.begin();
  XInput.setJoystickRange(ANALOG_MIN, ANALOG_MAX);
  XInput.setAutoSend(false);
  XInput.begin();
}

void loop() {
  // Read state
  psx.read();
  bool state_A = psx.buttonPressed(PSB_CROSS);
  bool state_B = psx.buttonPressed(PSB_CIRCLE);
  bool state_X = psx.buttonPressed(PSB_SQUARE);
  bool state_Y = psx.buttonPressed(PSB_TRIANGLE);
  bool state_LB = psx.buttonPressed(PSB_L1);
  bool state_RB = psx.buttonPressed(PSB_R1);
  bool state_LT = psx.buttonPressed(PSB_L2);
  bool state_RT = psx.buttonPressed(PSB_R2);
  bool state_BACK = psx.buttonPressed(PSB_SELECT);
  bool state_START = psx.buttonPressed(PSB_START);
  bool state_L3 = psx.buttonPressed(PSB_L3);
  bool state_R3 = psx.buttonPressed(PSB_R3);
  bool state_DpadU = psx.buttonPressed(PSB_PAD_UP);
  bool state_DpadD = psx.buttonPressed(PSB_PAD_DOWN);
  bool state_DpadL = psx.buttonPressed(PSB_PAD_LEFT);
  bool state_DpadR = psx.buttonPressed(PSB_PAD_RIGHT);

  // Set Joystick
  byte x, y;
  if (psx.getLeftAnalog(x, y)) {
    if (abs(x - ANALOG_CENTER) < JOY_DEADZONE && abs(y - ANALOG_CENTER) < JOY_DEADZONE) {
      x = ANALOG_CENTER;
      y = ANALOG_CENTER;
    }
    XInput.setJoystickX(JOY_LEFT, x);
    XInput.setJoystickY(JOY_LEFT, y, true);
  }
  if (psx.getRightAnalog(x, y)) {
    if (abs(x - ANALOG_CENTER) < JOY_DEADZONE && abs(y - ANALOG_CENTER) < JOY_DEADZONE) {
      x = ANALOG_CENTER;
      y = ANALOG_CENTER;
    }
    XInput.setJoystickX(JOY_RIGHT, x);
    XInput.setJoystickY(JOY_RIGHT, y, true);
  }

  // Set button state
  XInput.setButton(BUTTON_A, state_A);
  XInput.setButton(BUTTON_B, state_B);
  XInput.setButton(BUTTON_X, state_X);
  XInput.setButton(BUTTON_Y, state_Y);
  XInput.setButton(BUTTON_LB, state_LB);
  XInput.setButton(BUTTON_RB, state_RB);
  XInput.setButton(TRIGGER_LEFT, state_LT);
  XInput.setButton(TRIGGER_RIGHT, state_RT);
  XInput.setButton(BUTTON_BACK, state_BACK);
  XInput.setButton(BUTTON_START, state_START);
  XInput.setButton(BUTTON_L3, state_L3);
  XInput.setButton(BUTTON_R3, state_R3);
    if (state_DpadD && state_DpadL && state_DpadR){
    state_DpadD = false;
    state_DpadL = false;
    state_DpadR = false;
  }
  XInput.setDpad(state_DpadU, state_DpadD, state_DpadL, state_DpadR);

  // Send
  XInput.send();
  delay(1);
}
