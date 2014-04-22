#include "basedockwidget.h"

BaseDockWidget::BaseDockWidget(QWidget *pParent) 
: QDockWidget(pParent)
, m_pCore( nullptr )
{
}

BaseDockWidget::BaseDockWidget( QWidget* pParent, Qt::WindowFlags flags )
: QDockWidget( pParent, flags )
, m_pCore( nullptr )
{
}

BaseDockWidget::~BaseDockWidget()
{
}
