/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QObject>
#include <QList>
#include <QtDBus/QtDBus>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusError>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusPendingCallWatcher>
#include <QtDBus/QDBusObjectPath>
#include <QtDBus/QDBusPendingCall>

#include "qnetworkmanagerservice_linux_p.h"
#include "qnmdbushelper_p.h"

QTM_BEGIN_NAMESPACE

class QNetworkManagerInterfacePrivate
{
public:
    QDBusInterface *connectionInterface;
    bool valid;
};

QNetworkManagerInterface::QNetworkManagerInterface(QObject *parent)
        : QObject(parent)
{
    d = new QNetworkManagerInterfacePrivate();
    d->connectionInterface = new QDBusInterface(QLatin1String(NM_DBUS_SERVICE),
                                                QLatin1String(NM_DBUS_PATH),
                                                QLatin1String(NM_DBUS_INTERFACE),
                                                QDBusConnection::systemBus());
    if (!d->connectionInterface->isValid()) {
        d->valid = false;
        return;
    }
    d->valid = true;
    nmDBusHelper = new QNmDBusHelper(this);
    connect(nmDBusHelper, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
                    this,SIGNAL(propertiesChanged( const QString &, QMap<QString,QVariant>)));
    connect(nmDBusHelper,SIGNAL(pathForStateChanged(const QString &, quint32)),
            this, SIGNAL(stateChanged(const QString&, quint32)));

}

QNetworkManagerInterface::~QNetworkManagerInterface()
{
    delete d->connectionInterface;
    delete d;
}

bool QNetworkManagerInterface::isValid()
{
    return d->valid;
}

bool QNetworkManagerInterface::setConnections()
{
    if(!isValid() )
        return false;
    bool allOk = false;
    if (!QDBusConnection::systemBus().connect(QLatin1String(NM_DBUS_SERVICE),
                                  QLatin1String(NM_DBUS_PATH),
                                  QLatin1String(NM_DBUS_INTERFACE),
                                  QLatin1String("PropertiesChanged"),
                                nmDBusHelper,SLOT(slotPropertiesChanged( QMap<QString,QVariant>)))) {
        allOk = true;
    }
    if (!QDBusConnection::systemBus().connect(QLatin1String(NM_DBUS_SERVICE),
                                  QLatin1String(NM_DBUS_PATH),
                                  QLatin1String(NM_DBUS_INTERFACE),
                                  QLatin1String("DeviceAdded"),
                                this,SIGNAL(deviceAdded(QDBusObjectPath)))) {
        allOk = true;
    }
    if (!QDBusConnection::systemBus().connect(QLatin1String(NM_DBUS_SERVICE),
                                  QLatin1String(NM_DBUS_PATH),
                                  QLatin1String(NM_DBUS_INTERFACE),
                                  QLatin1String("DeviceRemoved"),
                                  this,SIGNAL(deviceRemoved(QDBusObjectPath)))) {
        allOk = true;
    }

    return allOk;
}

QDBusInterface *QNetworkManagerInterface::connectionInterface()  const
{
    return d->connectionInterface;
}

QList <QDBusObjectPath> QNetworkManagerInterface::getDevices() const
{
    QDBusReply<QList<QDBusObjectPath> > reply =  d->connectionInterface->call(QLatin1String("GetDevices"));
    return reply.value();
}

void QNetworkManagerInterface::activateConnection( const QString &serviceName,
                                                  QDBusObjectPath connectionPath,
                                                  QDBusObjectPath devicePath,
                                                  QDBusObjectPath specificObject)
{
    QDBusPendingCall pendingCall = d->connectionInterface->asyncCall(QLatin1String("ActivateConnection"),
                                                                    QVariant(serviceName),
                                                                    QVariant::fromValue(connectionPath),
                                                                    QVariant::fromValue(devicePath),
                                                                    QVariant::fromValue(specificObject));

   QDBusPendingCallWatcher *callWatcher = new QDBusPendingCallWatcher(pendingCall, this);
   connect(callWatcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                    this, SIGNAL(activationFinished(QDBusPendingCallWatcher*)));
}

void QNetworkManagerInterface::deactivateConnection(QDBusObjectPath connectionPath)  const
{
    d->connectionInterface->call(QLatin1String("DeactivateConnection"), QVariant::fromValue(connectionPath));
}

bool QNetworkManagerInterface::wirelessEnabled()  const
{
    return d->connectionInterface->property("WirelessEnabled").toBool();
}

bool QNetworkManagerInterface::wirelessHardwareEnabled()  const
{
    return d->connectionInterface->property("WirelessHardwareEnabled").toBool();
}

QList <QDBusObjectPath> QNetworkManagerInterface::activeConnections() const
{
    QVariant prop = d->connectionInterface->property("ActiveConnections");
    return prop.value<QList<QDBusObjectPath> >();
}

quint32 QNetworkManagerInterface::state()
{
    return d->connectionInterface->property("State").toUInt();
}

class QNetworkManagerInterfaceAccessPointPrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
    bool valid;
};

