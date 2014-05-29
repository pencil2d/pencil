#include "basedockwidget.h"

BaseDockWidget::BaseDockWidget(QWidget *pParent) 
: QDockWidget(pParent)
{
}

BaseDockWidget::BaseDockWidget( QWidget* pParent, Qt::WindowFlags flags )
: QDockWidget( pParent, flags )
{
}

BaseDockWidget::BaseDockWidget( QString strTitle, QWidget* pParent, Qt::WindowFlags flags )
: QDockWidget( strTitle, pParent, flags )
{

}

BaseDockWidget::~BaseDockWidget()
{
}
