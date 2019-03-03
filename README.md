




Evan's Notes:

Todo:

add memory 
- AllocateMemoryFast -> always just grabs the last item without the search?



organization
camera concept, entities concept
-perspective?
obj importer?


File API might want to look like:


struct FileInformation
{
    bool Found;
    bool IsOpen;
    const char* FullPath;
    size_t Size;
    time_t WriteTime;
    time_t CreateTime;
}

FileInformation RequestAssetFileInformation(ThreadContext *tc, const char* filename);
size_t LoadFileToBuffer(ThreadContext *tc, const char* FullPath, void *Buffer);

for usage as:

FileInformation info = RequestAssetFileInformation(tc, "bmptest.bmp");
if(info.Found)
{
    void *fileData = AllocateMemory(&FileMemoryBank, info.Size);
    LoadFileToBuffer(tc, info.FullPath, fileData);
}

the platform needs to find a path to the resource directory as well

How do we want to handle globals? We need a "global" memory bank that everything can use so we can pass around a "Malloc" to various things...what if we want to use std::vector<something, myMalloc()>

void SetGlobalMemoryBank(MemoryBank *mb)
{

}

//what if we serviced some of these globals every frame?

void* EmeakaMalloc(size_t requestedSize)
{
    MemoryBank *mb = GetGlobalMemoryBank(MemoryBank *mb);
    return AllocateMemory(mb, requestedSize);
}

void EmeakaFree(void* ptr)
{
    FreeMemory(ptr);
}

