#pragma once

template <unsigned char u8BuffSize>
class CT9Decoder
{
public:
   CT9Decoder(char (&C8WorkingBuffRef)[u8BuffSize])
       : C8WorkingBuff(C8WorkingBuffRef)
   {
   }

   void ProcessButton(unsigned char key)
   {
      if (key == 15)
      {
         number_mode = !number_mode;
         return; // add a return to prevent processing this key normally
      }

      if (number_mode && key <= 9)
      {
         C8WorkingBuff[c_index++] = '0' + key; // add the number to the working buffer
         C8WorkingBuff[c_index] = '\0';        // ensure string is null-terminated
         return;                               // add a return to prevent processing this key normally
      }
      if (key == 0)
      { // key 0 for space
         prev_key = 0;
         prev_letter = 0;
         C8WorkingBuff[c_index++] = ' ';
         return;
      }

      else if (key == 13)
      {
         ProcessBackSpace();
         return;
      }

      else if (key == 14)
      {
         ProcessStartKey();
         return;
      }

      if (prev_key == key && key != 14)
      {
         c_index = (c_index > 0) ? c_index - 1 : 0;
         C8WorkingBuff[c_index++] = T9_table[key][(++prev_letter) % numberOfLettersAssignedToKey[key]];
      }
      else
      {
         prev_letter = 0;
         C8WorkingBuff[c_index++] = T9_table[key][prev_letter];
      }
      C8WorkingBuff[c_index] = '\0';
      prev_key = key;
   }

   void ProcessStartKey()
   {
      prev_key = 14;
      prev_letter = 0;
   }

   void ProcessBackSpace()
   {
      c_index = (c_index > 0) ? c_index - 1 : 0;
      C8WorkingBuff[c_index] = '\0';
   }

   unsigned char GetIdx()
   {
      return c_index;
   }

   char (&C8WorkingBuff)[u8BuffSize];

private:
   static constexpr char T9_table[10][8] = {{' ', '\0', '\0', '\0', '\0', '\0', '\0', '\0'}, {'.', ',', '\'', '!', '?', '/', '#', '$'}, {'a', 'b', 'c', 'A', 'B', 'C', '\0', '\0'}, {'d', 'e', 'f', 'D', 'E', 'F', '\0', '\0'}, {'g', 'h', 'i', 'G', 'H', 'I', '\0', '\0'}, {'j', 'k', 'l', 'J', 'K', 'L', '\0', '\0'}, {'m', 'n', 'o', 'M', 'N', 'O', '\0', '\0'}, {'p', 'q', 'r', 's', 'P', 'Q', 'R', 'S'}, {'t', 'u', 'v', 'T', 'U', 'V', '\0', '\0'}, {'w', 'x', 'y', 'z', 'W', 'X', 'Y', 'Z'}};
   static constexpr unsigned char numberOfLettersAssignedToKey[10] = {1, 8, 6, 6, 6, 6, 6, 8, 6, 8};
   unsigned char prev_key = 0, prev_letter = 0;
   unsigned char c_index = 0;
   bool number_mode = false;
};