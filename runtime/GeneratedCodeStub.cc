#include "GeneratedCode.h"

namespace GeneratedFunctions {

size_t numLogIds = 0;

LogMetadata logId2Metadata[] = {
    {nullptr, nullptr, 0, NanoLog::SILENT_LOG_LEVEL},
};

ssize_t
(*compressFnArray[])(NanoLogInternal::Log::UncompressedEntry *re, char *out) = {
    nullptr,
};

void
(*decompressAndPrintFnArray[])(const char **in,
                               FILE *outputFd,
                               void (*aggFn)(const char*, ...)) = {
    nullptr,
};

long int
writeDictionary(char *buffer, char *endOfBuffer)
{
    (void)buffer;
    (void)endOfBuffer;
    return 0;
}

} // namespace GeneratedFunctions
