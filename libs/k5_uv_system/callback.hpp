#pragma once

template <class ReturnType, class... Args>
class CCallback
{
   void* pObj;
   void* pMethode;

   public:
   template <class T>
   constexpr CCallback(void* pObj, ReturnType (T::*pMethode)(Args...))
      :pObj(pObj), pMethode(*((void**)&pMethode)) {}
   constexpr CCallback()
      :pObj(nullptr), pMethode(nullptr) {}

   ReturnType operator()(Args... Arguments)
   {
      if(!pObj || !pMethode)
      {
         return ReturnType();
      }

      return ((ReturnType (*)(void*, Args...))pMethode)(pObj, Arguments...);
   }
};