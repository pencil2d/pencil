#include "pegbaraligner.h"

#include <QDebug>
#include <editor.h>
#include <pencilerror.h>

#include <bitmapimage.h>
#include <layerbitmap.h>
#include <layermanager.h>
#include <selectionmanager.h>

PegStatus::PegStatus(ErrorCode code, QPoint point)
    : Status(code), point(point)
{
}

PegBarAligner::PegBarAligner(Editor* editor) :
    mEditor(editor)
{
    mPegSearchRect = mEditor->select()->mySelectionRect().toAlignedRect();
}


Status PegBarAligner::align(const QStringList& layers)
{
    LayerBitmap* layerBitmap = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    BitmapImage* img = layerBitmap->getBitmapImageAtFrame(mEditor->currentFrame());
    PegStatus result = findPoint(*img);

    if (!result.ok())
    {
        return Status(Status::FAIL, "", tr("Peg hole not found!\nCheck selection, and please try again.", "PegBar error message"));
    }

    const int pegX = result.point.x();
    const int pegY = result.point.y();

    for (int i = 0; i < layers.count(); i++)
    {
        layerBitmap = static_cast<LayerBitmap*>(mEditor->layers()->findLayerByName(layers.at(i)));
        for (int k = layerBitmap->firstKeyFramePosition(); k <= layerBitmap->getMaxKeyFramePosition(); k++)
        {
            if (!layerBitmap->keyExists(k)) { continue; }

            img = layerBitmap->getBitmapImageAtFrame(k);
            img->enableAutoCrop(false);

            result = findPoint(*img);
            if (!result.ok())
            {
                const QString errorDescription = tr("Peg bar not found at %1, %2").arg(layerBitmap->name()).arg(k);
                return Status(result.code(), "", errorDescription);
            }
            img->moveTopLeft(QPoint(img->left() + (pegX - result.point.x()), img->top() + (pegY - result.point.y())));

            mEditor->frameModified(img->pos());
        }
    }

    mEditor->deselectAll();

    return Status::OK;
}


PegStatus PegBarAligner::findPoint(const BitmapImage& image) const
{
    QPoint point;
    const int left = mPegSearchRect.left();
    const int right = mPegSearchRect.right();
    const int top = mPegSearchRect.top();
    const int bottom = mPegSearchRect.bottom();
    const int grayValue = mGrayThreshold;

    bool foundX = false;

    for (int x = left; x <= right && !foundX; x++)
    {
        for (int y = top; y <= bottom; y++)
        {
            const QRgb& scan = image.constScanLine(x,y);
            if (qAlpha(scan) == 255 && qGray(scan) < grayValue)
            {
                foundX = true;
                point.setX(x);

                break;
            }
        }
    }

    bool foundY = false;
    for (int y = top; y <= bottom && !foundY; y++)
    {
        for (int x = left; x <= right; x++)
        {
            const QRgb& scan = image.constScanLine(x,y);
            if (qAlpha(scan) == 255 && qGray(scan) < grayValue)
            {
                foundY = true;
                point.setY(y);

                break;
            }
        }
    }

    if (foundX && foundY) {
        return PegStatus(Status::OK, point);
    }
    return Status::FAIL;
}
