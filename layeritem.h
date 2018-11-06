#ifndef LAYERITEM_H
#define LAYERITEM_H
#include<QString>
#include <QDataStream>
#include<QFile>
#include<QApplication>
#include<QModelIndex>
#include<QTime>
struct LayerItem
{
    bool isenable;
    QString note;
    QString date;
    QString time;
    QString id;
    QString html;
    QString color;
    QString mp3_cmd;
    QString img;
    int playtime;
};
Q_DECLARE_METATYPE(LayerItem)



 inline QDataStream& operator<<(QDataStream& out, const LayerItem& item)
 {
     out<<item.isenable<<item.note<<item.date<<item.time<<item.id<<item.html<<item.color<<item.mp3_cmd<<item.img<<item.playtime;
     return out;
 }

 inline QDataStream& operator>>(QDataStream& in,LayerItem& item)
 {
     in>>item.isenable>>item.note>>item.date>>item.time>>item.id>>item.html>>item.color>>item.mp3_cmd>>item.img>>item.playtime;;
    return in;
 }


// struct WindowTheme
// {
//    QString selectedcolor;
//    QString textcolor;
//    QString timecolor;
//    QString datecolor;
//    QString datebg;
//    QString tipcolor;
//    QString tipbg;
//    bool onpic;
//    bool offpic;
// };
// Q_DECLARE_METATYPE(WindowTheme)

// inline QDataStream& operator<<(QDataStream& out, const WindowTheme& item)
// {
//     out<<item.selectedcolor<<item.textcolor<<item.timecolor<<item.datecolor<<item.datebg<<item.tipcolor<<item.tipbg<<item.onpic<<item.offpic;
//     return out;
// }

// inline QDataStream& operator>>(QDataStream& in,WindowTheme& item)
// {
//     in>>item.selectedcolor>>item.textcolor>>item.timecolor>>item.datecolor>>item.datebg>>item.tipcolor>>item.tipbg>>item.onpic>>item.offpic;
//    return in;
// }



#endif // LAYERITEM_H
