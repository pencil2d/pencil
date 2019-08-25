#ifndef NOTOOL_H
#define NOTOOL_H

#include "basetool.h"

class NoTool: public BaseTool
{
    Q_OBJECT

public:
    explicit NoTool(QObject* parent);


private:

    void loadSettings() override;
    void pointerPressEvent(PointerEvent*) override;
    void pointerReleaseEvent(PointerEvent*) override;
    void pointerMoveEvent(PointerEvent*) override;

    ToolType type() override;
};

#endif // NOTOOL_H
