#ifndef IMAGESEQDIALOG_H
#define IMAGESEQDIALOG_H

#include <QDialog>

class QSpinBox;
class QLabel;
class QGridLayout;
class QGroupBox;
class Editor;

class ImageSeqDialog : public QDialog
{
    Q_OBJECT

public:
    ImageSeqDialog(QWidget *parent);

    void init();
    void seqNumber(QString strImgFile, Editor *mEditor);

public slots:
        void setSeqValue(int number);

private:
    void createIcons();

    QSpinBox *mSequenceSpaceBox;
    QLabel *sequenceBoxLabel;
};

#endif // IMAGESEQDIALOG_H
