namespace Hardware
{
   struct TPower
   {
      void EnableDbg();
   };

   struct TSystem
   {
      static void Delay(unsigned int u32Ticks);
   };

   struct TFlashLight
   {
      TFlashLight(TSystem& Sys) :System(Sys){};
      void On();
      void Off();
      void Toggle();
      void BlinkSync(unsigned char u8BlinksCnt);
      TSystem& System;
   };

   struct THardware
   {
      TPower Power;
      TSystem System;
      TFlashLight FlashLight = {System};
   };
}