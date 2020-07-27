/*
 키보드 USB 연결
 -> ESP32에서 전력 공급
 -> 데이터 수신(USB Host Shield 2.0 라이브러리)
 -> 수신한 데이터 블루투스 HID로 전송 (BleKeyboard 라이브러리)
*/
#include <hidboot.h>
#include <usbhub.h>
#include <SPI.h>
#include <BleKeyboard.h>

BleKeyboard bleKeyboard;

class KbdRptParser : public KeyboardReportParser
{
  void PrintKey(uint8_t mod, uint8_t key);

protected:
  void OnControlKeysChanged(uint8_t before, uint8_t after);

  void OnKeyDown(uint8_t mod, uint8_t key);
  void OnKeyUp(uint8_t mod, uint8_t key);
  void OnKeyPressed(uint8_t key);
};

void KbdRptParser::PrintKey(uint8_t m, uint8_t key)
{
  MODIFIERKEYS mod;
  *((uint8_t *)&mod) = m;
  Serial.print((mod.bmLeftCtrl == 1) ? "C" : " ");
  Serial.print((mod.bmLeftShift == 1) ? "S" : " ");
  Serial.print((mod.bmLeftAlt == 1) ? "A" : " ");
  Serial.print((mod.bmLeftGUI == 1) ? "G" : " ");

  Serial.print(" >");
  PrintHex<uint8_t>(key, 0x80);
  Serial.print("< ");

  Serial.print((mod.bmRightCtrl == 1) ? "C" : " ");
  Serial.print((mod.bmRightShift == 1) ? "S" : " ");
  Serial.print((mod.bmRightAlt == 1) ? "A" : " ");
  Serial.println((mod.bmRightGUI == 1) ? "G" : " ");
};

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
  Serial.print("DN ");
  PrintKey(mod, key);
  uint8_t c = OemToAscii(mod, key);
  Serial.print("\n==============");
  Serial.println((int)key);

  if (key == 42) //backspace
    bleKeyboard.press(KEY_BACKSPACE);
  else if (key == 43) // TAB
    bleKeyboard.press(KEY_TAB);
  else if (key == 41) // ESC
    bleKeyboard.write(KEY_ESC);
  else if (key == 40) // Enter
    bleKeyboard.press(KEY_RETURN);

  else if (key == 76) // Delete
    bleKeyboard.write(KEY_DELETE);
  else if (key == 73) // Insert
    bleKeyboard.write(KEY_INSERT);
  else if (key == 74) // Home
    bleKeyboard.write(KEY_HOME);
  else if (key == 77) // End
    bleKeyboard.write(KEY_END);
  else if (key == 75) // Page Up
    bleKeyboard.write(KEY_PAGE_UP);
  else if (key == 78) // Page Down
    bleKeyboard.write(KEY_PAGE_DOWN);

  /* 방향키(Arrow Key) */
  else if (key == 82) // Arrow up
    bleKeyboard.press(KEY_UP_ARROW);
  else if (key == 81) // Arrow down
    bleKeyboard.press(KEY_DOWN_ARROW);
  else if (key == 80) // Arrow left
    bleKeyboard.press(KEY_LEFT_ARROW);
  else if (key == 79) // Arrow right
    bleKeyboard.press(KEY_RIGHT_ARROW);

  /* 기능키(Function Key) */
  else if (key == 58) // F1
    bleKeyboard.write(KEY_F1);
  else if (key == 59) // F2
    bleKeyboard.write(KEY_F2);
  else if (key == 60) // F3
    bleKeyboard.write(KEY_F3);
  else if (key == 61) // F4
    bleKeyboard.write(KEY_F4);
  else if (key == 62) // F5
    bleKeyboard.write(KEY_F5);
  else if (key == 63) // F6
    bleKeyboard.write(KEY_F6);
  else if (key == 64) // F7
    bleKeyboard.write(KEY_F7);
  else if (key == 65) // F8
    bleKeyboard.write(KEY_F8);
  else if (key == 66) // F9
    bleKeyboard.write(KEY_F9);
  else if (key == 67) // F10
    bleKeyboard.write(KEY_F10);
  else if (key == 68) // F11
    bleKeyboard.write(KEY_F11);
  else if (key == 69) // F12
    bleKeyboard.write(KEY_F12);
    
  bleKeyboard.releaseAll();
  if (c)
    OnKeyPressed(c);
}

boolean controlKeyPressed = false;
void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after)
{
  MODIFIERKEYS beforeMod;
  *((uint8_t *)&beforeMod) = before;

  MODIFIERKEYS afterMod;
  *((uint8_t *)&afterMod) = after;
  if (!controlKeyPressed)
  {
    controlKeyPressed = true;
    if (beforeMod.bmLeftCtrl != afterMod.bmLeftCtrl)
    {
      bleKeyboard.press(KEY_LEFT_CTRL);
      Serial.println("LeftCtrl changed");
    }
    if (beforeMod.bmLeftShift != afterMod.bmLeftShift)
    {
      bleKeyboard.press(KEY_LEFT_SHIFT);
      Serial.println("LeftShift changed");
    }
    if (beforeMod.bmLeftAlt != afterMod.bmLeftAlt)
    {
      bleKeyboard.release(KEY_LEFT_ALT);
      Serial.println("LeftAlt changed");
    }
    if (beforeMod.bmLeftGUI != afterMod.bmLeftGUI)
    {
      bleKeyboard.press(KEY_LEFT_GUI);
      Serial.println("release changed");
    }

    if (beforeMod.bmRightCtrl != afterMod.bmRightCtrl)
    {
      bleKeyboard.release(KEY_RIGHT_CTRL);
      Serial.println("RightCtrl changed");
    }
    if (beforeMod.bmRightShift != afterMod.bmRightShift)
    {
      bleKeyboard.release(KEY_RIGHT_SHIFT);
      Serial.println("RightShift changed");
    }
    if (beforeMod.bmRightAlt != afterMod.bmRightAlt)
    {
      bleKeyboard.press(KEY_RIGHT_ALT);
      Serial.println("RightAlt changed");
    }
    if (beforeMod.bmRightGUI != afterMod.bmRightGUI)
    {
      bleKeyboard.release(KEY_RIGHT_GUI);
      Serial.println("RightGUI changed");
    }
  }
  else
  {
    controlKeyPressed = false;
    bleKeyboard.releaseAll();
  }
}

void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
  Serial.print("UP ");
  PrintKey(mod, key);
}

void KbdRptParser::OnKeyPressed(uint8_t key)
{
  Serial.print("ASCII: ");
  Serial.println((char)key);
  bleKeyboard.write((char)key);
};

USB Usb;
//USBHub     Hub(&Usb);
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> HidKeyboard(&Usb);

KbdRptParser Prs;

void setup()
{
  /* begin USB Host Shield */
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial)
    ; // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  Serial.println("Starting USB Host Shield work");
  if (Usb.Init() == -1)
    Serial.println("OSC did not start.");
  delay(200);
  HidKeyboard.SetReportParser(0, &Prs);

  /* begin BLE  Keyboard */
  bleKeyboard.begin();
  Serial.println("Starting BLE Keyboard work");
}

void loop()
{
  if (bleKeyboard.isConnected())
  {
    Usb.Task();
  }
}
