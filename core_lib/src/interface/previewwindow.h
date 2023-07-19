#ifndef PREVIEWWINDOW_H
#define PREVIEWWINDOW_H

#include <QWidget>
#include <editor.h>

class Editor;

namespace Ui {
class PreviewWindow;
}

class PreviewWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PreviewWindow(QWidget *parent = nullptr);
    ~PreviewWindow();

    void setCore(Editor* editor);

private:
    Ui::PreviewWindow *ui;

    void fiveBackwards();
    void oneBackwards();
    void oneForwards();
    void fiveForwards();
    void exitWindow();

    Editor* mEditor = nullptr;
};

#endif // PREVIEWWINDOW_H
