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

#ifndef QDECLARATIVEGALLERYQUERYCOUNT_H
#define QDECLARATIVEGALLERYQUERYCOUNT_H

#include <qgallerycountrequest.h>

#include <QtCore/qpointer.h>
#include <QtDeclarative/qdeclarative.h>

QTM_BEGIN_NAMESPACE

class QDeclarativeGalleryFilterBase;

class QDeclarativeGalleryQueryCount : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_ENUMS(State)
    Q_ENUMS(Result)
    Q_ENUMS(Scope)
    Q_PROPERTY(QAbstractGallery* gallery READ gallery WRITE setGallery)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(Result result READ result NOTIFY resultChanged)
    Q_PROPERTY(int currentProgress READ currentProgress NOTIFY progressChanged)
    Q_PROPERTY(int maximumProgress READ maximumProgress NOTIFY progressChanged)
    Q_PROPERTY(bool live READ isLive WRITE setLive)
    Q_PROPERTY(QString itemType READ itemType WRITE setItemType)
    Q_PROPERTY(Scope scope READ scope WRITE setScope)
    Q_PROPERTY(QVariant scopeItemId READ scopeItemId WRITE setScopeItemId)
    Q_PROPERTY(QDeclarativeGalleryFilterBase* filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum State
    {
        Inactive    = QGalleryAbstractRequest::Inactive,
        Active      = QGalleryAbstractRequest::Active,
        Cancelling  = QGalleryAbstractRequest::Cancelling,
        Idle        = QGalleryAbstractRequest::Idle
    };

    enum Result
    {
        NoResult                        = QGalleryAbstractRequest::NoResult,
        Succeeded                       = QGalleryAbstractRequest::NoResult,
        Cancelled                       = QGalleryAbstractRequest::NoResult,
        NoGallery                       = QGalleryAbstractRequest::NoResult,
        NotSupported                    = QGalleryAbstractRequest::NoResult,
        ConnectionError                 = QGalleryAbstractRequest::NoResult,
        InvalidItemError                = QGalleryAbstractRequest::NoResult,
        ItemTypeError                   = QGalleryAbstractRequest::NoResult,
        InvalidPropertyError            = QGalleryAbstractRequest::NoResult,
        PropertyTypeError               = QGalleryAbstractRequest::NoResult,
        UnsupportedFilterTypeError      = QGalleryAbstractRequest::NoResult,
        UnsupportedFilterOptionError    = QGalleryAbstractRequest::NoResult
    };

    enum Scope
    {
        AllDescendants,
        DirectDescendants
    };

    QDeclarativeGalleryQueryCount(QObject *parent = 0);

    QAbstractGallery *gallery() const { return m_request.gallery(); }
    void setGallery(QAbstractGallery *gallery) { m_request.setGallery(gallery); }

    State state() const { return State(m_request.state()); }
    Result result() const { return Result(m_request.result()); }

    int currentProgress() const { return m_request.currentProgress(); }
    int maximumProgress() const { return m_request.maximumProgress(); }

    bool isLive() const { return m_request.isLive(); }
    void setLive(bool live) { m_request.setLive(live); }

    QString itemType() const { return m_request.itemType(); }
    void setItemType(const QString &itemType) { m_request.setItemType(itemType); }

    Scope scope() const { return Scope(m_request.scope()); }
    void setScope(Scope scope) { m_request.setScope(QGalleryAbstractRequest::Scope(scope)); }

    QVariant scopeItemId() const { return m_request.scopeItemId(); }
    void setScopeItemId(const QVariant &itemId) { m_request.setScopeItemId(itemId); }

    QDeclarativeGalleryFilterBase *filter() const { return m_filter; }
    void setFilter(QDeclarativeGalleryFilterBase *filter) { m_filter = filter; }

    int count() const { return m_request.count(); }

    void classBegin();
    void componentComplete();

public Q_SLOTS:
    void reload();
    void cancel() { m_request.cancel(); }
    void clear() { m_request.clear(); }

Q_SIGNALS:
    void succeeded();
    void cancelled();
    void failed(int result);
    void finished(int result);
    void stateChanged();
    void resultChanged();
    void progressChanged();
    void countChanged();

private:
    QGalleryCountRequest m_request;
    QPointer<QDeclarativeGalleryFilterBase> m_filter;
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGalleryQueryCount))

#endif
