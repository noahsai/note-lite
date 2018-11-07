#include "desktopnote.h"
#include "ui_desktopnote.h"

extern QString cfgpath;

desktopNote::desktopNote(LayerItem &i ,bool alpha,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::desktopNote)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/note-lite.png"));
    setAttribute(Qt::WA_DeleteOnClose);
    this->setGeometry(QApplication::desktop()->width()-120,120,200,250);
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);//去边框，不在任务栏显示
    if(alpha){
        this->setAttribute(Qt::WA_TranslucentBackground,true);//透明背景,没阴影效果。好像只能在构造函数里才起作用。
        pix = QPixmap(size());
        pix.fill();//由于透明，需要预先准备白底
    }
    else this->setAttribute(Qt::WA_TranslucentBackground,false);//不透明，有阴影效果

    editor = nullptr;//必须
    //增加便签按钮

    bar = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(bar);
    QSpacerItem *hspacer = new QSpacerItem(40,20,QSizePolicy::Expanding) ;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(2);
    add = new QPushButton("✛",this);
    connect(add,SIGNAL(clicked(bool)),this,SIGNAL(addnote()));
    QFont f;
    f.setPixelSize(20);
    add->setFont(f);
    add->setToolTip("新建");
    add->setFlat(true);
    add->setFocusPolicy(Qt::NoFocus	);
    add->setFixedSize(30,30);
    //add->show();

    toedit = new QPushButton("✎",this);
    toedit->setToolTip("编辑");
    connect(toedit,SIGNAL(clicked(bool)),this,SLOT(toeditnote()));
    toedit->setFont(f);
    toedit->setFlat(true);
    toedit->setFocusPolicy(Qt::NoFocus	);
    //toedit->show();
    toedit->setFixedSize(30,30);

    refresh = new QPushButton("↻",this);
    refresh->setToolTip("刷新");
    connect(refresh,SIGNAL(clicked(bool)),this,SLOT(initnote()));
    refresh->setFont(f);
    refresh->setFlat(true);
    refresh->setFocusPolicy(Qt::NoFocus	);
    //toedit->show();
    refresh->setFixedSize(30,30);

    del = new QPushButton("✕",this);
    del->setToolTip("删除");
    connect(del,SIGNAL(clicked(bool)),this,SLOT(delnote()));
    del->setFont(f);
    del->setFlat(true);
    del->setFocusPolicy(Qt::NoFocus	);
    //toedit->show();
    del->setFixedSize(30,30);

    QPushButton* ex = new QPushButton("↵",this);
    ex->setToolTip("退出程序");
    connect(ex,SIGNAL(clicked(bool)),this,SLOT(exitprog()));
    ex->setFont(f);
    ex->setFlat(true);
    ex->setFocusPolicy(Qt::NoFocus	);
    //toedit->show();
    ex->setFixedSize(30,30);

    layout->addWidget(ex);
    layout->addSpacerItem(hspacer);
    layout->addWidget(del);
    layout->addWidget(refresh);
    layout->addWidget(toedit);
    layout->addWidget(add);
    bar->setLayout(layout);
    bar->setFixedHeight(30);
    bar->setBackgroundRole(QPalette::Background);
    bar->setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor("#e0ffffff"));
    bar->setPalette(pal);
    bar->hide();

    //初始化item
    item = i;//设置item
    moved = false;
    readpos();//读取配置
    initnote();//刷新便签内容
}

desktopNote::~desktopNote()
{
    readpos();
    delete ui;
}

editnote* desktopNote::createditor(){
    editnote *editor=nullptr;
    editor = new editnote(this);
    editor->setGeometry(this->rect());
    return editor;
}

void desktopNote::toeditnote()
{
    setWindowFlags(Qt::CustomizeWindowHint|Qt::Window|Qt::WindowTitleHint);//使用窗口管理器
    setWindowTitle("便签");
    this->showNormal();
    if(editor!=nullptr) return;
    QSize old =size();
    editor = createditor();
    editor->initsize = old;
    editor->setdata(item);
    //editor->setAttribute(Qt::WA_DeleteOnClose);
    this->layout()->addWidget(editor);
    this->layout()->setMargin(0);
    connect(editor,SIGNAL(setfinished(LayerItem,QPixmap)),this,SLOT(setfinished(LayerItem,QPixmap)));
    connect(editor,SIGNAL(editcancel()),this,SLOT(editcancel()));
    editor->setVisible(true);
    //qDebug()<<"mouseDoubleClickEvent";
}

void desktopNote::mouseDoubleClickEvent(QMouseEvent *event)
{
    toeditnote();
    event->accept();
}

void desktopNote::editcancel()
{
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);//去边框，不在任务栏显示
    delete editor;
    editor = nullptr;
    this->show();
}