QNetworkManagerInterfaceAccessPoint::QNetworkManagerInterfaceAccessPoint(const QString &dbusPathName, QObject *parent)
        : QObject(parent)
{
    d = new QNetworkManagerInterfaceAccessPointPrivate();
    d->path = dbusPathName;
    d->connectionInterface = new QDBusInterface(QLatin1String(NM_DBUS_SERVICE),
                                                d->path,
                                                QLatin1String(NM_DBUS_INTERFACE_ACCESS_POINT),
                                                QDBusConnection::systemBus());
    if (!d->connectionInterface->isValid()) {
        d->valid = false;
        return;
    }
    d->valid = true;

}

QNetworkManagerInterfaceAccessPoint::~QNetworkManagerInterfaceAccessPoint()
{
    delete d->connectionInterface;
    delete d;
}

bool QNetworkManagerInterfaceAccessPoint::isValid()
{
    return d->valid;
}

bool QNetworkManagerInterfaceAccessPoint::setConnections()
{
    if(!isValid() )
        return false;

    bool allOk = false;
    nmDBusHelper = new QNmDBusHelper(this);
    connect(nmDBusHelper, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
            this,SIGNAL(propertiesChanged( const QString &, QMap<QString,QVariant>)));

    if(QDBusConnection::systemBus().connect(QLatin1String(NM_DBUS_SERVICE),
                              d->path,
                              QLatin1String(NM_DBUS_INTERFACE_ACCESS_POINT),
                              QLatin1String("PropertiesChanged"),
                              nmDBusHelper,SLOT(slotPropertiesChanged( QMap<QString,QVariant>))) ) {
        allOk = true;

    }
    return allOk;
}

QDBusInterface *QNetworkManagerInterfaceAccessPoint::connectionInterface() const
{
    return d->connectionInterface;
}

quint32 QNetworkManagerInterfaceAccessPoint::flags() const
{
    return d->connectionInterface->property("Flags").toUInt();
}

quint32 QNetworkManagerInterfaceAccessPoint::wpaFlags() const
{
    return d->connectionInterface->property("WpaFlags").toUInt();
}

quint32 QNetworkManagerInterfaceAccessPoint::rsnFlags() const
{
    return d->connectionInterface->property("RsnFlags").toUInt();
}

QString QNetworkManagerInterfaceAccessPoint::ssid() const
{
    return d->connectionInterface->property("Ssid").toString();
}

quint32 QNetworkManagerInterfaceAccessPoint::frequency() const
{
    return d->connectionInterface->property("Frequency").toUInt();
}

QString QNetworkManagerInterfaceAccessPoint::hwAddress() const
{
    return d->connectionInterface->property("HwAddress").toString();
}

quint32 QNetworkManagerInterfaceAccessPoint::mode() const
{
    return d->connectionInterface->property("Mode").toUInt();
}

quint32 QNetworkManagerInterfaceAccessPoint::maxBitrate() const
{
    return d->connectionInterface->property("MaxBitrate").toUInt();
}

quint32 QNetworkManagerInterfaceAccessPoint::strength() const
{
    return d->connectionInterface->property("Strength").toUInt();
}

class QNetworkManagerInterfaceDevicePrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
    bool valid;
};

QNetworkManagerInterfaceDevice::QNetworkManagerInterfaceDevice(const QString &deviceObjectPath, QObject *parent)
        : QObject(parent)
{
    d = new QNetworkManagerInterfaceDevicePrivate();
    d->path = deviceObjectPath;
    d->connectionInterface = new QDBusInterface(QLatin1String(NM_DBUS_SERVICE),
                                                d->path,
                                                QLatin1String(NM_DBUS_INTERFACE_DEVICE),
                                                QDBusConnection::systemBus());
    if (!d->connectionInterface->isValid()) {
        d->valid = false;
        return;
    }
    d->valid = true;
}

QNetworkManagerInterfaceDevice::~QNetworkManagerInterfaceDevice()
{
    delete d->connectionInterface;
    delete d;
}

bool QNetworkManagerInterfaceDevice::isValid()
{
    return d->valid;
}

bool QNetworkManagerInterfaceDevice::setConnections()
{
    if(!isValid() )
        return false;

    bool allOk = false;
    nmDBusHelper = new QNmDBusHelper(this);
    connect(nmDBusHelper,SIGNAL(pathForStateChanged(const QString &, quint32)),
            this, SIGNAL(stateChanged(const QString&, quint32)));
    if(QDBusConnection::systemBus().connect(QLatin1String(NM_DBUS_SERVICE),
                              d->path,
                              QLatin1String(NM_DBUS_INTERFACE_DEVICE),
                              QLatin1String("StateChanged"),
                              nmDBusHelper,SLOT(deviceStateChanged(quint32)))) {
        allOk = true;
    }
    return allOk;
}

QDBusInterface *QNetworkManagerInterfaceDevice::connectionInterface() const
{
    return d->connectionInterface;
}

QString QNetworkManagerInterfaceDevice::udi() const
{
    return d->connectionInterface->property("Udi").toString();
}

QNetworkInterface QNetworkManagerInterfaceDevice::interface() const
{
    return QNetworkInterface::interfaceFromName(d->connectionInterface->property("Interface").toString());
}

quint32 QNetworkManagerInterfaceDevice::ip4Address() const
{
    return d->connectionInterface->property("Ip4Address").toUInt();
}

quint32 QNetworkManagerInterfaceDevice::state() const
{
    return d->connectionInterface->property("State").toUInt();
}

