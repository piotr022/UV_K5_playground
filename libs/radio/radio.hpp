#pragma once
#include "callback.hpp"
#include "registers.hpp"
#include "system.hpp"
#include <functional>

static constexpr auto operator""_Hz(unsigned long long Hertz)
{
   return Hertz / 10;
}

static constexpr auto operator""_KHz(unsigned long long KiloHertz)
{
   return KiloHertz * 1000_Hz;
}

static constexpr auto operator""_MHz(unsigned long long KiloHertz)
{
   return KiloHertz * 1000_KHz;
}

namespace Radio
{
   enum eIrq : unsigned short
   {
      FifoAlmostFull = 1 << 12,
      RxDone = 1 << 13,
   };

   enum class eFskMode : unsigned char
   {
      Fsk1200 = 0,
      Ffsk1200_1200_1800,
      Ffsk1200_1200_2400,
      NoaaSame,
      ModesCount,
   };

   struct TFskModeBits
   {
      unsigned char u8TxModeBits;
      unsigned char u8RxBandWidthBits;
      unsigned char u8RxModeBits;
   };

   constexpr TFskModeBits ModesBits[(int)eFskMode::ModesCount] = {
       // Tx mode    Rx badwitdh       Rx Mode
       {0b000, 0b000, 0b000}, // Fsk1200
       {0b001, 0b001, 0b111}, // Ffsk1200_1200_1800
       {0b011, 0b100, 0b100}, // Ffsk1200_1200_2400
       {0b101, 0b010, 0b000}, // NoaaSame
   };

   enum class eState : unsigned char
   {
      Idle,
      RxPending,
   };

   using CallbackRxDoneType = CCallback<void, unsigned char, bool>;
   template <const System::TOrgFunctions &Fw>
   class CBK4819
   {
      CallbackRxDoneType CallbackRxDone;
      unsigned char *p8RxBuff;
      unsigned char u8RxBuffSize;

   public:
      CBK4819() : State(eState::Idle), u16RxDataLen(0){};

      // void SetFrequency(unsigned int u32FrequencyD10)
      // {
      //    Fw.BK4819WriteFrequency(u32FrequencyD10);
      // }

      static unsigned int GetFrequency()
      {
         return (Fw.BK4819Read(0x39) << 16) | Fw.BK4819Read(0x38);
      }

      static signed short GetRssi()
      {
         short s16Rssi = ((Fw.BK4819Read(0x67) >> 1) & 0xFF);
         return s16Rssi - 160;
      }

      bool IsTx() { return Fw.BK4819Read(0x30) & 0b10; }

      bool IsSqlOpen() { return Fw.BK4819Read(0x0C) & 0b10; }

      static void SetFrequency(unsigned int u32Freq)
      {
         Fw.BK4819Write(0x39, ((u32Freq >> 16) & 0xFFFF));
         Fw.BK4819Write(0x38, (u32Freq & 0xFFFF));
         auto OldReg = Fw.BK4819Read(0x30);
         Fw.BK4819Write(0x30, 0);
         Fw.BK4819Write(0x30, OldReg);
      }

      void SetAgcTable(unsigned short *p16AgcTable)
      {
         for (unsigned char i = 0; i < 5; i++)
         {
            Fw.BK4819Write(0x10 + i, p16AgcTable[i]);
         }
      }

      void GetAgcTable(unsigned short *p16AgcTable)
      {
         for (unsigned char i = 0; i < 5; i++)
         {
            p16AgcTable[i] = Fw.BK4819Read(0x10 + i);
         }
      }

      void SetDeviationPresent(unsigned char u8Present)
      {
         auto Reg40 = Fw.BK4819Read(0x40);
         Reg40 &= ~(1 << 12);
         Reg40 |= (u8Present << 12);
         Fw.BK4819Write(0x40, Reg40);
      }

      void SetCalibration(unsigned char bOn)
      {
         auto Reg30 = Fw.BK4819Read(0x31);
         Reg30 &= ~(1 << 3);
         Reg30 |= (bOn << 3);
         Fw.BK4819Write(0x31, Reg30);
      }

      unsigned char GetAFAmplitude() { return Fw.BK4819Read(0x6F) & 0b1111111; }

      unsigned short GetVoiceAmplitude()
      {
         // Fw.BK4819Write(0x64, 0);
         return Fw.BK4819Read(0x64);
      }

      static void ToggleAFDAC(bool enabled)
      {
         auto Reg = Fw.BK4819Read(0x30);
         Reg &= ~(1 << 9);
         if (enabled)
            Reg |= (1 << 9);
         Fw.BK4819Write(0x30, Reg);
      }

