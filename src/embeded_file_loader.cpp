#include "embeded_file_loader.h"



// Read like fread
size_t memf_read(void* ptr, size_t size, size_t nmemb, MemFILE* mf) {
    size_t bytes = size * nmemb;
    if (mf->pos + bytes > mf->size)
        bytes = mf->size - mf->pos;
    memcpy(ptr, mf->buf + mf->pos, bytes);
    mf->pos += bytes;
    return bytes / size;
}

// Seek like fseek
int memf_seek(MemFILE* mf, long offset, int whence) {
    size_t newpos;
    switch (whence) {
        case SEEK_SET: newpos = offset;          break;
        case SEEK_CUR: newpos = mf->pos + offset;break;
        case SEEK_END: newpos = mf->size + offset;break;
        default:       return -1;
    }
    if (newpos > mf->size) return -1;
    mf->pos = newpos;
    return 0;
}

// Wrap it all up
MemFILE* mfopen_FILE0000(const char* name) {
    //if (strcmp(name, "FILE0000.GLB") != 0) return NULL;

    // Use the BIN2S symbols
    extern const uint8_t FILE0000_GLB[];
    extern const uint8_t FILE0000_GLB_end[];
    size_t len = (size_t)(FILE0000_GLB_end - FILE0000_GLB);

    MemFILE* mf = (MemFILE*)std::malloc(sizeof(MemFILE));
    mf->buf  = FILE0000_GLB;
    mf->size = len;
    mf->pos  = 0;
    return mf;
}

// Wrap it all up
MemFILE* mfopen_FILE0001(const char* name) {
    //if (strcmp(name, "FILE0001.GLB") != 0) return NULL;

    // Use the BIN2S symbols
    extern const uint8_t FILE0001_GLB[];
    extern const uint8_t FILE0001_GLB_end[];
    size_t len = (size_t)(FILE0001_GLB_end - FILE0001_GLB);

    MemFILE* mf = (MemFILE*)std::malloc(sizeof(MemFILE));
    mf->buf  = FILE0001_GLB;
    mf->size = len;
    mf->pos  = 0;
    return mf;
}

// Example usage
void loadViaMemFILE0000() {
    MemFILE* f = mfopen_FILE0000("scene.glb");
    if (!f) return;

    // pretend fread/fseek on f…
    uint8_t header[12];
    memf_read(header, 1, sizeof(header), f);
    // … etc.

    free(f);
}

// Example usage
void loadViaMemFILE0001() {
    MemFILE* f = mfopen_FILE0001("scene.glb");
    if (!f) return;

    // pretend fread/fseek on f…
    uint8_t header[12];
    memf_read(header, 1, sizeof(header), f);
    // … etc.

    free(f);
}