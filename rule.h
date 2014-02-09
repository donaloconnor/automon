#ifndef RULE_H
#define RULE_H

#include <QString>
#include <QObject>
#include <QtScript>

/*
    ==================================================================================================
    |  rule.h                                                                                        |
    |  Part of the Automon application.                                                              |
    |                                                                                                |
    |  Final Year Project - "An Embedded Automotive Monitoring Device"                               |
    |                                                                                                |
    |  By Donal O' Connor for completion of B.Sc (Hons) Software Development and Computer Networking |
    |  Email: donaloconnor@gmail.com                                                                 |
    |  Website/Blog: http://automon.killarneyonline.eu                                               |
    |                                                                                                |
    | Cork Institute of Technology, Cork, Ireland - http://www.cit.ie/                               |
    |                                                                                                |
    | Copyright © 2009 Donal O'Connor <donaloconnor@gmail.com>                                       |
    ==================================================================================================

    This class, Rule is used to create a rule in which if sensor values match the rule description,
    it emits a signal to handle the rule satisfied condition.
    It uses the QtScript module and a QtScriptEngine.
*/

#include "sensor.h"

namespace AutomonKernel
{
    class Rule : public QObject
    {
        Q_OBJECT

    public:
        Rule();
        void setRule(QString rule);
        void setRuleName(QString ruleName);
        QString getRule();
        QString getRuleName();
        bool checkIfSatisfied();
        bool addSensor(Sensor * sensor);
        bool activate();

    signals:
        void sendAlert(QString);

    public slots:
        void updateRule(double value);

    private:
        bool validateRule();

        QScriptEngine * m_scriptEngine;
        QString m_rule;
        QString m_ruleName;
        bool m_satisfied;
        QList<Sensor*> m_sensors;
    };
}
#endif // RULE_H
