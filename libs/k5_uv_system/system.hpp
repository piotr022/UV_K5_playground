#pragma once
namespace System
{
   using VoidFxPointer = void(*)(void);
   static constexpr auto CortexM0VectorsCnt = 16+32;

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
         unsigned int u32Centered,
         unsigned int u32MenuLen,
         unsigned int u32AsciiIdx,
         unsigned int u32Unknown0,
         unsigned int u32Unknown1,
         unsigned int u32Unknown2,
         unsigned int u32Unknown3
         );

      void(*FillScreenMemory)(unsigned int u32Param1);
      void(*DelayMs)(unsigned int u32Ms);
      void(*DelayUs)(unsigned int u32Us);
      int(*WriteSerialData)(unsigned char* p8Data, unsigned char u8Len);
      void(*BK4819Write)(unsigned int u32Address, unsigned int u32Data);
      unsigned int(*BK4819Read)(unsigned int u32Address);
      void(*FlushFramebufferToScreen)(void);
      unsigned int(*PollKeyboard)(void);
   };

   struct TOrgData
   {
      unsigned char* pDisplayBuffer;
      unsigned char* pSmallDigs;
      unsigned char* pSmallLeters;
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
      .PollKeyboard = (decltype(TOrgFunctions::PollKeyboard) (0xb0b8 + 1))
   };

   inline const TOrgData OrgData_01_26 =
   {
      .pDisplayBuffer = (decltype(TOrgData::pDisplayBuffer)) 0x20000704,
      .pSmallDigs = (decltype(TOrgData::pSmallDigs)) 0xD620,
      .pSmallLeters = (decltype(TOrgData::pSmallLeters)) 0xD348,
   };
}

extern "C" void __libc_init_array();