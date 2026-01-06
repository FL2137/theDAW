#ifndef DISTORTION_HPP
#define DISTORTION_HPP

#include <array>
#include <limits>
#include "Effect.hpp"
#include <QObject>
#include <QMainWindow>
#include "../gui/mainwindow.hpp"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class DistortionWidget;
}
QT_END_NAMESPACE

namespace digitaleffects
{

class Distortion : public EffectBase
{
public:

    template<typename T>
    inline void processSingleSample(T& data) const
    {
        if (!status.load(std::memory_order_acquire))
        {
            return;
        }

        float distortionValue = distortion.load(std::memory_order_acquire);
        float levelValue = level.load(std::memory_order_acquire);
        float toneValue = tone.load(std::memory_order_acquire);

        data = std::tanhf(data);

        if (data > 0)
        {
            data = 1.f - std::exp(-data);
        }
        else if(data < 0)
        {
            data = -1.f + std::exp(data);
        }
    }

    template<typename T, size_t N>
    inline void process(T* data) const
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
    DistortionWidget(QMainWindow* parent, digitaleffects::Distortion& distortion) : QWidget(parent)
    {
        connect(this, &DistortionWidget::changeStatus, this, [this](bool newStatus){
            distortionLogic->status = newStatus;
        });

        connect(this, &DistortionWidget::changeDistortion, this, [this](float newDistortion){
            distortionLogic->distortion;
        });

        connect(this, &DistortionWidget::changeLevel, this, [this](float newLevel){
            distortionLogic->level = newLevel;
        });

        connect(this, &DistortionWidget::changeTone, this,[this](float newTone){
            distortionLogic->tone = newTone;
        });
    }

    ~DistortionWidget() = default;

private slots:
    void changeStatus(bool newStatus);
    void changeDistortion(float newDistortion);
    void changeLevel(float newLevel);
    void changeTone(float newTone);

    std::shared_ptr<digitaleffects::Distortion> distortionLogic;
};

#endif // DISTORTION_HPP