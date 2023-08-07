#include "hardware/hardware.hpp"
#include "radio.hpp"
#include "registers.hpp"
#include "spectrum.hpp"
#include "system.hpp"
#include "uv_k5_display.hpp"
#include <string.h>

Hardware::THardware Hw;
Radio::CBK4819 RadioDriver;
CSpectrum<RadioDriver> Spectrum;

int main() {
  IRQ_RESET();
  return 0;
}

extern "C" void Reset_Handler() { IRQ_RESET(); }

extern "C" void SysTick_Handler() {
  static bool bFirstInit = false;
  if (!bFirstInit) {
    System::CopyDataSection();
    __libc_init_array();
    bFirstInit = true;
  }

  Spectrum.Handle();

  IRQ_SYSTICK();
}
