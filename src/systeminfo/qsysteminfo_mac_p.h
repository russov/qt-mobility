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
#ifndef QSYSTEMINFO_MAC_P_H
#define QSYSTEMINFO_MAC_P_H


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


#include <QObject>
#include <QSize>
#include <QHash>
#include "qsysteminfocommon_p.h"

#include "qsysteminfo.h"

#include "qsystemdeviceinfo.h"
#include "qsystemdisplayinfo.h"
#include "qsystemnetworkinfo.h"
#include "qsystemscreensaver.h"
#include "qsystemstorageinfo.h"
#include "qsystembatteryinfo.h"

#include <QTimer>
#include <QtCore/qthread.h>
#include <QtCore/qmutex.h>
 #include <QEventLoop>

#include <SystemConfiguration/SystemConfiguration.h>
//#include <CoreFoundation/CoreFoundation.h>
#include <DiskArbitration/DiskArbitration.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE
class QStringList;
class QTimer;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE

class QSystemNetworkInfo;

class QLangLoopThread;

class QSystemInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemInfoPrivate(QObject *parent = 0);
    virtual ~QSystemInfoPrivate();
    QString currentLanguage() const;
    QStringList availableLanguages() const;

    QString version(QSystemInfo::Version,  const QString &parameter = QString());

    QString currentCountryCode() const;
    bool hasFeatureSupported(QSystemInfo::Feature feature);
    void languageChanged(const QString &);
    static QSystemInfoPrivate *instance() {return self;}

Q_SIGNALS:
    void currentLanguageChanged(const QString &);

private:
    QTimer *langTimer;
    QString langCached;
    QLangLoopThread * langloopThread;
    static QSystemInfoPrivate *self;
    bool langThreadOk;

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

};

class QRunLoopThread;
class QSystemNetworkInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemNetworkInfoPrivate(QObject *parent = 0);
    virtual ~QSystemNetworkInfoPrivate();

    QSystemNetworkInfo::NetworkStatus networkStatus(QSystemNetworkInfo::NetworkMode mode);
    int networkSignalStrength(QSystemNetworkInfo::NetworkMode mode);
    int cellId();
    int locationAreaCode();

    QString currentMobileCountryCode();
    QString currentMobileNetworkCode();

    QString homeMobileCountryCode();
    QString homeMobileNetworkCode();

    QString networkName(QSystemNetworkInfo::NetworkMode mode);
    QString macAddress(QSystemNetworkInfo::NetworkMode mode);

    QNetworkInterface interfaceForMode(QSystemNetworkInfo::NetworkMode mode);
    static QSystemNetworkInfoPrivate *instance() {return self;}
    void wifiNetworkChanged(const QString &notification, const QString interfaceName);
    QString getDefaultInterface();
    QSystemNetworkInfo::NetworkMode currentMode();
    void ethernetChanged();
    QSystemNetworkInfo::CellDataTechnology cellDataTechnology();

Q_SIGNALS:
   void networkStatusChanged(QSystemNetworkInfo::NetworkMode, QSystemNetworkInfo::NetworkStatus);
   void networkSignalStrengthChanged(QSystemNetworkInfo::NetworkMode,int);
   void currentMobileCountryCodeChanged(const QString &);
   void currentMobileNetworkCodeChanged(const QString &);
   void networkNameChanged(QSystemNetworkInfo::NetworkMode, const QString &);
   void networkModeChanged(QSystemNetworkInfo::NetworkMode);
   void cellIdChanged(int);//1.2

public Q_SLOTS:
   void primaryInterface();

private:
    QTimer *rssiTimer;
    int signalStrengthCache;
    static QSystemNetworkInfoPrivate *self;
    QRunLoopThread * runloopThread;
    QString defaultInterface;
    QSystemNetworkInfo::NetworkMode modeForInterface(QString interfaceName);

private Q_SLOTS:
    void rssiTimeout();

protected:
    void startNetworkChangeLoop();
    bool isInterfaceActive(const char* netInterface);

    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);
    bool hasWifi;
    bool networkThreadOk;

};

class QSystemDisplayInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemDisplayInfoPrivate(QObject *parent = 0);
    virtual ~QSystemDisplayInfoPrivate();

    int displayBrightness(int screen);
    int colorDepth(int screen);

    QSystemDisplayInfo::DisplayOrientation orientation(int screen);
    float contrast(int screen);
    int getDPIWidth(int screen);
    int getDPIHeight(int screen);
    int physicalHeight(int screen);
    int physicalWidth(int screen);
    QSystemDisplayInfo::BacklightState backlightStatus(int screen); //1.2
};

class QDASessionThread;
class QSystemStorageInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemStorageInfoPrivate(QObject *parent = 0);
    virtual ~QSystemStorageInfoPrivate();

    qint64 availableDiskSpace(const QString &driveVolume);
    qint64 totalDiskSpace(const QString &driveVolume);
    QStringList logicalDrives();
    QSystemStorageInfo::DriveType typeForDrive(const QString &driveVolume);

    QString uriForDrive(const QString &driveVolume);//1.2
    QSystemStorageInfo::StorageState getStorageState(const QString &volume);//1.2

public Q_SLOTS:
    void storageChanged( bool added,const QString &vol);

Q_SIGNALS:
    void logicalDriveChanged(bool added,const QString &vol);
    void storageStateChanged(const QString &vol, QSystemStorageInfo::StorageState state); //1.2

private:
    QMap<QString, QString> mountEntriesMap;
    bool updateVolumesMap();
    void mountEntries();
    bool sessionThread();
    QMap<QString, QSystemStorageInfo::StorageState> stateMap;
    QTimer *storageTimer;

private Q_SLOTS:
    void checkAvailableStorage();

protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

    QDASessionThread *daSessionThread;
    bool sessionThreadStarted;
};

class QBluetoothListenerThread;
class QSystemDeviceInfoPrivate : public QObject
{
    Q_OBJECT

public:

    QSystemDeviceInfoPrivate(QObject *parent = 0);
    ~QSystemDeviceInfoPrivate();

    static QString imei();
    static QString imsi();
    static QString manufacturer();
    static QString model();
    static QString productName();

    QSystemDeviceInfo::InputMethodFlags inputMethodType();

    int  batteryLevel();

    QSystemDeviceInfo::SimStatus simStatus();
    bool isDeviceLocked();
    QSystemDeviceInfo::Profile currentProfile();
    QSystemDeviceInfo::ProfileDetails currentProfileDetails;

    QSystemDeviceInfo::PowerState currentPowerState();
    QSystemDeviceInfo::ThermalState currentThermalState();
    void setConnection();
    static QSystemDeviceInfoPrivate *instance();

    bool currentBluetoothPowerState();
    bool btThreadOk;

    QSystemDeviceInfo::KeyboardTypeFlags keyboardTypes(); //1.2
    bool isWirelessKeyboardConnected(); //1.2
    bool isKeyboardFlippedOpen();//1.2

    void keyboardConnected(bool connect);//1.2
    bool keypadLightOn(QSystemDeviceInfo::KeypadType type); //1.2
    void deviceLocked(bool isLocked); // 1.2
    QByteArray uniqueDeviceID(); //1.2
    QSystemDeviceInfo::LockTypeFlags lockStatus(); //1.2

    int messageRingtoneVolume();//1.2
    int voiceRingtoneVolume();//1.2
    bool vibrationActive();//1.2

Q_SIGNALS:
    void batteryLevelChanged(int);
    void batteryStatusChanged(QSystemDeviceInfo::BatteryStatus );

    void powerStateChanged(QSystemDeviceInfo::PowerState);
    void thermalStateChanged(QSystemDeviceInfo::ThermalState);
    void currentProfileChanged(QSystemDeviceInfo::Profile);
    void bluetoothStateChanged(bool);

    void wirelessKeyboardConnected(bool connected);//1.2
    void keyboardFlipped(bool open);//1.2
    void lockStatusChanged(QSystemDeviceInfo::LockTypeFlags); //1.2


private:
    int batteryLevelCache;
    QSystemDeviceInfo::PowerState currentPowerStateCache;
    QSystemDeviceInfo::BatteryStatus batteryStatusCache;
    static QSystemDeviceInfoPrivate *self;
    QBluetoothListenerThread *btThread;

    bool hasWirelessKeyboardConnected;
protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

};


