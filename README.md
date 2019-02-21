




Evan's Notes:

Todo:

add memory 
- AllocateMemoryFast -> always just grabs the last item without the search?

more fonts? actual font rendering?


organization
camera concept, entities concept
-perspective?
obj importer?

Allocators don't work for STB & file creation <- this is a big deal

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
