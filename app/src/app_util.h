#ifndef APP_UTIL_H
#define APP_UTIL_H

class PreferenceManager;
enum class SETTING;

void hideQuestionMark(QDialog& dlg);
void setMenuActionChecked(QAction* action, bool checked);
void bindPreferenceSetting(QAction* action, PreferenceManager*, const SETTING&);

#endif // APP_UTIL_H