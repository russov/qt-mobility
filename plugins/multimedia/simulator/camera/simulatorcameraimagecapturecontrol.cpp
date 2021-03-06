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

#include <QtCore/qstring.h>

#include "simulatorcameraimagecapturecontrol.h"
#include "simulatorcameraservice.h"
#include "simulatorcamerasession.h"
#include "simulatorcameracontrol.h"

SimulatorCameraImageCaptureControl::SimulatorCameraImageCaptureControl(SimulatorCameraSession *session, SimulatorCameraService *service) :
    QCameraImageCaptureControl(service),
    mReadyForCapture(true),
    m_driveMode(QCameraImageCapture::SingleImageCapture) // Default DriveMode
{
    m_session = session;

    m_service = service;
    m_cameraControl = qobject_cast<SimulatorCameraControl *>(m_service->requestControl(QCameraControl_iid));

    // Chain these signals from session class
    connect(m_session, SIGNAL(imageCaptured(const int, QImage)),
            this, SIGNAL(imageCaptured(const int, QImage)));
    connect(m_session, SIGNAL(imageSaved(const int, const QString&)),
            this, SIGNAL(imageSaved(const int, const QString&)));
    connect(m_session, SIGNAL(imageExposed(int)),
            this, SIGNAL(imageExposed(int)));
    connect(m_session, SIGNAL(captureError(int, int, const QString&)),
            this, SIGNAL(error(int, int, const QString&)));
}

SimulatorCameraImageCaptureControl::~SimulatorCameraImageCaptureControl()
{
}

bool SimulatorCameraImageCaptureControl::isReadyForCapture() const
{
    if (m_cameraControl && m_cameraControl->captureMode() != QCamera::CaptureStillImage) {
        return false;
    }

    return mReadyForCapture;
}

QCameraImageCapture::DriveMode SimulatorCameraImageCaptureControl::driveMode() const
{
    return m_driveMode;
}

void SimulatorCameraImageCaptureControl::setDriveMode(QCameraImageCapture::DriveMode mode)
{
    if (mode != QCameraImageCapture::SingleImageCapture) {
        emit error(0, QCamera::NotSupportedFeatureError, tr("DriveMode not supported."));
        return;
    }

    m_driveMode = mode;
}

int SimulatorCameraImageCaptureControl::capture(const QString &fileName)
{
    if (m_cameraControl && m_cameraControl->captureMode() != QCamera::CaptureStillImage) {
        emit error(0, QCameraImageCapture::NotReadyError, tr("Incorrect CaptureMode."));
        return 0;
    }
    updateReadyForCapture(false);
    int imageId = m_session->captureImage(fileName);
    updateReadyForCapture(true);
    return imageId;
}

void SimulatorCameraImageCaptureControl::cancelCapture()
{
}

void SimulatorCameraImageCaptureControl::updateReadyForCapture(bool ready)
{
    mReadyForCapture = ready;
    emit readyForCaptureChanged(mReadyForCapture);
}

// End of file
