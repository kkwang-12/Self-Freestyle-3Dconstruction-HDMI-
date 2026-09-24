#include "VehicleState.h"

VehicleState::VehicleState(QObject* parent)
    : QObject(parent)
{
}

int VehicleState::soc() const
{
    return soc_;
}

int VehicleState::engineRpm() const
{
    return engineRpm_;
}

int VehicleState::coolantTemp() const
{
    return coolantTemp_;
}

int VehicleState::hydraulicTemp() const
{
    return hydraulicTemp_;
}

QString VehicleState::workMode() const
{
    return workMode_;
}

void VehicleState::setSoc(int value)
{
    if (soc_ == value)
        return;

    soc_ = value;
    emit socChanged(soc_);
}

void VehicleState::setEngineRpm(int value)
{
    if (engineRpm_ == value)
        return;

    engineRpm_ = value;
    emit engineRpmChanged(engineRpm_);
}

void VehicleState::setCoolantTemp(int value)
{
    if (coolantTemp_ == value)
        return;

    coolantTemp_ = value;
    emit coolantTempChanged(coolantTemp_);
}

void VehicleState::setHydraulicTemp(int value)
{
    if (hydraulicTemp_ == value)
        return;

    hydraulicTemp_ = value;
    emit hydraulicTempChanged(hydraulicTemp_);
}

void VehicleState::setWorkMode(const QString& mode)
{
    if (workMode_ == mode)
        return;

    workMode_ = mode;
    emit workModeChanged(workMode_);
}
