#pragma once
#include "radio.hpp"
#include "system.hpp"
#include "types.hpp"
#include "uv_k5_display.hpp"

template <Radio::CBK4819 &RadioDriver> class CSpectrum {
public:
  static constexpr auto ExitKey = 13;
  static constexpr auto DrawingEndY = 42;
  static constexpr auto BarPos = 5 * 128;

  u8 rssiHistory[128] = {};
  u32 fMeasure;

  u8 peakT = 0;
  u8 peakRssi = 0;
  u8 peakI = 0;
  u32 peakF = 0;
  u8 rssiMin = 255;

  u16 scanDelay = 1200;

  bool resetBlacklist = false;

  CSpectrum()
      : DisplayBuff(gDisplayBuffer), Display(DisplayBuff),
        FontSmallNr(gSmallDigs), frequencyChangeStep(400_KHz), bwMul(2),
        rssiTriggerLevel(60) {
    Display.SetFont(&FontSmallNr);
  };

  void Scan() {
    u8 rssi = 0, rssiMax = 0;
    u8 iPeak = 0;
    u32 fPeak = currentFreq;

    fMeasure = GetFStart();

    RadioDriver.ToggleAFDAC(false);
    MuteAF();

    u16 scanStep = GetScanStep();
    u8 measurementsCount = GetMeasurementsCount();

    for (u8 i = 0;
         i < measurementsCount && (PollKeyboard() == 255 || resetBlacklist);
         ++i, fMeasure += scanStep) {
      if (!resetBlacklist && rssiHistory[i] == 255) {
        continue;
      }
      RadioDriver.SetFrequency(fMeasure);
      rssi = rssiHistory[i] = GetRssi();
      if (rssi > rssiMax) {
        rssiMax = rssi;
        fPeak = fMeasure;
        iPeak = i;
      }
      if (rssi < rssiMin) {
        rssiMin = rssi;
      }
    }
    resetBlacklist = false;
    ++peakT;

    if (rssiMax > peakRssi || peakT >= 16) {
      peakT = 0;
      peakRssi = rssiMax;
      peakF = fPeak;
      peakI = iPeak;
    }
  }

  void DrawSpectrum() {
    for (u8 x = 0; x < 128; ++x) {
      auto v = rssiHistory[x >> BWMul2XDiv()];
      if (v != 255) {
        Display.DrawHLine(Rssi2Y(v), DrawingEndY, x);
      }
    }
  }

  void DrawNums() {
    Display.SetCoursorXY(0, 0);
    Display.PrintFixedDigitsNumber3(scanDelay, 2, 2, 1);

    Display.SetCoursorXY(112, 0);
    Display.PrintFixedDigitsNumber3(GetBW(), 4, 2, 1);

    /* Display.SetCoursorXY(0, 0);
    Display.PrintFixedDigitsNumber2(rssiMinV, 0); */

    Display.SetCoursorXY(44, 0);
    Display.PrintFixedDigitsNumber3(peakF, 2, 6, 3);

    Display.SetCoursorXY(0, 48);
    Display.PrintFixedDigitsNumber3(GetFStart(), 4, 4, 1);

    Display.SetCoursorXY(98, 48);
    Display.PrintFixedDigitsNumber3(GetFEnd(), 4, 4, 1);

    Display.SetCoursorXY(57, 48);
    Display.PrintFixedDigitsNumber3(frequencyChangeStep, 4, 2, 1);

    /* Display.SetCoursorXY(0, 8);
    Display.PrintFixedDigitsNumber2(rssiMaxV, 0); */
  }

  void DrawRssiTriggerLevel() {
    u8 y = Rssi2Y(rssiTriggerLevel);
    for (u8 x = 0; x < 126; x += 4) {
      Display.DrawLine(x, x + 2, y);
    }
  }

  void DrawTicks() {
    // center
    gDisplayBuffer[BarPos + 64] = 0b00111000;
  }

  void DrawArrow(u8 x) {
    for (signed i = -2; i <= 2; ++i) {
      signed v = x + i;
      if (!(v & 128)) {
        gDisplayBuffer[BarPos + v] |= (0b01111000 << abs(i)) & 0b01111000;
      }
    }
  }

  void OnKey(u8 key) {
    switch (key) {
    case 14:
      UpdateRssiTriggerLevel(1);
      DelayMs(90);
      break;
    case 15:
      UpdateRssiTriggerLevel(-1);
      DelayMs(90);
      break;
    }
  }

  void OnKeyDown(u8 key) {
    switch (key) {
    case 1:
      if (scanDelay < 8000) {
        scanDelay += 200;
        rssiMin = 255;
      }
      break;
    case 7:
      if (scanDelay > 800) {
        scanDelay -= 200;
        rssiMin = 255;
      }
      break;
    case 3:
      UpdateBWMul(1);
      resetBlacklist = true;
      break;
    case 9:
      UpdateBWMul(-1);
      resetBlacklist = true;
      break;
    case 2:
      UpdateFreqChangeStep(100_KHz);
      break;
    case 8:
      UpdateFreqChangeStep(-100_KHz);
      break;
    case 11: // up
      UpdateCurrentFreq(frequencyChangeStep);
      resetBlacklist = true;
      break;
    case 12: // down
      UpdateCurrentFreq(-frequencyChangeStep);
      resetBlacklist = true;
      break;
    case 5:
      ToggleBacklight();
      break;
    case 0:
      Blacklist();
      break;
    }
    ResetPeak();
  }

  bool HandleUserInput() {
    btnPrev = btn;
    btn = PollKeyboard();
    if (btn == ExitKey) {
      DeInit();
      return false;
    }
    OnKey(btn);
    if (btn != 255 && btnPrev == 255) {
      OnKeyDown(btn);
    }
    return true;
  }

  void Render() {
    DisplayBuff.ClearAll();
    DrawTicks();
    DrawArrow(peakI << BWMul2XDiv());
    DrawSpectrum();
    DrawRssiTriggerLevel();
    DrawNums();
    FlushFramebufferToScreen();
  }

  void Update() {
    if (peakRssi >= rssiTriggerLevel) {
      Listen(1600);
      return;
    }
    Scan();
  }

  void UpdateRssiTriggerLevel(i32 diff) {
    if ((diff > 0 && rssiTriggerLevel < 255) ||
        (diff < 0 && rssiTriggerLevel > 0)) {
      rssiTriggerLevel += diff;
    }
  }

  void UpdateBWMul(i32 diff) {
    if ((diff > 0 && bwMul < 4) || (diff < 0 && bwMul > 0)) {
      bwMul += diff;
    }
    frequencyChangeStep = 100_KHz << bwMul;
  }

  void UpdateCurrentFreq(i64 diff) {
    if ((diff > 0 && currentFreq < 1300_MHz) ||
        (diff < 0 && currentFreq > 18_MHz)) {
      currentFreq += diff;
    }
  }

  void UpdateFreqChangeStep(i64 diff) {
    frequencyChangeStep = clamp(frequencyChangeStep + diff, 100_KHz, 2_MHz);
  }

  void Blacklist() { rssiHistory[peakI] = 255; }

  void Handle() {
    if (RadioDriver.IsLockedByOrgFw()) {
      return;
    }

    if (!isInitialized && IsFlashLightOn()) {
      TurnOffFlashLight();
      Init();
    }

    if (isInitialized && HandleUserInput()) {
      Update();
      Render();
    }
  }

private:
  void Init() {
    currentFreq = RadioDriver.GetFrequency();
    oldAFSettings = BK4819Read(0x47);
    oldBWSettings = BK4819Read(0x43);
    MuteAF();
    SetWideBW();
    isInitialized = true;
  }

  void DeInit() {
    DisplayBuff.ClearAll();
    FlushFramebufferToScreen();
    RadioDriver.SetFrequency(currentFreq);
    RestoreOldAFSettings();
    BK4819Write(0x43, oldBWSettings);
    isInitialized = false;
  }

  void ResetPeak() {
    peakRssi = 0;
    peakF = currentFreq;
    peakT = 0;
  }

  void SetWideBW() {
    auto Reg = BK4819Read(0x43);
    Reg &= ~(0b11 << 4);
    BK4819Write(0x43, Reg | (0b11 << 4));
  }
  void MuteAF() { BK4819Write(0x47, 0); }
  void RestoreOldAFSettings() { BK4819Write(0x47, oldAFSettings); }

  void Listen(u16 durationMs) {
    if (fMeasure != peakF) {
      fMeasure = peakF;
      RadioDriver.SetFrequency(fMeasure);
      RestoreOldAFSettings();
      RadioDriver.ToggleAFDAC(true);
    }
    for (u8 i = 0; i < 16 && PollKeyboard() == 255; ++i) {
      DelayMs(durationMs >> 4);
    }
    peakRssi = rssiHistory[peakI] = GetRssi();
  }

  u16 GetScanStep() { return 25_KHz >> (2 >> bwMul); }
  u32 GetBW() { return 200_KHz << bwMul; }
  u32 GetFStart() { return currentFreq - (100_KHz << bwMul); }
  u32 GetFEnd() { return currentFreq + (100_KHz << bwMul); }

  u8 BWMul2XDiv() { return clamp(4 - bwMul, 0, 2); }
  u8 GetMeasurementsCount() {
    if (bwMul == 3) {
      return 64;
    }
    if (bwMul > 3) {
      return 128;
    }
    return 32;
  }

  void ResetRSSI() {
    RadioDriver.ToggleRXDSP(false);
    RadioDriver.ToggleRXDSP(true);
  }

  u8 GetRssi() {
    ResetRSSI();

    DelayUs(scanDelay);
    return (BK4819Read(0x67) & 0x1FF) >> 1;
  }

  bool IsFlashLightOn() { return GPIOC->DATA & GPIO_PIN_3; }
  void TurnOffFlashLight() {
    GPIOC->DATA &= ~GPIO_PIN_3;
    gFlashLightStatus = 3;
  }

  void ToggleBacklight() { GPIOB->DATA ^= GPIO_PIN_6; }

  u8 Rssi2Y(u8 rssi) {
    return DrawingEndY - clamp(rssi - rssiMin, 0, DrawingEndY);
  }

  i32 clamp(i32 v, i32 min, i32 max) {
    if (v <= min)
      return min;
    if (v >= max)
      return max;
    return v;
  }

  u32 modulo(u32 num, u32 div) {
    while (num >= div)
      num -= div;
    return num;
  }

  TUV_K5Display DisplayBuff;
  CDisplay<const TUV_K5Display> Display;
  const TUV_K5SmallNumbers FontSmallNr;

  u32 frequencyChangeStep;
  u8 bwMul;
  u8 rssiTriggerLevel;

  u8 btn = 255;
  u8 btnPrev = 255;
  u32 currentFreq;
  u16 oldAFSettings;
  u16 oldBWSettings;

  bool isInitialized = false;
};
