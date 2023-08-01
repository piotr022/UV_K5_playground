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

      void(*DelayMs)(unsigned int u32Ms);
      void(*DelayUs)(unsigned int u32Us);
      int(*WriteSerialData)(unsigned char* p8Data, unsigned char u8Len);
      void(*BK4819Write)(unsigned int u32Address, unsigned int u32Data);
      unsigned int(*BK4819Read)(unsigned int u32Address);
      void(*FlushFramebufferToScreen)(void);
      unsigned int(*PollKeyboard)(void);
      char* (*FormatString)(char *, const char *, ...);
      void(*PrintSmallDigits)(unsigned int u32Len, const int* p32Number, int s32X, int s32Y);
      void(*PrintFrequency)(int frequency,int xpos,int ypos,int param_4,int param_5);
      void(*AirCopy72)(unsigned char*);
      void(*AirCopyFskSetup)();
      void(*BK4819Reset)();
      int(*IntDivide)(int s32Divident, int s32Divisor);
      void(*BK4819WriteFrequency)(unsigned int u32Frequency);
      void(*BK4819SetPaGain)(unsigned short u16PaBias, unsigned int u32Frequency);
      void(*BK4819ConfigureAndStartTxFsk)();
      void(*BK4819ConfigureAndStartRxFsk)();
      void(*BK4819SetGpio)(unsigned int u32Pin, bool bState);
      void(*FlushStatusbarBufferToScreen)();
      void(*UpdateStatusBar)();
      void(*AdcReadout)(unsigned short* p16Data1, unsigned short* p16Data2);
      void(*SomeAmStuff)(unsigned int u32Param);
      void (*IRQ_RESET)(void);
      void (*IRQ_SYSTICK)(void);
      // API available in the SRAM overlay installed by FW
      void (*ConfigureTrimValuesFromNVR)(void);
      unsigned int (*FLASH_ReadNvrWord)(unsigned int u32Offset);
      void (*SystemReset)(void);
      void (*FLASH_SetProgramTime)(void);
      void (*FLASH_SetMode)(unsigned int u32Mode);
      void (*FLASH_WakeFromDeepSleep)(void);
      void (*FLASH_SetEraseTime)(void);
      void (*FLASH_SetReadMode)(unsigned int u32Mode);
      void (*FLASH_Set_NVR_SEL)(unsigned int u32Sel);
      unsigned int (*FLASH_ReadByAPB)(unsigned int u32Offset);
      unsigned int (*FLASH_ReadByAHB)(unsigned int u32Offset);
      void (*FLASH_Unlock)(void);
      void (*FLASH_Lock)(void);
      void (*FLASH_MaskUnlock)(void);
      void (*FLASH_SetMaskSel)(unsigned int u32Mask);
      void (*FLASH_MaskLock)(void);
      void (*FLASH_Init)(unsigned int u32ReadMode);
      void (*FLASH_Start)(void);
      int (*FLASH_IsInitBusy)(void);
      int (*FLASH_IsBusy)(void);
      int (*FLASH_RebootToBootloader)(void);
   };

   struct TOrgData
   {
      unsigned char* pDisplayBuffer;
      unsigned char* pSmallDigs;
      unsigned char* pSmallLeters;
      unsigned char* p8FlashLightStatus;
      unsigned char* pStatusBarData;
      unsigned short* p16Voltage;
   };

   inline const TOrgFunctions OrgFunc_01_26 =
   {
      .PrintTextOnScreen = (decltype(TOrgFunctions::PrintTextOnScreen))(0x874C + 1),
      .DelayMs = (decltype(TOrgFunctions::DelayMs))   (0xD0EC + 1),
      .DelayUs = (decltype(TOrgFunctions::DelayUs))   (0xD100 + 1),
      .WriteSerialData = (int(*)(unsigned char*, unsigned char))(0xBE44 + 1),
      .BK4819Write = (decltype(TOrgFunctions::BK4819Write) (0xAF00 + 1)),
      .BK4819Read = (decltype(TOrgFunctions::BK4819Read) (0xA960 + 1)),
      .FlushFramebufferToScreen = (decltype(TOrgFunctions::FlushFramebufferToScreen) (0xB638 + 1)),
      .PollKeyboard = (decltype(TOrgFunctions::PollKeyboard) (0xb0b8 + 1)),
      .FormatString = (decltype(TOrgFunctions::FormatString) (0xC6E8 + 1)),
      .PrintSmallDigits = (decltype(TOrgFunctions::PrintSmallDigits) (0x870C + 1)),
      .PrintFrequency = (decltype(TOrgFunctions::PrintFrequency) (0x864C + 1)),
      .AirCopy72 = (decltype(TOrgFunctions::AirCopy72) (0xA67C + 1)),
      .AirCopyFskSetup = (decltype(TOrgFunctions::AirCopyFskSetup) (0xA518 + 1)),
      .BK4819Reset = (decltype(TOrgFunctions::BK4819Reset) (0xa7cc + 1)),
      .IntDivide = (decltype(TOrgFunctions::IntDivide) (0x128 + 1)),
      .BK4819WriteFrequency = (decltype(TOrgFunctions::BK4819WriteFrequency) (0xaabc + 1)),
      .BK4819SetPaGain = (decltype(TOrgFunctions::BK4819SetPaGain) (0xaad4 + 1)),
      .BK4819ConfigureAndStartTxFsk = (decltype(TOrgFunctions::BK4819ConfigureAndStartTxFsk) (0x1cd8 + 1)),
      .BK4819ConfigureAndStartRxFsk = (decltype(TOrgFunctions::BK4819ConfigureAndStartRxFsk) (0xa63c + 1)),
      .BK4819SetGpio = (decltype(TOrgFunctions::BK4819SetGpio) (0xa794 + 1)),
      .FlushStatusbarBufferToScreen = (decltype(TOrgFunctions::FlushStatusbarBufferToScreen) (0xb6b0 + 1)),
      .UpdateStatusBar = (decltype(TOrgFunctions::UpdateStatusBar) (0x9c10 + 1)),
      .AdcReadout = (decltype(TOrgFunctions::AdcReadout) (0x9d7c + 1)),
      .SomeAmStuff = (decltype(TOrgFunctions::SomeAmStuff) (0xc158 + 1)),
      .IRQ_RESET = (decltype(TOrgFunctions::IRQ_RESET) (0xd4 + 1)),
      .IRQ_SYSTICK = (decltype(TOrgFunctions::IRQ_SYSTICK) (0xc398 + 1)),
      .ConfigureTrimValuesFromNVR = (decltype(TOrgFunctions::ConfigureTrimValuesFromNVR)) (0x20000230 + 1),
      .FLASH_ReadNvrWord = (decltype(TOrgFunctions::FLASH_ReadNvrWord)) (0x20000214 + 1),
      .SystemReset = (decltype(TOrgFunctions::SystemReset)) (0x200001f8 + 1),
      .FLASH_SetProgramTime = (decltype(TOrgFunctions::FLASH_SetProgramTime)) (0x200001d4 + 1),
      .FLASH_SetMode = (decltype(TOrgFunctions::FLASH_SetMode)) (0x200001b4 + 1),
      .FLASH_WakeFromDeepSleep = (decltype(TOrgFunctions::FLASH_WakeFromDeepSleep)) (0x20000194 + 1),
      .FLASH_SetEraseTime = (decltype(TOrgFunctions::FLASH_SetEraseTime)) (0x20000170 + 1),
      .FLASH_SetReadMode = (decltype(TOrgFunctions::FLASH_SetReadMode)) (0x20000148 + 1),
      .FLASH_Set_NVR_SEL = (decltype(TOrgFunctions::FLASH_Set_NVR_SEL)) (0x20000128 + 1),
      .FLASH_ReadByAPB = (decltype(TOrgFunctions::FLASH_ReadByAPB)) (0x200000e8 + 1),
      .FLASH_ReadByAHB = (decltype(TOrgFunctions::FLASH_ReadByAHB)) (0x200000dc + 1),
      .FLASH_Unlock = (decltype(TOrgFunctions::FLASH_Unlock)) (0x200000d0 + 1),
      .FLASH_Lock = (decltype(TOrgFunctions::FLASH_Lock)) (0x200000c4 + 1),
      .FLASH_MaskUnlock = (decltype(TOrgFunctions::FLASH_MaskUnlock)) (0x200000b0 + 1),
      .FLASH_SetMaskSel = (decltype(TOrgFunctions::FLASH_SetMaskSel)) (0x20000094 + 1),
      .FLASH_MaskLock = (decltype(TOrgFunctions::FLASH_MaskLock)) (0x20000080 + 1),
      .FLASH_Init = (decltype(TOrgFunctions::FLASH_Init)) (0x2000005c + 1),
      .FLASH_Start = (decltype(TOrgFunctions::FLASH_Start)) (0x20000044 + 1),
      .FLASH_IsInitBusy = (decltype(TOrgFunctions::FLASH_IsInitBusy)) (0x2000002c + 1),
      .FLASH_IsBusy = (decltype(TOrgFunctions::FLASH_IsBusy)) (0x20000014 + 1),
      .FLASH_RebootToBootloader = (decltype(TOrgFunctions::FLASH_RebootToBootloader)) (0x20000000 + 1),
   };

   inline const TOrgData OrgData_01_26 =
   {
      .pDisplayBuffer = (decltype(TOrgData::pDisplayBuffer)) 0x20000704,
      .pSmallDigs = (decltype(TOrgData::pSmallDigs)) 0xD620,
      .pSmallLeters = (decltype(TOrgData::pSmallLeters)) 0xD348,
      .p8FlashLightStatus = (decltype(TOrgData::p8FlashLightStatus)) 0x200003b3,
      .pStatusBarData = (decltype(TOrgData::pStatusBarData)) 0x20000684,
      .p16Voltage = (decltype(TOrgData::p16Voltage)) 0x20000406,
   };
}

extern "C" void __libc_init_array();
