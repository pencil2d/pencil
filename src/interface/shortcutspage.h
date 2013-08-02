#ifndef SHORTCUTSPAGE_H
#define SHORTCUTSPAGE_H

#include <QWidget>
#include <QModelIndex>
#include <QKeySequence>
#include <QSettings>

class QTreeView;
class QStandardItem;
class QStandardItemModel;
class QLabel;
class QLineEdit;


namespace Ui
{
    class ShortcutsPage;
}

class ShortcutsPage : public QWidget
{
    Q_OBJECT
public:
    explicit ShortcutsPage(QWidget* parent = 0);

signals:

private slots:
    void tableItemClicked(const QModelIndex&);
    void keyCapLineEditTextChanged(QKeySequence);
    void restoreShortcutsButtonClicked();
    void clearButtonClicked();

private:
    bool isKeySequenceExist(const QSettings&, QString, QKeySequence);
    void removeDuplicateKeySequence(QSettings*, QKeySequence);
    void treeModelLoadShortcutsSetting();

    QStandardItemModel* m_treeModel;
    QModelIndex m_currentItemIndex;

    Ui::ShortcutsPage* ui;
};

#endif // SHORTCUTSPAGE_H
