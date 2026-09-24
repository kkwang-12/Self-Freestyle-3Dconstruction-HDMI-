#pragma once

#include <QWidget>
#include <QString>
#include <QVariantAnimation>

class CircularGauge : public QWidget
{
    Q_OBJECT

public:
    explicit CircularGauge(QWidget* parent = nullptr);

    void setTitle(const QString& title);
    void setUnit(const QString& unit);

    void setRange(double minimum, double maximum);
    void setValue(double value);

    void setMajorTickCount(int count);
    void setMinorTicksPerMajor(int count);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString title_ = "VALUE";
    QString unit_;

    double minimum_ = 0.0;
    double maximum_ = 100.0;

    double targetValue_ = 0.0;
    double displayValue_ = 0.0;

    int majorTickCount_ = 5;
    int minorTicksPerMajor_ = 4;

    QVariantAnimation* animation_ = nullptr;

private:
    double valueRatio() const;

    QPointF pointOnCircle(
        const QPointF& center,
        double radius,
        double angleDeg
    ) const;
};
