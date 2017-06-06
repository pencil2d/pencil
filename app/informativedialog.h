#ifndef INFORMATIVEDIALOG_H
#define INFORMATIVEDIALOG_H

#include <QDialog>

namespace Ui
{
class InformativeDialog;
}

class InformativeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InformativeDialog(QString title, QString description = QString(), QWidget *parent = 0 );
    ~InformativeDialog();

private:
    Ui::InformativeDialog *ui;
};

#endif // INFORMATIVEDIALOG_H
