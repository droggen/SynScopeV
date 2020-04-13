/*
   SynScopeV

   Copyright (C) 2008,2009,2010:
         Daniel Roggen, droggen@gmail.com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <QMessageBox>
#include "exportmosaicvideodialog.h"
#include "ui_exportmosaicvideodialog.h"

#include "stdio.h"


ExportMosaicVideoDialog::ExportMosaicVideoDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ExportMosaicVideoDialog)
{
   m_ui->setupUi(this);



   m_ui->checkbox_GlobalDouble->setCheckState(Qt::Checked);

   m_ui->checkBox_Title->setCheckState(Qt::Checked);
   m_ui->checkBox_PlotHAxis->setCheckState(Qt::Checked);
   m_ui->checkBox_PlotVAxis->setCheckState(Qt::Checked);
   m_ui->checkBox_PlotFrame->setCheckState(Qt::Checked);
   m_ui->checkBox_Transparent->setCheckState(Qt::Unchecked);
}

ExportMosaicVideoDialog::~ExportMosaicVideoDialog()
{
    delete m_ui;
}

void ExportMosaicVideoDialog::changeEvent(QEvent *e)
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
void ExportMosaicVideoDialog::SetParam(std::vector<MOSAICWINDOWPARAM> _mvp,std::vector<MOSAICWINDOWPARAM> _msp)
{
   msp=_msp;
   mvp=_mvp;

   printf("ExportMosaicVideoDialog::SetParam\n");
   SetupSignalTable();
   SetupVideoTable();
}
void ExportMosaicVideoDialog::SetupSignalTable()
{
   m_ui->tableWidget_Signal->setColumnCount(6);
   m_ui->tableWidget_Signal->setRowCount(msp.size());
   for(unsigned i=0;i<msp.size();i++)
   {
      m_ui->tableWidget_Signal->setItem(i,0,new QTableWidgetItem(msp[i].name));
      m_ui->tableWidget_Signal->item(i,0)->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
      m_ui->tableWidget_Signal->item(i,0)->setCheckState(msp[i].render?Qt::Checked:Qt::Unchecked);

      m_ui->tableWidget_Signal->setItem(i,1,new QTableWidgetItem(QString("%1").arg(msp[i].px)));
      m_ui->tableWidget_Signal->item(i,1)->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);

      m_ui->tableWidget_Signal->setItem(i,2,new QTableWidgetItem(QString("%1").arg(msp[i].py)));
      m_ui->tableWidget_Signal->item(i,2)->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);

      m_ui->tableWidget_Signal->setItem(i,3,new QTableWidgetItem(QString("%1").arg(msp[i].w)));
      m_ui->tableWidget_Signal->item(i,3)->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);

      m_ui->tableWidget_Signal->setItem(i,4,new QTableWidgetItem(QString("%1").arg(msp[i].h)));
      m_ui->tableWidget_Signal->item(i,4)->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);

      m_ui->tableWidget_Signal->setItem(i,5,new QTableWidgetItem(""));
      m_ui->tableWidget_Signal->item(i,5)->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
      m_ui->tableWidget_Signal->item(i,5)->setCheckState((msp[i].scale==2)?Qt::Checked:Qt::Unchecked);
   }
}
void ExportMosaicVideoDialog::SetupVideoTable()
{
   m_ui->tableWidget_Video->setColumnCount(6);
   m_ui->tableWidget_Video->setRowCount(mvp.size());
   printf("Setup video table. How many entrues: %d\n",mvp.size());
   for(unsigned i=0;i<mvp.size();i++)
   {
      m_ui->tableWidget_Video->setItem(i,0,new QTableWidgetItem(mvp[i].name));
      m_ui->tableWidget_Video->item(i,0)->setFlags(Qt::ItemIsUserCheckable|Qt::ItemIsEnabled);
      m_ui->tableWidget_Video->item(i,0)->setCheckState(mvp[i].render?Qt::Checked:Qt::Unchecked);

      m_ui->tableWidget_Video->setItem(i,1,new QTableWidgetItem(QString("%1").arg(mvp[i].px)));
      m_ui->tableWidget_Video->item(i,1)->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);

      m_ui->tableWidget_Video->setItem(i,2,new QTableWidgetItem(QString("%1").arg(mvp[i].py)));
      m_ui->tableWidget_Video->item(i,2)->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);

      m_ui->tableWidget_Video->setItem(i,3,new QTableWidgetItem(QString("%1").arg(mvp[i].w)));
      m_ui->tableWidget_Video->item(i,3)->setFlags(0);

      m_ui->tableWidget_Video->setItem(i,4,new QTableWidgetItem(QString("%1").arg(mvp[i].h)));
      m_ui->tableWidget_Video->item(i,4)->setFlags(0);

      m_ui->tableWidget_Video->setItem(i,5,new QTableWidgetItem(QString("%1").arg(mvp[i].scale)));
      m_ui->tableWidget_Video->item(i,5)->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);
   }
}

void ExportMosaicVideoDialog::GetParam(std::vector<MOSAICWINDOWPARAM> &_mvp,std::vector<MOSAICWINDOWPARAM> &_msp,int &bitrate,int &gop,int &step,bool &title,bool &plothaxis,bool &plotvaxis,bool &plotframe,bool &transparent,bool &centercurrentsample,bool &globaldouble)
{
   _msp=msp;
   _mvp=mvp;
   bitrate = m_ui->spinBox_Bitrate->value();
   gop = m_ui->spinBox_Gop->value();
   step = m_ui->spinBox_Step->value();
   title = m_ui->checkBox_Title->isChecked();
   plothaxis = m_ui->checkBox_PlotHAxis->isChecked();
   plotvaxis = m_ui->checkBox_PlotVAxis->isChecked();
   plotframe = m_ui->checkBox_PlotFrame->isChecked();
   transparent = m_ui->checkBox_Transparent->isChecked();
   centercurrentsample = m_ui->checkBox_CenterCurrentSample->isChecked();
   globaldouble = m_ui->checkbox_GlobalDouble->isChecked();
}
/**
   Check the signal values in the table for consistency and store them into the data structure
**/
bool ExportMosaicVideoDialog::CheckValues()
{
   QString title="Export video mosaic";
   for(int i=0;i<m_ui->tableWidget_Signal->rowCount();i++)
   {
      QString err = QString("Error: invalid coordinates/size for signal %1").arg(i);
      int v;
      if(!DataOk(m_ui->tableWidget_Signal->item(i,1)->text(),v)
         || !DataOk(m_ui->tableWidget_Signal->item(i,2)->text(),v)
         || !DataOk(m_ui->tableWidget_Signal->item(i,3)->text(),v)
         || !DataOk(m_ui->tableWidget_Signal->item(i,4)->text(),v))
      {
         m_ui->tableWidget_Signal->selectRow(i);
         QMessageBox::information(this,title,err);
         return false;
      }
      msp[i].render = m_ui->tableWidget_Signal->item(i,0)->checkState()==Qt::Checked;
      DataOk(m_ui->tableWidget_Signal->item(i,1)->text(),v);
      msp[i].px = v;
      DataOk(m_ui->tableWidget_Signal->item(i,2)->text(),v);
      msp[i].py = v;
      DataOk(m_ui->tableWidget_Signal->item(i,3)->text(),v);
      msp[i].w = v;
      DataOk(m_ui->tableWidget_Signal->item(i,4)->text(),v);
      msp[i].h = v;
      msp[i].scale = (m_ui->tableWidget_Signal->item(i,5)->checkState()==Qt::Checked)?2.0:1.0;

   }
   return true;
}
/**
   Check the signal values in the table for consistency and store them into the data structure
**/
bool ExportMosaicVideoDialog::CheckVideoValues()
{
   QString title="Export video mosaic";
   for(int i=0;i<m_ui->tableWidget_Video->rowCount();i++)
   {
      QString err = QString("Error: invalid coordinates for video %1").arg(i);
      QString err2 = QString("Error: invalid scale for video %1").arg(i);

      int v;
      if(!DataOk(m_ui->tableWidget_Video->item(i,1)->text(),v)
         || !DataOk(m_ui->tableWidget_Video->item(i,2)->text(),v))
      {
         m_ui->tableWidget_Video->selectRow(i);
         QMessageBox::information(this,title,err);
         return false;
      }
      double scale;
      bool ok;
      scale = m_ui->tableWidget_Video->item(i,5)->text().toDouble(&ok);
      if(!ok)
      {
         m_ui->tableWidget_Video->selectRow(i);
         QMessageBox::information(this,title,err2);
         return false;
      }
      mvp[i].render = m_ui->tableWidget_Video->item(i,0)->checkState()==Qt::Checked;
      DataOk(m_ui->tableWidget_Video->item(i,1)->text(),v);
      mvp[i].px = v;
      DataOk(m_ui->tableWidget_Video->item(i,2)->text(),v);
      mvp[i].py = v;
      mvp[i].scale = scale;
   }
   return true;
}
/**
   Returns true if the string is an integer >=0.
**/
bool ExportMosaicVideoDialog::DataOk(QString s,int &v)
{
   bool ok;
   v = s.toInt(&ok);
   if(v<0)
      ok=false;
   return ok;
}

