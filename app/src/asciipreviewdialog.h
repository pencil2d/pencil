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

    void setText(QString s);
    void setPageNumber(int n);

private:
    Ui::AsciiPreviewDialog *ui;
    QFontMetrics mMetrics;
};

#endif // ASCIIPREVIEWDIALOG_H
