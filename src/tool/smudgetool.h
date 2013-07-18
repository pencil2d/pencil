#ifndef SMUDGETOOL_H
#define SMUDGETOOL_H

#include "basetool.h"


class SmudgeTool : public BaseTool
{
    Q_OBJECT
public:
    explicit SmudgeTool(QObject *parent = 0);
    ToolType type();
    void loadSettings();
    QCursor cursor();

    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

signals:
    
public slots:
    
};

#endif // SMUDGETOOL_H
