#pragma once
#include <string.h>
#include <type_traits>
#include "system.hpp"

struct ILcd {
  virtual void UpdateScreen() = 0;
};

template <unsigned short _SizeX, unsigned short _SizeY,
          unsigned char _LineHeight>
struct IBitmap {
  constexpr IBitmap(const unsigned char *_pBuffStart)
      : pBuffStart(_pBuffStart){};
  static constexpr auto SizeX = _SizeX;
  static constexpr auto SizeY = _SizeY;
  static constexpr auto LineHeight = _LineHeight;
  static constexpr auto Lines = _SizeX / _LineHeight;
  static constexpr unsigned short GetCoursorPosition(unsigned char u8Line,
                                                     unsigned char u8XPos) {
    return (u8Line * SizeX) + u8XPos;
  }

  virtual bool GetPixel(unsigned char u8X, unsigned char u8Y) const = 0;
  virtual void SetPixel(unsigned char u8X, unsigned char u8Y) const = 0;
  virtual void *GetCoursorData(unsigned short u16CoursorPosition) const {
    return nullptr;
  }
  virtual void ClearAll() = 0;
  const unsigned char *pBuffStart;
};

struct IFont {
  virtual bool GetPixel(char c8Character, unsigned char u8X,
                        unsigned char u8Y) const = 0;
  virtual unsigned char *GetRaw(char c8Character) const = 0;
  virtual unsigned char GetSizeX(char c8Character) const = 0;
  virtual unsigned char GetSizeY(char c8Character) const = 0;
};

