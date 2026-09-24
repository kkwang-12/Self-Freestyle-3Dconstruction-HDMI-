#pragma once

#include <QMainWindow>

class QLabel;
class QPushButton;
class QStackedWidget;
class QWidget;

class CircularGauge;
class VehicleState;
class VehicleSimulator;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    // Data
    VehicleState* vehicleState_ = nullptr;
    VehicleSimulator* simulator_ = nullptr;

    // Page system
    QStackedWidget* pageStack_ = nullptr;

    QPushButton* homeButton_ = nullptr;
    QPushButton* vehicleButton_ = nullptr;
    QPushButton* energyButton_ = nullptr;
    QPushButton* statusButton_ = nullptr;
    QPushButton* settingsButton_ = nullptr;

    // HOME page
    CircularGauge* socGauge_ = nullptr;
    CircularGauge* rpmGauge_ = nullptr;

    QLabel* coolantValue_ = nullptr;
    QLabel* hydraulicValue_ = nullptr;
    QLabel* workModeValue_ = nullptr;

private:
    QWidget* createHomePage();
    QWidget* createVehiclePage();
    QWidget* createEnergyPage();
    QWidget* createStatusPage();
    QWidget* createSettingsPage();

    QWidget* createBottomNavigation();

    void setupVehicleState();
    void switchPage(int index);
    void updateNavigationStyle(int activeIndex);
};
