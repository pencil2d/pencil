#ifndef SHORTCUTSPAGE_H
#define SHORTCUTSPAGE_H

#include <QWidget>
#include <QModelIndex>

class QTreeView;
class QStandardItem;
class QStandardItemModel;


class ShortcutsPage : public QWidget
{
    Q_OBJECT
public:
    explicit ShortcutsPage(QWidget* parent = 0);

signals:

public slots:
    void tableItemChangs(QStandardItem*);
    void tableItemClicked(const QModelIndex&);

protected:
    void keyPressEvent(QKeyEvent*);    
    bool eventFilter(QObject*, QEvent*);
private:
    QStandardItemModel* m_treeModel;
    QTreeView* m_treeView;
    QStandardItem* m_pCurrentEditItem;
};

#endif // SHORTCUTSPAGE_H
