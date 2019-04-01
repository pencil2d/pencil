#ifndef CONSOLEWINDOW_H
#define CONSOLEWINDOW_H

#include <QMainWindow>

namespace Ui {
class ConsoleWindow;
}

class ConsoleWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ConsoleWindow(QWidget *parent = nullptr);
    ~ConsoleWindow() override;

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private slots:
    void runCommand();

private:
    Ui::ConsoleWindow *ui;
};

#endif // CONSOLEWINDOW_H