quint32 QNetworkManagerInterfaceDevice::deviceType() const
{
    return d->connectionInterface->property("DeviceType").toUInt();
}

QDBusObjectPath QNetworkManagerInterfaceDevice::ip4config() const
{
    QVariant prop = d->connectionInterface->property("Ip4Config");
    return prop.value<QDBusObjectPath>();
}

class QNetworkManagerInterfaceDeviceWiredPrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
    bool valid;
};

QNetworkManagerInterfaceDeviceWired::QNetworkManagerInterfaceDeviceWired(const QString &ifaceDevicePath, QObject *parent)
{
    d = new QNetworkManagerInterfaceDeviceWiredPrivate();
    d->path = ifaceDevicePath;
    d->connectionInterface = new QDBusInterface(QLatin1String(NM_DBUS_SERVICE),
                                                d->path,
                                                QLatin1String(NM_DBUS_INTERFACE_DEVICE_WIRED),
                                                QDBusConnection::systemBus(), parent);
    if (!d->connectionInterface->isValid()) {
        d->valid = false;
        return;
    }
    d->valid = true;
}

QNetworkManagerInterfaceDeviceWired::~QNetworkManagerInterfaceDeviceWired()
{
    delete d->connectionInterface;
    delete d;
}

bool QNetworkManagerInterfaceDeviceWired::isValid()
{

    return d->valid;
}

bool QNetworkManagerInterfaceDeviceWired::setConnections()
{
    if(!isValid() )
        return false;

    bool allOk = false;

    nmDBusHelper = new QNmDBusHelper(this);
    connect(nmDBusHelper, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
            this,SIGNAL(propertiesChanged( const QString &, QMap<QString,QVariant>)));
    if(QDBusConnection::systemBus().connect(QLatin1String(NM_DBUS_SERVICE),
                              d->path,
                              QLatin1String(NM_DBUS_INTERFACE_DEVICE_WIRED),
                              QLatin1String("PropertiesChanged"),
                              nmDBusHelper,SLOT(slotPropertiesChanged( QMap<QString,QVariant>))) )  {
        allOk = true;
    }
    return allOk;
}

QDBusInterface *QNetworkManagerInterfaceDeviceWired::connectionInterface() const
{
    return d->connectionInterface;
}

QString QNetworkManagerInterfaceDeviceWired::hwAddress() const
{
    return d->connectionInterface->property("HwAddress").toString();
}

quint32 QNetworkManagerInterfaceDeviceWired::speed() const
{
    return d->connectionInterface->property("Speed").toUInt();
}

bool QNetworkManagerInterfaceDeviceWired::carrier() const
{
    return d->connectionInterface->property("Carrier").toBool();
}

class QNetworkManagerInterfaceDeviceWirelessPrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
    bool valid;
};

QNetworkManagerInterfaceDeviceWireless::QNetworkManagerInterfaceDeviceWireless(const QString &ifaceDevicePath, QObject *parent)
{
    d = new QNetworkManagerInterfaceDeviceWirelessPrivate();
    d->path = ifaceDevicePath;
    d->connectionInterface = new QDBusInterface(QLatin1String(NM_DBUS_SERVICE),
                                                d->path,
                                                QLatin1String(NM_DBUS_INTERFACE_DEVICE_WIRELESS),
                                                QDBusConnection::systemBus(), parent);
    if (!d->connectionInterface->isValid()) {
        d->valid = false;
        return;
    }
    d->valid = true;
}

QNetworkManagerInterfaceDeviceWireless::~QNetworkManagerInterfaceDeviceWireless()
{
    delete d->connectionInterface;
    delete d;
}

bool QNetworkManagerInterfaceDeviceWireless::isValid()
{
    return d->valid;
}

bool QNetworkManagerInterfaceDeviceWireless::setConnections()
{
    if(!isValid() )
        return false;

    bool allOk = false;
    nmDBusHelper = new QNmDBusHelper(this);
    connect(nmDBusHelper, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
            this,SIGNAL(propertiesChanged( const QString &, QMap<QString,QVariant>)));

    connect(nmDBusHelper, SIGNAL(pathForAccessPointAdded(const QString &,QDBusObjectPath)),
            this,SIGNAL(accessPointAdded(const QString &,QDBusObjectPath)));

    connect(nmDBusHelper, SIGNAL(pathForAccessPointRemoved(const QString &,QDBusObjectPath)),
            this,SIGNAL(accessPointRemoved(const QString &,QDBusObjectPath)));

    if(!QDBusConnection::systemBus().connect(QLatin1String(NM_DBUS_SERVICE),
                              d->path,
                              QLatin1String(NM_DBUS_INTERFACE_DEVICE_WIRELESS),
                              QLatin1String("AccessPointAdded"),
                              nmDBusHelper, SLOT(slotAccessPointAdded( QDBusObjectPath )))) {
        allOk = true;
    }


    if(!QDBusConnection::systemBus().connect(QLatin1String(NM_DBUS_SERVICE),
                              d->path,
                              QLatin1String(NM_DBUS_INTERFACE_DEVICE_WIRELESS),
                              QLatin1String("AccessPointRemoved"),
                              nmDBusHelper, SLOT(slotAccessPointRemoved( QDBusObjectPath )))) {
        allOk = true;
    }


    if(!QDBusConnection::systemBus().connect(QLatin1String(NM_DBUS_SERVICE),
                              d->path,
                              QLatin1String(NM_DBUS_INTERFACE_DEVICE_WIRELESS),
                              QLatin1String("PropertiesChanged"),
                              nmDBusHelper,SLOT(slotPropertiesChanged( QMap<QString,QVariant>)))) {
        allOk = true;
    }

    return allOk;
}

