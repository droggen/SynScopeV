#ifndef VIDEOPREVIEWDIALOG_H
#define VIDEOPREVIEWDIALOG_H

#include <QDialog>
#include <QImage>
#include <QCloseEvent>

namespace Ui {
    class VideoPreviewDialog;
}

class VideoPreviewDialog : public QDialog {
    Q_OBJECT
public:
    VideoPreviewDialog(QWidget *parent = 0);
    ~VideoPreviewDialog();

    void setImage(QImage &img);
    bool wasCanceled();
    void setInfoText(QString txt);


protected:
   void changeEvent(QEvent *e);
   virtual void closeEvent (QCloseEvent*);
   bool abort;


private:
    Ui::VideoPreviewDialog *ui;

public slots:
   void reject();

private slots:
    void on_pushButton_clicked();
};

#endif // VIDEOPREVIEWDIALOG_H
