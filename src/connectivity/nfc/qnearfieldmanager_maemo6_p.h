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

#ifndef QNEARFIELDMANAGER_MAEMO6_P_H
#define QNEARFIELDMANAGER_MAEMO6_P_H

#include "qnearfieldmanager_p.h"
#include "qnearfieldmanager.h"
#include "qnearfieldtarget.h"

#include <QtCore/QWeakPointer>
#include <QtCore/QMap>
#include <QtCore/QPair>
#include <QtCore/QMetaMethod>
#include <QtCore/QBasicTimer>

#include <QtDBus/QDBusConnection>

QT_FORWARD_DECLARE_CLASS(QDBusObjectPath)
QT_FORWARD_DECLARE_CLASS(QDBusServiceWatcher)

class ComNokiaNfcManagerInterface;
class ComNokiaNfcAdapterInterface;
class NDEFHandlerAdaptor;
class AccessRequestorAdaptor;

QTM_BEGIN_NAMESPACE

class QNearFieldManagerPrivateImpl;

class NdefHandler : public QObject
{
    Q_OBJECT

public:
    NdefHandler(QNearFieldManagerPrivateImpl *manager, const QString &serviceName,
                const QString &path, QObject *object, const QMetaMethod &method);
    ~NdefHandler();

    bool isValid() const;

    QString serviceName() const;
    QString path() const;

private:
    Q_INVOKABLE void NDEFData(const QDBusObjectPath &target, const QByteArray &message);

    QNearFieldManagerPrivateImpl *m_manager;
    NDEFHandlerAdaptor *m_adaptor;
    QObject *m_object;
    QMetaMethod m_method;
    QString m_serviceName;
    QString m_path;
};

class QNearFieldManagerPrivateImpl : public QNearFieldManagerPrivate
{
    Q_OBJECT

public:
    QNearFieldManagerPrivateImpl();
    ~QNearFieldManagerPrivateImpl();

    bool isAvailable() const;

    bool startTargetDetection(const QList<QNearFieldTarget::Type> &targetTypes);
    void stopTargetDetection();

    QNearFieldTarget *targetForPath(const QString &path);

    int registerNdefMessageHandler(const QString &filter,
                                   QObject *object, const QMetaMethod &method);
    int registerNdefMessageHandler(QObject *object, const QMetaMethod &method);
    int registerNdefMessageHandler(const QNdefFilter &filter,
                                   QObject *object, const QMetaMethod &method);
    bool unregisterNdefMessageHandler(int handlerId);

    void requestAccess(QNearFieldManager::TargetAccessModes accessModes);
    void releaseAccess(QNearFieldManager::TargetAccessModes accessModes);

    // Access Agent Adaptor
    Q_INVOKABLE void AccessFailed(const QDBusObjectPath &target, const QString &kind,
                                  const QString &error);
    Q_INVOKABLE void AccessGranted(const QDBusObjectPath &target, const QString &kind);

protected:
    void timerEvent(QTimerEvent *event);

private slots:
    void emitTargetDetected(const QString &targetPath);
    void _q_targetDetected(const QDBusObjectPath &targetPath);
    void _q_targetLost(const QDBusObjectPath &targetPath);
    void _q_serviceOwnerChanged(const QString &serviceName, const QString &oldOwner,
                                const QString &newOwner);

private:
    bool ensureConnection() const;

    QDBusConnection m_connection;
    QDBusServiceWatcher *m_serviceWatcher;

    mutable ComNokiaNfcManagerInterface *m_manager;
    mutable ComNokiaNfcAdapterInterface *m_adapter;

    QList<QNearFieldTarget::Type> m_detectTargetTypes;
    QMap<QString, QWeakPointer<QNearFieldTarget> > m_targets;

    AccessRequestorAdaptor *m_accessAgent;

    QMap<int, NdefHandler *> m_registeredHandlers;

    QMap<QString, QBasicTimer> m_pendingDetectedTargets;
    QMap<QString, QNearFieldManager::TargetAccessModes> m_grantedModesForTarget;
};

QTM_END_NAMESPACE

#endif // QNEARFIELDMANAGER_MAEMO6_P_H
