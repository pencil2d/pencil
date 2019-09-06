#include "historyviewerwidget.h"
#include "editor.h"
#include "backupmanager.h"

HistoryViewerWidget::HistoryViewerWidget( QWidget* parent ) : BaseDockWidget( parent )
{
    setWindowTitle(tr("History", "HistoryViewerWidget window title"));
    undoView = new QUndoView();
    undoView->setAttribute(Qt::WA_QuitOnClose, false);
    undoView->setEmptyLabel("New canvas");
    setWidget(undoView);
}

HistoryViewerWidget::~HistoryViewerWidget()
{
}

void HistoryViewerWidget::initUI()
{
    BackupManager* manager = editor()->backups();
    undoView->setStack(manager->undoStack());
}

void HistoryViewerWidget::updateUI()
{

}
