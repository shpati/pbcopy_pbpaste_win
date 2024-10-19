// pbcopy.c - Reads from stdin and copies to clipboard
#include <windows.h>
#include <stdio.h>

#define CHUNK_SIZE 4096

int main() {
    // Read all input from stdin
    char* buffer = NULL;
    size_t bufferSize = 0;
    size_t totalSize = 0;
    char chunk[CHUNK_SIZE];
    size_t bytesRead;

    while ((bytesRead = fread(chunk, 1, CHUNK_SIZE, stdin)) > 0) {
        char* newBuffer = (char*)realloc(buffer, bufferSize + bytesRead + 1);
        if (newBuffer == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            free(buffer);
            return 1;
        }
        buffer = newBuffer;
        memcpy(buffer + bufferSize, chunk, bytesRead);
        bufferSize += bytesRead;
    }

    if (buffer != NULL) {
        buffer[bufferSize] = '\0';
        
        // Open clipboard
        if (!OpenClipboard(NULL)) {
            fprintf(stderr, "Failed to open clipboard\n");
            free(buffer);
            return 1;
        }

        // Empty clipboard
        EmptyClipboard();

        // Allocate global memory
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, bufferSize + 1);
        if (hMem == NULL) {
            fprintf(stderr, "Failed to allocate memory\n");
            CloseClipboard();
            free(buffer);
            return 1;
        }

        // Copy buffer to global memory
        char* clipboardData = (char*)GlobalLock(hMem);
        if (clipboardData == NULL) {
            fprintf(stderr, "Failed to lock memory\n");
            GlobalFree(hMem);
            CloseClipboard();
            free(buffer);
            return 1;
        }

        memcpy(clipboardData, buffer, bufferSize + 1);
        GlobalUnlock(hMem);

        // Set clipboard data
        if (SetClipboardData(CF_TEXT, hMem) == NULL) {
            fprintf(stderr, "Failed to set clipboard data\n");
            GlobalFree(hMem);
            CloseClipboard();
            free(buffer);
            return 1;
        }

        CloseClipboard();
        free(buffer);
    }

    return 0;
}