      static void ToggleRXDSP(bool enabled)
      {
         auto Reg = Fw.BK4819Read(0x30);
         Reg &= ~1;
         if (enabled)
            Reg |= 1;
         Fw.BK4819Write(0x30, Reg);
      }

      void SendSyncAirCopyMode72(unsigned char *p8Data)
      {
         Fw.BK4819ConfigureAndStartTxFsk();
         Fw.AirCopyFskSetup();
         Fw.AirCopy72(p8Data);
         Fw.BK4819SetGpio(1, false);
      }

      void DisablePa() { Fw.BK4819Write(0x30, Fw.BK4819Read(0x30) & ~0b1010); }

      void SetFskMode(eFskMode Mode)
      {
         auto const &ModeParams = ModesBits[(int)Mode];
         auto Reg58 = Fw.BK4819Read(0x58);
         Reg58 &= ~((0b111 << 1) | (0b111 << 10) | (0b111 << 13));
         Reg58 |= (ModeParams.u8RxBandWidthBits << 1) |
                  (ModeParams.u8RxModeBits << 10) | (ModeParams.u8TxModeBits << 13);
         Fw.BK4819Write(0x58, 0);
         Fw.BK4819Write(0x58, Reg58);
      }

      void FixIrqEnRegister() // original firmware overrides IRQ_EN reg, so we need
                              // to reenable it
      {
         auto const OldIrqEnReg = Fw.BK4819Read(0x3F);
         if ((OldIrqEnReg & (eIrq::FifoAlmostFull | eIrq::RxDone)) !=
             (eIrq::FifoAlmostFull | eIrq::RxDone))
         {
            Fw.BK4819Write(0x3F, OldIrqEnReg | eIrq::FifoAlmostFull | eIrq::RxDone);
         }
      }

      void RecieveAsyncAirCopyMode(unsigned char *p8Data, unsigned char u8DataLen,
                                   CallbackRxDoneType Callback)
      {
         if (!p8Data || !u8DataLen)
         {
            return;
         }

         CallbackRxDone = Callback;
         p8RxBuff = p8Data;
         u8RxBuffSize = u8DataLen;
         u16RxDataLen = 0;

         Fw.AirCopyFskSetup();
         Fw.BK4819ConfigureAndStartRxFsk();
         State = eState::RxPending;
      }

      void DisableFskModem()
      {
         auto const FskSettings = Fw.BK4819Read(0x58);
         Fw.BK4819Write(0x58, FskSettings & ~1);
      }

      void ClearRxFifoBuff()
      {
         auto const Reg59 = Fw.BK4819Read(0x59);
         Fw.BK4819Write(0x59, 1 << 14);
         Fw.BK4819Write(0x59, Reg59);
      }

      unsigned short GetIrqReg()
      {
         Fw.BK4819Write(0x2, 0);
         return Fw.BK4819Read(0x2);
      }

      bool CheckCrc() { return Fw.BK4819Read(0x0B) & (1 << 4); }

      bool IsLockedByOrgFw() { return !(GPIOC->DATA & 0b1); }

      unsigned short u16DebugIrq;

      void HandleRxDone()
      {
         ClearRxFifoBuff();
         DisableFskModem();
         State = eState::Idle;
         CallbackRxDone(u16RxDataLen, CheckCrc());
      }

      void InterruptHandler()
      {
         if (IsLockedByOrgFw())
         {
            return;
         }

         if (State == eState::RxPending)
         {
            FixIrqEnRegister();
            if (!(Fw.BK4819Read(0x0C) & 1)) // irq request indicator
            {
               return;
            }

            auto const IrqReg = GetIrqReg();

            if (IrqReg & eIrq::RxDone)
            {
               // HandleRxDone();
            }

            if (IrqReg & eIrq::FifoAlmostFull)
            {
               HandleFifoAlmostFull();
            }
         }
      }

      eState State;
      unsigned short u16RxDataLen;

   private:
      void HandleFifoAlmostFull()
      {
         for (unsigned char i = 0; i < 4; i++)
         {
            auto const RxData = Fw.BK4819Read(0x5F);
            if (p8RxBuff && u16RxDataLen < u8RxBuffSize - 2)
            {
               memcpy(p8RxBuff + u16RxDataLen, &RxData, 2);
            }

            u16RxDataLen += 2;
         }

         if (u16RxDataLen >= u8RxBuffSize)
         {
            State = eState::Idle;
            CallbackRxDone(u8RxBuffSize, CheckCrc());
         }
      }
   };
} // namespace Radio
