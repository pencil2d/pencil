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
            CenterOfView,
            CenterOfCanvas,
            CenterOfCamera,
            CenterOfCameraFollowed,
            None
        };

        static Type getTypeFromIndex(int index) {
            switch (index) {
            case 0:
                return CenterOfView;
            case 1:
                return CenterOfCanvas;
            case 2:
                return CenterOfCamera;
            case 3:
                return CenterOfCameraFollowed;
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
