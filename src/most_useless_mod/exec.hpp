#include "registers.hpp"

class CExec final
{
   public:
   CExec(){};

   void InterruptCallback()
   {
      CheckButtons();
   }

   private:
   void CheckButtons() const
   {
   }
};