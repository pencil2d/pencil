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

#ifndef CAMERAPROPERTIESDIALOG_H
#define CAMERAPROPERTIESDIALOG_H

#include <QDialog>

namespace Ui {
    class CameraPropertiesDialog;
}

class CameraPropertiesDialog : public QDialog
{
    Q_OBJECT
public:
    CameraPropertiesDialog(const QString& name, int width, int height);
    ~CameraPropertiesDialog() override;
    QString getName();
    void setName(const QString& name);
    int getWidth();
    void setWidth(int);
    int getHeight();
    void setHeight(int);

    // DOF properties
    int getDistance();
    void setDistance(int dist);
    qreal getAperture();
    void setAperture(qreal aperture);

private:
    Ui::CameraPropertiesDialog* ui = nullptr;

    qreal mDefaultApertureIndex = 4;
};

#endif // CAMERAPROPERTIESDIALOG_H
