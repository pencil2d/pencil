
#include "pencilsettings.h"
#include "util.h"

QKeySequence cmdKeySeq(QString strCommandName)
{
    strCommandName = QString("shortcuts/") + strCommandName;
    QKeySequence keySequence( pencilSettings()->value( strCommandName ).toString() );

    return keySequence;
}



