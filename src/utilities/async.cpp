#include "utilities/async.h"

namespace openflash
{
    namespace async
    {
        void delay(int frames)
        {
            for (int i = 0; i < frames; i++)
                bn::core::update();
        }
    }
}