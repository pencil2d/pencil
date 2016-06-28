#include "exportmoviedialog.h"
#include "ui_exportmoviedialog.h"

ExportMovieDialog::ExportMovieDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportMovieDialog)
{
    ui->setupUi(this);
}

ExportMovieDialog::~ExportMovieDialog()
{
    delete ui;
}