QDBusInterface *QNetworkManagerInterfaceDeviceWireless::connectionInterface() const
{
    return d->connectionInterface;
}

QList <QDBusObjectPath> QNetworkManagerInterfaceDeviceWireless::getAccessPoints()
{
    QDBusReply<QList<QDBusObjectPath> > reply = d->connectionInterface->call(QLatin1String("GetAccessPoints"));
    return reply.value();
}

QString QNetworkManagerInterfaceDeviceWireless::hwAddress() const
{
    return d->connectionInterface->property("HwAddress").toString();
}

quint32 QNetworkManagerInterfaceDeviceWireless::mode() const
{
    return d->connectionInterface->property("Mode").toUInt();
}

quint32 QNetworkManagerInterfaceDeviceWireless::bitrate() const
{
    return d->connectionInterface->property("Bitrate").toUInt();
}

QDBusObjectPath QNetworkManagerInterfaceDeviceWireless::activeAccessPoint() const
{
    return d->connectionInterface->property("ActiveAccessPoint").value<QDBusObjectPath>();
}

quint32 QNetworkManagerInterfaceDeviceWireless::wirelessCapabilities() const
{
    return d->connectionInterface->property("WirelelessCapabilities").toUInt();
}

class QNetworkManagerSettingsPrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
    bool valid;
};

class QNetworkManagerInterfaceDeviceGsmPrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
    bool valid;
};

QNetworkManagerInterfaceDeviceGsm::QNetworkManagerInterfaceDeviceGsm(const QString &ifaceDevicePath, QObject *parent)
{
    d = new QNetworkManagerInterfaceDeviceGsmPrivate();
    d->path = ifaceDevicePath;
    d->connectionInterface = new QDBusInterface(QLatin1String(NM_DBUS_SERVICE),
                                                d->path,
                                                QLatin1String(NM_DBUS_INTERFACE_DEVICE_GSM),
                                                QDBusConnection::systemBus(), parent);
    if (!d->connectionInterface->isValid()) {
        d->valid = false;
        return;
    }
    d->valid = true;
}

QNetworkManagerInterfaceDeviceGsm::~QNetworkManagerInterfaceDeviceGsm()
{
    delete d->connectionInterface;
    delete d;
}

bool QNetworkManagerInterfaceDeviceGsm::isValid()
{

    return d->valid;
}

bool QNetworkManagerInterfaceDeviceGsm::setConnections()
{
    if(!isValid() )
        return false;

    bool allOk = false;

    nmDBusHelper = new QNmDBusHelper(this);
    connect(nmDBusHelper, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
            this,SIGNAL(propertiesChanged( const QString &, QMap<QString,QVariant>)));
    if(QDBusConnection::systemBus().connect(QLatin1String(NM_DBUS_SERVICE),
                              d->path,
                              QLatin1String(NM_DBUS_INTERFACE_DEVICE_GSM),
                              QLatin1String("PropertiesChanged"),
                              nmDBusHelper,SLOT(slotPropertiesChanged( QMap<QString,QVariant>))) )  {
        allOk = true;
    }
    return allOk;
}

QDBusInterface *QNetworkManagerInterfaceDeviceGsm::connectionInterface() const
{
    return d->connectionInterface;
}

QNetworkManagerSettings::QNetworkManagerSettings(const QString &settingsService, QObject *parent)
        : QObject(parent)
{
    d = new QNetworkManagerSettingsPrivate();
    d->path = settingsService;
    d->connectionInterface = new QDBusInterface(settingsService,
                                                QLatin1String(NM_DBUS_PATH_SETTINGS),
                                                QLatin1String(NM_DBUS_IFACE_SETTINGS),
                                                QDBusConnection::systemBus());
    if (!d->connectionInterface->isValid()) {
        d->valid = false;
        return;
    }
    d->valid = true;
}

QNetworkManagerSettings::~QNetworkManagerSettings()
{
    delete d->connectionInterface;
    delete d;
}

bool QNetworkManagerSettings::isValid()
{
    return d->valid;
}

bool QNetworkManagerSettings::setConnections()
{
    bool allOk = false;

    if (!QDBusConnection::systemBus().connect(d->path, QLatin1String(NM_DBUS_PATH_SETTINGS),
                           QLatin1String(NM_DBUS_IFACE_SETTINGS), QLatin1String("NewConnection"),
                           this, SIGNAL(newConnection(QDBusObjectPath)))) {
        allOk = true;
    }

    return allOk;
}

QList <QDBusObjectPath> QNetworkManagerSettings::listConnections()
{
    QDBusReply<QList<QDBusObjectPath> > reply = d->connectionInterface->call(QLatin1String("ListConnections"));
    return  reply.value();
}

QDBusInterface *QNetworkManagerSettings::connectionInterface() const
{
    return d->connectionInterface;
}


class QNetworkManagerSettingsConnectionPrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
    QString service;
    QNmSettingsMap settingsMap;
    bool valid;
};

