#pragma once

extern "C" {
      void PrintTextOnScreen(const char* U8Text,
         unsigned int u32StartPixel,
         unsigned int u32StopPixel,
         unsigned int u32LineNumber,
         unsigned int u32PxPerChar,
         unsigned int u32Centered
         );

      void DelayMs(unsigned int u32Ms);
      void DelayUs(unsigned int u32Us);
      int WriteSerialData(unsigned char* p8Data, unsigned char u8Len);
      void BK4819Write(unsigned int u32Address, unsigned int u32Data);
      unsigned int BK4819Read(unsigned int u32Address);
      void FlushFramebufferToScreen(void);
      unsigned int PollKeyboard(void);
      char* FormatString(char *, const char *, ...);
      void PrintSmallDigits(unsigned int u32Len, const int* p32Number, int s32X, int s32Y);
      void PrintFrequency(int frequency,int xpos,int ypos,int param_4,int param_5);
      void AirCopy72(unsigned char*);
      void AirCopyFskSetup();
      void BK4819Reset();
      int IntDivide(int s32Divident, int s32Divisor);
      int Strlen(const char *string);
      void BK4819SetChannelBandwidth(bool narrow);
      void BK4819WriteFrequency(unsigned int u32Frequency);
      void BK4819SetPaGain(unsigned short u16PaBias, unsigned int u32Frequency);
      void BK4819ConfigureAndStartTxFsk();
      void BK4819ConfigureAndStartRxFsk();
      void BK4819SetGpio(unsigned int u32Pin, bool bState);
      void FlushStatusbarBufferToScreen();
      void UpdateStatusBar();
      void AdcReadout(unsigned short* p16Data1, unsigned short* p16Data2);
      void SomeAmStuff(unsigned int u32Param);
      void IRQ_RESET(void);
      void IRQ_SYSTICK(void);
      // API available in the SRAM overlay installed by FW
      void ConfigureTrimValuesFromNVR(void);
      unsigned int FLASH_ReadNvrWord(unsigned int u32Offset);
      void SystemReset(void);
      void FLASH_SetProgramTime(void);
      void FLASH_SetMode(unsigned int u32Mode);
      void FLASH_WakeFromDeepSleep(void);
      void FLASH_SetEraseTime(void);
      void FLASH_SetReadMode(unsigned int u32Mode);
      void FLASH_Set_NVR_SEL(unsigned int u32Sel);
      unsigned int FLASH_ReadByAPB(unsigned int u32Offset);
      unsigned int FLASH_ReadByAHB(unsigned int u32Offset);
      void FLASH_Unlock(void);
      void FLASH_Lock(void);
      void FLASH_MaskUnlock(void);
      void FLASH_SetMaskSel(unsigned int u32Mask);
      void FLASH_MaskLock(void);
      void FLASH_Init(unsigned int u32ReadMode);
      void FLASH_Start(void);
      int FLASH_IsInitBusy(void);
      int FLASH_IsBusy(void);
      int FLASH_RebootToBootloader(void);
};

extern unsigned char gDisplayBuffer[];
extern unsigned char gSmallDigs[];
extern unsigned char gSmallLeters[];
extern unsigned char gFlashLightStatus;
extern unsigned char gStatusBarData[];
extern unsigned short gVoltage;

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
}

extern "C" void __libc_init_array();
