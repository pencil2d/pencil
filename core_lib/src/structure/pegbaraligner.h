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
    PegStatus(ErrorCode code, QPoint point = {});
    QPoint point;
};

class PegBarAligner
{
    Q_DECLARE_TR_FUNCTIONS(PegBarAligner)
public:
    PegBarAligner(Editor* editor);

    Status align(const QStringList& layers);

private:
    PegStatus findPoint(const BitmapImage& image) const;

    Editor* mEditor = nullptr;

    const int mGrayThreshold = 121;
    QRect mPegSearchRect;
};

#endif // PEGBARALIGNER_H
