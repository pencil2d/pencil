/*

Pencil2D - Traditional Animation Software
Copyright (C) 2005-2007 Patrick Corrieri & Pascal Naidon
Copyright (C) 2012-2020 Matthew Chiawen Chang

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

*/
#ifndef PEGBARALIGNMENTDIALOG_H
#define PEGBARALIGNMENTDIALOG_H

#include <QDialog>
#include <QStringList>
#include "editor.h"

namespace Ui {
class PegBarAlignmentDialog;
}

class PegBarAlignmentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PegBarAlignmentDialog(Editor* editor, QWidget* parent = nullptr);
    ~PegBarAlignmentDialog();

    void setLayerList(QStringList layerList);
    QStringList getLayerList();
    int getRefKey() { return mRefkey; }
    QString getRefLayer() { return mRefLayer; }
    void updateRefKeyLabelText();

    void setAreaSelected(bool b);
    void setReferenceSelected(bool b);
    void setLayerSelected(bool b);

    void updatePegRegLayers();
    void updatePegRegDialog();
    void alignPegs();

public slots:
    void setBtnAlignEnabled();
    void setRefLayer(QString s);
    void setRefKey(int i);

private:
    void closeClicked();

    Ui::PegBarAlignmentDialog* ui;
    QStringList mLayernames;
    Editor* mEditor = nullptr;
    bool mAreaSelected = false;
    bool mReferenceSelected = false;
    bool mLayerSelected = false;
    QString mRefLayer;
    int mRefkey = 0;
};

#endif // PEGBARALIGNMENTDIALOG_H
