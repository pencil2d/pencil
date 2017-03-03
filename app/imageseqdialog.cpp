#include <QtWidgets>
#include <QSpinBox>
#include <QGridLayout>
#include <QGroupBox>

#include "imageseqdialog.h"
#include "editor.h"

ImageSeqDialog::ImageSeqDialog(QWidget* parent ) : QDialog(parent)
{
    setWindowTitle(tr("Image Sequence Dialog"));
    setFixedSize(300,180);
}

void ImageSeqDialog::init()
{
    QVBoxLayout* lay = new QVBoxLayout;
    QPushButton *okButton = new QPushButton(tr("OK"));
    QPushButton *closeButton = new QPushButton(tr("Cancel"));

    QGroupBox* sequenceBox = new QGroupBox();
    QHBoxLayout* buttonsLayout = new QHBoxLayout;

    sequenceBoxLabel = new QLabel(this);
    sequenceBoxLabel->setText(tr("Import an image every # frame"));

    mSequenceSpaceBox = new QSpinBox(this);
    mSequenceSpaceBox->setRange(1,64);
    mSequenceSpaceBox->setValue( 1 );

    QGridLayout* gridLayout = new QGridLayout();
    sequenceBox->setLayout(gridLayout);

    connect(closeButton, &QAbstractButton::clicked, this, &QDialog::reject);
    connect(okButton, &QAbstractButton::clicked, this, &QDialog::accept);
    connect(mSequenceSpaceBox, SIGNAL(valueChanged(int)), this, SLOT(setSeqValue(int)));

    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(closeButton);

    gridLayout->addWidget(sequenceBoxLabel, 0, 0);
    gridLayout->addWidget(mSequenceSpaceBox, 1, 0);

    lay->addWidget(sequenceBox);
    lay->addLayout(buttonsLayout);

    setLayout(lay);
}

void ImageSeqDialog::setSeqValue(int number)
{
    QSignalBlocker b1( mSequenceSpaceBox );
    mSequenceSpaceBox->setValue(number);
}

void ImageSeqDialog::seqNumber(QString strImgFile, Editor *mEditor)
{
    int number = mSequenceSpaceBox->value();

    mEditor->importImage( strImgFile );
    for (int i = 1; i < number; i++)
    {
        mEditor->scrubForward();
    }

}
