/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QNEARFIELDMANAGER_P_H
#define QNEARFIELDMANAGER_P_H

#include "qnearfieldmanager.h"
#include "qnearfieldtarget.h"
#include "qndefrecord.h"

#include <qmobilityglobal.h>

#include <QtCore/QObject>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QNdefFilter;

class QM_AUTOTEST_EXPORT QNearFieldManagerPrivate : public QObject
{
    Q_OBJECT

public:
    explicit QNearFieldManagerPrivate(QObject *parent = 0)
    :   QObject(parent)
    {
    }

    ~QNearFieldManagerPrivate()
    {
    }

    virtual bool startTargetDetection(const QList<QNearFieldTarget::Type> &targetTypes)
    {
        Q_UNUSED(targetTypes);

        return false;
    }

    virtual void stopTargetDetection()
    {
    }

    virtual int registerTargetDetectedHandler(QObject *object, const QMetaMethod &/*method*/)
    {
        Q_UNUSED(object);

        return -1;
    }

    virtual int registerTargetDetectedHandler(const QNdefFilter &/*filter*/,
                                              QObject *object, const QMetaMethod &/*method*/)
    {
        Q_UNUSED(object);

        return -1;
    }

    virtual bool unregisterTargetDetectedHandler(int handlerId)
    {
        Q_UNUSED(handlerId);

        return false;
    }

    virtual void requestAccess(QNearFieldManager::TargetAccessModes accessModes)
    {
        m_requestedModes |= accessModes;
    }

    virtual void releaseAccess(QNearFieldManager::TargetAccessModes accessModes)
    {
        m_requestedModes &= ~accessModes;
    }

signals:
    void targetDetected(QNearFieldTarget *target);
    void targetLost(QNearFieldTarget *target);

public:
    QNearFieldManager::TargetAccessModes m_requestedModes;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif // QNEARFIELDMANAGER_P_H