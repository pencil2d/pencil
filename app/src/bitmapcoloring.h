#ifndef BITMAPCOLORING_H
#define BITMAPCOLORING_H

#include <QWidget>
#include "basedockwidget.h"

class BitmapColoring : public BaseDockWidget
{
    Q_OBJECT
public:
    explicit BitmapColoring(QWidget *parent = nullptr);

    void initUI() override;
    void updateUI() override;

signals:

public slots:
};

#endif // BITMAPCOLORING_H
