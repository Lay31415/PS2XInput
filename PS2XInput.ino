/*
PS2XInput enables the use of gamepads for the PlayStation series via XInput.

Copyright (C) 2019-2020 by SukkoPera <software@sukkology.net>
Copyright (C) 2023 by Lay31415 <lay31415@bm5keys-forever.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include <XInput.h>
#include <EEPROM.h>

// PsxNewLib Config
////////////////////

// Define this if you need to change the ATT pin
// #define PIN_PS2_ATT 10

// Define these pins if not using HW SPI
// #define PIN_PS2_CLK 13
// #define PIN_PS2_DAT 12
// #define PIN_PS2_CMD 11

#if defined(PIN_PS2_CLK) && defined(PIN_PS2_DAT) && defined(PIN_PS2_CMD)
    // Use BitBang mode if all pins are defined
    #include <PsxControllerBitBang.h>
    #ifndef PIN_PS2_ATT
        #define PIN_PS2_ATT 10  // Set default value for ATT pin if undefined
    #endif
    PsxControllerBitBang<PIN_PS2_ATT, PIN_PS2_CMD, PIN_PS2_DAT, PIN_PS2_CLK> psx;
#else
    // Use SPI mode if some pins are undefined
    #include <PsxControllerHwSpi.h>
    #ifndef PIN_PS2_ATT
        #define PIN_PS2_ATT 10  // Set default value for ATT pin if undefined
    #endif
    PsxControllerHwSpi<PIN_PS2_ATT> psx;
#endif

// Function Analog stick value
////////////////////
#define JOY_DEADZONE 32   // Disabled at 0
#define ANALOG_MIN 0
#define ANALOG_MAX 255
const byte ANALOG_CENTER = (ANALOG_MAX - ANALOG_MIN) / 2;

// LED Config
////////////////////
#define LED_R A1
#define LED_G A3
#define LED_B A2
enum Dpad {DPAD, LS, RS, DPAD_LEN};
byte dpad;

void blank_LED() {
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
}

void show_LED() {
  digitalWrite(LED_B, HIGH);
  switch (dpad) {
    case LS:
      digitalWrite(LED_G, HIGH);
      break;
    case RS:
      digitalWrite(LED_R, HIGH);
      break;
  }
}

void flash_LED() {
// LED flashes according to the selected mode
  for (int flash = 0; flash < 3; flash++){
    blank_LED();
    delay(100);
    show_LED();
    delay(100);
  }
  delay(500);
  blank_LED();
  return;
}

// Main
////////////////////
void setup() {
  // Read savedata
  EEPROM.get(1, dpad);
  if (dpad >= DPAD_LEN) dpad = 0;

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  flash_LED();

  psx.begin();
  XInput.setJoystickRange(ANALOG_MIN, ANALOG_MAX);
  XInput.setAutoSend(false);
  XInput.begin();
}

void loop() {
  // Read state
  if (!psx.read()){
    XInput.releaseAll();
    XInput.send();
    delay(1);
    return;
  }
  const byte protocol = psx.getProtocol();

  bool state_X = psx.buttonPressed(PSB_CROSS);
  bool state_O = psx.buttonPressed(PSB_CIRCLE);
  bool state_Squ = psx.buttonPressed(PSB_SQUARE);
  bool state_Tri = psx.buttonPressed(PSB_TRIANGLE);
  bool state_L1 = psx.buttonPressed(PSB_L1);
  bool state_R1 = psx.buttonPressed(PSB_R1);
  bool state_L2 = psx.buttonPressed(PSB_L2);
  bool state_R2 = psx.buttonPressed(PSB_R2);
  bool state_Sl = psx.buttonPressed(PSB_SELECT);
  bool state_St = psx.buttonPressed(PSB_START);
  bool state_L3 = psx.buttonPressed(PSB_L3);
  bool state_R3 = psx.buttonPressed(PSB_R3);
  bool state_DpadU = psx.buttonPressed(PSB_PAD_UP);
  bool state_DpadD = psx.buttonPressed(PSB_PAD_DOWN);
  bool state_DpadL = psx.buttonPressed(PSB_PAD_LEFT);
  bool state_DpadR = psx.buttonPressed(PSB_PAD_RIGHT);

  // Mode
  if (state_Sl && state_St) {
    show_LED();
  } else {
    blank_LED();
  }
  // Dpad select
  ////////////////////
  if (state_Sl && state_St) {
    if (!state_DpadU && state_DpadL && !state_DpadD && !state_DpadR){
      dpad = DPAD;
    }
    if (!state_DpadU && !state_DpadL && state_DpadD && !state_DpadR){
      dpad = LS;
    }
    if (!state_DpadU && !state_DpadL && !state_DpadD && state_DpadR){
      dpad = RS;
    }
    if (dpad >= DPAD_LEN) dpad = 0;

    if ((state_DpadL || state_DpadD || state_DpadR) && !(state_DpadL && state_DpadD && state_DpadR)){
      // Reset state
      XInput.releaseAll();
      XInput.send();
      delay(1);

      // mode save
      EEPROM.put(1, dpad);

      // LED flashes according to the selected mode
      flash_LED();
    }
  }

  // Set Joystick
  byte x, y;
  if (psx.getLeftAnalog(x, y)) {
    if (abs(x - ANALOG_CENTER) < JOY_DEADZONE && abs(y - ANALOG_CENTER) < JOY_DEADZONE && protocol != PSPROTO_NEGCON) {
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
  XInput.setButton(BUTTON_A, state_X);
  XInput.setButton(BUTTON_B, state_O);
  XInput.setButton(BUTTON_X, state_Squ);
  XInput.setButton(BUTTON_Y, state_Tri);
  XInput.setButton(BUTTON_LB, state_L1);
  XInput.setButton(BUTTON_RB, state_R1);
  XInput.setButton(TRIGGER_LEFT, state_L2);
  XInput.setButton(TRIGGER_RIGHT, state_R2);
  XInput.setButton(BUTTON_BACK, state_Sl);
  XInput.setButton(BUTTON_START, state_St);
  XInput.setButton(BUTTON_L3, state_L3);
  XInput.setButton(BUTTON_R3, state_R3);

  // SOCD
  if (state_DpadD && state_DpadU){
    state_DpadD = false;
    state_DpadU = false;
  }
  if (state_DpadL && state_DpadR){
    state_DpadL = false;
    state_DpadR = false;
  }

  // Set Dpad
  switch (dpad) {
    case DPAD:
      XInput.setDpad(state_DpadU, state_DpadD, state_DpadL, state_DpadR);
      break;
    case LS:
      if(state_DpadU) XInput.setJoystickY(JOY_LEFT, ANALOG_MIN, true);
      if(state_DpadD) XInput.setJoystickY(JOY_LEFT, ANALOG_MAX, true);
      if(state_DpadL) XInput.setJoystickX(JOY_LEFT, ANALOG_MIN);
      if(state_DpadR) XInput.setJoystickX(JOY_LEFT, ANALOG_MAX);
      break;
    case RS:
      if(state_DpadU) XInput.setJoystickY(JOY_RIGHT, ANALOG_MIN, true);
      if(state_DpadD) XInput.setJoystickY(JOY_RIGHT, ANALOG_MAX, true);
      if(state_DpadL) XInput.setJoystickX(JOY_RIGHT, ANALOG_MIN);
      if(state_DpadR) XInput.setJoystickX(JOY_RIGHT, ANALOG_MAX);
      break;
  }

  if (protocol == PSPROTO_NEGCON){
    // Negcon I -> XInput RT
    XInput.setTrigger(TRIGGER_RIGHT, psx.getAnalogButton(PSAB_CROSS));
    // NeGcon A -> XInput A
    XInput.setButton(BUTTON_A, state_O);
    
    // Negcon II -> XInput LT
    XInput.setTrigger(TRIGGER_LEFT, psx.getAnalogButton(PSAB_SQUARE));
    XInput.setButton(BUTTON_X, false);
    
    // NeGcon L -> XInput RX+
    XInput.setJoystickX(JOY_RIGHT, psx.getAnalogButton(PSAB_L1)/2 + 127);
    XInput.setButton(BUTTON_LB, false);

    // NeGcon B -> XInput B
    XInput.setButton(BUTTON_B, state_Tri);
    XInput.setButton(BUTTON_Y, false);
  }

  // Send
  XInput.send();
  delay(1);
}
