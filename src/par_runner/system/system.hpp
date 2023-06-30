namespace System
{
   using VoidFxPointer = void(*)(void);
   static constexpr auto CortexM0VectorsCnt = 16+32;

   struct TVectorTable
   {
      VoidFxPointer Vectors[CortexM0VectorsCnt];
   };

   void JumpToOrginalFw();

   struct TOrgFunctions
   {
      void(*PrintTextOnScreen)(const char* U8Text, 
         unsigned int u32StartPixel,
         unsigned int u32StopPixel,
         unsigned int u32LineNumber,
         unsigned int u32Centered);

      void(*FillScreenMemory)(unsigned int u32Param1);
      void(*DelayMs)(unsigned int u32Ms);
      void(*DelayUs)(unsigned int u32Us);
      int(*WriteSerialData)(unsigned char* p8Data, unsigned char u8Len);
      void(*BK4819Write)(unsigned int u32Address, unsigned int u32Data);
      unsigned int(*BK4819Read)(unsigned int u32Address);
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
   };
}