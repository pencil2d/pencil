#include "pegbaraligner.h"

#include <QDebug>
#include <editor.h>
#include <pencilerror.h>

#include <bitmapimage.h>
#include <layerbitmap.h>
#include <layermanager.h>
#include <selectionmanager.h>

PegStatus::PegStatus(ErrorCode code)
    : Status(code)
{
}

PegbarAligner::PegbarAligner(Editor* editor) :
    mEditor(editor),
    mGrayThreshold(121)
{
    mPegSearchRect = mEditor->select()->mySelectionRect().toAlignedRect();
}


Status PegbarAligner::align(const QStringList layers)
{
    PegStatus result = Status::OK;

    LayerBitmap* layerbitmap = static_cast<LayerBitmap*>(mEditor->layers()->currentLayer());
    BitmapImage* img = layerbitmap->getBitmapImageAtFrame(mEditor->currentFrame());
    result = findPoint(*img);

    if (!result.ok())
    {
        return Status(Status::FAIL, "", QObject::tr("Peg hole not found!\nCheck selection, and please try again.", "PegBar error message"));
    }

    const int pegX = result.point.x();
    const int pegY = result.point.y();

    for (int i = 0; i < layers.count(); i++)
    {
        layerbitmap = static_cast<LayerBitmap*>(mEditor->layers()->findLayerByName(layers.at(i)));
        for (int k = layerbitmap->firstKeyFramePosition(); k <= layerbitmap->getMaxKeyFramePosition(); k++)
        {
            if (!layerbitmap->keyExists(k)) { continue; }

            img = layerbitmap->getBitmapImageAtFrame(k);
            img->enableAutoCrop(false);

            result = findPoint(*img);
            if (!result.ok())
            {
                const QString errorDescription = QObject::tr("Peg bar not found at %1, %2").arg(layerbitmap->name()).arg(k);
                return Status(result.code(), "", errorDescription);
            }
            img->moveTopLeft(QPoint(img->left() + (pegX - result.point.x()), img->top() + (pegY - result.point.y())));

            mEditor->frameModified(img->pos());
        }
    }

    mEditor->deselectAll();

    return Status::OK;
}


PegStatus PegbarAligner::findPoint(const BitmapImage& image) const
{
    PegStatus result = Status::FAIL;
    const int left = mPegSearchRect.left();
    const int right = mPegSearchRect.right();
    const int top = mPegSearchRect.top();
    const int bottom = mPegSearchRect.bottom();
    const int grayValue = mGrayThreshold;

    bool foundX = false;

    for (int x = left; x <= right; x++)
    {
        for (int y = top; y <= bottom; y++)
        {
            const QRgb& scan = image.constScanLine(x,y);
            if (qAlpha(scan) == 255 && qGray(scan) < grayValue)
            {
                foundX = true;
                result.point.setX(x);

                break;
            }
        }
        if (foundX) { break; }
    }

    bool foundY = false;
    for (int y = top; y <= bottom; y++)
    {
        for (int x = left; x <= right; x++)
        {
            const QRgb& scan = image.constScanLine(x,y);
            if (qAlpha(scan) == 255 && qGray(scan) < grayValue)
            {
                foundY = true;
                result.point.setY(y);

                break;
            }
        }
        if (foundY) { break; }
    }

    if (foundX && foundY) {
        result.updateStatus(Status::OK);
    }
    return result;
}