QNetworkManagerSettingsConnection::QNetworkManagerSettingsConnection(const QString &settingsService, const QString &connectionObjectPath, QObject *parent)
{
    qDBusRegisterMetaType<QNmSettingsMap>();
    d = new QNetworkManagerSettingsConnectionPrivate();
    d->path = connectionObjectPath;
    d->service = settingsService;
    d->connectionInterface = new QDBusInterface(settingsService,
                                                d->path,
                                                QLatin1String(NM_DBUS_IFACE_SETTINGS_CONNECTION),
                                                QDBusConnection::systemBus(), parent);
    if (!d->connectionInterface->isValid()) {
        d->valid = false;
        return;
    }
    d->valid = true;
    QDBusReply< QNmSettingsMap > rep = d->connectionInterface->call(QLatin1String("GetSettings"));
    d->settingsMap = rep.value();
}

QNetworkManagerSettingsConnection::~QNetworkManagerSettingsConnection()
{
    delete d->connectionInterface;
    delete d;
}

bool QNetworkManagerSettingsConnection::isValid()
{
    return d->valid;
}

bool QNetworkManagerSettingsConnection::setConnections()
{
    if(!isValid() )
        return false;

    bool allOk = false;
    if(!QDBusConnection::systemBus().connect(d->service, d->path,
                           QLatin1String(NM_DBUS_IFACE_SETTINGS_CONNECTION), QLatin1String("NewConnection"),
                           this, SIGNAL(updated(QNmSettingsMap)))) {
        allOk = true;
    }

    nmDBusHelper = new QNmDBusHelper(this);
    connect(nmDBusHelper, SIGNAL(pathForSettingsRemoved(const QString &)),
            this,SIGNAL(removed( const QString &)));

    if (!QDBusConnection::systemBus().connect(d->service, d->path,
                           QLatin1String(NM_DBUS_IFACE_SETTINGS_CONNECTION), QLatin1String("Removed"),
                           nmDBusHelper, SIGNAL(slotSettingsRemoved()))) {
        allOk = true;
    }

    return allOk;
}

QDBusInterface *QNetworkManagerSettingsConnection::connectionInterface() const
{
    return d->connectionInterface;
}

QNmSettingsMap QNetworkManagerSettingsConnection::getSettings()
{
    QDBusReply< QNmSettingsMap > rep = d->connectionInterface->call(QLatin1String("GetSettings"));
    d->settingsMap = rep.value();
    return d->settingsMap;
}

NMDeviceType QNetworkManagerSettingsConnection::getType()
{
    QNmSettingsMap::const_iterator i = d->settingsMap.find(QLatin1String("connection"));
    while (i != d->settingsMap.end() && i.key() == QLatin1String("connection")) {
        QMap<QString,QVariant> innerMap = i.value();
        QMap<QString,QVariant>::const_iterator ii = innerMap.find(QLatin1String("type"));
        while (ii != innerMap.end() && ii.key() == QLatin1String("type")) {
            QString devType = ii.value().toString();
            if (devType == QLatin1String("802-3-ethernet")) {
                return 	DEVICE_TYPE_802_3_ETHERNET;
            }
            if (devType == QLatin1String("802-11-wireless")) {
                return 	DEVICE_TYPE_802_11_WIRELESS;
            }
            ii++;
        }
        i++;
    }
    return 	DEVICE_TYPE_UNKNOWN;
}

bool QNetworkManagerSettingsConnection::isAutoConnect()
{
    QNmSettingsMap::const_iterator i = d->settingsMap.find(QLatin1String("connection"));
    while (i != d->settingsMap.end() && i.key() == QLatin1String("connection")) {
        QMap<QString,QVariant> innerMap = i.value();
        QMap<QString,QVariant>::const_iterator ii = innerMap.find(QLatin1String("autoconnect"));
        while (ii != innerMap.end() && ii.key() == QLatin1String("autoconnect")) {
            return ii.value().toBool();
            ii++;
        }
        i++;
    }
    return true; //default networkmanager is autoconnect
}

quint64 QNetworkManagerSettingsConnection::getTimestamp()
{
    QNmSettingsMap::const_iterator i = d->settingsMap.find(QLatin1String("connection"));
    while (i != d->settingsMap.end() && i.key() == QLatin1String("connection")) {
        QMap<QString,QVariant> innerMap = i.value();
        QMap<QString,QVariant>::const_iterator ii = innerMap.find(QLatin1String("timestamp"));
        while (ii != innerMap.end() && ii.key() == QLatin1String("timestamp")) {
            return ii.value().toUInt();
            ii++;
        }
        i++;
    }
    return 	0;
}

QString QNetworkManagerSettingsConnection::getId()
{
    QNmSettingsMap::const_iterator i = d->settingsMap.find(QLatin1String("connection"));
    while (i != d->settingsMap.end() && i.key() == QLatin1String("connection")) {
        QMap<QString,QVariant> innerMap = i.value();
        QMap<QString,QVariant>::const_iterator ii = innerMap.find(QLatin1String("id"));
        while (ii != innerMap.end() && ii.key() == QLatin1String("id")) {
            return ii.value().toString();
            ii++;
        }
        i++;
    }
    return 	QString();
}

