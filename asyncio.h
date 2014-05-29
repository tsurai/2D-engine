#ifndef ASYNCIO_H
#define ASYNCIO_H

#include <aio.h>
#include <errno.h>
#include <fcntl.h>

namespace asyncio {
    int write(int filedesc, __off_t offset, void *data, unsigned int nBytes, void callback(sigval_t));
}

#endif // ASYNCIO_H
