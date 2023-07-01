#pragma once
#include <type_traits>
#include <string.h>

struct ILcd
{
   virtual void UpdateScreen() = 0;
};

template<unsigned short _SizeX, unsigned short _SizeY, unsigned char _LineHeight>
struct IBitmap
{
   constexpr IBitmap(const unsigned char* _pBuffStart) :pBuffStart(_pBuffStart){};
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
   virtual void* GetCoursorData(unsigned short u16CoursorPosition) const {return nullptr;}

   const unsigned char* pBuffStart;
};

struct IFont
{
   virtual bool GetPixel(char c8Character, unsigned char u8X, unsigned char u8Y) const = 0;
   virtual unsigned char* GetRaw(char c8Character) const = 0;
   virtual unsigned char GetSizeX(char c8Character) const = 0;
   virtual unsigned char GetSizeY(char c8Character) const = 0;
};

template <class BitmapType>
class CDisplay
{
   public:
   constexpr CDisplay(BitmapType& _Bitmap)
      :Bitmap(_Bitmap), pCurrentFont(nullptr), u16CoursorPosition(0)
   {}

   void SetCoursor(unsigned char u8Line, unsigned char u8X) const
   {
      u16CoursorPosition = (u8Line * Bitmap.SizeX) + u8X;
   }

   void SetFont(const IFont* pFont) const
   {
      pCurrentFont = pFont;
   }

   unsigned char PrintCharacter(const char c8Character) const
   {
      if(!pCurrentFont)
      {
         return 0;
      }

      const auto* const pFontRawData = pCurrentFont->GetRaw(c8Character);
      auto* pCoursorPosition = Bitmap.GetCoursorData(u16CoursorPosition);
      auto const CopySize = pCurrentFont->GetSizeY(c8Character) * (BitmapType::LineHeight / 8);
      if(pCoursorPosition && !(BitmapType::LineHeight % 8))
      {
         if(pFontRawData)
            memcpy(pCoursorPosition, pFontRawData, CopySize);
         else
            memset(pCoursorPosition, 0, CopySize);
      }
      
      u16CoursorPosition += pCurrentFont->GetSizeY(c8Character);
      return 0;
   }

   void Print(const char* C8String)
   {
      for(unsigned char i = 0; i < strlen(C8String); i++)
      {
         PrintCharacter(C8String[i]);
      } 
   }

   private:
   const BitmapType& Bitmap;
   mutable const IFont* pCurrentFont;
   mutable unsigned short u16CoursorPosition;
};