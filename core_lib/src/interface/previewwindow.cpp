#include "previewwindow.h"
#include "ui_previewwindow.h"

#include "layermanager.h"

PreviewWindow::PreviewWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PreviewWindow)
{
    ui->setupUi(this);

    connect(ui->btn5back, &QPushButton::clicked, this, &PreviewWindow::fiveBackwards);
    connect(ui->btn1back, &QPushButton::clicked, this, &PreviewWindow::oneBackwards);
    connect(ui->btn1fwd, &QPushButton::clicked, this, &PreviewWindow::oneForwards);
    connect(ui->btn5fwd, &QPushButton::clicked, this, &PreviewWindow::fiveForwards);
    connect(ui->btnClose, &QPushButton::clicked, this, &PreviewWindow::exitWindow);
}

PreviewWindow::~PreviewWindow()
{
    delete ui;
}

void PreviewWindow::setCore(Editor *editor)
{
    mEditor = editor;
}

void PreviewWindow::fiveBackwards()
{
    if (mEditor->currentFrame() > 5)
        mEditor->scrubTo(mEditor->currentFrame() - 5);
    else
        mEditor->scrubTo(1);
}

void PreviewWindow::oneBackwards()
{
    if (mEditor->currentFrame() > 1)
        mEditor->scrubBackward();
}

void PreviewWindow::oneForwards()
{
    mEditor->scrubForward();
}

void PreviewWindow::fiveForwards()
{
    mEditor->scrubTo(mEditor->currentFrame() + 5);
}

void PreviewWindow::exitWindow()
{
    mEditor->layers()->setIsPreview(false);
    close();
}
