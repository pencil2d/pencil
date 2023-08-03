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

#ifndef TOOLSPAGE_H
#define TOOLSPAGE_H

class PreferenceManager;

namespace Ui {
class ToolsPage;
}

class ToolsPage : public QWidget
{
Q_OBJECT
public:
    ToolsPage();
    ~ToolsPage() override;
    void setManager(PreferenceManager* p) { mManager = p; }

public slots:
    void updateValues();
    void quickSizingChange(int);
    void setRotationIncrement(int);
    void rotationIncrementChange(int);
    void invertZoomDirectionChange(int);
private:
    Ui::ToolsPage* ui = nullptr;
    PreferenceManager* mManager = nullptr;
};

#endif // TOOLSPAGE_H
