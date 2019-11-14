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

    ui->cbImagePosition->addItem(tr("Center of current view"));
    ui->cbImagePosition->addItem(tr("Center of canvas (0,0)"));
    ui->cbImagePosition->addItem(tr("Center of camera, current frame"));
    ui->cbImagePosition->addItem(tr("Center of camera, follow camera"));

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
    mImportOption = ImportPosition::getTypeFromIndex(index);
}

void ImportPositionDialog::changeImportView()
{
    QTransform transform;
    if (mImportOption == ImportPosition::Type::CenterOfView)
    {
        QPointF centralPoint = mEditor->getScribbleArea()->getCentralPoint();
        transform = transform.fromTranslate(centralPoint.x(), centralPoint.y());
        mEditor->view()->setImportView(transform);
        QSettings settings(PENCIL2D, PENCIL2D);
        settings.setValue(IMPORT_REPOSITION_TYPE, ui->cbImagePosition->currentIndex());
        return;
    }
    else if (mImportOption == ImportPosition::Type::CenterOfCanvas)
    {
        transform = transform.fromTranslate(0, 0);
        mEditor->view()->setImportView(transform);
        QSettings settings(PENCIL2D, PENCIL2D);
        settings.setValue(IMPORT_REPOSITION_TYPE, ui->cbImagePosition->currentIndex());
        return;
    }
    else if (mImportOption == ImportPosition::Type::CenterOfCamera)
    {
        QRectF cameraRect = mEditor->getScribbleArea()->getCameraRect();
        transform = transform.fromTranslate(cameraRect.center().x(), cameraRect.center().y());
        mEditor->view()->setImportView(transform);
        QSettings settings(PENCIL2D, PENCIL2D);
        settings.setValue(IMPORT_REPOSITION_TYPE, ui->cbImagePosition->currentIndex());
        return;
    }

    QSettings settings(PENCIL2D, PENCIL2D);
    settings.setValue(IMPORT_REPOSITION_TYPE, ui->cbImagePosition->currentIndex());
}
