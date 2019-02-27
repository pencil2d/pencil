#ifndef APP_UTIL_H
#define APP_UTIL_H

inline
void hideQuestionMark(QDialog& dlg)
{
	Qt::WindowFlags eFlags = Qt::Dialog | Qt::WindowTitleHint;
	dlg.setWindowFlags( eFlags );
}

#endif // APP_UTIL_H