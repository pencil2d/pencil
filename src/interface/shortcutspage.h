#ifndef SHORTCUTSPAGE_H
#define SHORTCUTSPAGE_H

#include <QWidget>


class QStandardItem;


class ShortcutsPage : public QWidget
{
    Q_OBJECT
public:
    explicit ShortcutsPage(QWidget *parent = 0);
    
signals:
    
public slots:
    void tableItemChangs(QStandardItem*);
    
};

#endif // SHORTCUTSPAGE_H
