#ifndef NANOLOG_PORTABILITY_H
#define NANOLOG_PORTABILITY_H

#include <cstddef>
#include <cstdint>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <fcntl.h>
#include <io.h>
#include <malloc.h>
#include <process.h>
#include <sys/types.h>
#include <time.h>
#include <windows.h>
#ifdef ERROR
#undef ERROR
#endif
#ifndef O_DIRECT
#define O_DIRECT 0
#endif
#else
#include <aio.h>
#include <fcntl.h>
#include <sched.h>
#include <unistd.h>
#endif

#ifdef _MSC_VER
#define NANOLOG_ALWAYS_INLINE __forceinline
#elif defined(__GNUC__)
#define NANOLOG_ALWAYS_INLINE inline __attribute__((__always_inline__))
#else
#define NANOLOG_ALWAYS_INLINE inline
#endif

#ifdef _MSC_VER
#define NANOLOG_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
#define NANOLOG_NOINLINE __attribute__((__noinline__))
#else
#define NANOLOG_NOINLINE
#endif

#ifdef _MSC_VER
#define NANOLOG_PACK_PUSH __pragma(pack(push, 1))
#define NANOLOG_PACK_POP __pragma(pack(pop))
#elif defined(__GNUC__)
#define NANOLOG_PACK_PUSH _Pragma("pack(push, 1)")
#define NANOLOG_PACK_POP _Pragma("pack(pop)")
#else
#define NANOLOG_PACK_PUSH
#define NANOLOG_PACK_POP
#endif

#if _MSC_VER

#ifdef _USE_ATTRIBUTES_FOR_SAL
#undef _USE_ATTRIBUTES_FOR_SAL
#endif

#define _USE_ATTRIBUTES_FOR_SAL 1
#include <sal.h>

#define NANOLOG_PRINTF_FORMAT _Printf_format_string_
#define NANOLOG_PRINTF_FORMAT_ATTR(string_index, first_to_check)

#elif defined(__GNUC__)
#define NANOLOG_PRINTF_FORMAT
#define NANOLOG_PRINTF_FORMAT_ATTR(string_index, first_to_check) \
  __attribute__((__format__(__printf__, string_index, first_to_check)))
#else
#define NANOLOG_PRINTF_FORMAT
#define NANOLOG_PRINTF_FORMAT_ATTR(string_index, first_to_check)
#endif

#ifdef _WIN32
typedef SSIZE_T ssize_t;

inline char *
stpcpy(char *dest, const char *src)
{
    while (*src != '\0') {
        *dest++ = *src++;
    }
    *dest = '\0';
    return dest;
}

struct cpu_set_t {
    DWORD_PTR mask;
};

#ifndef CPU_ZERO
#define CPU_ZERO(set) ((set)->mask = 0)
#endif

#ifndef CPU_SET
#define CPU_SET(cpu, set) ((set)->mask |= (DWORD_PTR(1) << (cpu)))
#endif
#endif

namespace NanoLogPlatform {

#ifdef _WIN32
struct AioCb {
    int aio_fildes = -1;
    void *aio_buf = nullptr;
    std::size_t aio_nbytes = 0;
    int errorCode = 0;
    ssize_t result = 0;
    bool pending = false;
};
#else
using AioCb = aiocb;
#endif

inline int
openFile(const char *filename, int flags, int mode)
{
#ifdef _WIN32
    return _open(filename, flags, mode);
#else
    return open(filename, flags, mode);
#endif
}

inline int
closeFile(int fd)
{
#ifdef _WIN32
    return _close(fd);
#else
    return close(fd);
#endif
}

inline int
accessFile(const char *filename, int mode)
{
#ifdef _WIN32
    return _access(filename, mode);
#else
    return access(filename, mode);
#endif
}

inline int
flushFile(int fd)
{
#ifdef _WIN32
    return _commit(fd);
#else
    return fdatasync(fd);
#endif
}

inline int
alignedAlloc(void **ptr, std::size_t alignment, std::size_t size)
{
#ifdef _WIN32
    *ptr = _aligned_malloc(size, alignment);
    return (*ptr == nullptr) ? ENOMEM : 0;
#else
    return posix_memalign(ptr, alignment, size);
#endif
}

inline void
alignedFree(void *ptr)
{
#ifdef _WIN32
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

inline int
currentCpu()
{
#ifdef _WIN32
    return static_cast<int>(GetCurrentProcessorNumber());
#else
    return sched_getcpu();
#endif
}

inline int
aioWrite(AioCb *cb)
{
#ifdef _WIN32
    cb->pending = true;
    errno = 0;
    cb->result = _write(cb->aio_fildes,
                        cb->aio_buf,
                        static_cast<unsigned int>(cb->aio_nbytes));
    cb->errorCode = (cb->result < 0) ? errno : 0;
    cb->pending = false;
    return (cb->result < 0) ? -1 : 0;
#else
    return aio_write(cb);
#endif
}

inline int
aioError(AioCb *cb)
{
#ifdef _WIN32
    return cb->pending ? EINPROGRESS : cb->errorCode;
#else
    return aio_error(cb);
#endif
}

inline ssize_t
aioReturn(AioCb *cb)
{
#ifdef _WIN32
    return cb->result;
#else
    return aio_return(cb);
#endif
}

inline int
aioSuspend(const AioCb *const list[], int nent, const struct timespec *timeout)
{
#ifdef _WIN32
    (void)list;
    (void)nent;
    (void)timeout;
    return 0;
#else
    return aio_suspend(list, nent, timeout);
#endif
}

} // namespace NanoLogPlatform

#endif /* NANOLOG_PORTABILITY_H */
