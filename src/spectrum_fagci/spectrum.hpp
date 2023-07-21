#pragma once
#include "system.hpp"
#include "uv_k5_display.hpp"
#include "radio.hpp"

typedef unsigned char u8;
typedef signed short i16;
typedef unsigned short u16;
typedef signed int i32;
typedef unsigned int u32;
typedef signed long long i64;
typedef unsigned long long u64;

static constexpr auto operator""_Hz(u64 Hz) { return Hz / 10; }
static constexpr auto operator""_KHz(u64 KHz) { return KHz * 1000_Hz; }
static constexpr auto operator""_MHz(u64 KHz) { return KHz * 1000_KHz; }
static constexpr auto operator""_ms(u64 us) { return us * 1000; }
static constexpr auto operator""_s(u64 us) { return us * 1000_ms; }

template <const System::TOrgFunctions &Fw, const System::TOrgData &FwData, Radio::CBK4819<Fw> &RadioDriver>
class CSpectrum {
public:
  static constexpr auto ExitKey = 13;
  static constexpr auto DrawingSizeY = 16 + 6 * 8;
  static constexpr auto DrawingEndY = 42;
  static constexpr auto BarPos = 5 * 128;

  u8 rssiHistory[128] = {};
  u8 measurementsCount = 32;
  u8 rssiMin = 255, rssiMax = 0;
  u8 highestPeakX = 0;
  u8 highestPeakT = 0;
  u8 highestPeakRssi = 0;
  u32 highestPeakF = 0;
  u32 FStart, FEnd, fMeasure;

  CSpectrum()
      : DisplayBuff(FwData.pDisplayBuffer), FontSmallNr(FwData.pSmallDigs),
        Display(DisplayBuff), scanDelay(800), sampleZoom(2), scanStep(25_KHz),
        rssiTriggerLevel(65) {
    Display.SetFont(&FontSmallNr);
  };

  inline void Measure() {
    if (highestPeakRssi >= rssiTriggerLevel) {
      // listen
      if (fMeasure != highestPeakF) {
        fMeasure = highestPeakF;
        SetFrequency(fMeasure);
      }
      Fw.BK4819Write(0x47, u16OldAfSettings);
      Fw.DelayUs(1_s);

      // check signal level
      Fw.BK4819Write(0x47, 0); // AF

      highestPeakRssi = GetRssi(fMeasure, scanDelay);

      rssiHistory[highestPeakX >> sampleZoom] = highestPeakRssi;
      return;
    }

    u8 rssi = 0;
    u8 xPeak = 64;
    u32 fPeak = currentFreq;

    Fw.BK4819Write(0x47, 0);

    rssiMin = 255;
    rssiMax = 0;
    fMeasure = FStart;

    for (u8 i = 0; i < measurementsCount; ++i, fMeasure += scanStep) {
      rssi = rssiHistory[i] = GetRssi(fMeasure, scanDelay);
      if (rssi < rssiMin) {
        rssiMin = rssi;
      }
      if (rssi > rssiMax) {
        rssiMax = rssi;
        fPeak = fMeasure;
        xPeak = i << sampleZoom;
      }
    }

    ++highestPeakT;
    if (highestPeakT >= 8 || rssiMax > highestPeakRssi) {
      highestPeakT = 0;
      highestPeakRssi = rssiMax;
      highestPeakX = xPeak;
      highestPeakF = fPeak;
    }
  }

  inline void DrawSpectrum() {
    for (u8 x = 0; x < 128; ++x) {
      Display.DrawHLine(Rssi2Y(rssiHistory[x >> sampleZoom]), DrawingEndY, x);
    }
  }

  inline void DrawNums() {
    Display.SetCoursor(0, 0);
    Display.PrintFixedDigitsNumber2(scanDelay, 0);

    Display.SetCoursor(0, 8 * 2 + 5 * 7);
    Display.PrintFixedDigitsNumber2(scanStep << (7 - sampleZoom));

    Display.SetCoursor(1, 8 * 2 + 6 * 7);
    Display.PrintFixedDigitsNumber2(scanStep);

    Display.SetCoursor(1, 8 * 2 + 13 * 7);
    Display.PrintFixedDigitsNumber2(highestPeakRssi, 0);

    Display.SetCoursor(0, 8 * 2 + 10 * 7);
    Display.PrintFixedDigitsNumber2(highestPeakF);

    Display.SetCoursor(6, 8 * 2 + 4 * 7);
    Display.PrintFixedDigitsNumber2(currentFreq);

    Display.SetCoursor(1, 0);
    Display.PrintFixedDigitsNumber2(rssiTriggerLevel, 0);
  }

  inline void DrawRssiTriggerLevel() {
    Display.DrawLine(0, 127, Rssi2Y(rssiTriggerLevel));
  }

