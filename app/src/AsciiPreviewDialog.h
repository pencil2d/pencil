#ifndef ASCIIPREVIEWDIALOG_H
#define ASCIIPREVIEWDIALOG_H

#include <QDialog>

namespace Ui {
class AsciiPreviewDialog;
}

class AsciiPreviewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AsciiPreviewDialog(QWidget *parent = nullptr);
    ~AsciiPreviewDialog();

private:
    Ui::AsciiPreviewDialog *ui;
};

#endif // ASCIIPREVIEWDIALOG_H
