#include "asyncio.h"
#include <stdio.h>
#include <QDebug>

using namespace asyncio;

int asyncio::write(int filedesc, __off_t offset, void *data, unsigned int nBytes, void callback(sigval_t)) {
    int fd = open("./logs/text.txt", O_RDWR | O_APPEND);
    void *buf = (void *)malloc(10);

    aiocb aio;
    aio.aio_fildes = fd;
    aio.aio_offset = offset;
    aio.aio_buf = buf;
    aio.aio_reqprio = 0;
    aio.aio_nbytes = nBytes;

    //event.sigev_notify = SIGEV_THREAD;
    //event.sigev_notify_function = callback;
    aio.aio_sigevent.sigev_notify = SIGEV_NONE;

    if(aio_write(&aio) == -1);
        return errno;

    return 0;
}
