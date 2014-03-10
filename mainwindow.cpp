#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore>
#include <QtGui>
#include <qdom.h>
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, SIGNAL(clicked()),this, SLOT(loadPNG()));
    connect(ui->buttonLoadPLIST, SIGNAL(clicked()),this, SLOT(loadPLIST()));
    connect(ui->getPixButton, SIGNAL(clicked()), this, SLOT(getPixtures()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadPNG()
{
    QString path = QFileDialog::getOpenFileName(this, tr("open Image"), "/Users/fuzhuo/Documents/output/", "Image Files(*.png)");
    if(path.length()==0) return;
    qDebug()<<"loadPNG"<<path;
    int index = path.indexOf(".png");
    qDebug()<<index<<",len="<<path.length();
    if(index != path.length()-4){
        QMessageBox::information(NULL, tr("error"), tr("not a png file"));
        return;
    }
    this->pixFileName = path;
    this->ui->pixNameLabel->setText(path);

    //pix =QPixmap("/Users/fuzhuo/Documents/beding-hd.png");
    pix =QPixmap(path);
    QSize size = ui->label->size();
    QPixmap pix2;
    if(pix.size().width()/size.width() >= pix.size().height()/size.height())
        pix2 = pix.scaledToWidth(size.width());
    else pix2 = pix.scaledToHeight(size.height());
    //QPixmap pix2=pix.scaled(size.width(),size.height());
    ui->label->setPixmap(pix2);
}

void MainWindow::loadPLIST()
{
    QString path = QFileDialog::getOpenFileName(this, tr("open plist file"), "/Users/fuzhuo/Documents/output/", "Plist Files(*.plist)");
    if(path.length()==0) return;
    qDebug()<<"load plist file";
    int index = path.indexOf(".plist");
    qDebug()<<index<<",len="<<path.length();
    if(index != path.length()-6){
        QMessageBox::information(NULL, tr("error"), tr("not a plist file"));
        return;
    }
    this->plistFileName = path;
    this->ui->plistNameLabel->setText(path);
    //parsePlistFile("/Users/fuzhuo/Documents/being-hd.plist");
    return;
}

void MainWindow::parsePlistFile(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        qDebug()<<"error open file:"<<fileName;
        return;
    }
    const QByteArray &buffer = file.readAll();
    QString errorstr;
    int errorLine;
    int errorCol;
    QDomDocument doc;
    if(!doc.setContent(buffer,false, &errorstr, &errorLine, &errorCol)){
        qDebug()<<"open doc file"<<errorstr<<","<<errorLine<<","<<errorCol;
        return;
    }
    qDebug("output");
    QDomElement root = doc.documentElement();
    QDomNode child = root.firstChild();//<dict>
    QDomNode pixsNode = child.firstChild().nextSibling();//<dict> for all files
    QDomNode pixNameNode = pixsNode.firstChild();
    QDomNode pixInfoNode = pixNameNode.nextSibling();
    child = pixsNode.firstChild();
    while(!child.isNull()){
        pixNameNode = child;
        pixInfoNode = child.nextSibling();
        QString fName = pixNameNode.toElement().text();
        //qDebug()<<"pixNameNode:"<< fName;
        QDomNode info = pixInfoNode.firstChild().nextSibling();
        QString infoStr = info.toElement().text();
        //qDebug()<<"info:"<<infoStr;
        QString xs,ys,ws,hs;
        int start = 2;
        int end = infoStr.indexOf(',',start);
        xs=infoStr.mid(start,end-start);
        start=end+1;

        end = infoStr.indexOf('}',start);
        ys=infoStr.mid(start,end-start);
        start=end+3;

        end = infoStr.indexOf(',',start);
        ws=infoStr.mid(start,end-start);
        start=end+1;

        end = infoStr.indexOf('}',start);
        hs=infoStr.mid(start,end-start);
        int x=xs.toInt();
        int y=ys.toInt();
        int w=ws.toInt();
        int h=hs.toInt();
        qDebug()<<x<<","<<y<<","<<w<<","<<h;
        QDomNode rotateNode=info.nextSibling().nextSibling().nextSibling().nextSibling();
        QString needRotate = rotateNode.toElement().tagName();
        qDebug()<<"needRotate:"<<needRotate;
        bool isNeedRotate=false;
        if(needRotate == "true"){
            isNeedRotate = true;
            qDebug("rotate");
            w=hs.toInt();
            h=ws.toInt();
        }
        QPixmap savePix = pix.copy(x,y,w,h);
        QImage img = savePix.toImage();
        if(isNeedRotate){
            QMatrix matrix;
            matrix.rotate(-90.0);
            img = img.transformed(matrix);
        }

        QString path = QString("%1/%2").arg(outputPathName).arg(fName);
        img.save(path, "PNG");
        child=pixInfoNode.nextSibling();
    }
    qDebug("finish it!!!");
}

void MainWindow::getPixtures()
{
    if(pixFileName.length()==0 || plistFileName.length()==0){
        qDebug("error png or plist file not select");
        QMessageBox::information(NULL, tr("error"), tr("png or plist file not select"));
        return;
    }
    QString dir = QFileDialog::getExistingDirectory(this, tr("Selet ouput directory"), "/Users/fuzhuo/Documents/output",
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(dir.length()==0) {
        qDebug("error png or plist file not select");
        QMessageBox::information(NULL, tr("error"), tr("not a nice path"));
        return;
    }
    this->outputPathName = dir;
    qDebug()<<"output:"<<outputPathName;
    this->ui->outputPathEdit->setText(dir);
    parsePlistFile(plistFileName);
}
