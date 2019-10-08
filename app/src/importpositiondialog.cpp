#include "importpositiondialog.h"
#include "ui_importpositiondialog.h"

#include "editor.h"
#include "viewmanager.h"

#include "scribblearea.h"

ImportPositionDialog::ImportPositionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportPositionDialog)
{
    ui->setupUi(this);

    ui->cbImagePosition->addItem(tr("Center of canvas"));
    ui->cbImagePosition->addItem(tr("Center of active camera"));

    connect(ui->cbImagePosition, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ImportPositionDialog::didChangeComboBoxIndex);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ImportPositionDialog::changeImportView);

    QSettings settings(PENCIL2D, PENCIL2D);
    int value = settings.value(IMPORT_REPOSITION_TYPE).toInt();

    ui->cbImagePosition->setCurrentIndex(value);
    didChangeComboBoxIndex(value);
}

ImportPositionDialog::~ImportPositionDialog()
{
    delete ui;
}

void ImportPositionDialog::didChangeComboBoxIndex(const int index)
{
    switch (index) {
    case 0: {
        mImportOption = ImportPositionType::CenterOfCanvas;
        break;
    }
    case 1: {
        mImportOption = ImportPositionType::CenterOfCamera;
        break;
    }
    default:
        break;
    }
}

void ImportPositionDialog::changeImportView()
{
    QTransform transform;
    if (mImportOption == ImportPositionType::CenterOfCanvas)
    {
        QPointF centralPoint = mEditor->getScribbleArea()->getCentralPoint();
        transform = transform.fromTranslate(centralPoint.x(), centralPoint.y());
    }
    else if (mImportOption == ImportPositionType::CenterOfCamera)
    {
        QRectF cameraRect = mEditor->getScribbleArea()->getCameraRect();
        transform = transform.fromTranslate(cameraRect.center().x(), cameraRect.center().y());
    }
    mEditor->view()->setImportView(transform);

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(IMPORT_REPOSITION_TYPE, ui->cbImagePosition->currentIndex());
}
