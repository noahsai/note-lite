#include "notelite.h"

QString cfgpath;

notelite::notelite(QObject *parent) :
    QObject(parent)
{
    QApplication::setQuitOnLastWindowClosed(true);
    cfgpath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/note_lite";
    QDir().mkpath(cfgpath);

    notify = NULL;
    //========
    readlist();
    if(notelist.isEmpty()) addanote();
    //========
    timer.setInterval(900);
    timer.setSingleShot(false);
    connect(&timer , SIGNAL(timeout()),this , SLOT(timeout()));
    timer.start();
}

notelite::~notelite()
{
    timer.stop();
    savelist();
    for(int i=0;i<notelist.count();i++)
    {
        delete notelist.at(i);
    }
}

bool notelite::readlist(){
    tasklist.clear();
    QFile file;
    QString path = cfgpath+"/notelitelist";
    qDebug()<<"readlist"<<path;
    file.setFileName(path);
    QDataStream in(&file);
    if(file.open(QIODevice::ReadOnly))
    {
        for(;!in.atEnd();)
        {
            LayerItem* item = new LayerItem;//notelist存的是指针！！所以需要不同的指针！！item自身是变化的！！
            in>>*item;
            mkdesknote(item); //选择配合notelist.append(item)使用;
            notelist.append(item);
            //qDebug()<<item->id;
        }
        updatetasklist();
        qDebug()<<"readlist finished,notelist:"<<endl<<notelist;
        file.close();
    }
    else {
        qDebug()<<"open notelitelist error.";
        return false;
    }
    return true;
}

bool notelite::savelist(){
    QFile file;
    QString path = cfgpath+"/notelitelist";
    qDebug()<<"savelist"<<path;
    file.setFileName(path);
    if(file.open(QIODevice::WriteOnly))
    {
        QDataStream out(&file);
        //qDebug()<<layerList.count();
        for(int i=0;i<notelist.count();i++)
        {
            out<<(*(notelist[i]));
        }
    }
    else {
        qDebug()<<"open notelitelist error.";
        return false;
    }
    file.close();
    return true;
}
void notelite::mkdesknote( LayerItem* i){
    LayerItem *item = i;
    if(item->id.isEmpty()) {
        item->id = QDateTime().currentDateTime().toString("yyyy.MM.dd HH:mm:ss:zzz");
        qDebug()<<"id:"<<item->id;
    }
    qDebug()<<"add item:"<<item->id;
    desktopNote *note ;
    if(item->color.indexOf("png")!=-1)
        note = new desktopNote(*item,true,NULL);
    else
        note = new desktopNote((*item),false,NULL);
    connect(note,SIGNAL(finished(LayerItem)),this,SLOT(noteedited(const LayerItem&)));
    connect(note,SIGNAL(addnote()),this,SLOT(addanote()));
    connect(note,SIGNAL(notedel(LayerItem)),this,SLOT(delnote(const LayerItem&)));
    connect(note,SIGNAL(needreboot(LayerItem)),this,SLOT(noteneedreboot(const LayerItem&)));

}

void notelite::noteedited(const LayerItem& item){
    LayerItem *i = new LayerItem;
    *i = item;
    int n = finditem(i);
    qDebug()<<"item found:"<<n;
    if(n==-1) return;
    delete notelist[n];
    notelist[n] = i;
    updatetasklist();
}

void notelite::addanote()
{
    LayerItem *item = new LayerItem;
    item->color="#ffffff";
    item->isenable = true;
    item->note="";
    item->id = "";
    mkdesknote(item); //选择配合notelist.append(item)使用;
    notelist.append(item);
}

void notelite::delnote(const LayerItem &item)
{
    LayerItem *i = new LayerItem;
    *i = item;
    int n = finditem(i);
    if(n==-1) return;
    delete notelist[n];
    notelist.removeAt(n);
    //删除配置文件
    QString cfg = cfgpath+"/"+item.id;
    if(QFile(cfg).remove()) qDebug()<<"poscfg removed";
    else qDebug()<<"poscfg remove error";
    //
    delete sender();
    //
   // savelist();
    if(notelist.count() == 0){
        qApp->quit();
    }
    updatetasklist();

}

int notelite::finditem(LayerItem* it){
    for(int i=0 ; i<notelist.length();i++)
    {
        if(notelist[i]->id == it->id) return i;
    }
    return -1;
}

void notelite::noteneedreboot(const LayerItem& item){
    desktopNote* note = qobject_cast<desktopNote *>(sender());
    delete note;
    LayerItem *i = new LayerItem;
    *i = item;
    int n = finditem(i);
    mkdesknote(notelist[n]); //选择配合notelist.append(item)使用;
}

void notelite::timeout(){
    QTime now;
    now = now.currentTime();
    if(now.toString("hh:mm:ss")==("00:00:00")){
        //凌晨就要重新生成tasklist
        updatetasklist();
    }

    int playtime=0;
    QString mp3 , img ,text;
    for(int i= 0;i<tasklist.length();i++)
    {
        if(now.toString("h:m:s")==(tasklist[i]->time) ){
            if(notify == NULL) {
                notify = new Notify(NULL);
            }
            qDebug()<<"work:"<<tasklist[i];
            QMimeDatabase db;
            QMimeType mime = db.mimeTypeForFile(tasklist[i]->mp3_cmd);  //
            QString type = mime.name();
            //qDebug()<<type;
            //不匹配时返回application/octet-stream
            if(tasklist[i]->mp3_cmd.isEmpty() ){
                //什么都不用干,依然使用就的mp3
            }
            else if(type.indexOf("audio")!=-1){
                mp3 = tasklist[i]->mp3_cmd;
            }
            else if(type == "application/octet-stream"){
                QProcess pro;
                pro.startDetached(tasklist[i]->mp3_cmd);
                qDebug()<<"run cmd:"<<tasklist[i]->mp3_cmd;
            }
            else if(type == "application/x-sharedlib"||type == "application/x-shellscript"){
                QProcess pro;
                pro.startDetached(("bash \""+tasklist[i]->mp3_cmd +"\""));
                qDebug()<<"run shellscript sharedlib:"<<tasklist[i]->mp3_cmd;
            }
            else {
                QProcess pro;
                pro.startDetached(("xdg-open  \""+tasklist[i]->mp3_cmd+"\""));
                qDebug()<<"run xdg-open"<<tasklist[i]->mp3_cmd ;
            }
            if(!tasklist[i]->img.isEmpty() ){
                img = tasklist[i]->img;
            }
            if(playtime < tasklist[i]->playtime) playtime = tasklist[i]->playtime;

            if(!text.isEmpty()) text += "\n";
            text = text  + tasklist[i]->note ;
           //不要去除notelist里面的！！这只是闹铃
            tasklist.removeAt(i);
            --i;//因为实时改动list会导致list长度也改变。
            //不要updatetasklist了。直接删除tasklist里面的就行了
        }
    }
    if(playtime!=0) {
        notify->init(playtime , mp3 ,img);
        notify->message(text);//这时会启动notify计时
        notify->show();
    }

}

void notelite::updatetasklist(){
    tasklist.clear();
    for(int i=0;i<notelist.count();i++)
    {
        //out<<(*(notelist[i]));
        QDate d;
        QDate today;
        d = d.fromString(notelist[i]->date , "yyyy.MM.dd");
        today = today.currentDate();
        if(today == d && notelist[i]->isenable) tasklist.append(notelist[i]);
    }
    qDebug()<<"tasklist:"<<tasklist;
}
