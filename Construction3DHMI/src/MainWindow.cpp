#include "MainWindow.h"

#include "CircularGauge.h"
#include "OpenGLWidget.h"
#include "VehicleSimulator.h"
#include "VehicleState.h"
#include <QSizePolicy>

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>


// ============================================================
// Small status card
// ============================================================

static QWidget* createValueCard(
    const QString& title,
    QLabel*& valueLabel,
    const QString& unit
)
{
    auto* card = new QFrame;

    card->setStyleSheet(
        "QFrame {"
        " background-color: #101B24;"
        " border: 1px solid #294554;"
        " border-radius: 8px;"
        "}"
        "QLabel {"
        " border: none;"
        " background: transparent;"
        "}"
    );

    auto* layout = new QVBoxLayout(card);

    layout->setContentsMargins(8, 6, 8, 6);
    layout->setSpacing(1);
    layout->setAlignment(Qt::AlignCenter);

    auto* titleLabel = new QLabel(title);

    titleLabel->setAlignment(Qt::AlignCenter);

    titleLabel->setStyleSheet(
        "color: #8FA9B8;"
        "font-size: 11px;"
    );

    valueLabel = new QLabel("--");

    valueLabel->setAlignment(Qt::AlignCenter);

    valueLabel->setStyleSheet(
        "color: #55E6FF;"
        "font-size: 27px;"
        "font-weight: 700;"
    );

    auto* unitLabel = new QLabel(unit);

    unitLabel->setAlignment(Qt::AlignCenter);

    unitLabel->setStyleSheet(
        "color: #8FA9B8;"
        "font-size: 10px;"
    );

    layout->addWidget(titleLabel);
    layout->addWidget(valueLabel);
    layout->addWidget(unitLabel);

    return card;
}


// ============================================================
// Placeholder page
// ============================================================

static QWidget* createPlaceholderPage(
    const QString& title,
    const QString& description
)
{
    auto* page = new QWidget;

    auto* layout = new QVBoxLayout(page);

    layout->setAlignment(Qt::AlignCenter);

    auto* titleLabel = new QLabel(title);

    titleLabel->setAlignment(Qt::AlignCenter);

    titleLabel->setStyleSheet(
        "color: #55E6FF;"
        "font-size: 34px;"
        "font-weight: 700;"
    );

    auto* descriptionLabel =
        new QLabel(description);

    descriptionLabel->setAlignment(Qt::AlignCenter);

    descriptionLabel->setStyleSheet(
        "color: #78909C;"
        "font-size: 15px;"
    );

    layout->addStretch();

    layout->addWidget(titleLabel);

    layout->addSpacing(12);

    layout->addWidget(descriptionLabel);

    layout->addStretch();

    return page;
}


// ============================================================
// Constructor
// ============================================================

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Construction3DHMI");

    resize(1280, 720);

    setMinimumSize(1024, 600);


    // --------------------------------------------------------
    // Global background
    // --------------------------------------------------------

    auto* central = new QWidget(this);

    central->setStyleSheet(
        "QWidget {"
        " background-color: #071017;"
        " color: white;"
        "}"
    );


    auto* root =
        new QVBoxLayout(central);

    root->setContentsMargins(
        12, 10, 12, 10
    );

    root->setSpacing(8);


    // ========================================================
    // TOP BAR
    // ========================================================

    auto* topBar = new QFrame;

    topBar->setFixedHeight(52);

    topBar->setStyleSheet(
        "QFrame {"
        " background-color: #0D1820;"
        " border: 1px solid #203746;"
        " border-radius: 8px;"
        "}"
    );


    auto* topLayout =
        new QHBoxLayout(topBar);

    topLayout->setContentsMargins(
        18, 0, 18, 0
    );


    auto* logo =
        new QLabel("CET200");

    logo->setStyleSheet(
        "font-size: 20px;"
        "font-weight: 700;"
        "color: #55E6FF;"
        "border: none;"
    );


    auto* status =
        new QLabel("●  SYSTEM NORMAL");

    status->setStyleSheet(
        "font-size: 12px;"
        "color: #62E58B;"
        "border: none;"
    );


    auto* time =
        new QLabel("14:32");

    time->setStyleSheet(
        "font-size: 16px;"
        "font-weight: 700;"
        "border: none;"
    );


    topLayout->addWidget(logo);

    topLayout->addStretch();

    topLayout->addWidget(status);

    topLayout->addSpacing(40);

    topLayout->addWidget(time);


    root->addWidget(topBar);


    // ========================================================
    // PAGE STACK
    // ========================================================

    pageStack_ =
        new QStackedWidget;

    pageStack_->setStyleSheet(
        "QStackedWidget {"
        " border: none;"
        " background: #071017;"
        "}"
    );


    pageStack_->addWidget(
        createHomePage()
    );

    pageStack_->addWidget(
        createVehiclePage()
    );

    pageStack_->addWidget(
        createEnergyPage()
    );

    pageStack_->addWidget(
        createStatusPage()
    );

    pageStack_->addWidget(
        createSettingsPage()
    );


    root->addWidget(
        pageStack_,
        1
    );


    // ========================================================
    // NAVIGATION
    // ========================================================

    root->addWidget(
        createBottomNavigation()
    );


    setCentralWidget(central);


    // ========================================================
    // Vehicle data
    // ========================================================

    setupVehicleState();

    switchPage(0);
}


