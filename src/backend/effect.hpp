#ifndef EFFECT_HPP
#define EFFECT_HPP

#include <QWidget>

namespace digitaleffects {

using DataType = float;

enum EffectStatus
{
    INVALID = 0,
    UNUSED = 1,
    IDLE = 2,
    RUNNING = 3
};

template<size_t N>
class EffectBase {
public:
    unsigned int id;

    virtual void process() = 0;
};

}

#endif // EFFECT_HPP