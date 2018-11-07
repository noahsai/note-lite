#ifndef desktopnote_H
#define desktopnote_H
#include <edit.h>
#include <QWidget>
#include<layeritem.h>
#include<QPushButton>
#include<QDesktopWidget>
#include<QBitmap>
#include<QSettings>
#include<QTextEdit>
#include<QAbstractTextDocumentLayout>
#include<QHBoxLayout>
namespace Ui {
class desktopNote;
}

class desktopNote : public QWidget
{
    Q_OBJECT

public:
    explicit desktopNote(LayerItem& i,bool alpha,QWidget *parent = 0);
    ~desktopNote();
    editnote* createditor();
    void setdata(const LayerItem&);

public slots:

private slots:
    void setfinished(const LayerItem&,const QPixmap&);
    void toeditnote();
    void delnote();
    void editcancel();
    void initnote();
    void exitprog();

signals:
    void addnote();
    void finished( LayerItem);
    void notedel( LayerItem);
    void needreboot(LayerItem);
private:
    Ui::desktopNote *ui;
    void closeEvent(QCloseEvent *event);
    void paintEvent(QPaintEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void savepos();
    void readpos();
    QPoint oldpos;
    editnote *editor;
    QPixmap pix;
    LayerItem item;
    QPainter painter;
    QPushButton *add,*toedit,*refresh,*del;
    QWidget *bar;
    bool moved;
};

#endif // desktopnote_H