QString QNetworkManagerSettingsConnection::getUuid()
{
    QNmSettingsMap::const_iterator i = d->settingsMap.find(QLatin1String("connection"));
    while (i != d->settingsMap.end() && i.key() == QLatin1String("connection")) {
        QMap<QString,QVariant> innerMap = i.value();
        QMap<QString,QVariant>::const_iterator ii = innerMap.find(QLatin1String("uuid"));
        while (ii != innerMap.end() && ii.key() == QLatin1String("uuid")) {
            return ii.value().toString();
            ii++;
        }
        i++;
    }
    // is no uuid, return the connection path
    return 	d->connectionInterface->path();
}

QString QNetworkManagerSettingsConnection::getSsid()
{
    QNmSettingsMap::const_iterator i = d->settingsMap.find(QLatin1String("802-11-wireless"));
    while (i != d->settingsMap.end() && i.key() == QLatin1String("802-11-wireless")) {
        QMap<QString,QVariant> innerMap = i.value();
        QMap<QString,QVariant>::const_iterator ii = innerMap.find(QLatin1String("ssid"));
        while (ii != innerMap.end() && ii.key() == QLatin1String("ssid")) {
            return ii.value().toString();
            ii++;
        }
        i++;
    }
    return 	QString();
}

QString QNetworkManagerSettingsConnection::getMacAddress()
{
    if(getType() == DEVICE_TYPE_802_3_ETHERNET) {
        QNmSettingsMap::const_iterator i = d->settingsMap.find(QLatin1String("802-3-ethernet"));
        while (i != d->settingsMap.end() && i.key() == QLatin1String("802-3-ethernet")) {
            QMap<QString,QVariant> innerMap = i.value();
            QMap<QString,QVariant>::const_iterator ii = innerMap.find(QLatin1String("mac-address"));
            while (ii != innerMap.end() && ii.key() == QLatin1String("mac-address")) {
                return ii.value().toString();
                ii++;
            }
            i++;
        }
    }

    else if(getType() == DEVICE_TYPE_802_11_WIRELESS) {
        QNmSettingsMap::const_iterator i = d->settingsMap.find(QLatin1String("802-11-wireless"));
        while (i != d->settingsMap.end() && i.key() == QLatin1String("802-11-wireless")) {
            QMap<QString,QVariant> innerMap = i.value();
            QMap<QString,QVariant>::const_iterator ii = innerMap.find(QLatin1String("mac-address"));
            while (ii != innerMap.end() && ii.key() == QLatin1String("mac-address")) {
                return ii.value().toString();
                ii++;
            }
            i++;
        }
    }
    return 	QString();
}

QStringList  QNetworkManagerSettingsConnection::getSeenBssids()
{
 if(getType() == DEVICE_TYPE_802_11_WIRELESS) {
        QNmSettingsMap::const_iterator i = d->settingsMap.find(QLatin1String("802-11-wireless"));
        while (i != d->settingsMap.end() && i.key() == QLatin1String("802-11-wireless")) {
            QMap<QString,QVariant> innerMap = i.value();
            QMap<QString,QVariant>::const_iterator ii = innerMap.find(QLatin1String("seen-bssids"));
            while (ii != innerMap.end() && ii.key() == QLatin1String("seen-bssids")) {
                return ii.value().toStringList();
                ii++;
            }
            i++;
        }
    }
 return QStringList();
}

class QNetworkManagerConnectionActivePrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
    bool valid;
};

QNetworkManagerConnectionActive::QNetworkManagerConnectionActive( const QString &activeConnectionObjectPath, QObject *parent)
{
    d = new QNetworkManagerConnectionActivePrivate();
    d->path = activeConnectionObjectPath;
    d->connectionInterface = new QDBusInterface(QLatin1String(NM_DBUS_SERVICE),
                                                d->path,
                                                QLatin1String(NM_DBUS_INTERFACE_ACTIVE_CONNECTION),
                                                QDBusConnection::systemBus(), parent);
    if (!d->connectionInterface->isValid()) {
        d->valid = false;
        return;
    }
    d->valid = true;
}

QNetworkManagerConnectionActive::~QNetworkManagerConnectionActive()
{
    delete d->connectionInterface;
    delete d;
}

bool QNetworkManagerConnectionActive::isValid()
{
    return d->valid;
}

bool QNetworkManagerConnectionActive::setConnections()
{
    if(!isValid() )
        return false;

    bool allOk = false;
    nmDBusHelper = new QNmDBusHelper(this);
    connect(nmDBusHelper, SIGNAL(pathForPropertiesChanged(const QString &,QMap<QString,QVariant>)),
            this,SIGNAL(propertiesChanged( const QString &, QMap<QString,QVariant>)));
    if(QDBusConnection::systemBus().connect(QLatin1String(NM_DBUS_SERVICE),
                              d->path,
                              QLatin1String(NM_DBUS_INTERFACE_ACTIVE_CONNECTION),
                              QLatin1String("PropertiesChanged"),
                              nmDBusHelper,SLOT(slotPropertiesChanged( QMap<QString,QVariant>))) )  {
        allOk = true;
    }

    return allOk;
}

QDBusInterface *QNetworkManagerConnectionActive::connectionInterface() const
{
    return d->connectionInterface;
}

QString QNetworkManagerConnectionActive::serviceName() const
{
    return d->connectionInterface->property("ServiceName").toString();
}

