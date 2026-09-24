#pragma once

#include <QObject>
#include <QString>

class VehicleState : public QObject
{
    Q_OBJECT

public:
    explicit VehicleState(QObject* parent = nullptr);

    int soc() const;
    int engineRpm() const;
    int coolantTemp() const;
    int hydraulicTemp() const;
    QString workMode() const;

    void setSoc(int value);
    void setEngineRpm(int value);
    void setCoolantTemp(int value);
    void setHydraulicTemp(int value);
    void setWorkMode(const QString& mode);

signals:
    void socChanged(int value);
    void engineRpmChanged(int value);
    void coolantTempChanged(int value);
    void hydraulicTempChanged(int value);
    void workModeChanged(const QString& mode);

private:
    int soc_ = 95;
    int engineRpm_ = 950;
    int coolantTemp_ = 75;
    int hydraulicTemp_ = 68;

    QString workMode_ = "STANDARD";
};
