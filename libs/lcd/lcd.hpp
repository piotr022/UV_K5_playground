#pragma once
#include <type_traits>
#include <string.h>

struct ILcd
{
   virtual void UpdateScreen() = 0;
};

template <unsigned short _SizeX, unsigned short _SizeY, unsigned char _LineHeight>
struct IBitmap
{
   constexpr IBitmap(const unsigned char *_pBuffStart) : pBuffStart(_pBuffStart){};
   static constexpr auto SizeX = _SizeX;
   static constexpr auto SizeY = _SizeY;
   static constexpr auto LineHeight = _LineHeight;
   static constexpr auto Lines = _SizeX / _LineHeight;
   static constexpr unsigned short GetCoursorPosition(unsigned char u8Line, unsigned char u8XPos)
   {
      return (u8Line * SizeX) + u8XPos;
   }

   virtual bool GetPixel(unsigned char u8X, unsigned char u8Y) const = 0;
   virtual void SetPixel(unsigned char u8X, unsigned char u8Y) const = 0;
   virtual void *GetCoursorData(unsigned short u16CoursorPosition) const { return nullptr; }
   virtual void ClearAll() = 0;
   const unsigned char *pBuffStart;
};

struct IFont
{
   virtual bool GetPixel(char c8Character, unsigned char u8X, unsigned char u8Y) const = 0;
   virtual unsigned char *GetRaw(char c8Character) const = 0;
   virtual unsigned char GetSizeX(char c8Character) const = 0;
   virtual unsigned char GetSizeY(char c8Character) const = 0;
};

template <class BitmapType>
class CDisplay
{
public:
   constexpr CDisplay(BitmapType &_Bitmap)
       : Bitmap(_Bitmap), pCurrentFont(nullptr), u16CoursorPosition(0)
   {
   }

   void SetCoursor(unsigned char u8Line, unsigned char u8X) const
   {
      u16CoursorPosition = (u8Line * Bitmap.SizeX) + u8X;
   }

   void SetFont(const IFont *pFont) const
   {
      pCurrentFont = pFont;
   }

   void DrawLine(int sx, int ex, int ny)
   {
      for (int i = sx; i <= ex; i++)
      {
         if (i < Bitmap.SizeX && ny < Bitmap.SizeY)
         {
            Bitmap.SetPixel(i, ny);
         }
      }
   }

   void DrawCircle(unsigned char cx, unsigned char cy, unsigned int r, bool bFilled = false)
   {
      int x = 0;
      int y = r;
      int d = 3 - 2 * r;
      if (!r)
         return;

      while (y >= x)
      {
         // when bFilled is true, draw lines to fill the circle
         if (bFilled)
         {
            DrawLine(cx - x, cx + x, cy - y);
            DrawLine(cx - y, cx + y, cy - x);
            DrawLine(cx - x, cx + x, cy + y);
            DrawLine(cx - y, cx + y, cy + x);
         }
         else
         {
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
         else
         {
            y--;
            d += 4 * (x - y) + 10;
         }
      }
   }

   void DrawRectangle(unsigned char sx, unsigned char sy, unsigned int width, unsigned int height, bool bFilled)
   {
      // Draw vertical lines
      for (unsigned int y = sy; y < sy + height; y++)
      {
         if (y < Bitmap.SizeY)
         {
            Bitmap.SetPixel(sx, y);
            Bitmap.SetPixel(sx + width - 1, y);
         }
      }

      // Draw horizontal lines
      for (unsigned int x = sx; x < sx + width; x++)
      {
         if (x < Bitmap.SizeX)
         {
            Bitmap.SetPixel(x, sy);
            Bitmap.SetPixel(x, sy + height - 1);
         }
      }

      // If filled, draw horizontal lines within the rectangle
      if (bFilled)
      {
         for (unsigned int x = sx + 1; x < sx + width - 1; x++)
         {
            if (x < Bitmap.SizeX)
            {
               for (unsigned int y = sy + 1; y < sy + height - 1; y++)
               {
                  if (y < Bitmap.SizeY)
                  {
                     Bitmap.SetPixel(x, y);
                  }
               }
            }
         }
      }
   }

   unsigned char PrintCharacter(const char c8Character) const
   {
      if (!pCurrentFont)
      {
         return 0;
      }

      const auto *const pFontRawData = pCurrentFont->GetRaw(c8Character);
      auto *pCoursorPosition = Bitmap.GetCoursorData(u16CoursorPosition);
      auto const CopySize = pCurrentFont->GetSizeY(c8Character) * (BitmapType::LineHeight / 8);
      if (pCoursorPosition && !(BitmapType::LineHeight % 8))
      {
         if (pFontRawData)
            memcpy(pCoursorPosition, pFontRawData, CopySize);
         else
            memset(pCoursorPosition, 0, CopySize);
      }

      u16CoursorPosition += pCurrentFont->GetSizeY(c8Character);
      return 0;
   }

   void Print(const char *C8String) const
   {
      for (unsigned char i = 0; i < strlen(C8String); i++)
      {
         PrintCharacter(C8String[i]);
      }
   }

   unsigned char PrintFixedDigtsNumer(int s32Number, unsigned char u8Digts)
   {
      char U8NumBuff[32];
      memset(U8NumBuff, 0, sizeof(U8NumBuff));

      auto pString = U8NumBuff;
      if(s32Number < 0)
      {
         *pString++ = '-';
         u8Digts--;
      }

      unsigned char u8DigtsCnt = u8Digts;
      while(u8DigtsCnt--)
      {
         char c8Char;
         if(u8DigtsCnt == 0)
         {
            c8Char = '0' + (s32Number % 10);
         }
         else
         {
            c8Char = '0' + ((s32Number / ((u8DigtsCnt)*10)) % 10);
         }

         *pString++ = c8Char;
      }

      Print(U8NumBuff);
      return u8Digts * pCurrentFont->GetSizeX('0');
   }

private:
   const BitmapType &Bitmap;
   mutable const IFont *pCurrentFont;
   mutable unsigned short u16CoursorPosition;
};