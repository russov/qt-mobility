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

#include "qgeosearchmanagerengine_nokia.h"
#include "qgeosearchreply_nokia.h"

#include <qgeoaddress.h>
#include <qgeocoordinate.h>
#include <QNetworkProxy>
#include <QMap>

QGeoSearchManagerEngineNokia::QGeoSearchManagerEngineNokia(
    const QMap<QString, QVariant> &parameters, QGeoServiceProvider::Error *error,
    QString *errorString)
        : QGeoSearchManagerEngine(parameters),
        m_host("address.s2g.gate5.de")
{
    m_networkManager = new QNetworkAccessManager(this);

    QList<QString> keys = parameters.keys();

    if (keys.contains("places.proxy")) {
        QString proxy = parameters.value("places.proxy").toString();
        if (!proxy.isEmpty())
            m_networkManager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, proxy, 8080));
    }

    if (keys.contains("places.host")) {
        QString host = parameters.value("places.host").toString();
        if (!host.isEmpty())
            m_host = host;
    }

    setSupportsGeocoding(true);

    QGeoSearchManager::SearchTypes supportedSearchTypes;
    supportedSearchTypes |= QGeoSearchManager::SearchGeocode;
    setSupportedSearchTypes(supportedSearchTypes);

    if (error)
        *error = QGeoServiceProvider::NoError;

    if (errorString)
        *errorString = "";
}

QGeoSearchManagerEngineNokia::~QGeoSearchManagerEngineNokia() {}

QGeoSearchReply* QGeoSearchManagerEngineNokia::geocode(const QGeoAddress &address,
        QGeoBoundingArea *bounds)
{
    Q_UNUSED(bounds)

    if (!supportsGeocoding()) {
        QGeoSearchReply *reply = new QGeoSearchReply(QGeoSearchReply::UnsupportedOptionError,
                "Geocoding is not supported by this service provider.", this);
        emit error(reply, reply->error(), reply->errorString());
        return reply;
    }

    QString requestString = "http://";
    requestString += m_host;
    requestString += "/nsp?vi=where&la=eng-uk&to=20&q=";


    if (!address.street().isEmpty()) {
        requestString += address.street();
        requestString += ",";
    }

    if (!address.streetNumber().isEmpty()) {
        requestString += address.streetNumber();
        requestString += ",";
    }

    if (!address.postCode().isEmpty()) {
        requestString += address.postCode();
        requestString += ",";
    }

    if (!address.city().isEmpty()) {
        requestString += address.city();
        requestString += ",";
    }

    if (!address.state().isEmpty()) {
        requestString += address.state();
        requestString += ",";
    }

    if (!address.country().isEmpty()) {
        requestString += address.country();
    }

    return search(requestString);
}

// NOTE: reverse geocoding is not supported and the default implementation
// same error message, so this function is not needed.
//QGeoSearchReply* QGeoSearchManagerEngineNokia::reverseGeocode(const QGeoCoordinate &coordinate,
//    const QGeoBoundingBox &bounds)
//{
//    Q_UNUSED(bounds)

//    if (!supportsReverseGeocoding()) {
//        QGeoSearchReply *reply = new QGeoSearchReply(QGeoSearchReply::UnsupportedOptionError,
//            "Reverse geocoding is not supported by this service provider.", this);
//        emit error(reply, reply->error(), reply->errorString());
//        return reply;
//    }

//    QString requestString = "http://";
//    requestString += m_host;

//    return search(requestString);
//}

QGeoSearchReply* QGeoSearchManagerEngineNokia::search(const QString &searchString,
        QGeoSearchManager::SearchTypes searchTypes,
        int limit,
        int offset,
        QGeoBoundingArea *bounds)
{
    // NOTE this will eventually replaced by a much improved implementation
    // which will make use of the additionLandmarkManagers()
    if ((searchTypes != QGeoSearchManager::SearchTypes(QGeoSearchManager::SearchAll))
            && ((searchTypes & supportedSearchTypes()) != searchTypes)) {

        QGeoSearchReply *reply = new QGeoSearchReply(QGeoSearchReply::UnsupportedOptionError,
                "The selected search type is not supported by this service provider.", this);
        emit error(reply, reply->error(), reply->errorString());
        return reply;
    }

    Q_UNUSED(bounds)

    if (!supportsGeocoding()) {
        QGeoSearchReply *reply = new QGeoSearchReply(QGeoSearchReply::UnsupportedOptionError,
                "Geocoding is not supported by this service provider.", this);
        emit error(reply, reply->error(), reply->errorString());
        return reply;
    }

    QString q = searchString;

    QString requestString = "http://";
    requestString += m_host;
    requestString += "/nsp?";
    requestString += "vi=where&la=eng-uk&to=20&q=";
    requestString += q;

    return search(requestString);
}

QGeoSearchReply* QGeoSearchManagerEngineNokia::search(QString requestString)
{
    QNetworkReply *networkReply = m_networkManager->get(QNetworkRequest(QUrl(requestString)));
    QGeoSearchReplyNokia *reply = new QGeoSearchReplyNokia(networkReply, this);

    connect(reply,
            SIGNAL(finished()),
            this,
            SLOT(placesFinished()));

    connect(reply,
            SIGNAL(error(QGeoSearchReply::Error, QString)),
            this,
            SLOT(placesError(QGeoSearchReply::Error, QString)));

    return reply;
}

void QGeoSearchManagerEngineNokia::placesFinished()
{
    QGeoSearchReply *reply = qobject_cast<QGeoSearchReply*>(sender());

    if (!reply)
        return;

    if (receivers(SIGNAL(finished(QGeoSearchReply*))) == 0) {
        reply->deleteLater();
        return;
    }

    emit finished(reply);
}

void QGeoSearchManagerEngineNokia::placesError(QGeoSearchReply::Error error, const QString &errorString)
{
    QGeoSearchReply *reply = qobject_cast<QGeoSearchReply*>(sender());

    if (!reply)
        return;

    if (receivers(SIGNAL(error(QGeoSearchReply*, QGeoSearchReply::Error, QString))) == 0) {
        reply->deleteLater();
        return;
    }

    emit this->error(reply, error, errorString);
}