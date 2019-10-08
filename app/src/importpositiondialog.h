#ifndef IMPORTPOSITIONDIALOG_H
#define IMPORTPOSITIONDIALOG_H

#include <QDialog>

namespace Ui {
class ImportPositionDialog;
}

enum class ImportPositionType {
    CenterOfCamera,
    CenterOfCanvas
};

class Editor;

class ImportPositionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImportPositionDialog(QWidget *parent = nullptr);
    ~ImportPositionDialog();

    void setCore(Editor* editor) { mEditor = editor; }

private slots:
    void didChangeComboBoxIndex(const int index);
    void changeImportView();

private:
    Ui::ImportPositionDialog *ui;

    ImportPositionType mImportOption = ImportPositionType::CenterOfCamera;
    Editor* mEditor = nullptr;
};

#endif // IMPORTPOSITIONDIALOG_H
