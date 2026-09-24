#include "MainWindow.h"
#include "OpenGLWidget.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>


static QLabel* createValueCard(
    const QString& title,
    const QString& value,
    const QString& unit
)
{
    auto* label = new QLabel;

    label->setAlignment(Qt::AlignCenter);

    label->setText(
        "<div style='font-size:14px;color:#8FA9B8;'>"
        + title +
        "</div>"
        "<div style='font-size:34px;font-weight:700;color:#55E6FF;'>"
        + value +
        "</div>"
        "<div style='font-size:12px;color:#8FA9B8;'>"
        + unit +
        "</div>"
    );

    label->setStyleSheet(
        "QLabel {"
        " background-color: #101B24;"
        " border: 1px solid #294554;"
        " border-radius: 12px;"
        " padding: 12px;"
        "}"
    );

    return label;
}


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Construction3DHMI");

    resize(1280, 720);

    setMinimumSize(1024, 600);


    // ========================================================
    // Central Widget
    // ========================================================

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

    auto* topBar =
        new QFrame;

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
        "font-size: 13px;"
        "color: #62E58B;"
        "border: none;"
    );


    auto* time =
        new QLabel("14:32");

    time->setStyleSheet(
        "font-size: 17px;"
        "font-weight: 600;"
        "border: none;"
    );


    topLayout->addWidget(logo);

    topLayout->addStretch();

    topLayout->addWidget(status);

    topLayout->addSpacing(40);

    topLayout->addWidget(time);


    root->addWidget(topBar);


    // ========================================================
    // MAIN AREA
    // ========================================================

    auto* mainArea =
        new QWidget;

    auto* mainLayout =
        new QHBoxLayout(mainArea);

    mainLayout->setContentsMargins(
        0, 0, 0, 0
    );

    mainLayout->setSpacing(8);


    // --------------------------------------------------------
    // LEFT : SOC
    // --------------------------------------------------------

    auto* soc =
        createValueCard(
            "SOC",
            "95",
            "%"
        );

    soc->setFixedWidth(180);

    mainLayout->addWidget(soc);


    // --------------------------------------------------------
    // CENTER : 3D
    // --------------------------------------------------------

    auto* openGLWidget =
        new OpenGLWidget;

    openGLWidget->setMinimumWidth(500);

    mainLayout->addWidget(
        openGLWidget,
        1
    );


    // --------------------------------------------------------
    // RIGHT : RPM
    // --------------------------------------------------------

    auto* rpm =
        createValueCard(
            "ENGINE SPEED",
            "950",
            "r/min"
        );

    rpm->setFixedWidth(180);

    mainLayout->addWidget(rpm);


    root->addWidget(
        mainArea,
        1
    );


    // ========================================================
    // VEHICLE STATUS
    // ========================================================

    auto* statusArea =
        new QWidget;

    statusArea->setFixedHeight(82);

    auto* statusLayout =
        new QHBoxLayout(statusArea);

    statusLayout->setContentsMargins(
        0, 0, 0, 0
    );

    statusLayout->setSpacing(8);


    statusLayout->addWidget(
        createValueCard(
            "COOLANT",
            "75",
            "°C"
        )
    );


    statusLayout->addWidget(
        createValueCard(
            "HYDRAULIC OIL",
            "68",
            "°C"
        )
    );


    statusLayout->addWidget(
        createValueCard(
            "WORK MODE",
            "STANDARD",
            ""
        )
    );


    root->addWidget(
        statusArea
    );


    // ========================================================
    // BOTTOM NAVIGATION
    // ========================================================

    auto* bottomBar =
        new QFrame;

    bottomBar->setFixedHeight(72);

    bottomBar->setStyleSheet(
        "QFrame {"
        " background-color: #0D1820;"
        " border: 1px solid #203746;"
        " border-radius: 8px;"
        "}"
    );


    auto* navLayout =
        new QHBoxLayout(bottomBar);

    navLayout->setContentsMargins(
        8, 7, 8, 7
    );

    navLayout->setSpacing(8);


    const QStringList navItems =
    {
        "⌂  HOME",
        "▣  VEHICLE",
        "⚡  ENERGY",
        "⌁  STATUS",
        "⚙  SETTINGS"
    };


    for (
        int i = 0;
        i < navItems.size();
        ++i
    )
    {
        auto* button =
            new QPushButton(
                navItems[i]
            );


        button->setCursor(
            Qt::PointingHandCursor
        );


        button->setStyleSheet(
            i == 0
            ?
            "QPushButton {"
            " background-color: #13769B;"
            " border: 1px solid #55E6FF;"
            " border-radius: 7px;"
            " color: white;"
            " font-size: 14px;"
            " font-weight: 600;"
            "}"
            :
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
            "}"
        );


        navLayout->addWidget(
            button,
            1
        );
    }


    root->addWidget(
        bottomBar
    );


    setCentralWidget(
        central
    );
}