QDBusObjectPath QNetworkManagerConnectionActive::connection() const
{
    QVariant prop = d->connectionInterface->property("Connection");
    return prop.value<QDBusObjectPath>();
}

QDBusObjectPath QNetworkManagerConnectionActive::specificObject() const
{
    QVariant prop = d->connectionInterface->property("SpecificObject");
    return prop.value<QDBusObjectPath>();
}

QList<QDBusObjectPath> QNetworkManagerConnectionActive::devices() const
{
    QVariant prop = d->connectionInterface->property("Devices");
    return prop.value<QList<QDBusObjectPath> >();
}

quint32 QNetworkManagerConnectionActive::state() const
{
    return d->connectionInterface->property("State").toUInt();
}

bool QNetworkManagerConnectionActive::defaultRoute() const
{
    return d->connectionInterface->property("Default").toBool();
}

class QNetworkManagerIp4ConfigPrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
    bool valid;
};

QNetworkManagerIp4Config::QNetworkManagerIp4Config( const QString &deviceObjectPath, QObject *parent)
{
    d = new QNetworkManagerIp4ConfigPrivate();
    d->path = deviceObjectPath;
    d->connectionInterface = new QDBusInterface(QLatin1String(NM_DBUS_SERVICE),
                                                d->path,
                                                QLatin1String(NM_DBUS_INTERFACE_IP4_CONFIG),
                                                QDBusConnection::systemBus(), parent);
    if (!d->connectionInterface->isValid()) {
        d->valid = false;
        return;
    }
    d->valid = true;
}

QNetworkManagerIp4Config::~QNetworkManagerIp4Config()
{
    delete d->connectionInterface;
    delete d;
}

bool QNetworkManagerIp4Config::isValid()
{
    return d->valid;
}

QStringList QNetworkManagerIp4Config::domains() const
{
    return d->connectionInterface->property("Domains").toStringList();
}


class QModemManagerGsmNetworkInterfacePrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
    bool valid;
};

QModemManagerGsmNetworkInterface::QModemManagerGsmNetworkInterface(const QString &dbusPathName, QObject *parent)
{
    d = new QModemManagerGsmNetworkInterfacePrivate();
    d->path = dbusPathName;
    d->connectionInterface = new QDBusInterface("org.freedesktop.ModemManager",
                                                        d->path,
                                                        "org.freedesktop.ModemManager.Modem.Gsm.Network",
                                                        QDBusConnection::systemBus(), parent);
    if (!d->connectionInterface->isValid()) {
        d->valid = false;
        return;
    }
    d->valid = true;
    qDBusRegisterMetaType<MMRegistrationInfoType>();
}

QModemManagerGsmNetworkInterface::~QModemManagerGsmNetworkInterface()
{
    delete d->connectionInterface;
    delete d;
}

bool QModemManagerGsmNetworkInterface::isValid()
{
    return d->valid;
}
uint QModemManagerGsmNetworkInterface::signalQuality()
{
    QDBusReply<uint > reply = d->connectionInterface->call(QLatin1String("GetSignalQuality"));
    return reply.value();

}

MMRegistrationInfoType QModemManagerGsmNetworkInterface::registrationStatus()
{
    QDBusReply<MMRegistrationInfoType > reply = d->connectionInterface->call(QLatin1String("GetRegistrationInfo"));

    return reply.value();
}

uint QModemManagerGsmNetworkInterface::accessTechnology()
{
    return d->connectionInterface->property("AccessTechnology").toUInt();
}

uint QModemManagerGsmNetworkInterface::networkMode()
{
    QDBusReply<uint > reply = d->connectionInterface->call(QLatin1String("GetNetworkMode"));

    return reply.value();
}

uint QModemManagerGsmNetworkInterface::allowedMode()
{
    return d->connectionInterface->property("AllowedMode").toUInt();
}

QMmScanResult QModemManagerGsmNetworkInterface::scan()
{
    QDBusReply<QMmScanResult> reply = d->connectionInterface->call(QLatin1String("Scan"));
    return reply.value();
}

QDBusArgument &operator << (QDBusArgument &argument, const MMRegistrationInfoType &info)
{
    uint temp;

    temp = (uint) info.status;
    argument.beginStructure();
    argument << temp << info.operatorCode << info.operatorName;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator >> (const QDBusArgument &argument, MMRegistrationInfoType &info)
{
    uint temp;

    argument.beginStructure();
    argument >> temp >> info.operatorCode >> info.operatorName;
    info.status = (MMRegistrationInfoType::MMRegistrationStatus) temp;
    argument.endStructure();
    return argument;
}

class QModemManagerModemGsmCardPrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
    bool valid;
};

QModemManagerModemGsmCard::QModemManagerModemGsmCard(const QString &dbusPathName, QObject *parent)
{
    d = new QModemManagerModemGsmCardPrivate();
    d->path = dbusPathName;
    d->connectionInterface = new QDBusInterface("org.freedesktop.ModemManager",
                                                d->path,
                                                "org.freedesktop.ModemManager.Modem.Gsm.Card",
                                                QDBusConnection::systemBus(), parent);
    if (!d->connectionInterface->isValid()) {
        d->valid = false;
        return;
    }
    d->valid = true;
}