void ExportMosaicVideoDialog::accept()
{
   printf("Accept slot\n");
   if(!CheckValues())
      return;
   if(!CheckVideoValues())
      return;
   done(QDialog::Accepted);
}





/*
  Sets the width, height, and double parameters of all the signals.
*/
void ExportMosaicVideoDialog::on_pushButton_ApplyAllSignal_clicked()
{
   int w = m_ui->spinBox_Width->value();
   int h = m_ui->spinBox_Height->value();
   bool dbl = m_ui->checkBox_Double->checkState()==Qt::Checked;
   for(unsigned i=0;i<msp.size();i++)
   {
      msp[i].w = w;
      msp[i].h = h;
      msp[i].scale = dbl?2.0:1.0;
   }
   SetupSignalTable();
}

/*
  Sets the signal video in a vertical organization
*/
void ExportMosaicVideoDialog::on_pushButton_clicked()
{
   if(!CheckValues())
      return;

   int vs = m_ui->spinBox_VerticalSpacing->value();
   int yp=0;

   for(unsigned i=0;i<msp.size();i++)
   {
      if(!msp[i].render)
         continue;
      msp[i].px=0;
      msp[i].py=yp;
      yp += vs + msp[i].h*(msp[i].scale);
   }
   SetupSignalTable();
}



