#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QObject>

#include "system.h"
#include "settings.h"
#include "thermostat.h"
#include "statisticssampler.h"

class Simulator : public QObject
{
    Q_OBJECT
public:
    Simulator(QObject* parent = 0);
    ~Simulator();

    System m_system;
    Settings m_settings;
    Thermostat *m_thermostat;
    StatisticsSampler *m_sampler;

public slots:
    void step();

signals:
    void stepCompleted();
};

#endif
