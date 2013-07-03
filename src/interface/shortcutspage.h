#ifndef SHORTCUTSPAGE_H
#define SHORTCUTSPAGE_H

#include <QWidget>
#include <QModelIndex>

class QStandardItem;
class QStandardItemModel;
class QDialog;


class ShortcutsPage : public QWidget
{
    Q_OBJECT
public:
    explicit ShortcutsPage(QWidget* parent = 0);
    
signals:
    
public slots:
    void tableItemChangs(QStandardItem*);
    void tableItemDoubleClicked(const QModelIndex&);

private:
    QStandardItemModel* m_pTableModel;
    QDialog* m_pChooseKeyDialog;
};

#endif // SHORTCUTSPAGE_H