QModemManagerModemGsmCard::~QModemManagerModemGsmCard()
{
    delete d->connectionInterface;
    delete d;
}

QString QModemManagerModemGsmCard::imsi()
{
    QDBusReply<QString > reply = d->connectionInterface->call(QLatin1String("GetImsi"));
    return reply.value();
}

QString QModemManagerModemGsmCard::imei()
{
    QDBusReply<QString > reply = d->connectionInterface->call(QLatin1String("GetImei"));
    return reply.value();
}

//QModemManagerModemGsmCard::CellModesFlags QModemManagerModemGsmCard::supportedCellModes()
//{
//  //  QDBusReply<CellModesFlags > reply = qvariant_cast<uint>(d->connectionInterface->property("SupportedModes"));
//    return reply.value();
//}


class QModemManagerModemLocationPrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
    bool valid;
};

QModemManagerModemLocation::QModemManagerModemLocation(const QString &dbusPathName, QObject *parent)
{
    d = new QModemManagerModemLocationPrivate();
    d->path = dbusPathName;
    d->connectionInterface = new QDBusInterface("org.freedesktop.ModemManager",
                             d->path,
                             "org.freedesktop.ModemManager.Modem.Location",
                             QDBusConnection::systemBus(), parent);
    if (!d->connectionInterface->isValid()) {
        d->valid = false;
        return;
    }
    d->valid = true;

    qDBusRegisterMetaType<QMmLocationMap>();
}

QModemManagerModemLocation::~QModemManagerModemLocation()
{
    delete d->connectionInterface;
    delete d;
}

bool QModemManagerModemLocation::isValid()
{
    return d->valid;
}


QMmLocationMap QModemManagerModemLocation::location()
{
    QDBusReply<QMmLocationMap > reply = d->connectionInterface->call(QLatin1String("GetLocation"));
    return reply.value();
}

bool QModemManagerModemLocation::locationEnabled()
{
    bool enabled = d->connectionInterface->property("Enabled").toBool();
    return enabled;
}

void QModemManagerModemLocation::enableLocation(bool enable,bool useSignals)
{
    QList<QVariant> args;
    args << qVariantFromValue(enable) << qVariantFromValue(useSignals);

    QDBusMessage reply = d->connectionInterface->callWithArgumentList(QDBus::AutoDetect,
                                                                      QLatin1String("Enable"),
                                                                      args);
    bool ok = true;
    if(reply.type() != QDBusMessage::ReplyMessage) {
        qWarning() << reply.errorMessage();
        ok = false;
    }

}

QModemManagerModemSimple::QModemManagerModemSimple(const QString &dbusPathName, QObject *parent)
    : QDBusAbstractInterface("org.freedesktop.ModemManager",
                             dbusPathName,
                             "org.freedesktop.ModemManager.Modem.Simple",
                             QDBusConnection::systemBus(), parent)
{
}

QModemManagerModemSimple::~QModemManagerModemSimple()
{
}

QVariantMap QModemManagerModemSimple::status()
{
    QDBusReply<QVariantMap > reply = this->call(QLatin1String("GetStatus"));
    return reply.value();
}


class QModemManagerModemPrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
    bool valid;
};

QModemManagerModem::QModemManagerModem(const QString &dbusPathName, QObject *parent)
{
    d = new QModemManagerModemPrivate();
    d->path = dbusPathName;
    d->connectionInterface = new QDBusInterface("org.freedesktop.ModemManager",
                                                        d->path,
                                                        "org.freedesktop.ModemManager.Modem",
                                                        QDBusConnection::systemBus(), parent);
    if (!d->connectionInterface->isValid()) {
        d->valid = false;
        return;
    }
    d->valid = true;
}

QModemManagerModem::~QModemManagerModem()
{
    delete d->connectionInterface;
    delete d;
}

bool QModemManagerModem::enabled()
{
    bool enabled = d->connectionInterface->property("Enabled").toBool();
    return enabled;
}

QString QModemManagerModem::unlockRequired()
{
    return d->connectionInterface->property("UnlockRequired").toString();
}

uint QModemManagerModem::type()
{
    return d->connectionInterface->property("Type").toUInt();
}

uint QModemManagerModem::ipMethod()
{
    return d->connectionInterface->property("IpMethod").toUInt();

}


class QModemManagerPrivate
{
public:
    QDBusInterface *connectionInterface;
    QString path;
    bool valid;
};

QModemManager::QModemManager(QObject *parent)
{
    d = new QModemManagerPrivate();
    d->path = "/org/freedesktop/ModemManager";
    d->connectionInterface = new QDBusInterface("org.freedesktop.ModemManager",
                                                        d->path,
                                                        "org.freedesktop.ModemManager",
                                                        QDBusConnection::systemBus(), parent);
    if (!d->connectionInterface->isValid()) {
        d->valid = false;
        return;
    }
    d->valid = true;
}

QModemManager::~QModemManager()
{
    delete d->connectionInterface;
    delete d;
}

QList<QDBusObjectPath> QModemManager::enumerateDevices()
{
    QDBusReply<QList<QDBusObjectPath> > reply = d->connectionInterface->call(QLatin1String("EnumerateDevices"));
    return reply.value();
}

#include "moc_qnetworkmanagerservice_linux_p.cpp"

QTM_END_NAMESPACE