#include "distortion.hpp"
#include "ui_distortionwidget.h"
#include "../gui/mainwindow.hpp"

DistortionWidget::DistortionWidget(QWidget* parent) : QWidget(parent)
{
    
    this->distortionLogic = new digitaleffects::Distortion();

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

    if (auto mw = qobject_cast<MainWindow*>(parent))
    {
        connect(this, &DistortionWidget::newEffectCreated, mw, &MainWindow::onNewEffectCreated);
    }
}

DistortionWidget::~DistortionWidget()
{
    delete ui;
    delete distortionLogic;
}