// ============================================================
// HOME PAGE
// ============================================================

QWidget* MainWindow::createHomePage()
{
    auto* page = new QWidget;

    auto* root = new QVBoxLayout(page);

    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(8);


    // ========================================================
    // MAIN DISPLAY AREA
    // ========================================================

    auto* mainArea = new QWidget;

    mainArea->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );

    auto* mainLayout = new QHBoxLayout(mainArea);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(8);


    // ========================================================
    // LEFT : SOC GAUGE
    // ========================================================

    socGauge_ = new CircularGauge(mainArea);

    socGauge_->setTitle("SOC");
    socGauge_->setUnit("%");

    socGauge_->setRange(
        0,
        100
    );

    socGauge_->setMajorTickCount(6);
    socGauge_->setMinorTicksPerMajor(4);

    socGauge_->setValue(95);

    socGauge_->setMinimumSize(
        250,
        250
    );

    socGauge_->setMaximumWidth(
        330
    );

    socGauge_->setSizePolicy(
        QSizePolicy::Preferred,
        QSizePolicy::Expanding
    );


    // ========================================================
    // CENTER : CET200 3D
    // ========================================================

    auto* openGLWidget =
        new OpenGLWidget(mainArea);

    openGLWidget->setMinimumSize(
        500,
        300
    );

    openGLWidget->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );


    // ========================================================
    // RIGHT : ENGINE RPM GAUGE
    // ========================================================

    rpmGauge_ = new CircularGauge(mainArea);

    rpmGauge_->setTitle("ENGINE");
    rpmGauge_->setUnit("r/min");

    rpmGauge_->setRange(
        0,
        2500
    );

    rpmGauge_->setMajorTickCount(6);
    rpmGauge_->setMinorTicksPerMajor(4);

    rpmGauge_->setValue(950);

    rpmGauge_->setMinimumSize(
        250,
        250
    );

    rpmGauge_->setMaximumWidth(
        330
    );

    rpmGauge_->setSizePolicy(
        QSizePolicy::Preferred,
        QSizePolicy::Expanding
    );


    // ========================================================
    // ADD TO MAIN LAYOUT
    // ========================================================

    mainLayout->addWidget(
        socGauge_,
        0,
        Qt::AlignVCenter
    );

    mainLayout->addWidget(
        openGLWidget,
        1
    );

    mainLayout->addWidget(
        rpmGauge_,
        0,
        Qt::AlignVCenter
    );


    // ========================================================
    // VEHICLE INFORMATION
    // ========================================================

    auto* vehicleInfo =
        new QWidget(page);

    vehicleInfo->setFixedHeight(82);


    auto* infoLayout =
        new QHBoxLayout(vehicleInfo);

    infoLayout->setContentsMargins(
        0, 0, 0, 0
    );

    infoLayout->setSpacing(8);


    infoLayout->addWidget(
        createValueCard(
            "COOLANT",
            coolantValue_,
            "°C"
        )
    );


    infoLayout->addWidget(
        createValueCard(
            "HYDRAULIC OIL",
            hydraulicValue_,
            "°C"
        )
    );


    infoLayout->addWidget(
        createValueCard(
            "WORK MODE",
            workModeValue_,
            ""
        )
    );


    // ========================================================
    // IMPORTANT:
    // mainArea gets all remaining vertical space
    // ========================================================

    root->addWidget(
        mainArea,
        1
    );

    root->addWidget(
        vehicleInfo,
        0
    );


    return page;
}



// ============================================================
// VEHICLE PAGE
// ============================================================

QWidget* MainWindow::createVehiclePage()
{
    return createPlaceholderPage(
        "VEHICLE",
        "Vehicle parameters / kinematics / actuator state"
    );
}


// ============================================================
// ENERGY PAGE
// ============================================================

QWidget* MainWindow::createEnergyPage()
{
    return createPlaceholderPage(
        "ENERGY",
        "Battery / power / energy management"
    );
}


// ============================================================
// STATUS PAGE
// ============================================================

QWidget* MainWindow::createStatusPage()
{
    return createPlaceholderPage(
        "STATUS",
        "System status / alarm / diagnostics"
    );
}


// ============================================================
// SETTINGS PAGE
// ============================================================

QWidget* MainWindow::createSettingsPage()
{
    return createPlaceholderPage(
        "SETTINGS",
        "Display / system / HMI configuration"
    );
}


// ============================================================
// Bottom Navigation
// ============================================================

