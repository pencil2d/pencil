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
#ifndef OBJECT_H
#define OBJECT_H

#include <QObject>
#include <QList>
#include <QColor>
#include "layer.h"
#include "layerbitmap.h"
#include "layervector.h"
#include "colourref.h"


class Object : public QObject
{
    Q_OBJECT

public slots:
    void modification() { modified = true; }
    void toggleMirror() { mirror = !mirror; }//TODO toggles mirror button
    void resetMirror() { mirror = false; }
    void imageCheck(int);

signals:
    void imageAdded(int);
    void imageAdded(int,int);
    void imageRemoved(int);

public:
    Object();
    virtual ~Object();

    QString strCurrentFilePath;

    QDomElement createDomElement(QDomDocument& doc);
    bool loadDomElement(QDomElement element,  QString filePath);

    bool read(QString filePath);
    bool write(QString filePath);

    QString name;
    bool modified;
    bool mirror; // if true, the returned image is flipped horizontally
    QList<Layer*> layer;
    QList<ColourRef> myPalette;

    //void paintImage(QPainter &painter, int frameNumber, const QRectF &source, const QRectF &target, bool background, qreal curveOpacity, bool antialiasing, bool niceGradients);
    void paintImage(QPainter& painter, int frameNumber, bool background, qreal curveOpacity, bool antialiasing, int gradients);

    ColourRef getColour(int i);
    void setColour(int index, QColor newColour) { myPalette[index].colour = newColour; }
    void addColour(QColor);
    void addColour(ColourRef newColour) { myPalette.append(newColour); }
    bool removeColour(int index);
    void renameColour(int i, QString text);
    int getColourCount() { return myPalette.size();}
    bool importPalette(QString filePath);
    bool exportPalette(QString filePath);
    bool savePalette(QString filePath);
    bool loadPalette(QString filePath);
    void loadDefaultPalette();


    void addNewBitmapLayer();
    void addNewVectorLayer();
    void addNewSoundLayer();
    void addNewCameraLayer();
    Layer* getLayer(int i);
    int getLayerCount() { return layer.size(); }
    int getMaxID();
    void moveLayer(int i, int j);
    void deleteLayer(int i);

    void playSoundIfAny(int frame,int fps);
    void stopSoundIfAny();

    void defaultInitialisation();

    void exportFrames(int frameStart, int frameEnd, QMatrix view, Layer* currentLayer, QSize exportSize, QString filePath, const char* format, int quality, bool background, bool antialiasing, int gradients, QProgressDialog* progress, int progressMax);
    void exportFrames1(int frameStart, int frameEnd, QMatrix view, Layer* currentLayer, QSize exportSize, QString filePath, const char* format, int quality, bool background, bool antialiasing, int gradients, QProgressDialog* progress, int progressMax, int fps, int exportFps);
    void exportMovie(int startFrame, int endFrame, QMatrix view, Layer* currentLayer, QSize exportSize, QString filePath, int fps, int exportFps, QString exportFormat);
    void exportX(int frameStart, int frameEnd, QMatrix view, QSize exportSize, QString filePath,  bool antialiasing, int gradients);
    void exportIm(int frameStart, int frameEnd, QMatrix view, QSize exportSize, QString filePath,  bool antialiasing, int gradients);
    void exportFlash(int startFrame, int endFrame, QMatrix view, QSize exportSize, QString filePath, int fps, int compression);

};

#endif

