#include "distortion.hpp"
#include "ui_distortionwidget.h"


DistortionWidget::DistortionWidget(QWidget* parent) : QWidget(parent)
{
    connect(ui->powerButton, &QPushButton::pressed, this, [this](){
        distortionLogic->status = !distortionLogic->status.load();
    });
    
    connect(ui->levelDial, &QDial::valueChanged, this, [this](float newLevel){
        distortionLogic->level = newLevel;
    });
    
    connect(ui->toneDial, &QDial::valueChanged, this,[this](float newTone){
            distortionLogic->tone = newTone;
    });

    connect(ui->distDial, &QDial::valueChanged, this, [this](float newDistortion) {
        distortionLogic->distortion = newDistortion;
    });
}
