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
class PreferenceManager;

namespace Ui
{
    class ShortcutsPage;
}

class ShortcutsPage : public QWidget
{
    Q_OBJECT
public:
    explicit ShortcutsPage(QWidget* parent = 0);

    void setManager( PreferenceManager* p ) { mManager = p; }

private slots:
    void tableItemClicked(const QModelIndex&);
    void keyCapLineEditTextChanged(QKeySequence);
    void restoreShortcutsButtonClicked();
    void clearButtonClicked();

private:
    bool isKeySequenceExist(const QSettings&, QString, QKeySequence);
    void removeDuplicateKeySequence(QSettings*, QKeySequence);
    void treeModelLoadShortcutsSetting();

    QStandardItemModel* m_treeModel = nullptr;
    QModelIndex m_currentItemIndex;
    
    Ui::ShortcutsPage* ui = nullptr;

    PreferenceManager* mManager = nullptr;
};

#endif // SHORTCUTSPAGE_H
