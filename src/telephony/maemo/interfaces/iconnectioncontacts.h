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

#ifndef ICONNECTIONCONTACTS_H
#define ICONNECTIONCONTACTS_H

#include <QtCore/QtGlobal>
#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

#include "maemo/interfaces/itypes.h"
#include "maemo/interfaces/iabstract.h"
#include "maemo/interfaces/iconnection.h"
#include "maemo/dbusproxy.h"

namespace DBus
{
    namespace Interfaces
    {
        class IConnectionContacts : public DBus::IAbstract
        {
            Q_OBJECT

        public:
            static inline const char *staticInterfaceName()
            {
                return "org.freedesktop.Telepathy.Connection.Interface.Contacts";
            }

            IConnectionContacts(
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IConnectionContacts(
                const QDBusConnection& connection,
                const QString& busName,
                const QString& objectPath,
                QObject* parent = 0
            );
            IConnectionContacts(DBus::DBusProxy *proxy);
            explicit IConnectionContacts(const DBus::Interfaces::IConnection& mainInterface);
            IConnectionContacts(const DBus::Interfaces::IConnection& mainInterface, QObject* parent);

            Q_PROPERTY(QStringList ContactAttributeInterfaces READ ContactAttributeInterfaces)
            inline QStringList ContactAttributeInterfaces() const
            {
                return qvariant_cast<QStringList>(internalPropGet("ContactAttributeInterfaces"));
            }

        public Q_SLOTS:
            inline QDBusPendingReply<DBus::Interfaces::ContactAttributesMap> GetContactAttributes(const DBus::Interfaces::UIntList& handles, const QStringList& interfaces, bool hold)
            {
                if (!invalidationReason().isEmpty()) {
                    return QDBusPendingReply<DBus::Interfaces::ContactAttributesMap>(QDBusMessage::createError(
                        invalidationReason(),
                        invalidationMessage()
                    ));
                }
                QList<QVariant> argumentList;
                argumentList << QVariant::fromValue(handles) << QVariant::fromValue(interfaces) << QVariant::fromValue(hold);
                return asyncCallWithArgumentList(QLatin1String("GetContactAttributes"), argumentList);
            }

        protected:
            virtual void invalidate(DBus::DBusProxy *, const QString &, const QString &);
        };
    }//Interfaces
}//DBus
Q_DECLARE_METATYPE(DBus::Interfaces::IConnectionContacts*)

#endif //ICONNECTIONCONTACTS_H