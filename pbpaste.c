// pbpaste.c - Outputs clipboard contents to stdout
#include <windows.h>
#include <stdio.h>

// Convert CRLF to LF if output_unix is true
char* convertOutputLineEndings(const char* input, size_t* size, int output_unix) {
    if (!output_unix) return _strdup(input);

    size_t inputLen = strlen(input);
    char* output = (char*)malloc(inputLen + 1);
    if (!output) return NULL;

    size_t j = 0;
    for (size_t i = 0; i < inputLen; i++) {
        if (input[i] == '\r' && i + 1 < inputLen && input[i + 1] == '\n') {
            continue;  // Skip \r when followed by \n
        }
        output[j++] = input[i];
    }
    output[j] = '\0';
    *size = j;
    return output;
}

int main(int argc, char* argv[]) {
    // Check if --unix flag is present
    int output_unix = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--unix") == 0) {
            output_unix = 1;
            break;
        }
    }

    if (!OpenClipboard(NULL)) {
        fprintf(stderr, "Failed to open clipboard\n");
        return 1;
    }

    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == NULL) {
        fprintf(stderr, "No text data in clipboard\n");
        CloseClipboard();
        return 1;
    }

    char* clipboardData = (char*)GlobalLock(hData);
    if (clipboardData == NULL) {
        fprintf(stderr, "Failed to lock memory\n");
        CloseClipboard();
        return 1;
    }

    // Convert line endings if needed
    size_t size = strlen(clipboardData);
    char* convertedData = convertOutputLineEndings(clipboardData, &size, output_unix);
    
    if (convertedData) {
        // Write to stdout in binary mode to preserve line endings
        fwrite(convertedData, 1, size, stdout);
        free(convertedData);
    }

    GlobalUnlock(hData);
    CloseClipboard();

    return 0;
}