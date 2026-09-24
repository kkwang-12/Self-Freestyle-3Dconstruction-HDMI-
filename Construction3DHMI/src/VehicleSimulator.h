#pragma once

#include <QObject>

class QTimer;
class VehicleState;

class VehicleSimulator : public QObject
{
    Q_OBJECT

public:
    explicit VehicleSimulator(
        VehicleState* state,
        QObject* parent = nullptr
    );

    void start();

private slots:
    void updateSimulation();

private:
    VehicleState* state_ = nullptr;
    QTimer* timer_ = nullptr;

    float phase_ = 0.0f;
};
