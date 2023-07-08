#pragma once
namespace System
{
   using VoidFxPointer = void(*)(void);
   static constexpr auto CortexM0VectorsCnt = 16+32;

   struct TCortexM0Stacking
   {
      unsigned int R0;
      unsigned int R1;
      unsigned int R2;
      unsigned int R3;
      unsigned int R12;
      unsigned int LR;
      unsigned int PC;
      unsigned int xPSR;
   };

   struct TVectorTable
   {
      VoidFxPointer Vectors[CortexM0VectorsCnt];
   };

   template <class T, auto... Args>
   struct TStaticWrapper
   {
      static T& GetInstance()
      {
         static T StaticObj(Args...);
         return StaticObj;
      }
   };

   void JumpToOrginalFw();
   void CopyDataSection();

   struct TOrgFunctions
   {
      void(*PrintTextOnScreen)(const char* U8Text, 
         unsigned int u32StartPixel,
         unsigned int u32StopPixel,
         unsigned int u32LineNumber,
         unsigned int u32PxPerChar,
         unsigned int u32Centered
         );

      void(*FillScreenMemory)(unsigned int u32Param1);
      void(*DelayMs)(unsigned int u32Ms);
      void(*DelayUs)(unsigned int u32Us);
      int(*WriteSerialData)(unsigned char* p8Data, unsigned char u8Len);
      void(*BK4819Write)(unsigned int u32Address, unsigned int u32Data);
      unsigned int(*BK4819Read)(unsigned int u32Address);
      void(*FlushFramebufferToScreen)(void);
      unsigned int(*PollKeyboard)(void);
      char* (*sprintf)(char *, const char *, ...);
      void(*FillWithZero)(unsigned char* p8Data, unsigned int u32Size);
      char* (*FormatString)(char *, const char *, ...);

   };

   struct TOrgData
   {
      unsigned char* pDisplayBuffer;
      unsigned char* pSmallDigs;
      unsigned char* pSmallLeters;
      unsigned char *p8FlashLightStatus;
   };

   inline const TOrgFunctions OrgFunc_01_26 =
   {
      .PrintTextOnScreen = (decltype(TOrgFunctions::PrintTextOnScreen))(0x874C + 1),
      .FillScreenMemory = (decltype(TOrgFunctions::FillScreenMemory))(0xb70c + 1),
      .DelayMs = (decltype(TOrgFunctions::DelayMs))   (0xD0EE + 1),
      .DelayUs = (decltype(TOrgFunctions::DelayUs))   (0xD100 + 1),
      .WriteSerialData = (int(*)(unsigned char*, unsigned char))(0xBE44 + 1),
      .BK4819Write = (decltype(TOrgFunctions::BK4819Write) (0xAF00 + 1)),
      .BK4819Read = (decltype(TOrgFunctions::BK4819Read) (0xA960 + 1)),
      .FlushFramebufferToScreen = (decltype(TOrgFunctions::FlushFramebufferToScreen) (0xB638 + 1)),
      .PollKeyboard = (decltype(TOrgFunctions::PollKeyboard) (0xb0b8 + 1)),
      .sprintf = (decltype(TOrgFunctions::sprintf) (0xc8ec + 1)),
      .FillWithZero = (decltype(TOrgFunctions::FillWithZero) (0x1AA + 1)),
      .FormatString = (decltype(TOrgFunctions::FormatString) (0xC6E8 + 1)),
   };

   inline const TOrgData OrgData_01_26 =
   {
      .pDisplayBuffer = (decltype(TOrgData::pDisplayBuffer)) 0x20000704,
      .pSmallDigs = (decltype(TOrgData::pSmallDigs)) 0xD620,
      .pSmallLeters = (decltype(TOrgData::pSmallLeters)) 0xD348,
      .p8FlashLightStatus = (decltype(TOrgData::p8FlashLightStatus)) 0x200003b3,
   };
}

extern "C" void __libc_init_array();