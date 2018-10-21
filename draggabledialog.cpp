#include "draggabledialog.h"
#include "ui_draggabledialog.h"

DraggableDialog::DraggableDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DraggableDialog)
{
    ui->setupUi(this);

    m_startDraging = false;
    //setWindowFlags(Qt::FramelessWindowHint); // TODO: ?????
    //...

}

DraggableDialog::~DraggableDialog()
{
    delete ui;
}

//--------------------------------------------------------------------------------

void DraggableDialog::keyPressEvent(QKeyEvent *e)
{
    //agar karbar dokmeye Escape ra mizad moshkel ijad mishod, in ra neveshtam
    if( e->key() == Qt::Key_Escape )
        reject(); //close();
    //...
    QDialog::keyPressEvent(e);
}

void DraggableDialog::mouseMoveEvent(QMouseEvent *e)
{
    if ((e->buttons() & Qt::LeftButton) && m_startDraging)
    {
        move(e->globalPos() - m_dragPosition);
        e->accept();
    }
    //...
    QDialog::mouseMoveEvent(e);
}

void DraggableDialog::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        m_startDraging = true;
        m_dragPosition = e->globalPos() - frameGeometry().topLeft();
        e->accept();
    }
    //...
    QDialog::mousePressEvent(e);
}

void DraggableDialog::mouseReleaseEvent(QMouseEvent *e)
{
    m_startDraging = false;
    //...
    QDialog::mouseReleaseEvent(e);
}
