#ifndef RETIMEDIALOG_H
#define RETIMEDIALOG_H

#include <QDialog>

namespace Ui {
class RetimeDialog;
}

class RetimeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RetimeDialog(QWidget *parent = nullptr);
    ~RetimeDialog();

    void setOrigFps(int origFps);
    int getNewFps();

    qreal getNewSpeed();

private:
    Ui::RetimeDialog *ui;
};

#endif // RETIMEDIALOG_H
