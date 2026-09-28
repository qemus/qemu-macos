#include <llvm-c/BitReader.h>
#include <llvm-c/Core.h>

#include <stdio.h>
#include <string.h>

static void usage(const char *program)
{
    fprintf(stderr, "Usage: %s <input> -o <output>\n", program);
}

int main(int argc, char **argv)
{
    const char *input = NULL;
    const char *output = NULL;
    LLVMMemoryBufferRef buffer = NULL;
    LLVMModuleRef module = NULL;
    char *message = NULL;
    int i;

    if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        puts("air-dis 1.0");
        return 0;
    }

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            if (++i >= argc || output != NULL) {
                usage(argv[0]);
                return 2;
            }
            output = argv[i];
            continue;
        }

        if (argv[i][0] == '-' || input != NULL) {
            usage(argv[0]);
            return 2;
        }

        input = argv[i];
    }

    if (input == NULL || output == NULL) {
        usage(argv[0]);
        return 2;
    }

    if (LLVMCreateMemoryBufferWithContentsOfFile(input, &buffer, &message) != 0) {
        fprintf(stderr, "air-dis: failed to read %s%s%s\n",
                input, message != NULL ? ": " : "", message != NULL ? message : "");
        if (message != NULL) {
            LLVMDisposeMessage(message);
        }
        return 1;
    }

    if (LLVMParseBitcode2(buffer, &module) != 0) {
        fprintf(stderr, "air-dis: failed to parse LLVM bitcode from %s\n", input);
        LLVMDisposeMemoryBuffer(buffer);
        return 1;
    }

    LLVMDisposeMemoryBuffer(buffer);
    buffer = NULL;

    if (LLVMPrintModuleToFile(module, output, &message) != 0) {
        fprintf(stderr, "air-dis: failed to write %s%s%s\n",
                output, message != NULL ? ": " : "", message != NULL ? message : "");
        if (message != NULL) {
            LLVMDisposeMessage(message);
        }
        LLVMDisposeModule(module);
        return 1;
    }

    LLVMDisposeModule(module);
    return 0;
}
