#include "emeaka.h"
#include "catch.hpp"
#include "emeaka_memory.cpp"

bool CheckValid(MemoryBank *bank)
{
   MemoryAllocationHeader *cursor = (MemoryAllocationHeader *)bank->Start;
   while (cursor != nullptr)
   {
      if(cursor->Next && cursor->Next->Previous != cursor)
      {
         return false;
      } 

      cursor = cursor->Next;
   }
   return true;
}

TEST_CASE("Splitting A Large Empty entry Works...","[memory]")
{
    MemoryBank bank;
    void *memory = malloc(10000);
    InitializeMemoryBank(&bank, memory, 10000);

    void *test1 = AllocateMemory(&bank,1);
    void *test2 = AllocateMemory(&bank,1000);
    void *test3 = AllocateMemory(&bank,1);
    REQUIRE(CheckValid(&bank));
    FreeMemory(test2);
    void *test4 = AllocateMemory(&bank, 1);
    REQUIRE(CheckValid(&bank));
}

TEST_CASE("0 size gives nullptr", "[memory]")
{
    MemoryBank bank;
    void *memory = malloc(1000);
    InitializeMemoryBank(&bank, memory, 1000);

    void *result = AllocateMemory(&bank, 0);
    REQUIRE(result == nullptr);
}

TEST_CASE("No Space Gives Null Ptr", "[memory]")
{
    MemoryBank bank;
    void *memory = malloc(1000);
    InitializeMemoryBank(&bank, memory, 1000);

    void *result = AllocateMemory(&bank, 2000);
    REQUIRE(result == nullptr);
}

TEST_CASE("Memory Creation", "[memory]")
{
    size_t testBankSize = 2000;
    size_t headerSize = sizeof(MemoryAllocationHeader);

    void *bank_start = malloc(testBankSize);
    MemoryBank bank;
    InitializeMemoryBank(&bank, bank_start, testBankSize);
    REQUIRE(bank.Size == testBankSize);
    REQUIRE(bank.Entries == 1);
    REQUIRE(bank.Used == headerSize);

    size_t test1Size = 100;
    void *test1 = AllocateMemory(&bank, test1Size);
    MemoryAllocationHeader *test1Header = (MemoryAllocationHeader *)((size_t)test1 - headerSize);
    REQUIRE(bank.Size == testBankSize);
    REQUIRE(bank.Entries == 2);
    REQUIRE(bank.Used == (2 * headerSize) + test1Size);
    REQUIRE(test1Header->Previous == nullptr);
    REQUIRE(test1Header->Next != nullptr);
    REQUIRE(test1Header->InUse == true);
    REQUIRE(test1Header->Bank == &bank);
    REQUIRE(test1Header->Size == test1Size);

    size_t test2Size = 100;
    void *test2 = AllocateMemory(&bank, test2Size);
    MemoryAllocationHeader *test2Header = (MemoryAllocationHeader *)((size_t)test2 - headerSize);
    REQUIRE(bank.Size == testBankSize);
    REQUIRE(bank.Entries == 3);
    REQUIRE(bank.Used == (3 * headerSize) + test1Size + test2Size);
    REQUIRE(test2Header->Previous != nullptr);
    REQUIRE(test2Header->Next != nullptr);
    REQUIRE(test2Header->InUse == true);
    REQUIRE(test2Header->Bank == &bank);
    REQUIRE(test2Header->Size == test2Size);
}

TEST_CASE("Memory Freeing", "[memory]")
{
    size_t testBankSize = 2000;
    size_t headerSize = sizeof(MemoryAllocationHeader);

    void *bank_start = malloc(testBankSize);
    MemoryBank bank;
    InitializeMemoryBank(&bank, bank_start, testBankSize);
    REQUIRE(bank.Size == testBankSize);
    REQUIRE(bank.Entries == 1);
    REQUIRE(bank.Used == headerSize);

    size_t test1Size = 100;
    void *test1 = AllocateMemory(&bank, test1Size);
    MemoryAllocationHeader *test1Header = (MemoryAllocationHeader *)((size_t)test1 - headerSize);

    size_t test2Size = 100;
    void *test2 = AllocateMemory(&bank, test2Size);
    MemoryAllocationHeader *test2Header = (MemoryAllocationHeader *)((size_t)test2 - headerSize);

    size_t test3Size = 100;
    void *test3 = AllocateMemory(&bank, test3Size);
    MemoryAllocationHeader *test3Header = (MemoryAllocationHeader *)((size_t)test3 - headerSize);

    REQUIRE(bank.Entries == 4);

    SECTION("Delete Last Defined Element (always combines with end)")
    {
        //covers the gobble up next case.
        FreeMemory(test3);

        REQUIRE(bank.Entries == 3);
        REQUIRE(test3Header->InUse == false);
        REQUIRE(test3Header->Size == testBankSize - (3 * headerSize) - test1Size - test2Size);
        REQUIRE(test3Header->Next == nullptr);
        REQUIRE(test3Header->Previous == test2Header);
        REQUIRE(bank.Used == test1Size + test2Size + 3 * headerSize);
    }

    SECTION("Delete First Element")
    {
        FreeMemory(test1);
        REQUIRE(bank.Entries == 4);
        REQUIRE(test1Header->InUse == false);
        REQUIRE(test1Header->Size == test1Size);
        REQUIRE(test1Header->Previous == nullptr);
        REQUIRE(test1Header->Next == test2Header);
        REQUIRE(bank.Used == test3Size + test2Size + 4 * headerSize);
    }

    SECTION("Delete Middle Element")
    {
        FreeMemory(test2);
        REQUIRE(bank.Entries == 4);
        REQUIRE(test2Header->InUse == false);
        REQUIRE(test2Header->Size == test2Size);
        REQUIRE(test2Header->Previous == test1Header);
        REQUIRE(test2Header->Next == test3Header);
        REQUIRE(bank.Used == test3Size + test1Size + 4 * headerSize);
    }

    SECTION("Combine Previous Element")
    {
        FreeMemory(test1);
        REQUIRE(bank.Entries == 4);
        FreeMemory(test2);
        REQUIRE(bank.Entries == 3);

        REQUIRE(test1Header->Previous == nullptr);
        REQUIRE(test1Header->Next == test3Header);
        REQUIRE(test1Header->Size == test1Size + test2Size + headerSize);

    }

    SECTION("Combine Prev and Next, Wipe to Zero")
    {
        FreeMemory(test1);
        FreeMemory(test3);
        REQUIRE(bank.Entries == 3);
        FreeMemory(test2);
        REQUIRE(bank.Entries == 1);
        REQUIRE(test1Header->Next == nullptr);
        REQUIRE(test1Header->Previous == nullptr);
        REQUIRE(bank.Used == headerSize);
    }
}