#pragma once
#include "system.hpp"
#include "registers.hpp"
#include <functional>

namespace Radio
{
   enum class eModulation : unsigned char
   {
      Ffsk,
      Fsk,
   };

   struct TRadioConf
   {
      eModulation Modulation;
   };

   enum class eState : unsigned char
   {
      Idle,
      RxPending,
   };

   struct IRadioUser
   {
      virtual void RxDoneHandler(){};
   };

   template <const System::TOrgFunctions &Fw>
   class CBK4819
   {
   public:
      CBK4819(){};

      void SetFrequency(unsigned int u32FrequencyD10)
      {
         Fw.BK4819WriteFrequency(u32FrequencyD10);
      }

      unsigned int GetFrequency()
      {
         return 0;
      }

      void SendSyncAirCopyMode72(unsigned char *p8Data)
      {
         Fw.BK4819ConfigureAndStartTxFsk();
         Fw.AirCopyFskSetup();
         Fw.AirCopy72(p8Data);
      }

      void RecieveAsyncAirCopyMode(unsigned char *p8Data, unsigned char u8DataLen, IRadioUser* pUser)
      {
         if (!p8Data || !u8DataLen)
         {
            return;
         }

         pRadioUser = pUser;
         p8RxBuff = p8Data;
         u8RxBuffSize = u8DataLen;

         Fw.AirCopyFskSetup();
         Fw.BK4819Write(0x59, 1 << 14); // clear rx fifo
         Fw.BK4819Write(0x59, 0b0011'0000'0110'1000); // enable rx
         Fw.BK4819Write(0x3f, 1 << 13); // enable rx done irq

         Fw.BK4819Write(0x30, 0); 
         Fw.BK4819Write(0x30, 0b1011'1101'1111'0001);
         State = eState::RxPending;
      }

      bool IsLockedByOrgFw()
      {
         return !(GPIOC->DATA & 0b1);
      }

      void InterruptHandler()
      {
         if (IsLockedByOrgFw())
         {
            return;
         }

         if (State == eState::RxPending)
         {
            auto const IrqReg = Fw.BK4819Read(0x02);
            if (IrqReg & (1 << 13))
            {
               State = eState::Idle;
               Fw.BK4819Write(0x2, 0);
               unsigned char u8DataLen = Fw.BK4819Read(0x02);
               u8DataLen = (u8DataLen >> 8) & 0xFF;
               if (!p8RxBuff)
                  return;

               u8DataLen = u8RxBuffSize < u8DataLen ? u8RxBuffSize : u8DataLen;
               for (unsigned char i = 0; i < u8DataLen; i++)
               {
                  p8RxBuff[i] = Fw.BK4819Read(0x5F);
               }
               
               if(pRadioUser)
               {
                  pRadioUser->RxDoneHandler();
               }
            }
         }
      }

   private:
      IRadioUser* pRadioUser;
      unsigned char *p8RxBuff;
      unsigned char u8RxBuffSize;
      eState State;
   };
}