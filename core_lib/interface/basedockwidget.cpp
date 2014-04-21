#include "basedockwidget.h"

BaseDockWidget::BaseDockWidget(QWidget *pParent) 
    : QDockWidget(pParent)
    , m_pCore( nullptr )
{
}

BaseDockWidget::~BaseDockWidget()
{
}
