#ifndef EXPORTMOVIEDIALOG_H
#define EXPORTMOVIEDIALOG_H

#include <QDialog>

namespace Ui {
class ExportMovieDialog;
}

class ExportMovieDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportMovieDialog(QWidget *parent = 0);
    ~ExportMovieDialog();

private:
    Ui::ExportMovieDialog *ui;
};

#endif // EXPORTMOVIEDIALOG_H
