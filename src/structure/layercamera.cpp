/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation;

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "layercamera.h"
#include <QtDebug>

Camera::Camera()
{
    view = QMatrix();
}

Camera::~Camera()
{
}

// ------

CameraPropertiesDialog::CameraPropertiesDialog(QString name, int width, int height) : QDialog()
{

    QLabel* nameLabel = new QLabel(tr("Camera name:"));
    nameBox = new QLineEdit();
    nameBox->setText(name);
    QHBoxLayout* nameLayout = new QHBoxLayout();
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameBox);

    QLabel* sizeLabel = new QLabel(tr("Camera size:"));
    widthBox = new QSpinBox();
    heightBox = new QSpinBox();
    widthBox->setValue(width);
    heightBox->setValue(height);
    widthBox->setMaximum(10000);
    heightBox->setMaximum(10000);
    widthBox->setMinimum(1);
    heightBox->setMinimum(1);
    QHBoxLayout* sizeLayout = new QHBoxLayout();
    sizeLayout->addWidget(sizeLabel);
    sizeLayout->addWidget(widthBox);
    sizeLayout->addWidget(heightBox);

    QPushButton* okButton = new QPushButton(tr("Ok"));
    QPushButton* cancelButton = new QPushButton(tr("Cancel"));
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    QGridLayout* layout = new QGridLayout();
    layout->addLayout(nameLayout, 0, 0);
    layout->addLayout(sizeLayout, 1, 0);
    layout->addLayout(buttonLayout, 2, 0);
    setLayout(layout);
    connect(okButton, SIGNAL(pressed()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(pressed()), this, SLOT(reject()));
}

CameraPropertiesDialog::~CameraPropertiesDialog()
{
}

QString CameraPropertiesDialog::getName()
{
    return nameBox->text();
}

void CameraPropertiesDialog::setName(QString name)
{
    nameBox->setText(name);
}

int CameraPropertiesDialog::getWidth()
{
    return widthBox->value();
}

void CameraPropertiesDialog::setWidth(int width)
{
    widthBox->setValue(width);
}

int CameraPropertiesDialog::getHeight()
{
    return heightBox->value();
}

void CameraPropertiesDialog::setHeight(int height)
{
    heightBox->setValue(height);
}

// ------

LayerCamera::LayerCamera(Object* object) : LayerImage(object)
{
    type = Layer::CAMERA;
    name = QString("Camera Layer");
    viewRect = QRect( QPoint(-320,-240), QSize(640,480) );
    dialog = NULL;
    addImageAtFrame(1);
}

LayerCamera::~LayerCamera()
{
    while (!framesCamera.empty())
        delete framesCamera.takeFirst();
}

// ------

Camera* LayerCamera::getCameraAtIndex(int index)
{
    if ( index < 0 || index >= framesCamera.size() )
    {
        return NULL;
    }
    else
    {
        return framesCamera.at(index);
    }
}

Camera* LayerCamera::getCameraAtFrame(int frameNumber)
{
    int index = getIndexAtFrame(frameNumber);
    return getCameraAtIndex(index);
}

Camera* LayerCamera::getLastCameraAtFrame(int frameNumber, int increment)
{
    int index = getLastIndexAtFrame(frameNumber);
    return getCameraAtIndex(index + increment);
}

QMatrix LayerCamera::getViewAtFrame(int frameNumber)
{
    //qDebug() << ">> " << frameNumber;
    int index = getLastIndexAtFrame(frameNumber);
    int frame1 = -1;
    int frame2 = -1;
    Camera* camera1 = getCameraAtIndex(index);
    if (camera1) frame1 = framesPosition.at(index);
    Camera* camera2 = getCameraAtIndex(index+1);
    if (camera2) frame2 = framesPosition.at(index+1);
    if (camera1 == NULL && camera2 == NULL)
    {
        return QMatrix();
    }
    if (camera1 == NULL && camera2 != NULL)
    {
        return camera2->view;
    }
    if (camera2 == NULL && camera1 != NULL)
    {
        return camera1->view;
    }
    if (camera1 != NULL && camera2 != NULL)
    {
        // linear interpolation
        qreal c2 = (frameNumber-frame1+0.0)/(frame2-frame1);
        qreal c1 = 1.0 - c2;
        //qDebug() << ">> -- " << c1 << c2;
        return QMatrix( c1*camera1->view.m11() + c2*camera2->view.m11(),
                        c1*camera1->view.m12() + c2*camera2->view.m12(),
                        c1*camera1->view.m21() + c2*camera2->view.m21(),
                        c1*camera1->view.m22() + c2*camera2->view.m22(),
                        c1*camera1->view.dx() + c2*camera2->view.dx(),
                        c1*camera1->view.dy() + c2*camera2->view.dy()	);
    }
}

QRect LayerCamera::getViewRect()
{
    return viewRect;
}

// -----

QImage* LayerCamera::getImageAtIndex(int index)
{
    /*if ( index < 0 || index >= framesImage.size() ) {
    	return NULL;
    } else {
    	return framesImage.at(index);
    }*/
    return NULL;
}

