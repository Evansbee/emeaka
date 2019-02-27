#pragma once
#include <cstdlib>

struct MemoryBank;

struct MemoryAllocationHeader
{
  size_t Size;
  bool InUse;
  MemoryAllocationHeader *Previous; //points to the header
  MemoryAllocationHeader *Next; //points to the header
  MemoryBank *Bank; 
};

struct MemoryBank
{
  void *Start; //start of the bank
  size_t Size;
  size_t Entries;
  size_t Used; //will inlude the header space
  MemoryAllocationHeader *NextFree; //Points to the first byte of header of Freespace at end
};

void SetGlobalMemoryBank(MemoryBank *mb);
MemoryBank *GetGlobalMemoryBank();
void CopyMemory(void *dst, const void *src, size_t bytes);
void DumpMemoryBank(MemoryBank *bank);
void InitializeMemoryBank(MemoryBank *bank, void *start, size_t size);
void *AllocateMemory(MemoryBank *bank, size_t size);
void CombineWithNextHeader(MemoryAllocationHeader* header);
void FreeMemory(void *address);
void *ReallocateMemory(MemoryBank *bank, void *src, size_t size);
void *GlobalMemoryBankMalloc(size_t size);
void GlobalMemoryBankFree(void *ptr);
void *GlobalMemoryBankRealloc(void *ptr, size_t size);