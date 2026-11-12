#ifndef SIGNALCURVE_HPP
#define SIGNALCURVE_HPP

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <iostream>
#include "../math/mathutil.hpp"
#include <cstring>
#include <array>
#include <vector>
#include "../backend/distortion.hpp"

#define GRID_SIZE 25

class SignalCurve : public QGraphicsItem  {

public:

    SignalCurve(uint32_t pointCount) {
        m_points.resize(pointCount);
    }

    SignalCurve(QPointF* points, uint32_t pointCount) {
        m_points = {points, pointCount + points};
        translateToLocalXY();
    }
    
    ~SignalCurve() {
        qDebug() << "Log::SignalCurve::dtor()";
    }

    QRectF boundingRect() const override {
        qreal penWidth = 1.0f;
        return QRect(0, 0, 1000, 400);
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* options, QWidget* widget) override {

        painter->setPen(QPen(Qt::red, 1, Qt::SolidLine));
        painter->translate(QPoint(500, 200));
        painter->drawLines(m_points.data(), static_cast<int>(m_points.size()) / 2);

    }

    void setSignalPoints(QPointF* points, uint32_t pointCount) {
        m_points = {points, points + pointCount};
    }

    std::vector<QPointF>& getPoints() {
        return m_points;
    }

private:
    std::vector<QPointF> m_points{};

    const uint32_t OFFSET_X = 000;
    const uint32_t OFFSET_Y = 2000u;

    void translateToLocalXY() {
        for (uint32_t i = 0; i < m_points.size(); i++) {
            qreal& x = m_points[i].rx();
            qreal& y = m_points[i].ry();
            x *= GRID_SIZE;
            y *= GRID_SIZE;
        }
    }

};


class CoordinateSystem : public QGraphicsScene {
public:
    CoordinateSystem() {
        this->setSceneRect(0, 0, 1000, 400);
        m_width = 1000;
        m_height = 400;
    }

    ~CoordinateSystem() {
        qDebug() << "Log::CoordinateSystem::dtor()";
    }

    void setup() {
        drawGrid();
        drawAxis();
    }

    void addSignal(SignalCurve* signal) {
        this->addItem(signal);
    }

private:
    qreal m_width;
    qreal m_height;

    void drawAxis() {
        QPen axisPen(Qt::black, 1, Qt::SolidLine);

        this->addLine(QLineF(0.0, m_height / 2.0, m_width, m_height / 2.0), axisPen);
        this->addLine(QLineF(m_width / 2.0, 0.0, m_width / 2.0, m_height), axisPen);
    }

    void drawGrid() {

        uint32_t columnCount = (m_width / GRID_SIZE) + 1u;
        uint32_t rowCount = (m_height / GRID_SIZE) + 1u;

        QPen gridPen(QColor(128, 128, 128, 100), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

        for (uint16_t i = 0; i < rowCount; ++i) {
            addLine(0.0, GRID_SIZE * i, m_width, GRID_SIZE * i, gridPen);
        }
        for (uint16_t i = 0; i < columnCount; ++i) {
            addLine(GRID_SIZE * i, 0.0, GRID_SIZE * i, m_height, gridPen);
        }
    }

};



#endif // SIGNALCURVE_HPP