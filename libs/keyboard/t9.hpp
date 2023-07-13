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
      if (key == 0)
      { // key 0 for space
         prev_key = 0;
         prev_letter = 0;
         C8WorkingBuff[c_index++] = ' ';
         return;
      }

      if (key == 1)
      {
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
   static constexpr char T9_table[10][4] = {{' ', '\0', '\0', '\0'}, {'\0', '\0', '\0', '\0'}, {'a', 'b', 'c', '\0'}, {'d', 'e', 'f', '\0'}, {'g', 'h', 'i', '\0'}, {'j', 'k', 'l', '\0'}, {'m', 'n', 'o', '\0'}, {'p', 'q', 'r', 's'}, {'t', 'u', 'v', '\0'}, {'w', 'x', 'y', 'z'}};
   static constexpr unsigned char numberOfLettersAssignedToKey[10] = {1, 0, 3, 3, 3, 3, 3, 4, 3, 4};
   unsigned char prev_key = 0, prev_letter = 0;
   unsigned char c_index = 0;
};