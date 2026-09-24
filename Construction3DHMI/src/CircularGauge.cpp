#include "CircularGauge.h"

#include <QPainter>
#include <QPainterPath>
#include <QPen>

#include <algorithm>
#include <cmath>

namespace
{
constexpr double PI = 3.14159265358979323846;

// 仪表从左下开始，顺时针 270°
constexpr double START_ANGLE = 225.0;
constexpr double SWEEP_ANGLE = 270.0;
}


// ============================================================
// Constructor
// ============================================================

CircularGauge::CircularGauge(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(190, 190);

    animation_ =
        new QVariantAnimation(this);

    animation_->setDuration(220);

    animation_->setEasingCurve(
        QEasingCurve::OutCubic
    );

    connect(
        animation_,
        &QVariantAnimation::valueChanged,
        this,
        [this](const QVariant& value)
        {
            displayValue_ =
                value.toDouble();

            update();
        }
    );
}


// ============================================================
// Public API
// ============================================================

void CircularGauge::setTitle(
    const QString& title
)
{
    title_ = title;
    update();
}


void CircularGauge::setUnit(
    const QString& unit
)
{
    unit_ = unit;
    update();
}


void CircularGauge::setRange(
    double minimum,
    double maximum
)
{
    minimum_ = minimum;
    maximum_ = maximum;

    targetValue_ =
        std::clamp(
            targetValue_,
            minimum_,
            maximum_
        );

    displayValue_ =
        std::clamp(
            displayValue_,
            minimum_,
            maximum_
        );

    update();
}


void CircularGauge::setMajorTickCount(
    int count
)
{
    majorTickCount_ =
        std::max(2, count);

    update();
}


void CircularGauge::setMinorTicksPerMajor(
    int count
)
{
    minorTicksPerMajor_ =
        std::max(0, count);

    update();
}


// ============================================================
// Animated value
// ============================================================

void CircularGauge::setValue(double value)
{
    value =
        std::clamp(
            value,
            minimum_,
            maximum_
        );

    targetValue_ = value;

    animation_->stop();

    animation_->setStartValue(
        displayValue_
    );

    animation_->setEndValue(
        targetValue_
    );

    animation_->start();
}


// ============================================================
// Helpers
// ============================================================

double CircularGauge::valueRatio() const
{
    if (maximum_ <= minimum_)
        return 0.0;

    return std::clamp(
        (displayValue_ - minimum_) /
        (maximum_ - minimum_),
        0.0,
        1.0
    );
}


QPointF CircularGauge::pointOnCircle(
    const QPointF& center,
    double radius,
    double angleDeg
) const
{
    // Qt 屏幕坐标 Y 向下
    const double rad =
        angleDeg *
        PI /
        180.0;

    return QPointF(
        center.x() +
            std::cos(rad) * radius,

        center.y() -
            std::sin(rad) * radius
    );
}


// ============================================================
// Paint
// ============================================================

