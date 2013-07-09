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
    void pressRestoreShortcutsButton();

private:
    bool isKeySequenceExist(const QSettings&, QString, QKeySequence);
    void loadShortcutsFromSetting();

    QStandardItemModel* m_treeModel;
    QStandardItem* m_currentActionItem;
    QStandardItem* m_currentKeySeqItem;


    Ui::ShortcutsPage* ui;
};

#endif // SHORTCUTSPAGE_H
