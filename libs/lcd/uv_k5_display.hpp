#pragma once
#include "lcd.hpp"

struct TUV_K5Display : public IBitmap<128, 56, 8>
{
   constexpr TUV_K5Display(const unsigned char* p8ScreenData) : IBitmap(p8ScreenData){};
   bool GetPixel(unsigned char u8X, unsigned char u8Y) const override
   {
      return 0;
   }

   void SetPixel(unsigned char u8X, unsigned char u8Y) const override
   {
      unsigned char u8Line = u8Y / LineHeight;
      unsigned char* pStart = (unsigned char*)pBuffStart;
      auto Position = GetCoursorPosition(u8Line, u8X);
      if(Position > (SizeY / LineHeight) * SizeX)
      {
         return;
      }
      
      *(pStart + Position) |= 0b1 << (u8Y % LineHeight);
   }

   void* GetCoursorData(unsigned short u16CoursorPosition) const override
   {
      return (void*) (&pBuffStart[u16CoursorPosition]);
   }

   void ClearAll() override
   {
      memset((void*)pBuffStart, 0, (SizeY / LineHeight) * SizeX);
   }
};

struct TUV_K5StatusBar : public IBitmap<128, 8, 8>
{
   constexpr TUV_K5StatusBar(const unsigned char* p8ScreenData) : IBitmap(p8ScreenData){};
   bool GetPixel(unsigned char u8X, unsigned char u8Y) const override
   {
      return 0;
   }

   void SetPixel(unsigned char u8X, unsigned char u8Y) const override
   {
      unsigned char u8Line = u8Y / LineHeight;
      unsigned char* pStart = (unsigned char*)pBuffStart;
      auto Position = GetCoursorPosition(u8Line, u8X);
      if(Position > (SizeY / LineHeight) * SizeX)
      {
         return;
      }
      
      *(pStart + Position) |= 0b1 << (u8Y % LineHeight);
   }

   void* GetCoursorData(unsigned short u16CoursorPosition) const override
   {
      return (void*) (&pBuffStart[u16CoursorPosition]);
   }

   void ClearAll() override
   {
      memset((void*)pBuffStart, 0, (SizeY / LineHeight) * SizeX);
   }
};

struct TUV_K5SmallNumbers : public IFont
{
   static constexpr auto FixedSizeY = 7;
   constexpr TUV_K5SmallNumbers(unsigned char* _p8FontData)
   :pFontData(_p8FontData){}

   bool GetPixel(char c8Character, unsigned char u8X, unsigned char u8Y) const override
   {
      return 0;
   }

   unsigned char* GetRaw(char c8Character) const override
   {
      if(c8Character < '0' || c8Character > '9') // 45 - minus
      {
         if(c8Character == '-')
         {
            return pFontData + (FixedSizeY * 10);
         }

         return nullptr;
      }

      return pFontData + ((c8Character - '0') * FixedSizeY);
   }

   unsigned char GetSizeX(char c8Character) const override
   {
      return 8;
   }

   unsigned char GetSizeY(char c8Character) const override
   {
      return FixedSizeY;
   }

   unsigned char* const pFontData;
};

struct TUV_K5Small : public IFont
{
   static constexpr auto FixedSizeY = 7;
   constexpr TUV_K5Small(unsigned char* _p8FontData)
   :pFontData(_p8FontData){}

   bool GetPixel(char c8Character, unsigned char u8X, unsigned char u8Y) const override
   {
      return 0;
   }

   unsigned char* GetRaw(char c8Character) const override
   {
      if(c8Character < '0' || c8Character > '9') // 45 - minus
      {
         if(c8Character == '-')
         {
            return pFontData + (FixedSizeY * 10);
         }

         return nullptr;
      }

      return pFontData + ((c8Character - '0') * FixedSizeY);
   }

   unsigned char GetSizeX(char c8Character) const override
   {
      return 8;
   }

   unsigned char GetSizeY(char c8Character) const override
   {
      return FixedSizeY;
   }

   unsigned char* const pFontData;
};