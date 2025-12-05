#ifndef EFFECT_HPP
#define EFFECT_HPP

namespace digitaleffects {

class EffectBase {
public:
    virtual void process() = 0;

    unsigned int id;
    bool status;
};

}

#endif // EFFECT_HPP