template <class BitmapType> class CDisplay {
public:
  constexpr CDisplay(BitmapType &_Bitmap)
      : Bitmap(_Bitmap), pCurrentFont(nullptr), u16CoursorPosition(0) {}

  void SetCoursor(unsigned char u8Line, unsigned char u8X) const {
    u16CoursorPosition = (u8Line * Bitmap.SizeX) + u8X;
  }

  void SetCoursorXY(unsigned char x, unsigned char y) const {
    u16CoursorPosition = x + (y << 4);
  }

  void SetFont(const IFont *pFont) const { pCurrentFont = pFont; }

  void DrawLine(int sx, int ex, int ny) {
    for (int i = sx; i <= ex; i++) {
      if (i < Bitmap.SizeX && ny < Bitmap.SizeY) {
        Bitmap.SetPixel(i, ny);
      }
    }
  }

  void DrawHLine(int sy, int ey, int nx, bool bCropped = false) {
    for (int i = sy; i <= ey; i++) {
      if (i < Bitmap.SizeY && nx < Bitmap.SizeX && (!bCropped || i % 2)) {
        Bitmap.SetPixel(nx, i);
      }
    }
  }

  void DrawCircle(unsigned char cx, unsigned char cy, unsigned int r,
                  bool bFilled = false) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;
    if (!r)
      return;

    while (y >= x) {
      // when bFilled is true, draw lines to fill the circle
      if (bFilled) {
        DrawLine(cx - x, cx + x, cy - y);
        DrawLine(cx - y, cx + y, cy - x);
        DrawLine(cx - x, cx + x, cy + y);
        DrawLine(cx - y, cx + y, cy + x);
      } else {
        if (cx + x < Bitmap.SizeX && cy + y < Bitmap.SizeY)
          Bitmap.SetPixel(cx + x, cy - y); // 1st quarter
        if (cx + y < Bitmap.SizeX && cy + x < Bitmap.SizeY)
          Bitmap.SetPixel(cx + y, cy - x); // 2nd quarter
        if (cx - x >= 0 && cy + y < Bitmap.SizeY)
          Bitmap.SetPixel(cx - x, cy - y); // 3rd quarter
        if (cx - y >= 0 && cy + x < Bitmap.SizeY)
          Bitmap.SetPixel(cx - y, cy - x); // 4th quarter
        if (cx + x < Bitmap.SizeX && cy + y < Bitmap.SizeY)
          Bitmap.SetPixel(cx + x, cy + y); // 5th quarter
        if (cx + y < Bitmap.SizeX && cy - x >= 0)
          Bitmap.SetPixel(cx + y, cy + x); // 6th quarter
        if (cx - x >= 0 && cy - y >= 0)
          Bitmap.SetPixel(cx - x, cy + y); // 7th quarter
        if (cx - y >= 0 && cy - x >= 0)
          Bitmap.SetPixel(cx - y, cy + x); // 8th quarter
      }
      x++;
      if (d < 0)
        d += 4 * x + 6;
      else {
        y--;
        d += 4 * (x - y) + 10;
      }
    }
  }

  void DrawRectangle(unsigned char sx, unsigned char sy, unsigned char width,
                     unsigned char height, bool bFilled) {
    unsigned char maxX =
        (sx + width < Bitmap.SizeX) ? sx + width : Bitmap.SizeX;
    unsigned char maxY =
        (sy + height < Bitmap.SizeY) ? sy + height : Bitmap.SizeY;

    // Draw vertical lines
    for (unsigned char y = sy; y < maxY; y++) {
      Bitmap.SetPixel(sx, y);
      Bitmap.SetPixel(maxX - 1, y);
    }

    // Draw horizontal lines
    for (unsigned char x = sx; x < maxX; x++) {
      Bitmap.SetPixel(x, sy);
      Bitmap.SetPixel(x, maxY - 1);
    }

    // If filled, draw horizontal lines within the rectangle
    if (bFilled) {
      for (unsigned char x = sx + 1; x < maxX - 1; x++) {
        for (unsigned char y = sy + 1; y < maxY - 1; y++) {
          Bitmap.SetPixel(x, y);
        }
      }
    }
  }

  unsigned char PrintCharacter(const char c8Character) const {
    if (!pCurrentFont) {
      return 0;
    }

    const auto *const pFontRawData = pCurrentFont->GetRaw(c8Character);
    auto *pCoursorPosition = Bitmap.GetCoursorData(u16CoursorPosition);
    auto const CopySize =
        pCurrentFont->GetSizeY(c8Character) * (BitmapType::LineHeight / 8);
    if (pCoursorPosition && !(BitmapType::LineHeight % 8)) {
      if (pFontRawData)
        memcpy(pCoursorPosition, pFontRawData, CopySize);
      else
        memset(pCoursorPosition, 0, CopySize);
    }

    u16CoursorPosition += pCurrentFont->GetSizeY(c8Character);
    return 0;
  }

  void Print(const char *C8String) const {
    for (unsigned char i = 0; i < Strlen(C8String); i++) {
      PrintCharacter(C8String[i]);
    }
  }

  unsigned char PrintFixedDigtsNumer(int s32Number, unsigned char u8Digts) {
    char U8NumBuff[32];
    memset(U8NumBuff, 0, sizeof(U8NumBuff));

    char *pString = U8NumBuff + u8Digts;
    *pString = '\0';

    if (s32Number < 0) {
      U8NumBuff[0] = '-';
      s32Number = -s32Number;
    }

    while (u8Digts--) {
      *--pString = '0' + (s32Number % 10);
      s32Number /= 10;
    }

    Print(U8NumBuff);
    return u8Digts * pCurrentFont->GetSizeX('0');
  }

  static constexpr int powersOfTen[9] = {
      1,        // 10^0
      10,       // 10^1
      100,      // 10^2
      1000,     // 10^3
      10000,    // 10^4
      100000,   // 10^5
      1000000,  // 10^6
      10000000, // 10^7
      100000000 // 10^8
  };
  void PrintFixedDigitsNumber2(int s32Number, unsigned char u8DigsToCut = 2,
                               unsigned char u8FixedDigtsCnt = 0) {
    char U8NumBuff[11] = {0}; // 9 digits, sign, and null terminator
    int startIdx = 0;
    bool isNegative = false;

    if (s32Number < 0) {
      PrintCharacter('-');
      // U8NumBuff[0] = '-';
      s32Number = -s32Number;
      // isNegative = true;
    }

    for (int i = 8; i >= u8DigsToCut;
         --i) // assuming powersOfTen is an array of powers of 10
    {
      int digit = 0;
      while (s32Number >= powersOfTen[i]) {
        s32Number -= powersOfTen[i];
        ++digit;
      }
      U8NumBuff[isNegative + (8 - i)] = '0' + digit;

      // We found the first non-zero digit
      if (digit != 0 && startIdx == (isNegative ? 1 : 0))
        startIdx = isNegative + (8 - i);
    }

    // If the number was 0, we write a single 0.
    if (startIdx == (isNegative ? 1 : 0))
      U8NumBuff[isNegative] = '0';

    // Print the string from the start index
    if (u8FixedDigtsCnt) {
      startIdx = 9 - u8DigsToCut - u8FixedDigtsCnt;
    }

    Print(U8NumBuff + startIdx);
  }
  void PrintFixedDigitsNumber3(int s32Number, unsigned char u8DigsToCut = 2,
                               unsigned char u8FixedDigtsCnt = 0,
                               unsigned char pointAt = 128) {
    char U8NumBuff[11] = {0}; // 9 digits, sign, and null terminator
    unsigned char startIdx = 0;

    for (unsigned char i = 8; i >= u8DigsToCut; --i) {
      int digit = 0;
      while (s32Number >= powersOfTen[i]) {
        s32Number -= powersOfTen[i];
        ++digit;
      }
      U8NumBuff[8 - i] = '0' + digit;

      // We found the first non-zero digit
      if (digit != 0 && startIdx == 0)
        startIdx = 8 - i;
    }

    // Print the string from the start index
    startIdx = 9 - u8DigsToCut - u8FixedDigtsCnt;

    const char *str = U8NumBuff + startIdx;
    const char len = Strlen(str);
    const char dot[1] = {64};
    for (unsigned char i = 0; i < len; i++) {
      if (pointAt == len - i) {
        auto *pCoursorPosition = Bitmap.GetCoursorData(u16CoursorPosition + 1);
        memcpy(pCoursorPosition, dot, 1);
        u16CoursorPosition += 2;
      }
      PrintCharacter(str[i]);
    }
  }

private:
  const BitmapType &Bitmap;
  mutable const IFont *pCurrentFont;
  mutable unsigned short u16CoursorPosition;
};
