#pragma once

namespace Button
{
   enum : unsigned char
   {
      Ok = 10,
      Up = 11,
      Down = 12,
      Exit = 13,
   };
}

template<class KeyboardUserType>
class CKeyboard
{
   static constexpr auto ReleasedRawState = 0xFF;
   public:
   CKeyboard(KeyboardUserType& _KeyboardUser): 
      KeyboardUser(_KeyboardUser), 
      u8LastKey(ReleasedRawState){};

   void Handle(unsigned char u8RawButton)
   {
      if(u8RawButton == u8LastKey)
      {
         return;
      }

      if(u8LastKey == ReleasedRawState)
      {
         KeyboardUser.HandlePressedButton(u8RawButton);
      }
      else
      {
         KeyboardUser.HandleReleasedButton(u8LastKey);
      }

      u8LastKey = u8RawButton;
   }

   private:
   KeyboardUserType& KeyboardUser;
   public:
   unsigned char u8LastKey;
};