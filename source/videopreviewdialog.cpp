#include <Qt>
#include <QPixmap>
#include <QPainter>
#include "videopreviewdialog.h"
#include "ui_videopreviewdialog.h"

#include "stdio.h"

VideoPreviewDialog::VideoPreviewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VideoPreviewDialog)
{
    ui->setupUi(this);



    abort = false;
}

VideoPreviewDialog::~VideoPreviewDialog()
{
    delete ui;
}

void VideoPreviewDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void VideoPreviewDialog::setImage(QImage &img)
{
   QPixmap pixmap(img.size());
   QPainter painter;
   painter.begin(&pixmap);
   painter.drawImage(0,0,img);
   painter.end();

   ui->label->setPixmap(pixmap);
}

void VideoPreviewDialog::on_pushButton_clicked()
{
    abort = true;
}
bool VideoPreviewDialog::wasCanceled()
{
   return abort;
}
void VideoPreviewDialog::setInfoText(QString txt)
{
   ui->label_2->setText(txt);
}

/**
   Blocks closing with the escape key, and instead signal to abort
**/
void VideoPreviewDialog::reject()
{
   abort=true;
}

/**
   Blocks closing with the close menus
**/
void VideoPreviewDialog::closeEvent(QCloseEvent *e)
{
   e->ignore();
}



