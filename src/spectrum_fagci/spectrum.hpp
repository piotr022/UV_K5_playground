#pragma once
#include "system.hpp"
#include "uv_k5_display.hpp"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

static constexpr auto operator""_Hz(unsigned long long Hertz) {
  return Hertz / 10;
}

static constexpr auto operator""_KHz(unsigned long long KiloHertz) {
  return KiloHertz * 1000_Hz;
}

static constexpr auto operator""_MHz(unsigned long long KiloHertz) {
  return KiloHertz * 1000_KHz;
}

template <const System::TOrgFunctions &Fw, const System::TOrgData &FwData>
class CSpectrum {
public:
  static constexpr u8 EnableKey = 13;
  static constexpr auto DrawingSizeY = 16 + 6 * 8;
  static constexpr auto DrawingEndY = 42;
  static constexpr u32 BarPos = 640; // 5 * 128
  u8 rssiHistory[128] = {};
  u8 rssiMin = 255, rssiMax = 0;
  u8 highestPeakX = 0;
  u8 highestPeakT = 0;
  u8 highestPeakRssi = 0;
  u32 highestPeakF = 0;
  u32 FStart;

  CSpectrum()
      : DisplayBuff(FwData.pDisplayBuffer), FontSmallNr(FwData.pSmallDigs),
        Display(DisplayBuff), bDisplayCleared(true), u32ScanRange(1600_KHz),
        working(0), scanDelay(800), isUserInput(false) {
    Display.SetFont(&FontSmallNr);
  };

  inline void Measure() {
    u8 rssi = 0;
    u8 xPeak = 0;
    u32 fPeak, fMeasure = FStart;

    rssiMin = 255;
    rssiMax = 0;

    for (u8 i = 0; i < 64; ++i, fMeasure += 25_KHz) {
      rssi = rssiHistory[i] = GetRssi(fMeasure);
      if (rssi > rssiMax) {
        rssiMax = rssi;
        fPeak = fMeasure;
        xPeak = i << 1;
      }
      if (rssi < rssiMin) {
        rssiMin = rssi;
      }
    }

    ++highestPeakT;
    if (rssiMax > highestPeakRssi || highestPeakT >= 8) {
      highestPeakT = 0;
      highestPeakRssi = rssiMax;
      highestPeakX = xPeak;
      highestPeakF = fPeak;
    }
  }

  inline void DrawSpectrum() {
    u8 rssi, sub;
    for (u8 i = 0; i < 128; ++i) {
      rssi = rssiHistory[i >> 1];
      sub = clamp(rssi - rssiMin, 0, DrawingSizeY);
      Display.DrawHLine(DrawingEndY - sub, DrawingEndY, i);
    }
    Display.SetCoursor(0, 0);
    Display.PrintFixedDigitsNumber2(scanDelay, 0);

    Display.SetCoursor(6, 0);
    Display.PrintFixedDigitsNumber2(currentFreq);

    Display.SetCoursor(6, 8 * 2 + 10 * 7);
    Display.PrintFixedDigitsNumber2(highestPeakF);
  }

  inline void DrawTicks() {
    u32 f = modulo(FStart, 1_MHz);
    for (u8 i = 0; i < 128; ++i, f += 12500_Hz) {
      u8 barValue = 0b00001000;
      modulo(f, 100_KHz) || (barValue |= 0b00010000);
      modulo(f, 500_KHz) || (barValue |= 0b00100000);
      modulo(f, 1_MHz) || (barValue |= 0b11000000);

      *(FwData.pDisplayBuffer + BarPos + i) |= barValue;
    }

    // center
    *(FwData.pDisplayBuffer + BarPos + 64) |= 0b10101010;
  }

  inline void DrawArrow(u8 x) {
    u8 *peakPos = FwData.pDisplayBuffer + BarPos + x;
    x > 1 && (*(peakPos - 2) |= 0b01000000);
    x > 0 && (*(peakPos - 1) |= 0b01100000);
    (*(peakPos) |= 0b01110000);
    x < 127 && (*(peakPos + 1) |= 0b01100000);
    x < 128 && (*(peakPos + 2) |= 0b01000000);
  }