void CircularGauge::paintEvent(
    QPaintEvent*
)
{
    QPainter painter(this);

    painter.setRenderHint(
        QPainter::Antialiasing,
        true
    );

    const double side =
        std::min(
            width(),
            height()
        );

    const QPointF center(
        width() / 2.0,
        height() / 2.0
    );

    const double radius =
        side * 0.44;


    // ========================================================
    // BACKGROUND
    // ========================================================

    painter.setPen(
        QPen(
            QColor("#24323A"),
            1.0
        )
    );

    painter.setBrush(
        QColor("#071017")
    );

    painter.drawEllipse(
        center,
        radius + 8,
        radius + 8
    );


    // ========================================================
    // OUTER RING
    // ========================================================

    painter.setBrush(
        Qt::NoBrush
    );

    painter.setPen(
        QPen(
            QColor("#202A30"),
            7.0
        )
    );

    painter.drawEllipse(
        center,
        radius,
        radius
    );


    // ========================================================
    // BLUE INNER ARC
    // ========================================================

    QRectF arcRect(
        center.x() - radius + 13,
        center.y() - radius + 13,
        (radius - 13) * 2,
        (radius - 13) * 2
    );

    painter.setPen(
        QPen(
            QColor("#244E59"),
            4.0,
            Qt::SolidLine,
            Qt::RoundCap
        )
    );

    painter.drawArc(
        arcRect,
        static_cast<int>(
            START_ANGLE * 16
        ),
        static_cast<int>(
            -SWEEP_ANGLE * 16
        )
    );


    // ========================================================
    // ACTIVE ARC
    // ========================================================

    const double ratio =
        valueRatio();

    painter.setPen(
        QPen(
            QColor("#42DDF5"),
            4.5,
            Qt::SolidLine,
            Qt::RoundCap
        )
    );

    painter.drawArc(
        arcRect,
        static_cast<int>(
            START_ANGLE * 16
        ),
        static_cast<int>(
            -SWEEP_ANGLE *
            ratio *
            16
        )
    );


    // ========================================================
    // TICKS
    // ========================================================

    const int intervals =
        majorTickCount_ - 1;

    const int totalSteps =
        intervals *
        (minorTicksPerMajor_ + 1);


    for (
        int i = 0;
        i <= totalSteps;
        ++i
    )
    {
        const double t =
            static_cast<double>(i) /
            totalSteps;

        const double angle =
            START_ANGLE -
            SWEEP_ANGLE * t;

        const bool major =
            i %
            (minorTicksPerMajor_ + 1)
            == 0;


        const double outerRadius =
            radius - 1;

        const double innerRadius =
            major
            ? radius - 15
            : radius - 9;


        const QPointF p1 =
            pointOnCircle(
                center,
                outerRadius,
                angle
            );

        const QPointF p2 =
            pointOnCircle(
                center,
                innerRadius,
                angle
            );


        painter.setPen(
            QPen(
                major
                    ? QColor("#F1F7F9")
                    : QColor("#56636A"),

                major
                    ? 3.5
                    : 2.0,

                Qt::SolidLine,
                Qt::RoundCap
            )
        );


        painter.drawLine(
            p1,
            p2
        );
    }


    // ========================================================
    // NUMERIC SCALE
    // ========================================================

    QFont scaleFont =
        painter.font();

    scaleFont.setPixelSize(
        std::max(
            10,
            static_cast<int>(
                side * 0.055
            )
        )
    );

    scaleFont.setWeight(
        QFont::Medium
    );

    painter.setFont(
        scaleFont
    );

    painter.setPen(
        QColor("#E6EEF2")
    );


    for (
        int i = 0;
        i < majorTickCount_;
        ++i
    )
    {
        const double t =
            static_cast<double>(i) /
            (majorTickCount_ - 1);

        const double angle =
            START_ANGLE -
            SWEEP_ANGLE * t;


        const double value =
            minimum_ +
            (maximum_ - minimum_) *
            t;


        const QPointF pos =
            pointOnCircle(
                center,
                radius * 0.70,
                angle
            );


        const QString text =
            QString::number(
                static_cast<int>(
                    std::round(value)
                )
            );


        QRectF textRect(
            pos.x() - 30,
            pos.y() - 14,
            60,
            28
        );


        painter.drawText(
            textRect,
            Qt::AlignCenter,
            text
        );
    }


    // ========================================================
    // INNER DARK ARC
    // ========================================================

    const double innerRadius =
        radius * 0.52;


    QRectF innerArcRect(
        center.x() - innerRadius,
        center.y() - innerRadius,
        innerRadius * 2,
        innerRadius * 2
    );


    painter.setPen(
        QPen(
            QColor("#1B2025"),
            9.0,
            Qt::SolidLine,
            Qt::FlatCap
        )
    );


    painter.drawArc(
        innerArcRect,
        static_cast<int>(
            START_ANGLE * 16
        ),
        static_cast<int>(
            -SWEEP_ANGLE * 16
        )
    );


    // ========================================================
    // NEEDLE
    // ========================================================

    const double needleAngle =
        START_ANGLE -
        SWEEP_ANGLE *
        ratio;


    const QPointF needleTip =
        pointOnCircle(
            center,
            radius * 0.72,
            needleAngle
        );


    const QPointF needleStart =
        pointOnCircle(
            center,
            radius * 0.25,
            needleAngle + 180.0
        );


    // subtle glow
    painter.setPen(
        QPen(
            QColor(
                66,
                221,
                245,
                70
            ),
            8.0,
            Qt::SolidLine,
            Qt::RoundCap
        )
    );


    painter.drawLine(
        needleStart,
        needleTip
    );


    // needle
    painter.setPen(
        QPen(
            QColor("#F3F8FA"),
            3.0,
            Qt::SolidLine,
            Qt::RoundCap
        )
    );


    painter.drawLine(
        needleStart,
        needleTip
    );


    // ========================================================
    // CENTER HUB
    // ========================================================

    painter.setPen(
        Qt::NoPen
    );

    painter.setBrush(
        QColor("#42DDF5")
    );

    painter.drawEllipse(
        center,
        4.0,
        4.0
    );


    // ========================================================
    // TITLE
    // ========================================================

    QFont titleFont =
        painter.font();

    titleFont.setPixelSize(
        std::max(
            10,
            static_cast<int>(
                side * 0.045
            )
        )
    );

    titleFont.setWeight(
        QFont::Medium
    );


    painter.setFont(
        titleFont
    );

    painter.setPen(
        QColor("#8FA9B8")
    );


    QRectF titleRect(
        center.x() -
            radius * 0.5,

        center.y() -
            radius * 0.26,

        radius,

        25
    );


    painter.drawText(
        titleRect,
        Qt::AlignCenter,
        title_
    );


    // ========================================================
    // CENTER VALUE
    // ========================================================

    QFont valueFont =
        painter.font();

    valueFont.setPixelSize(
        std::max(
            28,
            static_cast<int>(
                side * 0.17
            )
        )
    );

    valueFont.setWeight(
        QFont::Light
    );


    painter.setFont(
        valueFont
    );

    painter.setPen(
        QColor("#F4F8FA")
    );


    QRectF valueRect(
        center.x() -
            radius * 0.65,

        center.y() -
            radius * 0.06,

        radius * 1.3,

        radius * 0.45
    );


    painter.drawText(
        valueRect,
        Qt::AlignCenter,
        QString::number(
            static_cast<int>(
                std::round(
                    displayValue_
                )
            )
        )
    );


    // ========================================================
    // UNIT
    // ========================================================

    QFont unitFont =
        painter.font();

    unitFont.setPixelSize(
        std::max(
            9,
            static_cast<int>(
                side * 0.043
            )
        )
    );

    unitFont.setWeight(
        QFont::Normal
    );


    painter.setFont(
        unitFont
    );

    painter.setPen(
        QColor("#A9BAC2")
    );


    QRectF unitRect(
        center.x() -
            radius * 0.5,

        center.y() +
            radius * 0.33,

        radius,

        24
    );


    painter.drawText(
        unitRect,
        Qt::AlignCenter,
        unit_
    );
}
