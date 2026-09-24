#include "VehicleSimulator.h"
#include "VehicleState.h"

#include <QTimer>

#include <cmath>

VehicleSimulator::VehicleSimulator(
    VehicleState* state,
    QObject* parent
)
    : QObject(parent),
      state_(state)
{
    timer_ = new QTimer(this);

    timer_->setInterval(100);

    connect(
        timer_,
        &QTimer::timeout,
        this,
        &VehicleSimulator::updateSimulation
    );
}

void VehicleSimulator::start()
{
    timer_->start();
}

void VehicleSimulator::updateSimulation()
{
    phase_ += 0.08f;

    // 模拟发动机怠速波动
    const int rpm =
        950 +
        static_cast<int>(
            std::sin(phase_) * 120.0f
        );

    // 冷却液温度缓慢变化
    const int coolant =
        75 +
        static_cast<int>(
            std::sin(phase_ * 0.15f) * 3.0f
        );

    // 液压油温度缓慢变化
    const int hydraulic =
        68 +
        static_cast<int>(
            std::sin(phase_ * 0.12f + 1.0f) * 4.0f
        );

    state_->setEngineRpm(rpm);
    state_->setCoolantTemp(coolant);
    state_->setHydraulicTemp(hydraulic);
}
