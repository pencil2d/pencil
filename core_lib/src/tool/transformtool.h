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
#ifndef TRANSFORMTOOL_H
#define TRANSFORMTOOL_H

#include "basetool.h"

class TransformTool : public BaseTool
{
    Q_OBJECT
public:
    TransformTool(QObject* parent = nullptr);
    ~TransformTool();

    ToolSettings& settings() override { return mSettings.general(); }
    const TransformSettings& transformSettings() const { return mSettings; }

    void setShowSelectionInfo(bool enabled);
    void setAntiAliasingEnabled(bool enabled);

signals:
    void showSelectionInfoChanged(bool enabled);
    void antiAliasingChanged(bool enabled);

protected:
    TransformSettings mSettings;
};

#endif // TRANSFORMTOOL_H
