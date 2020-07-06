#ifndef ADDTRANSPARENCYTOPAPERDIALOG_H
#define ADDTRANSPARENCYTOPAPERDIALOG_H

#include <QWidget>

class Editor;

namespace Ui {
class AddTransparencyToPaperDialog;
}

class AddTransparencyToPaperDialog : public QWidget
{
    Q_OBJECT

public:
    explicit AddTransparencyToPaperDialog(QWidget *parent = nullptr);
    ~AddTransparencyToPaperDialog();

    void setCore(Editor* editor);

public slots:
    void updateTraceButton();

signals:
    void closeDialog();

private:
    Ui::AddTransparencyToPaperDialog *ui;

    void traceScannedDrawings();
    void closeClicked();

    Editor* mEditor = nullptr;
};

#endif // ADDTRANSPARENCYTOPAPERDIALOG_H