bool LayerCamera::addImageAtFrame(int frameNumber)
{
    int index = getIndexAtFrame(frameNumber);
    if (index == -1)
    {
        //framesImage.append(new QImage(imageSize, QImage::Format_ARGB32_Premultiplied));
        Camera* camera = new Camera();
        camera->view = getViewAtFrame(frameNumber);
        framesCamera.append(camera);
        framesPosition.append(frameNumber);
        framesSelected.append(false);
        framesFilename.append("");
        framesModified.append(false);
        bubbleSort();
        int frameNumber1 = frameNumber;
        int frameNumber2 = frameNumber;
        if (index>0) frameNumber1 = framesPosition.at(index-1);
        if (index<framesPosition.size()-1) frameNumber1 = framesPosition.at(index+1);
        emit imageAdded(frameNumber1, frameNumber2);
        return true;
    }
    else
    {
        return false;
    }
}

void LayerCamera::removeImageAtFrame(int frameNumber)
{
    int index = getIndexAtFrame(frameNumber);
    if (index != -1  && framesPosition.size() != 1)
    {
        delete framesCamera.at(index);
        framesCamera.removeAt(index);
        framesPosition.removeAt(index);
        framesSelected.removeAt(index);
        framesFilename.removeAt(index);
        framesModified.removeAt(index);
        bubbleSort();
        emit imageRemoved(frameNumber);
    }
}

void LayerCamera::loadImageAtFrame(int frameNumber, QMatrix view)
{
    if (getIndexAtFrame(frameNumber) == -1) addImageAtFrame(frameNumber);
    int index = getIndexAtFrame(frameNumber);
    framesCamera[index] = new Camera();
    framesCamera[index]->view = view;
    emit imageAdded(frameNumber);
}

void LayerCamera::swap(int i, int j)
{
    LayerImage::swap(i, j);
    framesCamera.swap(i,j);
}

bool LayerCamera::saveImage(int index, QString path, int layerNumber)
{
    QString layerNumberString = QString::number(layerNumber);
    QString frameNumberString = QString::number(framesPosition.at(index));
    while ( layerNumberString.length() < 3) layerNumberString.prepend("0");
    while ( frameNumberString.length() < 3) frameNumberString.prepend("0");
    //framesFilename[index] = path+"/"+layerNumberString+"."+frameNumberString+".png";
    framesFilename[index] = layerNumberString+"."+frameNumberString+".png";
    //qDebug() << "Write " << framesFilename.at(index);

    //framesCamera[index]->image->save(path +"/"+ framesFilename.at(index),"PNG");
    framesModified[index] = false;

    return true;
}


void LayerCamera::editProperties()
{
    bool ok;
    if (dialog == NULL) dialog = new CameraPropertiesDialog(name, viewRect.width(), viewRect.height());
    dialog->setName(name);
    dialog->setWidth(viewRect.width());
    dialog->setHeight(viewRect.height());
    int result = dialog->exec();
    if (result == QDialog::Accepted)
    {
        name = dialog->getName();
        viewRect = QRect(-dialog->getWidth()/2, -dialog->getHeight()/2, dialog->getWidth(), dialog->getHeight());
    }
}

QDomElement LayerCamera::createDomElement(QDomDocument& doc)
{
    QDomElement layerTag = doc.createElement("layer");
    layerTag.setAttribute("name", name);
    layerTag.setAttribute("visibility", visible);
    layerTag.setAttribute("type", type);
    layerTag.setAttribute("width", viewRect.width());
    layerTag.setAttribute("height", viewRect.height());
    for(int index=0; index < framesPosition.size() ; index++)
    {
        QDomElement keyTag = doc.createElement("camera");
        keyTag.setAttribute("frame", framesPosition.at(index));

        keyTag.setAttribute("m11", framesCamera[index]->view.m11());
        keyTag.setAttribute("m12", framesCamera[index]->view.m12());
        keyTag.setAttribute("m21", framesCamera[index]->view.m21());
        keyTag.setAttribute("m22", framesCamera[index]->view.m22());
        keyTag.setAttribute("dx", framesCamera[index]->view.dx());
        keyTag.setAttribute("dy", framesCamera[index]->view.dy());
        layerTag.appendChild(keyTag);
    }
    return layerTag;
}

void LayerCamera::loadDomElement(QDomElement element, QString filePath)
{
    name = element.attribute("name");
    //visible = (element.attribute("visibility") == "1");
    visible = true;
    type = element.attribute("type").toInt();

    int width = element.attribute("width").toInt();
    int height = element.attribute("height").toInt();
    viewRect = QRect(-width/2,-height/2,width,height);

    QDomNode imageTag = element.firstChild();
    while (!imageTag.isNull())
    {
        QDomElement imageElement = imageTag.toElement();
        if (!imageElement.isNull())
        {
            if (imageElement.tagName() == "camera")
            {
                int frame = imageElement.attribute("frame").toInt();

                qreal m11 = imageElement.attribute("m11").toDouble();
                qreal m12 = imageElement.attribute("m12").toDouble();
                qreal m21 = imageElement.attribute("m21").toDouble();
                qreal m22 = imageElement.attribute("m22").toDouble();
                qreal dx = imageElement.attribute("dx").toDouble();
                qreal dy = imageElement.attribute("dy").toDouble();

                loadImageAtFrame(frame, QMatrix(m11,m12,m21,m22,dx,dy) );
            }
            /*if (imageElement.tagName() == "image") {
            	int frame = imageElement.attribute("frame").toInt();
            	addImageAtFrame( frame );
            	getBitmapImageAtFrame( frame )->loadDomElement(imageElement, filePath);
            }*/
        }
        imageTag = imageTag.nextSibling();
    }
}
