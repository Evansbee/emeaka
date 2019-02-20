
#include "emeaka.h"
#include <cstdio>

void InitialzeMemoryBank(MemoryBank *bank, void *start, size_t size)
{
   bank->Start = start;
   bank->Size=size;
   bank->Entries = 1;
   bank->Used = sizeof(MemoryAllocationHeader);
   bank->NextFree = (MemoryAllocationHeader *)start;

   MemoryAllocationHeader *header = (MemoryAllocationHeader *)start;
   header->InUse = false;
   header->Next = nullptr;
   header->Previous = nullptr;
   header->Bank = bank;
   header->Size = size - sizeof(MemoryAllocationHeader);
}

void *AllocateMemory(MemoryBank *bank, size_t size)
{
   MemoryAllocationHeader *current = (MemoryAllocationHeader *)bank->Start;

   while(current->InUse || current->Size < size)
   {
      if(current->Next != nullptr)
      {
         current = current->Next;
      }
      else
      {
         return nullptr;
      }
   }

   void *returnValue = (void *)((size_t)current + sizeof(MemoryAllocationHeader));
   current->InUse = true;

   
      const size_t MinNewFragmentSize = sizeof(int64_t); //this way it at least lest us have something useful in there..
      //there was something there
      size_t leftOverSize = current->Size - size;
      if((leftOverSize > (sizeof(MemoryAllocationHeader) + MinNewFragmentSize)))
      {
         MemoryAllocationHeader *newHeader = (MemoryAllocationHeader *)((size_t)returnValue + size);
         newHeader->Previous = current;
         newHeader->Next = current->Next;
         current->Next = newHeader;
         newHeader->InUse = false;
         newHeader->Size = leftOverSize - sizeof(MemoryAllocationHeader);
         current->Size = size;
         newHeader->Bank = bank;
         bank->Entries += 1;
         bank->Size += sizeof(MemoryAllocationHeader) + size;
         if(newHeader->Next == nullptr)
         {
            bank->NextFree = newHeader;
         }
      }
      else
      {
         bank->Size += current->Size;
      }
   //add new header afterwards...
   return returnValue;
}

void FreeMemory(void *address)
{
   
   MemoryAllocationHeader *header = (MemoryAllocationHeader *)((size_t)address - sizeof(MemoryAllocationHeader));
   header->InUse = false;
   header->Bank->Used -= header->Size;
   if(header->Next && !header->Next->InUse)
   {
      //combine with next
      header->Size += sizeof(MemoryAllocationHeader) + header->Next->Size;
      header->Next = header->Next->Next;
      header->Bank->Entries -= 1;
      header->Bank->Size -= sizeof(MemoryAllocationHeader);
   }

   if(header->Previous && !header->Previous->InUse)
   {
      header->Previous->Size += sizeof(MemoryAllocationHeader) + header->Size;
      header->Previous->Next = header->Next;
      header->Previous->Bank->Entries -= 1;
      header->Previous->Bank->Size -= sizeof(MemoryAllocationHeader);
   }
}

void DumpMemoryBank(MemoryBank *bank)
{
   printf(":::::START MEMORY DUMP:::::\n");
   printf("Start: %p, Size: %zu/%zu, Entries: %zu\n",bank->Start, bank->Used, bank->Size,bank->Entries);
   printf("Entries...\n");

   size_t i = 1;
   MemoryAllocationHeader *cursor = (MemoryAllocationHeader*)bank->Start;
   while(cursor != nullptr)
   {
      printf("%zu: [%p, S: %zu, Prev: %p, Next: %p, In Use: %d]\n",
      i++, cursor,
      cursor->Size, cursor->Previous, cursor->Next, cursor->InUse);

      cursor = cursor->Next;
   }
   printf(":::::END MEMORY DUMP:::::\n");
}