QWidget* MainWindow::createBottomNavigation()
{
    auto* bottomBar =
        new QFrame;

    bottomBar->setFixedHeight(
        72
    );


    bottomBar->setStyleSheet(
        "QFrame {"
        " background-color: #0D1820;"
        " border: 1px solid #203746;"
        " border-radius: 8px;"
        "}"
    );


    auto* layout =
        new QHBoxLayout(bottomBar);

    layout->setContentsMargins(
        8, 7, 8, 7
    );

    layout->setSpacing(8);


    homeButton_ =
        new QPushButton(
            "⌂  HOME"
        );


    vehicleButton_ =
        new QPushButton(
            "▣  VEHICLE"
        );


    energyButton_ =
        new QPushButton(
            "⚡  ENERGY"
        );


    statusButton_ =
        new QPushButton(
            "⌁  STATUS"
        );


    settingsButton_ =
        new QPushButton(
            "⚙  SETTINGS"
        );


    QPushButton* buttons[] =
    {
        homeButton_,
        vehicleButton_,
        energyButton_,
        statusButton_,
        settingsButton_
    };


    for (auto* button : buttons)
    {
        button->setCursor(
            Qt::PointingHandCursor
        );

        button->setMinimumHeight(
            48
        );

        layout->addWidget(
            button,
            1
        );
    }


    connect(
        homeButton_,
        &QPushButton::clicked,
        this,
        [this]()
        {
            switchPage(0);
        }
    );


    connect(
        vehicleButton_,
        &QPushButton::clicked,
        this,
        [this]()
        {
            switchPage(1);
        }
    );


    connect(
        energyButton_,
        &QPushButton::clicked,
        this,
        [this]()
        {
            switchPage(2);
        }
    );


    connect(
        statusButton_,
        &QPushButton::clicked,
        this,
        [this]()
        {
            switchPage(3);
        }
    );


    connect(
        settingsButton_,
        &QPushButton::clicked,
        this,
        [this]()
        {
            switchPage(4);
        }
    );


    return bottomBar;
}


// ============================================================
// Page switching
// ============================================================

void MainWindow::switchPage(int index)
{
    if (!pageStack_)
        return;


    pageStack_->setCurrentIndex(
        index
    );


    updateNavigationStyle(
        index
    );
}


// ============================================================
// Navigation style
// ============================================================

void MainWindow::updateNavigationStyle(
    int activeIndex
)
{
    QPushButton* buttons[] =
    {
        homeButton_,
        vehicleButton_,
        energyButton_,
        statusButton_,
        settingsButton_
    };


    const QString normalStyle =
        "QPushButton {"
        " background-color: #102532;"
        " border: 1px solid #294554;"
        " border-radius: 7px;"
        " color: #D5E4EB;"
        " font-size: 14px;"
        "}"
        "QPushButton:hover {"
        " background-color: #174258;"
        " border-color: #55E6FF;"
        "}";


    const QString activeStyle =
        "QPushButton {"
        " background-color: #13769B;"
        " border: 1px solid #55E6FF;"
        " border-radius: 7px;"
        " color: white;"
        " font-size: 14px;"
        " font-weight: 700;"
        "}";


    for (int i = 0; i < 5; ++i)
    {
        buttons[i]->setStyleSheet(
            i == activeIndex
                ? activeStyle
                : normalStyle
        );
    }
}


// ============================================================
// Vehicle State
// ============================================================

void MainWindow::setupVehicleState()
{
    vehicleState_ =
        new VehicleState(this);


    simulator_ =
        new VehicleSimulator(
            vehicleState_,
            this
        );


    // --------------------------------------------------------
    // SOC
    // --------------------------------------------------------

    connect(
        vehicleState_,
        &VehicleState::socChanged,
        this,
        [this](int value)
        {
            socGauge_->setValue(
                value
            );
        }
    );


    // --------------------------------------------------------
    // Engine RPM
    // --------------------------------------------------------

    connect(
        vehicleState_,
        &VehicleState::engineRpmChanged,
        this,
        [this](int value)
        {
            rpmGauge_->setValue(
                value
            );
        }
    );


    // --------------------------------------------------------
    // Coolant
    // --------------------------------------------------------

    connect(
        vehicleState_,
        &VehicleState::coolantTempChanged,
        this,
        [this](int value)
        {
            coolantValue_->setText(
                QString::number(value)
            );
        }
    );


    // --------------------------------------------------------
    // Hydraulic oil
    // --------------------------------------------------------

    connect(
        vehicleState_,
        &VehicleState::hydraulicTempChanged,
        this,
        [this](int value)
        {
            hydraulicValue_->setText(
                QString::number(value)
            );
        }
    );


    // --------------------------------------------------------
    // Work mode
    // --------------------------------------------------------

    connect(
        vehicleState_,
        &VehicleState::workModeChanged,
        this,
        [this](
            const QString& mode
        )
        {
            workModeValue_->setText(
                mode
            );
        }
    );


    // ========================================================
    // Initial values
    // ========================================================

    socGauge_->setValue(
        vehicleState_->soc()
    );


    rpmGauge_->setValue(
        vehicleState_->engineRpm()
    );


    coolantValue_->setText(
        QString::number(
            vehicleState_->coolantTemp()
        )
    );


    hydraulicValue_->setText(
        QString::number(
            vehicleState_->hydraulicTemp()
        )
    );


    workModeValue_->setText(
        vehicleState_->workMode()
    );


    simulator_->start();
}
