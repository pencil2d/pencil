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
#include "importpositiondialog.h"
#include "ui_importpositiondialog.h"

#include <QSettings>
#include "editor.h"
#include "viewmanager.h"
#include "scribblearea.h"

ImportPositionDialog::ImportPositionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportPositionDialog)
{
    ui->setupUi(this);

    ui->cbImagePosition->addItem(tr("Center of current view"));
    ui->cbImagePosition->addItem(tr("Center of canvas (0,0)"));
    ui->cbImagePosition->addItem(tr("Center of camera, current frame"));
    ui->cbImagePosition->addItem(tr("Center of camera, follow camera"));

    connect(ui->cbImagePosition, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &ImportPositionDialog::didChangeComboBoxIndex);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ImportPositionDialog::changeImportView);

    QSettings settings(PENCIL2D, PENCIL2D);
    int value = settings.value(IMPORT_REPOSITION_TYPE).toInt();

    ui->cbImagePosition->setCurrentIndex(value);
    didChangeComboBoxIndex(value);
}

ImportPositionDialog::~ImportPositionDialog()
{
    delete ui;
}

void ImportPositionDialog::didChangeComboBoxIndex(const int index)
{
    mImportOption = ImportPosition::getTypeFromIndex(index);
}

void ImportPositionDialog::changeImportView()
{
    mEditor->view()->setImportFollowsCamera(false);
    QTransform transform;
    if (mImportOption == ImportPosition::Type::CenterOfView)
    {
        QPointF centralPoint = mEditor->getScribbleArea()->getCentralPoint();
        transform = transform.fromTranslate(centralPoint.x(), centralPoint.y());
        mEditor->view()->setImportView(transform);
        QSettings settings(PENCIL2D, PENCIL2D);
        settings.setValue(IMPORT_REPOSITION_TYPE, ui->cbImagePosition->currentIndex());
        return;
    }
    else if (mImportOption == ImportPosition::Type::CenterOfCanvas)
    {
        transform = transform.fromTranslate(0, 0);
        mEditor->view()->setImportView(transform);
        QSettings settings(PENCIL2D, PENCIL2D);
        settings.setValue(IMPORT_REPOSITION_TYPE, ui->cbImagePosition->currentIndex());
        return;
    }
    else if (mImportOption == ImportPosition::Type::CenterOfCamera)
    {
        QRectF cameraRect = mEditor->getScribbleArea()->getCameraRect(); // Must be QRectF for the precision of cameraRect.center()
        transform = transform.fromTranslate(cameraRect.center().x(), cameraRect.center().y());
        mEditor->view()->setImportView(transform);
        QSettings settings(PENCIL2D, PENCIL2D);
        settings.setValue(IMPORT_REPOSITION_TYPE, ui->cbImagePosition->currentIndex());
        return;
    }

    mEditor->view()->setImportFollowsCamera(true);
    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(IMPORT_REPOSITION_TYPE, ui->cbImagePosition->currentIndex());
}
