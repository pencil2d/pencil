/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#include "titlebarwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QIcon>
#include <QToolButton>
#include <QResizeEvent>
#include <QStyle>
#include <QPainter>
#include <QDebug>

#include "platformhandler.h"
#include "buttonappearancewatcher.h"

TitleBarWidget::TitleBarWidget(QWidget* parent)
    : QWidget(parent)
{

    QHBoxLayout* vLayout = new QHBoxLayout();

    vLayout->setContentsMargins(2,0,2,0);
    vLayout->setSpacing(0);

    QWidget* contentWidget = createCustomTitleBarWidget(this);
    vLayout->addWidget(contentWidget);

    contentWidget->setMinimumHeight(28);

    setLayout(vLayout);
}

TitleBarWidget::~TitleBarWidget()
{
}

QWidget* TitleBarWidget::createCustomTitleBarWidget(QWidget* parent)
{
    bool isDarkmode = PlatformHandler::isDarkMode();
    QWidget* containerWidget = new QWidget(parent);

    mContainerLayout = new QHBoxLayout(parent);

    mCloseButton = new QToolButton(parent);

    mCloseButton->setStyleSheet(flatButtonStylesheet());

    QSize iconSize = QSize(14,14);
    QSize padding = QSize(2,2);

    IconResource closeButtonRes;
    closeButtonRes.lightMode = QIcon("://icons/themes/playful/window/window-close-button-normal.svg");
    closeButtonRes.darkMode = QIcon("://icons/themes/playful/window/window-close-button-normal-darkm.svg");

    QIcon closeIcon = closeButtonRes.iconForMode(isDarkmode);

    IconResource closeHoverButtonRes;
    closeHoverButtonRes.lightMode = QIcon("://icons/themes/playful/window/window-close-button-active.svg");
    closeHoverButtonRes.darkMode = closeHoverButtonRes.lightMode;

    mCloseButton->setIcon(closeIcon);
    mCloseButton->setIconSize(iconSize);
    mCloseButton->setFixedSize(iconSize + padding);
    mCloseButton->installEventFilter(new ButtonAppearanceWatcher(closeButtonRes,
                                                                 closeHoverButtonRes,
                                                            this));

    connect(mCloseButton, &QToolButton::clicked, this, &TitleBarWidget::closeButtonPressed);

    IconResource dockButtonRes;
    dockButtonRes.lightMode = QIcon("://icons/themes/playful/window/window-float-button-normal.svg");
    dockButtonRes.darkMode = QIcon("://icons/themes/playful/window/window-float-button-normal-darkm.svg");

    IconResource dockHoverButtonRes;
    dockHoverButtonRes.lightMode = QIcon("://icons/themes/playful/window/window-float-button-active.svg");
    dockHoverButtonRes.darkMode = dockHoverButtonRes.lightMode;

    mDockButton = new QToolButton(parent);

    QIcon dockIcon = dockButtonRes.iconForMode(isDarkmode);
    mDockButton->setIcon(dockIcon);
    mDockButton->setStyleSheet(flatButtonStylesheet());

    mDockButton->setIconSize(iconSize);
    mDockButton->setFixedSize(iconSize + padding);
    mDockButton->installEventFilter(new ButtonAppearanceWatcher(dockButtonRes, dockHoverButtonRes, this));

    connect(mDockButton, &QToolButton::clicked, this, &TitleBarWidget::undockButtonPressed);

    mTitleLabel = new QLabel(parent);
    mTitleLabel->setAlignment(Qt::AlignVCenter);

#ifdef __APPLE__
    mContainerLayout->addWidget(mCloseButton);
    mContainerLayout->addWidget(mDockButton);
    mContainerLayout->addWidget(mTitleLabel);
#else
    mContainerLayout->addWidget(mTitleLabel);
    mContainerLayout->addWidget(mDockButton);
    mContainerLayout->addWidget(mCloseButton);
#endif

    mContainerLayout->setSpacing(3);
    mContainerLayout->setContentsMargins(0,0,0,0);

    containerWidget->setLayout(mContainerLayout);
    containerWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    containerWidget->setMinimumSize(QSize(1,1));

    return containerWidget;
}

void TitleBarWidget::setChildWidget(QWidget* widget)
{
    if (mHasChildWidget) {
        mContainerLayout->removeWidget(widget);
    }
    mContainerLayout->addWidget(widget);
    mHasChildWidget = true;
}

QString TitleBarWidget::flatButtonStylesheet() const
{
    return "QToolButton { border: 0; }";
}

void TitleBarWidget::setTitle(const QString &title)
{
    mTitleLabel->setText(title);
}

void TitleBarWidget::lock(bool locked)
{
    if (locked) {
        hideButtons(true);
    } else {
        hideButtonsIfNeeded(this->width());
    }
    mIsLocked = locked;
}

void TitleBarWidget::hideButtons(bool hide)
{
    mCloseButton->setHidden(hide);
    mDockButton->setHidden(hide);
}

void TitleBarWidget::resizeEvent(QResizeEvent *resizeEvent)
{
    QWidget::resizeEvent(resizeEvent);

    hideButtonsIfNeeded(resizeEvent->size().width());
}

void TitleBarWidget::hideButtonsIfNeeded(int width)
{
    if (width <= mWidthOfFullLayout) {
        hideButtons(true);
    } else if (!mIsLocked) {
        hideButtons(false);
    }
}

void TitleBarWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    // This is to ensure that after the titlebar has been hidden with buttons hidden
    // the layout width is smaller, so we enable them again briefly to get the correct width.
    hideButtons(false);

    mWidthOfFullLayout = layout()->sizeHint().width();
    hideButtonsIfNeeded(size().width());
}

void TitleBarWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.save();
    painter.setBrush(palette().color(QPalette::Midlight));
    painter.setPen(Qt::NoPen);
    painter.drawRect(this->rect());

    QPen pen(palette().color(QPalette::Mid));
    int penWidth = 1;
    pen.setWidth(penWidth);
    painter.setPen(pen);
    painter.drawLine(QPoint(this->rect().x(),
                            this->rect().height()-penWidth),
                     QPoint(this->rect().width(),
                            this->rect().height()-penWidth));
    painter.restore();
}
