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

#ifndef GENERALPAGE_H
#define GENERALPAGE_H

class QAbstractButton;
class PreferenceManager;

namespace Ui {
class GeneralPage;
}

class GeneralPage : public QWidget
{
Q_OBJECT
public:
    GeneralPage();
    ~GeneralPage() override;
    void setManager(PreferenceManager* p) { mManager = p; }

public slots:
    void updateValues();
    void gridWidthChanged(int value);
    void gridHeightChanged(int value);
    void actionSafeCheckBoxStateChanged(int b);
    void actionSafeAreaChanged(int value);
    void titleSafeCheckBoxStateChanged(int b);
    void titleSafeAreaChanged(int value);
    void SafeAreaHelperTextCheckBoxStateChanged(int b);

signals:
    void windowOpacityChange(int value);

private slots:
    void languageChanged(int i);
    void shadowsCheckboxStateChanged(int b);
    void antiAliasCheckboxStateChanged(int b);
    void toolCursorsCheckboxStateChanged(int b);
    void canvasCursorCheckboxStateChanged(int b);
    void highResCheckboxStateChanged(int b);
    void gridCheckBoxStateChanged(int b);
    void curveSmoothingChanged(int value);
    void backgroundChanged(QAbstractButton* button);
    void frameCacheNumberChanged(int value);
    void invertScrollDirectionBoxStateChanged(int b);

private:

    void updateSafeHelperTextEnabledState();

    Ui::GeneralPage* ui = nullptr;
    PreferenceManager* mManager = nullptr;
};

#endif // GENERALPAGE_H
