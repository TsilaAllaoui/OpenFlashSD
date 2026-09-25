#ifndef REQUESTS_H
#define REQUESTS_H

namespace openflash
{
    enum class request_status
    {
        PENDING,
        IDLE,
        RECENTLY_CHANGED
    };
    
    enum class confirmation_request_status
    {
        PENDING,
        NEGATIVE,
        POSITIVE
    };
}

#endif // REQUESTS_H