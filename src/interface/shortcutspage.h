#ifndef SHORTCUTSPAGE_H
#define SHORTCUTSPAGE_H

#include <QWidget>
#include <QModelIndex>

class QStandardItem;


class ShortcutsPage : public QWidget
{
    Q_OBJECT
public:
    explicit ShortcutsPage(QWidget *parent = 0);
    
signals:
    
public slots:
    void tableItemChangs(QStandardItem*);
    void tableItemDoubleClicked(const QModelIndex &);

protected:
    void keyPressEvent ( QKeyEvent * event );
};

#endif // SHORTCUTSPAGE_H
