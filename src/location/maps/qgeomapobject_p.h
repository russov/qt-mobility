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

#ifndef QGEOMAPOBJECT_P_H
#define QGEOMAPOBJECT_P_H

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

#include "qgeomapobject.h"
#include "qgeomapobjectinfo.h"

#include <QList>
#include <QRectF>
#include "qgeoboundingbox.h"

class QPainter;
class QPainterPath;
class QStyleOptionGraphicsItem;

QTM_BEGIN_NAMESPACE

class QGeoMapObject;
class QGeoMapData;
class QGeoMapDataPrivate;

//class QGeoMapObjectInfo
//{
//public:
//    QGeoMapObjectInfo(QGeoMapData *mapData, QGeoMapObject *mapObject);
//    virtual ~QGeoMapObjectInfo();

//    virtual void addToParent() = 0;
//    virtual void removeFromParent() = 0;

//    virtual void objectUpdate() = 0;
//    virtual void mapUpdate() = 0;

//    virtual QGeoBoundingBox boundingBox() const = 0;
//    virtual bool contains(const QGeoCoordinate &coord) const = 0;

//    QGeoMapData* mapData;
//    QGeoMapObject* mapObject;

//private:
//    Q_DISABLE_COPY(QGeoMapObjectInfo)
//};

class QGeoMapObjectPrivate
{
public:
    QGeoMapObjectPrivate(QGeoMapObject *impl, QGeoMapObject *parent, QGeoMapObject::Type type = QGeoMapObject::ContainerType);
    QGeoMapObjectPrivate(QGeoMapObject *impl, QGeoMapData *mapData, QGeoMapObject::Type type = QGeoMapObject::ContainerType);
    virtual ~QGeoMapObjectPrivate();

    void setMapData(QGeoMapData *mapData);

    void addToParent(QGeoMapObject *parent);
    void removeFromParent();

    void objectUpdate();
    void mapUpdate();

    QGeoBoundingBox boundingBox() const;
    bool contains(const QGeoCoordinate &coord) const;

    QGeoMapObject::Type type;
    QGeoMapObject *parent;
    QList<QGeoMapObject *> children;

    int zValue;
    bool isVisible;
    bool isSelected;

    QGeoMapData *mapData;
    mutable QGeoMapObjectInfo *info;

    QGeoMapObject *q_ptr;
    Q_DECLARE_PUBLIC(QGeoMapObject)
    Q_DISABLE_COPY(QGeoMapObjectPrivate)
};

QTM_END_NAMESPACE

#endif