#ifndef XSHEET_H
#define XSHEET_H

#include <QDialog>

namespace Ui {
class Xsheet;
}

class Xsheet : public QDialog
{
    Q_OBJECT

public:
    explicit Xsheet(QWidget *parent = nullptr);
    ~Xsheet();

private:
    Ui::Xsheet *ui;
};

#endif // XSHEET_H
