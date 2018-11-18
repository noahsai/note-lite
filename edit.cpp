#include "edit.h"
#include "ui_edit.h"

editnote::editnote(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::edit)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/note-lite.png"));
    calendar = NULL;
    mouseclick = false;
    this->setMinimumSize(50,50);//最小宽度50
    this->setSizePolicy(QSizePolicy::Ignored ,QSizePolicy::Preferred );
    ui->size->setValue(12);//默认值，最大最小在ui里设置了
    //setbgcolor("ffffff");
    inititem();//初始化一个item
    QFont fo;
    fo.setPointSize(20);
    ui->note->document()->setDefaultFont(fo);
    ui->color->setStyleSheet("QPushButton{color:#000000;}");
    ui->toolbar->setParent(this);
    ui->toolbar->setWindowFlags(Qt::CustomizeWindowHint|Qt::Window|Qt::WindowTitleHint);//使用窗口管理器
    ui->toolbar->setWindowTitle("编辑");
    ui->toolbar->showNormal();
    ui->toolbar->move(parent->x(),parent->y()+parent->height()+60);
}

editnote::~editnote()
{
    delete ui;
}

void editnote::inititem()
{
    QString date = QDate::currentDate().toString("yyyy-MM-dd");
    item.date = date;
    //默认当天
    item.time=QTime::currentTime().toString("h:m:s");
    //默认现在时间
    ui->enable->setChecked(true);
    item.isenable = true ;//默认开启

    item.isenable = true;
}


void editnote::on_note_textChanged()
{
//    QString text;
//    text = ui->note->toPlainText().remove("\n").remove(" ");
//    item.note= text;
//    item.html=ui->note->toHtml();
//    qDebug()<<"textchanged:"<<item.html;

}


void editnote::setdata(LayerItem &value)
{
    //qDebug()<<"setdata:"<<value.html;
    ui->note->clear();
    item = value;
    //ui->time ->setText( value.time);
    ui->note->setHtml(value.html);
    ui->enable->setChecked(value.isenable);
    setbgcolor(item.color,true);
    //======
    if(value.date.isEmpty())
    {
        QString date = QDate::currentDate().toString("yyyy-MM-dd");
        item.date = date;
    }
    ui->date ->setText( item.date);
    //=======
    if(value.time.isEmpty()) {
        item.time=QTime::currentTime().toString("h:m:s");
    }

    QTime t;
    t = t.fromString(item.time , "h:m:s");
    ui->hour->setValue(t.hour());
    ui->min->setValue(t.minute());
    ui->sec->setValue(t.second());
    if(item.playtime>ui->playtime->maximum()||item.playtime<ui->playtime->minimum())
        item.playtime = 5;
    ui->playtime->setValue(item.playtime);
    ui->mp3_cmd->setText(item.mp3_cmd);

    qDebug()<<item.time<<t;
    //qDebug()<<this->geometry()<<ui->note->size();
}



void editnote::on_pushButton_clicked()
{
    QString text,html;
    text = ui->note->toPlainText().remove("\n").remove(" ");
    item.note= text;
    html = ui->note->toHtml();
    html = html.replace("-qt-paragraph-type:empty;"," ");//同下
    html = html.replace("<br />"," ");//这里曲线修复qt组件setHtml首行是<br />时会自动增加换行的bug！
    item.html=html;
    item.date = ui->date->text();
    item.time = QString().setNum(ui->hour->value())+":"+QString().setNum(ui->min->value())+":"+QString().setNum(ui->sec->value());
    item.playtime = ui->playtime->value();
    item.mp3_cmd = ui->mp3_cmd->text();
    item.isenable = ui->enable->isChecked();
    qDebug()<<item.date<<item.time<<item.time;
   // on_time_editingFinished();
    //qDebug()<<"warp"<<ui->note->document()->defaultTextOption().wrapMode();
   // qDebug()<<ui->note->width()<<ui->note->height();
    QPixmap pix(ui->note->width(),ui->note->height());
    if(item.color.indexOf("png")!=-1)//只有png时才支持透明，jpg和纯色不需要
    {
        pix.fill(Qt::transparent);
    }
    qDebug()<<pix.rect();

    QPainter painter(&pix);
    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.clip = QRectF(pix.rect());
    ui->note->document()->documentLayout()->draw(&painter,ctx);
    //ui->note->document()->drawContents(&painter);//不能画背景

   // pix.save(cfgpath+"/66.png");
    //qDebug()<<this->geometry()<<ui->note->size();
   // pix.scaled();
    //if(item.note.isEmpty()) item.note = "空白事件";
    if(item.color.isEmpty()) item.color = "#ffffff";
    //qDebug()<<"edited:"<<item.html;
    emit setfinished(item,pix);

}


