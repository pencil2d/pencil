#ifndef BACKUPCONTENTVIEWER_H
#define BACKUPCONTENTVIEWER_H

#include <QUndoView>
#include "basedockwidget.h"

class HistoryViewerWidget : public BaseDockWidget
{
public:
    explicit HistoryViewerWidget( QWidget* parent );
    virtual ~HistoryViewerWidget();

    QUndoView* undoView;

    void initUI() override;
    void updateUI() override;

};

#endif // BACKUPCONTENTVIEWER_H
