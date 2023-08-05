#pragma once
#include "system.hpp"
#include "uv_k5_display.hpp"
#include "keyboard.hpp"

class CT9Texting
{
public:
   CT9Texting()
       : DisplayBuff(gDisplayBuffer),
         Display(DisplayBuff),
         Keyboard(*this),
         bDisplayCleared(true),
         bEnabled(0)
   {
   };

   char T9_table[10][4] = {{' ', '\0', '\0', '\0'}, {'\0', '\0', '\0', '\0'}, {'a', 'b', 'c', '\0'}, {'d', 'e', 'f', '\0'}, {'g', 'h', 'i', '\0'}, {'j', 'k', 'l', '\0'}, {'m', 'n', 'o', '\0'}, {'p', 'q', 'r', 's'}, {'t', 'u', 'v', '\0'}, {'w', 'x', 'y', 'z'}};
   unsigned char numberOfLettersAssignedToKey[10] = {1, 0, 3, 3, 3, 3, 3, 4, 3, 4};
   unsigned char prev_key = 0, prev_letter = 0;
   char cMessage[30];
   unsigned char c_index = 0;
   bool keyReleased = true;

   void insert_char_to_cMessage(unsigned char key)
   {
      if (prev_key == key && key != 14)
      {
         c_index = (c_index > 0) ? c_index - 1 : 0;
         cMessage[c_index++] = T9_table[key][(++prev_letter) % numberOfLettersAssignedToKey[key]];
      }
      else
      {
         prev_letter = 0;
         cMessage[c_index++] = T9_table[key][prev_letter];
      }
      cMessage[c_index] = '\0';
      prev_key = key;
   }

   void process_starkey()
   {
      prev_key = 14;
      prev_letter = 0;
   }

   void process_backspace()
   {
      c_index = (c_index > 0) ? c_index - 1 : 0;
      cMessage[c_index] = ' ';
   }

   void Handle()
   {
      if (!(GPIOC->DATA & 0b1))
      {
         return;
      }

      if (!FreeToDraw())
      {
         if (!bDisplayCleared)
         {
            bDisplayCleared = true;
            ClearDrawings();
            // Clear cMessage
            memset(cMessage, 0, 30);
            c_index = 0;
            FlushFramebufferToScreen();
         }

         return;
      }

      if (bDisplayCleared)
      {
      }

      bDisplayCleared = false;

      unsigned char key;
      key = u8LastBtnPressed;

      if (key == 0xff)
      {
         keyReleased = true;
         return;
      }
      else if (keyReleased)
      {
         keyReleased = false;
         if (key == 0)
         { // key 0 for space
            prev_key = 0;
            prev_letter = 0;
            cMessage[c_index++] = ' ';
         }

         else if (key == 13)
         {
            process_backspace();
            return;
         }

         else if (key == 14)
         {
            process_starkey();
            return;
         }

         else if (key == 10)
         {
            bEnabled = false;
            return;
         }

         else
         {
            insert_char_to_cMessage(key);
         }
      }
      prev_key = key;
      // Display.DrawRectangle(0,0, 7, 7, 0);
      ClearDrawings();
      PrintTextOnScreen(cMessage, 0, 128, 0, 8, 0);
      FlushFramebufferToScreen();
   }

private:
   bool
   FreeToDraw()
   {
      bool bFlashlight = (GPIOC->DATA & GPIO_PIN_3);
      if (bFlashlight)
      {
         bEnabled = true;
         GPIOC->DATA &= ~GPIO_PIN_3;
         gFlashLightStatus = 3;
      }

      if (bEnabled)
      {
         u8LastBtnPressed = PollKeyboard();
      }

      // u8LastBtnPressed = PollKeyboard();
      // if (u8LastBtnPressed == EnableKey)
      // {
      //    u8PressCnt++;
      // }

      // if (u8PressCnt > (bEnabled ? 3 : PressDuration))
      // {
      //    u8PressCnt = 0;
      //    bEnabled = !bEnabled;
      // }

      return bEnabled;
   }

   void ClearDrawings()
   {
      memset(gDisplayBuffer, 0, (DisplayBuff.SizeX / 8) * DisplayBuff.SizeY);
   }

   TUV_K5Display DisplayBuff;
   CDisplay<const TUV_K5Display> Display;
   CKeyboard<CT9Texting> Keyboard;

   bool bDisplayCleared;

   unsigned char u8LastBtnPressed;
   bool bEnabled;
};