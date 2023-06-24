namespace System
{
   using VoidFxPointer = void(*)(void);
   static constexpr auto CortexM0VectorsCnt = 16+32;

   struct TVectorTable
   {
      VoidFxPointer Vectors[CortexM0VectorsCnt];
   };

   template <int VectorNr>
   inline void CommonIrqWrapper()
   {
      extern TVectorTable OrginalFwVevtorTable;
      if (OrginalFwVevtorTable.Vectors[VectorNr])
      {
         OrginalFwVevtorTable.Vectors[VectorNr]();
      }
   }
}