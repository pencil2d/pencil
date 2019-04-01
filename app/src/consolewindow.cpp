#include "consolewindow.h"
#include "ui_consolewindow.h"

#include <QRegularExpression>

#include "mainwindow2.h"

ConsoleWindow::ConsoleWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConsoleWindow)
{
    ui->setupUi(this);

    // Actions
    connect(ui->prompt, &QLineEdit::returnPressed, this, &ConsoleWindow::runCommand);
    ui->prompt->installEventFilter(this);

    // Styling
    ui->prompt->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->prompt->setFocus();

    mMainWindow = new MainWindow2(this);
}

ConsoleWindow::~ConsoleWindow()
{
    delete ui;
}

bool ConsoleWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->prompt)
    {
        if (event->type() == QEvent::FocusIn)
        {
            ui->promptPrefix->setStyleSheet("border:none;background:rgba(0,255,0,0.25);");
            ui->prompt->setStyleSheet("border:none;background:rgba(0,255,0,0.25);");
        }
        else if (event->type() == QEvent::FocusOut)
        {
            ui->promptPrefix->setStyleSheet("border:none;background:transparent;");
            ui->prompt->setStyleSheet("border:none;background:transparent;");
        }
    }
    return false;
}

void ConsoleWindow::runCommand()
{
    // Get command
    QString command = ui->prompt->text().trimmed();
    // Clean command for robustness
    command = command.toLower().replace(QRegularExpression("[^A-Za-z 0-9\\.]"), "");
    // Echo to output
    ui->console->appendPlainText("> " + command);
    // Clear prompt
    ui->prompt->setText("");

    // Take action!

    // TODO
}
