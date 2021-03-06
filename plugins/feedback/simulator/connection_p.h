/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CONNECTION_H
#define CONNECTION_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qmobilityglobal.h>
#include "qfeedbackdata_simulator_p.h"
#include <QtCore/QObject>
#include <QtCore/QList>

QT_BEGIN_HEADER

QT_FORWARD_DECLARE_CLASS(QLocalSocket)
class QFeedbackSimulator;

QTM_BEGIN_NAMESPACE


namespace Simulator {

class MobilityConnection;

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(MobilityConnection *mobilityCon);
    static Connection *instance();

    void connect(QFeedbackSimulator *plugin);
    int startEffect(int actuatorId, const QString &info, int duration);
    bool resumeEffect(int effectId);
    bool pauseEffect(int effectId);
    bool stopEffect(int effectId);
    void setEffectDuration(int effectId, int duration);
    void setActuatorEnabled(int actuatorId, bool enabled);

private slots:
    // called remotely
    void initialFeedbackDataSent();
    void setActuator(QtMobility::ActuatorData data);
    void removeActuator(int id);
    void setDefaultActuator(int id);
    void setFeedbackEffectState(int effectId, int state);

private:
    void getInitialData();

    MobilityConnection *mConnection;
    bool mRegisteredWithSimulator;
    bool mInitialDataReceived;
    QFeedbackSimulator *mPlugin;
};

} // namespace Simulator

QTM_END_NAMESPACE
QT_END_HEADER

#endif // CONNECTION_H
