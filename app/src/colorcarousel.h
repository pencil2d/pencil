#ifndef COLORCAROUSEL_H
#define COLORCAROUSEL_H

#include <QWidget>
#include <QItemSelection>

struct Swatch;
class ColorQueueModel;

namespace Ui {
class ColorCarousel;
}

class ColorCarousel : public QWidget
{
    Q_OBJECT
public:
    explicit ColorCarousel(QWidget* parent);
    ~ColorCarousel() override;

    void addSwatch(Swatch swatch);

    void setLeftButtonShortcut(const QKeySequence& sequence);
    void setRightButtonShortcut(const QKeySequence& sequence);

Q_SIGNALS:
    void recentColorUpdated(const QColor& color);
private:

    void recentColorSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void cycleRecentColorsLeft();
    void cycleRecentColorsRight();

    void recentColorChanged(const QModelIndex &index);
    void updateRecentColors(Swatch& recentColor);
    void updateRecentColorsSelectedState(int index);


    Ui::ColorCarousel* ui = nullptr;
    ColorQueueModel* mColorQueueModel;
};

#endif // COLORCAROUSEL_H
