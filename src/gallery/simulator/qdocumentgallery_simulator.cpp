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

#include "qdocumentgallery.h"

#include "qabstractgallery_p.h"

#include "qgalleryitemrequest.h"
#include "qgalleryqueryrequest.h"
#include "qgallerytyperequest.h"

#include "qgalleryabstractresponse.h"

#include "qsimulatorgalleryresultset_p.h"
#include "docgalleryconnection_simulator.h"

#include <QtCore/qmetaobject.h>

Q_DECLARE_METATYPE(QVector<QStringList>)

QTM_BEGIN_NAMESPACE

using namespace Simulator;

class QDocumentGalleryPrivate : public QAbstractGalleryPrivate
{
public:
    QGalleryAbstractResponse *createItemResponse(QGalleryItemRequest *request);
    QGalleryAbstractResponse *createTypeResponse(QGalleryTypeRequest *request);
    QGalleryAbstractResponse *createQueryResponse(QGalleryQueryRequest *request);

private:
};

QGalleryAbstractResponse *QDocumentGalleryPrivate::createTypeResponse(QGalleryTypeRequest *request)
{
    Q_UNUSED(request);
    return new QGalleryAbstractResponse(QDocumentGallery::NoGallery);
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createItemResponse(QGalleryItemRequest *request)
{
    return new QSimulatorGalleryResultSet(request);
}

QGalleryAbstractResponse *QDocumentGalleryPrivate::createQueryResponse(
        QGalleryQueryRequest *request)
{
    return new QSimulatorGalleryResultSet(request);
}


QDocumentGallery::QDocumentGallery(QObject *parent)
    : QAbstractGallery(*new QDocumentGalleryPrivate, parent)
{
}

QDocumentGallery::~QDocumentGallery()
{
}

bool QDocumentGallery::isRequestSupported(QGalleryAbstractRequest::RequestType type) const
{
    if (type == QGalleryAbstractRequest::ItemRequest || type == QGalleryAbstractRequest::QueryRequest)
        return true;
    return false;
}

QStringList QDocumentGallery::itemTypePropertyNames(const QString &itemType) const
{
    Q_UNUSED(itemType);
    return QStringList();
}

QGalleryProperty::Attributes QDocumentGallery::propertyAttributes(
        const QString &propertyName, const QString &itemType) const
{
    Q_UNUSED(propertyName);
    Q_UNUSED(itemType);
    return QGalleryProperty::CanRead;
}

QGalleryAbstractResponse *QDocumentGallery::createResponse(QGalleryAbstractRequest *request)
{
    Q_D(QDocumentGallery);

    switch (request->type()) {
    case QGalleryAbstractRequest::QueryRequest:
        return d->createQueryResponse(static_cast<QGalleryQueryRequest *>(request));
    case QGalleryAbstractRequest::ItemRequest:
        return d->createItemResponse(static_cast<QGalleryItemRequest *>(request));
    case QGalleryAbstractRequest::TypeRequest:
        return d->createTypeResponse(static_cast<QGalleryTypeRequest *>(request));
    default:
        return 0;
    }
}

#include "moc_qdocumentgallery.cpp"

QTM_END_NAMESPACE

