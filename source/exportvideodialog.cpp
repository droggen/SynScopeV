#include "exportvideodialog.h"
#include "ui_exportvideodialog.h"



ExportVideoDialog::ExportVideoDialog(const PLOTVIEWPARAM &p, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ExportVideoDialog)
{
    m_ui->setupUi(this);

    m_ui->spinBox_Width->setValue(p.videow);
    m_ui->spinBox_Height->setValue(p.videoh);
}

ExportVideoDialog::~ExportVideoDialog()
{
    delete m_ui;
}

void ExportVideoDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
PLOTVIEWPARAM ExportVideoDialog::getParam()
{
   PLOTVIEWPARAM p;
   p.videow = m_ui->spinBox_Width->value();
   p.videoh = m_ui->spinBox_Height->value();
   p.bitrate= m_ui->spinBox_Bitrate->value();
   p.gop = m_ui->spinBox_Gop->value();
   p.signalstep = m_ui->spinBox_SignalStep->value();
   p.drawtitle = m_ui->checkBox_Title->isChecked();
   p.scale = m_ui->checkBox_Double->isChecked() ? 2:1;
   return p;
}
/*
unsigned ExportVideoDialog::getWidth()
{
   return m_ui->spinBox_Width->value();
}
unsigned ExportVideoDialog::getHeight()
{
   return m_ui->spinBox_Height->value();
}
unsigned ExportVideoDialog::getBitrate()
{
   return m_ui->spinBox_Bitrate->value();
}
unsigned ExportVideoDialog::getGop()
{
   return m_ui->spinBox_Gop->value();
}
unsigned ExportVideoDialog::getSignalStep()
{
   return m_ui->spinBox_SignalStep->value();
}

bool ExportVideoDialog::getTitle()
{
   return m_ui->checkBox_Title->isChecked();
}

bool ExportVideoDialog::getDouble()
{
   return m_ui->checkBox_Double->isChecked();
}*/



/**
  Return the parameters for the video encoding: frame size, bitrate, etc
**/
bool GetVideoParameters(PLOTVIEWPARAM &param)
{
   QScopedPointer<ExportVideoDialog> dlg(new ExportVideoDialog(param));
   if(dlg->exec() == QDialog::Rejected)
      return false;
   param = dlg->getParam();
   // This is not present in the video dialog, so make them default to true
   param.center=true;
   param.drawhaxis=true;
   param.drawvaxis=true;
   param.drawframe=true;
 return true;
}
