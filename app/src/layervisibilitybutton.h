#ifndef LAYERVISIBILITYBUTTON_H
#define LAYERVISIBILITYBUTTON_H

#include <QObject>
#include <QWidget>

class Layer;
class QIcon;
class QToolButton;
class Editor;
class QPixmap;

enum class LayerVisibilityContext
{
    LOCAL,
    GLOBAL
};

class LayerVisibilityButton : public QWidget
{
    Q_OBJECT
public:
    LayerVisibilityButton(QWidget* parent,
                          const LayerVisibilityContext& visibilityContext,
                          Layer* layer,
                          Editor* editor);

    void paintEvent(QPaintEvent* event) override;

    void paintLocalVisibilityState(QPainter& painter, const QPalette& palette);
    void paintGlobalVisibilityState(QPainter& painter, const QPalette& palette);

    void mousePressEvent(QMouseEvent* event) override;

    QSize sizeHint() const override;

signals:
    void visibilityChanged();

private:

    Layer* mLayer = nullptr;
    Editor* mEditor = nullptr;
    QSize mVisibilityCircleSize = QSize(10,10);

    bool mIconValid = false;

    const LayerVisibilityContext mVisibilityContext;

};

#endif // LAYERVISIBILITYBUTTON_H
