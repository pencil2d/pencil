#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

class QSpinBox;
class QLabel;
class QGridLayout;
class QGroupBox;

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    AboutDialog(QWidget *parent);

    void init();

private:
    void createIcons();

    QSpinBox *mSequenceSpaceBox;
    QLabel *aboutText;
    QLabel *logo;
    QLabel *logoText;
    QLabel *devInfoText;
};

#endif // ABOUTDIALOG_H

