#ifndef DRAGGABLEDIALOG_H
#define DRAGGABLEDIALOG_H

#include <QDialog>
#include <QMouseEvent>

namespace Ui {
class DraggableDialog;
}

class DraggableDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DraggableDialog(QWidget *parent = 0);
    ~DraggableDialog();

protected:
    void keyPressEvent(QKeyEvent *e);

    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    Ui::DraggableDialog *ui;

    bool m_startDraging;
    QPoint m_dragPosition;

};

#endif // DRAGGABLEDIALOG_H
