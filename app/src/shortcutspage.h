/*

Pencil - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2013-2018 Matt Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
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
    void keyCapLineEditTextChanged();
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