class QSystemScreenSaverPrivate : public QObject
{
    Q_OBJECT

public:
    QSystemScreenSaverPrivate(QObject *parent = 0);
    ~QSystemScreenSaverPrivate();

    bool screenSaverInhibited();
    bool setScreenSaverInhibit();
    void setScreenSaverInhibited(bool on);

private:
    QString screenPath;
    QString settingsPath;
    bool screenSaverSecure;
    bool isInhibited;
    QTimer *ssTimer;

private Q_SLOTS:
    void activityTimeout();

};

class QRunLoopThread : public QObject
{
    Q_OBJECT

public:
    QRunLoopThread(QObject *parent = 0);
    ~QRunLoopThread();
    bool keepRunning;
    void stop();

public Q_SLOTS:
   void doWork();

protected:
   QThread t;
private:
    void startNetworkChangeLoop();
    QMutex mutex;
    SCDynamicStoreRef storeSession;
    CFRunLoopSourceRef runloopSource;

private Q_SLOTS:
};

class QLangLoopThread : public QObject
{
    Q_OBJECT

public:
    QLangLoopThread(QObject *parent = 0);
    ~QLangLoopThread();
    bool keepRunning;
    void stop();

public Q_SLOTS:
    void doWork();

private:
    QMutex mutex;
    QThread t;
};

class QDASessionThread : public QObject
{
    Q_OBJECT

public:
    QDASessionThread(QObject *parent = 0);
    ~QDASessionThread();
    bool keepRunning;
    void stop();
    DASessionRef session;
public Q_SLOTS:
    void doWork();
Q_SIGNALS:
    void logicalDrivesChanged(bool added,const QString & vol);

protected:
    QThread t;

private:
    QMutex mutex;

};

class QBluetoothListenerThread : public QObject
{
    Q_OBJECT

public:
    QBluetoothListenerThread(QObject *parent = 0);
    ~QBluetoothListenerThread();
    bool keepRunning;
    QThread t;
    void setupConnectNotify();

public Q_SLOTS:
    void emitBtPower(bool);
    void stop();
    void doWork();

Q_SIGNALS:
    void bluetoothPower(bool);

protected:
    IONotificationPortRef port;
    CFRunLoopRef rl;
    CFRunLoopSourceRef rls;

private:
    QMutex mutex;

private Q_SLOTS:
};

class QSystemBatteryInfoPrivate : public QObject
{
    Q_OBJECT
public:
    QSystemBatteryInfoPrivate(QObject *parent = 0);
    ~QSystemBatteryInfoPrivate();


    QSystemBatteryInfo::ChargerType chargerType() const;
    QSystemBatteryInfo::ChargingState chargingState() const;

    int nominalCapacity() const;
    int remainingCapacityPercent() const;
    int remainingCapacity() const;

    int voltage() const;
    int remainingChargingTime() const;
    int currentFlow() const;
    int remainingCapacityBars() const;
    int maxBars() const;
    QSystemBatteryInfo::BatteryStatus batteryStatus() const;
    QSystemBatteryInfo::EnergyUnit energyMeasurementUnit();
    void getBatteryInfo();

Q_SIGNALS:
    void batteryLevelChanged(int level);
    void batteryStatusChanged(QSystemBatteryInfo::BatteryStatus batteryStatus);


    void chargingStateChanged(QSystemBatteryInfo::ChargingState chargingState);
    void chargerTypeChanged(QSystemBatteryInfo::ChargerType chargerType);

    void nominalCapacityChanged(int);
    void remainingCapacityPercentChanged(int);
    void remainingCapacityChanged(int);
    void batteryCurrentFlowChanged(int);

    void currentFlowChanged(int);
    void cumulativeCurrentFlowChanged(int);
    void remainingCapacityBarsChanged(int);
    void remainingChargingTimeChanged(int);

private:
    QSystemBatteryInfo::BatteryStatus currentBatStatus;
    QSystemBatteryInfo::ChargingState curChargeState;
    QSystemBatteryInfo::ChargerType curChargeType;

    int currentBatLevelPercent;
    int currentVoltage;
    int dischargeRate;
    int capacity;
    int timeToFull;
    int remainingEnergy;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif /*QSYSTEMSINFO_MAC_P_H*/

// End of file

