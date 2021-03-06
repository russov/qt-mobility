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

#ifndef QBLUETOOTHTRANSFERREPLYSYMBIAN_H
#define QBLUETOOTHTRANSFERREPLYSYMBIAN_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qbluetoothtransferreply.h"
#include <e32base.h>
#include <obex.h>

#include <QTemporaryFile>
#include <QIODevice>
#include <QTimer>

QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QBluetoothServiceDiscoveryAgent;
class QBluetoothServiceInfo;

_LIT( KBTSProtocol, "RFCOMM" ); // The Bluetooth transport layer

class QBluetoothTransferReplySymbian : public QBluetoothTransferReply, public CActive
{
    Q_OBJECT

public:
    enum state
        {
            EIdle,
            EConnecting,
            ESending,
            EDisconnecting
        };

    QBluetoothTransferReplySymbian(QIODevice *input, QBluetoothTransferManager::Operation, QObject *parent = 0);
    ~QBluetoothTransferReplySymbian();

    void abort();
    bool isFinished() const;
    bool isRunning() const;

    bool start();

    void setAddress(const QBluetoothAddress &address);

    QBluetoothTransferReply::TransferError error() const;
    QString errorString() const;

private:
    void sendObject(QString filename);
    void disconnect();

    //From CActive
    void DoCancel();
    void RunL();

    static bool copyToTempFile(QIODevice *to, QIODevice *from);

private slots:
    void startTransfer();

    void serviceDiscovered(const QBluetoothServiceInfo &info);
    void serviceDiscoveryFinished();

    void updateProgress();

private:
    QIODevice *m_source;
    QString m_tempfilename;

    QBluetoothAddress m_address;
    QBluetoothServiceDiscoveryAgent* m_discoveryAgent;

    bool m_running;
    bool m_finished;

    CObexClient* m_client;
    CObexFileObject* m_object;

    QBluetoothTransferReply::TransferError m_error;
    QString m_errorStr;

    state m_state;
    qint64 m_fileSize;
    int m_port;

    QTimer *m_timer;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif // QBLUETOOTHTRANSFERREPLYPRIVATE_H