void desktopNote::initnote()
{
    //qDebug()<<"init:"<<item.html;
    QPixmap p(this->width(),this->height());
    if(item.color.indexOf("png")!=-1)
    {
        p.fill(Qt::transparent);
    }
    QPainter painter(&p);

    QTextFrameFormat f;
    f.setMargin(2);
    f.setWidth(this->width());//重要！居中之类的需要此值做基准
    f.setHeight(this->height());

    if(item.color.indexOf(QRegularExpression("png|jpg"))==-1)
    {
        f.setBackground(QBrush(QColor(item.color)));
    }
    else {
        QPixmap pix(item.color);
        pix = pix.scaled(width(),height(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        f.setBackground(QBrush(QColor(Qt::transparent),pix));
    }

    QTextDocument doc;
    QTextOption opt = doc.defaultTextOption();
    opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    doc.setDefaultTextOption(opt);
    doc.setPageSize(size());
    doc.setHtml(item.html);
    doc.rootFrame()->setFrameFormat(f);

    QAbstractTextDocumentLayout::PaintContext ctx;
    ctx.clip = QRectF(p.rect());
    doc.documentLayout()->draw(&painter,ctx);
    //p.save(cfgpath+"/66.png");
    pix  = p;
    if(pix.hasAlpha()^this->testAttribute(Qt::WA_TranslucentBackground)){
      emit needreboot(item);
    }
    resize(pix.size());
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);//去边框，不在任务栏显示
    this->show();
    update();

}

void desktopNote::setdata(const LayerItem& i ){
    item = i;
}

void desktopNote::setfinished(const LayerItem& i,const QPixmap& p)
{
    setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);//去边框，不在任务栏显示
    item = i;
    pix = p;
    delete editor;
    //editor->close();
    editor = nullptr;

    savepos();
    emit finished(item);
    if(pix.hasAlpha()^this->testAttribute(Qt::WA_TranslucentBackground)){
        emit needreboot(item);
    }
    else {
        this->showNormal();
    }
    qDebug()<<"desktopnote"<<"setfinished";
}

void desktopNote::paintEvent(QPaintEvent *event)
{
   QPainter painter(this);

    painter.drawPixmap(0,0,pix);
    event->accept();
}

void desktopNote::enterEvent(QEvent *event){
//    add->setGeometry(this->width()-30,this->height()-30,30,30);
//    noteclose->setGeometry(0,this->height()-30,30,30);
//    event->accept();
    if(editor == NULL)
    {
        bar->setGeometry(0,this->height()-30,this->width(),30);
        bar->show();
        event->accept();
    }
}

void desktopNote::leaveEvent(QEvent *event){
 //   add->hide();
//    noteclose->hide();
    if(editor == NULL)
    {
        bar->hide();
        event->accept();
    }
}

void desktopNote::mousePressEvent(QMouseEvent* event)
{
    if(event->button()==Qt::LeftButton )
    {
        oldpos=event->globalPos()-this->pos();
        setCursor(Qt::ClosedHandCursor);
    }
}

void desktopNote::mouseMoveEvent(QMouseEvent * event){
    moved = true;
    move(event->globalPos()-oldpos);//貌似linux要这样
    event->accept();
}

void desktopNote::mouseReleaseEvent(QMouseEvent * event){
    setCursor(Qt::ArrowCursor);
   if(moved) savepos();
   moved =false;
   event->accept();
}

void desktopNote::savepos()
{
    if(item.id.isEmpty()) return;
    QString cfg = cfgpath+"/"+item.id;
    QSettings set(cfg,QSettings::NativeFormat);
    set.setValue("geometry",QVariant(this->geometry()));
    qDebug()<<item.id<<"savepos"<<set.value("geometry",QRect(QApplication::desktop()->width()-120,120,200,250)).toRect();

}

void desktopNote::readpos()
{
    if(item.id.isEmpty()) return;
    QString cfg = cfgpath+"/"+item.id;
    QSettings set(cfg,QSettings::NativeFormat);
    setGeometry(set.value("geometry",QRect(QApplication::desktop()->width()-120,120,200,250)).toRect());
    qDebug()<<item.id<<"readpos"<<set.value("geometry",QRect(QApplication::desktop()->width()-120,120,200,250)).toRect();
}

void desktopNote::delnote(){
    int result;
    result = QMessageBox::question(this,"删除便签？","删除后将无法恢复，是否删除？","否","是");
    if (result!=0) {
        emit notedel(item);
    }
}

void desktopNote:: closeEvent(QCloseEvent* event){
    editcancel();
    event->ignore();
}

void desktopNote::exitprog(){
    int result;
    result = QMessageBox::question(this,"退出程序？","是否关闭Note-lite？","否","是");
    if (result!=0) {
        qApp->quit();
    }
}