  void HandleUserInput() {
    switch (u8LastBtnPressed) {
    case 1:
      UpdateScanDelay(50);
      break;
    case 7:
      UpdateScanDelay(-50);
      break;
    case 11: // up
      UpdateCurrentFreq(100_KHz);
      break;
    case 12: // down
      UpdateCurrentFreq(-100_KHz);
      break;
    default:
      isUserInput = false;
    }
  }

  void Render() {
    DrawTicks();
    DrawArrow(highestPeakX);
    DrawSpectrum();
  }

  void Update() {
    if (bDisplayCleared) {
      currentFreq = GetFrequency();
      OnUserInput();
      u16OldAfSettings = Fw.BK4819Read(0x47);
      Fw.BK4819Write(0x47, 0); // mute AF during scan
    }
    bDisplayCleared = false;

    HandleUserInput();

    Measure();
  }

  void UpdateScanDelay(short diff) {
    scanDelay += diff;
    scanDelay = clamp(scanDelay, 100, 2000);
    OnUserInput();
  }

  void UpdateCurrentFreq(long long diff) {
    currentFreq += diff;
    currentFreq = clamp(currentFreq, 18_MHz, 1300_MHz);
    OnUserInput();
  }

  inline void OnUserInput() {
    isUserInput = true;
    FStart = currentFreq - (u32ScanRange >> 1);

    // reset peak
    highestPeakT = 0;
    highestPeakRssi = 0;
    highestPeakX = 0;
    highestPeakF = 0;
  }

  void Handle() {
    if (!(GPIOC->DATA & 0b1)) {
      return;
    }

    if (!FreeToDraw()) {
      RestoreParams();
      return;
    }

    Update();

    DisplayBuff.ClearAll();
    Render();
    Fw.FlushFramebufferToScreen();
  }

private:
  void RestoreParams() {
    if (!bDisplayCleared) {
      bDisplayCleared = true;
      DisplayBuff.ClearAll();
      Fw.FlushFramebufferToScreen();
      SetFrequency(currentFreq);
      Fw.BK4819Write(0x47, u16OldAfSettings); // set previous AF settings
    }
  }

  void SetFrequency(unsigned int u32Freq) {
    Fw.BK4819Write(0x39, ((u32Freq >> 16) & 0xFFFF));
    Fw.BK4819Write(0x38, (u32Freq & 0xFFFF));
    Fw.BK4819Write(0x30, 0);
    Fw.BK4819Write(0x30, 0xbff1);
  }

  u8 GetRssi(u32 f, u16 delay = 800) {
    SetFrequency(f);
    Fw.DelayUs(delay);
    return Fw.BK4819Read(0x67);
  }

  u32 GetFrequency() {
    return ((Fw.BK4819Read(0x39) << 16) | Fw.BK4819Read(0x38));
  }

  bool FreeToDraw() {
    bool bFlashlight = (GPIOC->DATA & GPIO_PIN_3);
    if (bFlashlight) {
      working = true;
      GPIOC->DATA &= ~GPIO_PIN_3;
      *FwData.p8FlashLightStatus = 3;
    }

    if (working) {
      u8LastBtnPressed = Fw.PollKeyboard();
    }

    bool bPtt = !(GPIOC->DATA & GPIO_PIN_5);
    if (bPtt || u8LastBtnPressed == EnableKey) {
      working = false;
    }

    return working;
  }

  unsigned clamp(unsigned v, unsigned min, unsigned max) {
    if (v < min)
      return min;
    if (v > max)
      return max;
    return v;
  }

  unsigned modulo(unsigned num, unsigned div) {
    while (num >= div)
      num -= div;
    return num;
  }

  TUV_K5Display DisplayBuff;
  const TUV_K5SmallNumbers FontSmallNr;
  CDisplay<const TUV_K5Display> Display;
  bool bDisplayCleared;

  u32 u32ScanRange;
  u32 currentFreq;
  u16 u16OldAfSettings;
  u8 u8LastBtnPressed;
  bool working;
  u16 scanDelay;
  bool isUserInput;
};
