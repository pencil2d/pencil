
#include <QDebug>
#include <QMap>
#include <QSettings>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTreeView>
#include <QStandardItemModel>
#include <QKeyEvent>
#include <QKeySequence>
#include "pencilsettings.h"
#include "shortcutspage.h"



ShortcutsPage::ShortcutsPage(QWidget *parent) :
    QWidget(parent),
    m_pCurrentEditItem( nullptr ),
    m_treeModel( nullptr ),
    m_treeView( nullptr )
{
    QSettings* pSettings = pencilSettings();

    pSettings->beginGroup("shortcuts");

    m_treeModel = new QStandardItemModel(pSettings->allKeys().size(), 2, this);

    int i = 0;
    foreach (QString strCmdName, pSettings->allKeys())
    {
        QString strKeySequence = pSettings->value(strCmdName).toString();

        m_treeModel->setItem(i, 0, new QStandardItem(strCmdName));
        m_treeModel->setItem(i, 1, new QStandardItem(strKeySequence));

        m_treeModel->item(i, 0)->setEditable(false);
        m_treeModel->item(i, 1)->setEditable(true);

        i++;
    }
    pSettings->endGroup();

    m_treeView = new QTreeView(this);
    m_treeView->setModel(m_treeModel);

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addWidget(new QLabel("Action Name:"));
    vLayout->addWidget(new QLineEdit());

    QGroupBox* groupbox = new QGroupBox("Set Shortcuts", this);
    groupbox->setLayout(vLayout);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_treeView);
    layout->addWidget(groupbox);

    setLayout(layout);

    connect(m_treeModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(tableItemChangs(QStandardItem*)));
    connect(m_treeView, SIGNAL(clicked(const QModelIndex&)), this, SLOT(tableItemClicked(const QModelIndex&)));

    m_treeView->installEventFilter(this);
}

void ShortcutsPage::tableItemChangs(QStandardItem* pItem)
{
    qDebug("Item Changes! at(%d, %d) = %s", pItem->row(), pItem->column(), pItem->text().toStdString().c_str());
}

void ShortcutsPage::tableItemClicked( const QModelIndex& modelIndex )
{
    qDebug("Clicked!");

    m_treeView->edit(modelIndex);
    m_pCurrentEditItem = m_treeModel->itemFromIndex(modelIndex);

    //m_pCurrentEditItem->setText("kerker");
    //int rowIndex = modelIndex.row();

    //QString strCmdName = m_pTableModel->item(rowIndex, 0)->text();
}

void ShortcutsPage::keyPressEvent(QKeyEvent* event)
{
    if ( m_pCurrentEditItem == nullptr )
    {
        event->ignore();
        return;
    }

    if (event->key() == Qt::Key_Control ||
        event->key() == Qt::Key_Shift   ||
        event->key() == Qt::Key_Alt     ||
        event->key() == Qt::Key_Meta )
    {
        // only modifier key is not allowed.
        return;
    }

    int keyInt = event->key();

    if (event->modifiers() & Qt::Key_Control)
    {
        keyInt += Qt::Key_Control;
    }
    if (event->modifiers() & Qt::Key_Shift)
    {
        keyInt += Qt::Key_Shift;
    }
    if (event->modifiers() & Qt::Key_Alt)
    {
        keyInt += Qt::Key_Alt;
    }

    QString strKeySeq = QKeySequence(keyInt).toString(QKeySequence::NativeText);

    m_pCurrentEditItem->setText(strKeySeq);
    qDebug() << "Current Item:" << m_pCurrentEditItem->text();
}

bool ShortcutsPage::eventFilter(QObject* object, QEvent* event)
{
    if (object == m_treeView)
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            qDebug() << "Key Press!" << keyEvent->text();

            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return QObject::eventFilter(object, event);
    }
}

