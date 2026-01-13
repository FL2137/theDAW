#ifndef DISTORTION_HPP
#define DISTORTION_HPP

#include <array>
#include <limits>
#include "Effect.hpp"
#include <QObject>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class DistortionWidget;
}
QT_END_NAMESPACE

namespace digitaleffects
{

template<size_t N>
class Distortion : public EffectBase
{
public:

    inline void process(DataType* data) const
    {
        if (!status.load(std::memory_order_acquire))
        {
            return;
        }

        float distortionValue = distortion.load(std::memory_order_acquire);
        float levelValue = level.load(std::memory_order_acquire);
        float toneValue = tone.load(std::memory_order_acquire);

        for (int sampleIdx = 0; sampleIdx < N; ++sampleIdx)
        {
            auto& sample = data[sampleIdx];

            sample = std::tanhf(sample);

            if (sample > 0)
            {
                sample = 1.f - std::exp(-sample);
            }
            else if(sample < 0)
            {
                sample = -1.f + std::exp(sample);
            }
        }
    }

    std::atomic<bool> status;
    std::atomic<float> distortion;
    std::atomic<float> level;
    std::atomic<float> tone;
};

}

class DistortionWidget : public QWidget
{
public:
    Q_OBJECT
    DistortionWidget(QWidget* parent = nullptr);
    ~DistortionWidget();

signals:
    void changeStatus(bool newStatus);
    void changeDistortion(float newDistortion);
    void changeLevel(float newLevel);
    void changeTone(float newTone);

    void newEffectCreated(digitaleffects::EffectBase* effect);

private:
    digitaleffects::Distortion* distortionLogic;

    Ui::DistortionWidget* ui;
};

#endif // DISTORTION_HPP