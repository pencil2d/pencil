#ifndef PEGBARALIGNER_H
#define PEGBARALIGNER_H

#include <pencilerror.h>

#include <QPoint>
#include <QRectF>

class BitmapImage;
class Editor;

class PegStatus : public Status
{
public:
    PegStatus(const ErrorCode code);
    QPoint point;
};

class PegbarAligner
{
public:
    PegbarAligner(Editor* editor);

    Status align(QStringList layers);

private:
    PegStatus findPoint(const BitmapImage& image) const;

    Editor* mEditor = nullptr;

    const int mGrayThreshold;
    QRect mPegSearchRect;
};

#endif // PEGBARALIGNER_H