/**
   Set Render checkbox for all signals
**/
void ExportMosaicVideoDialog::on_pushButton_RenderAll_clicked()
{
   if(!CheckValues())
      return;
   for(unsigned i=0;i<msp.size();i++)
   {
      msp[i].render=true;
   }
   SetupSignalTable();
}
/**
   Clear Render checkbox for all signals
**/
void ExportMosaicVideoDialog::on_pushButton_RenderNone_clicked()
{
   if(!CheckValues())
      return;
   for(unsigned i=0;i<msp.size();i++)
   {
      msp[i].render=false;
   }
   SetupSignalTable();
}
/**
   Set Render checkbox for all videos
**/
void ExportMosaicVideoDialog::on_pushButton_RenderVideoAll_clicked()
{
   if(!CheckVideoValues())
      return;
   for(unsigned i=0;i<mvp.size();i++)
   {
      mvp[i].render=true;
   }
   SetupVideoTable();
}
/**
   Clear Render checkbox for all videos
**/
void ExportMosaicVideoDialog::on_pushButton_RenderVideoNone_clicked()
{
   if(!CheckVideoValues())
      return;
   for(unsigned i=0;i<mvp.size();i++)
   {
      mvp[i].render=false;
   }
   SetupVideoTable();
}
/**
   Organize the video vertically
**/
void ExportMosaicVideoDialog::on_pushButton_OrganizeVideoVertically_clicked()
{
   if(!CheckVideoValues())
      return;

   int vs = m_ui->spinBox_VideoVerticalSpacing->value();
   int yp=0;

   for(unsigned i=0;i<mvp.size();i++)
   {
      if(!mvp[i].render)
         continue;
      mvp[i].px=0;
      mvp[i].py=yp;
      yp += vs + mvp[i].h*mvp[i].scale;
   }
   SetupVideoTable();
}

/**
   Set all the signal X value to:
**/
void ExportMosaicVideoDialog::on_pushButton_SetSigalX_clicked()
{
   if(!CheckValues())
      return;

   bool ok;
   int x = m_ui->lineEdit_SignalValue->text().toInt(&ok);
   if(!ok || x<0)
      return;

   for(unsigned i=0;i<msp.size();i++)
   {
      msp[i].px = x;
   }

   SetupSignalTable();
}