void editnote::on_size_valueChanged(int arg1)
{
    if(arg1 == -1) return;
    if(mouseclick) {
        mouseclick =false;
        return;
    }
    qDebug()<<"fontsizevaluechange"<<arg1;
     size = arg1;
    change(0);
}
void editnote::on_bold_clicked()
{
    change(1);
}
void editnote::on_color_clicked()
{
    color = QColorDialog::getColor(Qt::white, this);
    if(!color.isValid()) return;
  //  qDebug()<<color;
    change(2);

}


void editnote::change(int mode)
{
    textcur = ui->note->textCursor();
    //获取当前选择状态
    //qDebug()<< textcur.position();
    QTextCursor tmp = textcur;
    //tmp备份当前选择状态
    int end = textcur.selectionEnd();
    int start = textcur.selectionStart();
    //if(!textcur.atEnd()) textcur.movePosition(QTextCursor::Right);//如果从右往左划，需要回到右移一个字符，才能获取选区的开头字符的属性。
    textcur.setPosition(end);

    QFont fo;
//    if(textcur.charFormat().font().bold())  fo.setBold(false);
//    else fo.setBold(true);
    qDebug()<<tmp.selectionStart()<<tmp.selectionEnd();
    for(int i=end;i>start;i--)//最you出发
    {
        textcur.setPosition(i);
        QTextCharFormat fom=textcur.charFormat();

        textcur.movePosition(QTextCursor::Left,QTextCursor::KeepAnchor);
        //qDebug()<< textcur.position();
        switch(mode)
        {
        case 0:
            //设置大小
            fo.setPointSize(size);
            fom.setFont(fo,QTextCharFormat::FontPropertiesSpecifiedOnly);
            break;
        case 1:
            //设置粗体
            bold =!(textcur.charFormat().font().bold());//取反
            fo.setBold(bold);
            fom.setFont(fo,QTextCharFormat::FontPropertiesSpecifiedOnly);
            break;
        case 2:
            fom.setForeground(QBrush(color));
            break;
        default:
            return;
        }
        textcur.setCharFormat( fom);
//        ui->note->setTextCursor(textcur);
        //切换粗体状态
        QTextCharFormat f=textcur.charFormat();
        QFont fo = f.font();
        qDebug()<<textcur.position()<<fo.pointSize();

    }
    if(end==start) {
        QTextCharFormat fom=textcur.charFormat();
        fo.setPointSize(size);
        fo.setBold(bold);
        fom.setFont(fo,QTextCharFormat::FontPropertiesSpecifiedOnly);
        fom.setForeground(QBrush(color));
        textcur.setCharFormat( fom);
        ui->note->setTextCursor(textcur);
        return;
    }
    //ui->note->setTextCursor(textcur);
    ui->note->setTextCursor(tmp);//还原状态

}


