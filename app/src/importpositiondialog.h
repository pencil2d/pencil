#ifndef IMPORTPOSITIONDIALOG_H
#define IMPORTPOSITIONDIALOG_H

#include <QDialog>

namespace Ui {
class ImportPositionDialog;
}

class Editor;

class ImportPositionDialog : public QDialog
{
    Q_OBJECT

    struct ImportPosition {

        enum Type {
            CenterOfCamera,
            CenterOfCanvas,
            None
        };

        static Type getTypeFromIndex(int index) {
            switch (index) {
                case 0:
                    return CenterOfCanvas;
                case 1:
                    return CenterOfCamera;
                default:
                    return None;
            }
        }
    };

public:
    explicit ImportPositionDialog(QWidget *parent = nullptr);
    ~ImportPositionDialog();

    void setCore(Editor* editor) { mEditor = editor; }

private slots:
    void didChangeComboBoxIndex(const int index);
    void changeImportView();

private:
    Ui::ImportPositionDialog *ui;

    ImportPosition::Type mImportOption;
    Editor* mEditor = nullptr;
};

#endif // IMPORTPOSITIONDIALOG_H
