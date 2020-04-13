#ifndef EXPORTMOSAICVIDEODIALOG_H
#define EXPORTMOSAICVIDEODIALOG_H

#include <QtGui/QDialog>
#include "data.h"

namespace Ui {
    class ExportMosaicVideoDialog;
}


typedef struct
{
   // input
   QString name;
   VIEWTYPE vt;
   unsigned w,h;
   // output
   unsigned px,py;
   double scale;
   bool render;
   int view;
} MOSAICWINDOWPARAM;


class ExportMosaicVideoDialog : public QDialog {
    Q_OBJECT
public:
    ExportMosaicVideoDialog(QWidget *parent = 0);
    ~ExportMosaicVideoDialog();

   void SetParam(std::vector<MOSAICWINDOWPARAM> _mvp,std::vector<MOSAICWINDOWPARAM> _msp);
   void GetParam(std::vector<MOSAICWINDOWPARAM> &_mvp,std::vector<MOSAICWINDOWPARAM> &_msp,int &bitrate,int &gop,int &step,bool &title,bool &plothaxis,bool &plotvaxis,bool &plotframe,bool &transparent,bool &centercurrentsample,bool &globaldouble);


protected:
    void changeEvent(QEvent *e);
    bool CheckValues();
    bool CheckVideoValues();
    bool DataOk(QString s,int &v);
    void SetupSignalTable();
    void SetupVideoTable();

    std::vector<MOSAICWINDOWPARAM> mvp;
    std::vector<MOSAICWINDOWPARAM> msp;

private:
    Ui::ExportMosaicVideoDialog *m_ui;


   public slots:
      virtual void accept();

private slots:
    void on_pushButton_SetSigalX_clicked();
    void on_pushButton_OrganizeVideoVertically_clicked();
    void on_pushButton_RenderVideoNone_clicked();
    void on_pushButton_RenderVideoAll_clicked();
    void on_pushButton_RenderNone_clicked();
    void on_pushButton_RenderAll_clicked();
    void on_pushButton_clicked();
    void on_pushButton_ApplyAllSignal_clicked();

};

#endif // EXPORTMOSAICVIDEODIALOG_H