  inline void DrawTicks() {
    u32 f = modulo(FStart, 1_MHz);
    u32 step = scanStep >> sampleZoom;
    for (u8 i = 0; i < 128; ++i, f += step) {
      u8 barValue = 0b00001000;
      modulo(f, 100_KHz) < step && (barValue |= 0b00010000);
      modulo(f, 500_KHz) < step && (barValue |= 0b00100000);
      modulo(f, 1_MHz) < step && (barValue |= 0b11000000);

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
      UpdateScanDelay(200);
      break;
    case 7:
      UpdateScanDelay(-200);
      break;
    case 2:
      UpdateSampleZoom(1);
      break;
    case 8:
      UpdateSampleZoom(-1);
      break;
    case 3:
      UpdateRssiTriggerLevel(5);
      break;
    case 9:
      UpdateRssiTriggerLevel(-5);
      break;
    case 4:
      UpdateScanStep(-1);
      break;
    case 6:
      UpdateScanStep(1);
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
    DisplayBuff.ClearAll();
    DrawTicks();
    DrawArrow(highestPeakX);
    DrawSpectrum();
    DrawRssiTriggerLevel();
    DrawNums();
    Fw.FlushFramebufferToScreen();
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

  void UpdateRssiTriggerLevel(i32 diff) {
    rssiTriggerLevel = clamp(rssiTriggerLevel + diff, 10, 255);
    OnUserInput();
  }

  void UpdateScanDelay(i32 diff) {
    scanDelay = clamp(scanDelay + diff, 800, 3200);
    OnUserInput();
  }

  void UpdateSampleZoom(i32 diff) {
    sampleZoom = clamp(sampleZoom - diff, 0, 5);
    measurementsCount = 1 << (7 - sampleZoom);
    OnUserInput();
  }

  void UpdateCurrentFreq(i64 diff) {
    currentFreq = clamp(currentFreq + diff, 18_MHz, 1300_MHz);
    OnUserInput();
  }

  void UpdateScanStep(i32 diff) {
    if (diff > 0 && scanStep < 25_KHz) {
      scanStep <<= 1;
    }
    if (diff < 0 && scanStep > 6250_Hz) {
      scanStep >>= 1;
    }
    OnUserInput();
  }

  inline void OnUserInput() {
    isUserInput = true;
    u32 halfOfScanRange = scanStep << (6 - sampleZoom);
    FStart = currentFreq - halfOfScanRange;
    FEnd = currentFreq + halfOfScanRange;

    // reset peak
    highestPeakT = 0;
    highestPeakRssi = 0;
    highestPeakX = 64;
    highestPeakF = currentFreq;

    Fw.DelayUs(90_ms);
  }

  void Handle() {
    if (RadioDriver.IsLockedByOrgFw()) {
      return;
    }

    if (!working) {
      if (IsFlashLightOn()) {
        working = true;
        TurnOffFlashLight();
      }
      return;
    }

    u8LastBtnPressed = Fw.PollKeyboard();
    if (u8LastBtnPressed == ExitKey) {
      working = false;
      RestoreParams();
      return;
    }
    Update();
    Render();
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

  void SetFrequency(u32 f) {
    Fw.BK4819Write(0x39, (f >> 16) & 0xFFFF);
    Fw.BK4819Write(0x38, f & 0xFFFF);
    Fw.BK4819Write(0x30, 0);
    Fw.BK4819Write(0x30, 0xbff1);
  }

  u8 GetRssi(u32 f, u32 delay = 800) {
    SetFrequency(f);
    Fw.DelayUs(delay);
    return Fw.BK4819Read(0x67);
  }

  u32 GetFrequency() {
    return (Fw.BK4819Read(0x39) << 16) | Fw.BK4819Read(0x38);
  }

  inline bool IsFlashLightOn() { return GPIOC->DATA & GPIO_PIN_3; }
  inline void TurnOffFlashLight() {
    GPIOC->DATA &= ~GPIO_PIN_3;
    *FwData.p8FlashLightStatus = 3;
  }

  inline u8 Rssi2Y(u8 rssi) {
    return clamp(DrawingEndY - (rssi - rssiMin), 1, DrawingEndY);
  }

  inline i32 clamp(i32 v, i32 min, i32 max) {
    if (v < min)
      return min;
    if (v > max)
      return max;
    return v;
  }

  inline u32 modulo(u32 num, u32 div) {
    while (num >= div)
      num -= div;
    return num;
  }

  TUV_K5Display DisplayBuff;
  const TUV_K5SmallNumbers FontSmallNr;
  CDisplay<const TUV_K5Display> Display;

  u8 u8LastBtnPressed;
  u32 currentFreq;
  u16 u16OldAfSettings;

  u16 scanDelay;
  u8 sampleZoom;
  u32 scanStep;
  u8 rssiTriggerLevel;

  bool working = false;
  bool isUserInput = false;
  bool bDisplayCleared = true;
};
