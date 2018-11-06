#ifndef NOTELITE_H
#define NOTELITE_H

#include <QWidget>
#include<layeritem.h>
#include<desktopnote.h>
#include<edit.h>
#include<notify.h>
#include<QProcess>
#include<QMimeDatabase>
#include<QMimeType>

class notelite : public QObject
{
    Q_OBJECT

public:
    explicit notelite(QObject *parent = 0);
    ~notelite();

private slots:
    void noteedited(const LayerItem&);
    void addanote();
    void delnote(const LayerItem& );
    void noteneedreboot(const LayerItem&);
    void timeout();
private:

    bool readlist();
    bool savelist();
    void mkdesknote( LayerItem*);
    void updatetasklist();
    int finditem(LayerItem*);

    QList<LayerItem*> notelist;
    QList< LayerItem* > tasklist;
    QTimer timer;
    Notify *notify;

};

#endif // NOTELITE_H