void editnote::on_align_activated(int index)
{
    textcur = ui->note->textCursor();
    QTextBlockFormat f= ui->note->textCursor().blockFormat();
    switch (index) {
    case 0:
        f.setAlignment(Qt::AlignHCenter);
        break;
    case 1:
        f.setAlignment(Qt::AlignLeft);
        break;
    case 2:
        f.setAlignment(Qt::AlignRight);
        break;
    default:
        break;
    }
    textcur.setBlockFormat(f);
    ui->note->setTextCursor(textcur);

}

void editnote::on_note_cursorPositionChanged()
{
    mouseclick = true;
    textcur = ui->note->textCursor();
    QTextBlockFormat f= ui->note->textCursor().blockFormat();
    QTextCharFormat fom=textcur.charFormat();
    QFont fo = fom.font();
    ui->size->setValue(fo.pointSize());//这句不能用，用了会触发ui.size的槽函数
    qDebug()<<"cursorPositionChanged"<<fo.pointSize();


    QFont fon;
    fon.setBold(fo.bold());
    ui->bold->setFont(fon);

    QString sty ;
    sty = sty +"QPushButton{color:"+fom.foreground().color().name()+";}";
    ui->color->setStyleSheet(sty);

    switch (f.alignment()) {
    case Qt::AlignHCenter:
        ui->align->setCurrentIndex(0);
        break;
    case Qt::AlignLeft:
        ui->align->setCurrentIndex(1);
        break;
    case Qt::AlignRight:
        ui->align->setCurrentIndex(2);
        break;
    default:
        break;
    }
    //qDebug()<<fom.font().pointSize();

}

void editnote::on_size_editingFinished()//这个用起来不爽
{
//    size = ui->size->value();
//    change(0);
}


void editnote::setbgcolor(QString color,bool init)
{

    QTextFrameFormat f;
    f.setMargin(2);
    if(color.indexOf(QRegularExpression("png|jpg"))==-1)
    {
        f.setBackground(QBrush(QColor(color)));
    }
    else {
        QPixmap pix(color);
       if(init) pix = pix.scaled(initsize,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
       else pix = pix.scaled(ui->note->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
       f.setBackground(QBrush(QColor(Qt::transparent),pix));
    }
    ui->note->document()->rootFrame()->setFrameFormat(f);

}

void editnote::on_enable_clicked(bool checked)
{
    item.isenable = checked;
}

void editnote::on_cancel_clicked()
{
    emit editcancel();
}

void editnote::on_bgchoose_activated(int index)
{
    if(index == 0)
    {
    QColor color;
    color = QColorDialog::getColor(Qt::white, this);
    if(!color.isValid()) return;
    item.color = color.name();
    }
    else if(index == 1)
    {
        QString home = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();
        QString file =  QFileDialog::getOpenFileName(this,"选择图片",home,tr("图片 (*.png *.jpg)"));
        if(file.isNull()) return;
        item.color = file;
    }
    setbgcolor(item.color,false);
    qDebug()<<item.color;
}

void editnote::on_date_clicked()
{
    if(!calendar){
        calendar = new QCalendarWidget(ui->toolbar);
        calendar->setWindowFlags(Qt::FramelessWindowHint|Qt::Popup);
        connect(calendar , SIGNAL(clicked(QDate)) ,this ,SLOT(setdate(QDate)));
    }
    //qDebug()<<ui->toolbar->mapToGlobal(ui->date->pos());

    calendar->move(ui->toolbar->x(),ui->toolbar->mapToGlobal(ui->date->pos()).y()+ui->date->height());
    calendar->show();
    calendar->setSelectedDate(QDate().fromString(item.date,"yyyy-MM-dd"));
    calendar->showSelectedDate();
}

void editnote::setdate(QDate date){
    ui->date->setText(date.toString("yyyy-MM-dd"));
}

void editnote::on_findfile_clicked()
{
    QString home = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();
    QString file =  QFileDialog::getOpenFileName(this,"选择文件",home,tr("所有 (*)"));
    if(file.isEmpty()) return;
    else ui->mp3_cmd->setText(file);
}
