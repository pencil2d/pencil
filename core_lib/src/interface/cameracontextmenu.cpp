/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/

#include "cameracontextmenu.h"

#include <QMenu>

#include "cameraeasingtype.h"
#include "layercamera.h"
#include "camera.h"

CameraContextMenu::CameraContextMenu(int frameNumber, QPoint coordinate, Layer* layer) :
    mFrameNumber(frameNumber), mMouseCoord(coordinate), mCurrentLayer(layer)

{
}

void CameraContextMenu::exec()
{
    int frameNumber = mFrameNumber;
    QPoint pos = mMouseCoord;

    Layer* curLayer = mCurrentLayer;

    LayerCamera* layer = static_cast<LayerCamera*>(curLayer);
    int nextFrame = curLayer->getNextKeyFramePosition(frameNumber);

    QMenu* cameraMenu = new QMenu();

    connect(cameraMenu, &QMenu::aboutToHide, [=] {
        emit aboutToClose();
    });

    QMenu* cameraInterpolationMenu = cameraMenu->addMenu(tr("Easing: frame %1 to %2").arg( QString::number(frameNumber), QString::number(nextFrame)));

    cameraInterpolationMenu->setEnabled(curLayer->getMaxKeyFramePosition() != frameNumber);

    Camera* selectedKey = layer->getCameraAtFrame(frameNumber);
    if (selectedKey != nullptr) {
        QAction* selectedAction = cameraInterpolationMenu->addAction(tr("Selected: ") + getInterpolationText(selectedKey->getEasingType()));
        selectedAction->setDisabled(true);
    }

    cameraInterpolationMenu->addAction(tr("Linear"), [=] { layer->setCameraEasingAtFrame(CameraEasingType::LINEAR, frameNumber); });
    cameraInterpolationMenu->addSeparator();
    QMenu* inMenu = cameraInterpolationMenu->addMenu(tr("In"));
    QMenu* outMenu = cameraInterpolationMenu->addMenu(tr("Out"));
    QMenu* inOutMenu = cameraInterpolationMenu->addMenu(tr("In-Out"));
    QMenu* outInMenu = cameraInterpolationMenu->addMenu(tr("Out-In"));

    QString slow = tr("Slow");
    QString moderate = tr("Moderate");
    QString quick = tr("Quick");
    QString fast = tr("Fast");
    QString faster = tr("Faster");
    QString fastest = tr("Faster");
    QString circleBased = tr("Circle-based");
    QString overshoot = tr("Overshoot");
    QString elastic = tr("Elastic");
    QString bounce = tr("Bounce");

    inMenu->addAction(slow, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INSINE, frameNumber); });
    outMenu->addAction(slow, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTSINE, frameNumber); });
    inOutMenu->addAction(slow, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INOUTSINE, frameNumber); });
    outInMenu->addAction(slow, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTINSINE, frameNumber); });
    inMenu->addAction(moderate, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INQUAD, frameNumber); });
    outMenu->addAction(moderate, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTQUAD, frameNumber); });
    inOutMenu->addAction(moderate, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INOUTQUAD, frameNumber); });
    outInMenu->addAction(moderate, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTINQUAD, frameNumber); });
    inMenu->addAction(quick, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INCUBIC, frameNumber); });
    outMenu->addAction(quick, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTCUBIC, frameNumber); });
    inOutMenu->addAction(quick, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INOUTCUBIC, frameNumber); });
    outInMenu->addAction(quick, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTINCUBIC, frameNumber); });
    inMenu->addAction(fast, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INQUART, frameNumber); });
    outMenu->addAction(fast, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTQUART, frameNumber); });
    inOutMenu->addAction(fast, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INOUTQUART, frameNumber); });
    outInMenu->addAction(fast, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTINQUART, frameNumber); });
    inMenu->addAction(faster, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INQUINT, frameNumber); });
    outMenu->addAction(faster, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTQUINT, frameNumber); });
    inOutMenu->addAction(faster, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INOUTQUINT, frameNumber); });
    outInMenu->addAction(faster, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTINQUINT, frameNumber); });
    inMenu->addAction(fastest, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INEXPO, frameNumber); });
    outMenu->addAction(fastest, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTEXPO, frameNumber); });
    inOutMenu->addAction(fastest, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INOUTEXPO, frameNumber); });
    outInMenu->addAction(fastest, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTINEXPO, frameNumber); });
    inMenu->addAction(circleBased, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INCIRC, frameNumber); });
    outMenu->addAction(circleBased, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTCIRC, frameNumber); });
    inOutMenu->addAction(circleBased, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INOUTCIRC, frameNumber); });
    outInMenu->addAction(circleBased, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTINCIRC, frameNumber); });
    inMenu->addAction(overshoot, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INBACK, frameNumber); });
    outMenu->addAction(overshoot, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTBACK, frameNumber); });
    inOutMenu->addAction(overshoot, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INOUTBACK, frameNumber); });
    outInMenu->addAction(overshoot, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTINBACK, frameNumber); });
    inMenu->addAction(elastic, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INELASTIC, frameNumber); });
    outMenu->addAction(elastic, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTELASTIC, frameNumber); });
    inOutMenu->addAction(elastic, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INOUTELASTIC, frameNumber); });
    outInMenu->addAction(elastic, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTINELASTIC, frameNumber); });
    inMenu->addAction(bounce, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INBOUNCE, frameNumber); });
    outMenu->addAction(bounce, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTBOUNCE, frameNumber); });
    inOutMenu->addAction(bounce, [=] { layer->setCameraEasingAtFrame(CameraEasingType::INOUTBOUNCE, frameNumber); });
    outInMenu->addAction(bounce, [=] { layer->setCameraEasingAtFrame(CameraEasingType::OUTINBOUNCE, frameNumber); });

    QMenu* cameraFieldMenu = cameraMenu->addMenu(tr("Transform"));
    cameraFieldMenu->addAction(tr("Reset all"), [=] { layer->resetCameraAtFrame(CameraFieldOption::RESET_FIELD, frameNumber); });
    cameraFieldMenu->addSeparator();
    cameraFieldMenu->addAction(tr("Reset position"), [=] { layer->resetCameraAtFrame(CameraFieldOption::RESET_TRANSLATION, frameNumber); });
    cameraFieldMenu->addAction(tr("Reset scale"), [=] { layer->resetCameraAtFrame(CameraFieldOption::RESET_SCALING, frameNumber); });
    cameraFieldMenu->addAction(tr("Reset rotation"), [=] { layer->resetCameraAtFrame(CameraFieldOption::RESET_ROTATION, frameNumber); });
    cameraFieldMenu->addSeparator();
    cameraFieldMenu->addAction(tr("Align horizontally to frame: %1").arg(QString::number(nextFrame)), [=] { layer->resetCameraAtFrame(CameraFieldOption::ALIGN_HORIZONTAL, frameNumber); });
    cameraFieldMenu->addAction(tr("Align vertically to frame: %1").arg(QString::number(nextFrame)), [=] { layer->resetCameraAtFrame(CameraFieldOption::ALIGN_VERTICAL, frameNumber); });
    cameraFieldMenu->addSeparator();
    cameraFieldMenu->addAction(tr("Hold to keyframe %1").arg(QString::number(nextFrame)), [=] { layer->resetCameraAtFrame(CameraFieldOption::HOLD_FRAME, frameNumber); });

    cameraMenu->exec(pos);
}
