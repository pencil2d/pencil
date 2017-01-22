#ifndef SELECTTOOL_H
#define SELECTTOOL_H

#include "basetool.h"


class SelectTool : public BaseTool
{
    Q_OBJECT

public:
    explicit SelectTool( QObject* parent = 0 );
	ToolType type() override { return SELECT; }
    void loadSettings() override;
    QCursor cursor() override;

    void mousePressEvent( QMouseEvent* ) override;
    void mouseReleaseEvent( QMouseEvent* ) override;
    void mouseMoveEvent( QMouseEvent* ) override;

    bool keyPressEvent(QKeyEvent *event) override;
};

#endif
