/*
   SynScopeV

   Copyright (C) 2008,2009:
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

#include "savesyncfiledialog.h"
#include "ui_savesyncfiledialog.h"



SaveSyncFileDialog::SaveSyncFileDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SaveSyncFileDialog)
{
    m_ui->setupUi(this);
}

SaveSyncFileDialog::~SaveSyncFileDialog()
{
    delete m_ui;
}

void SaveSyncFileDialog::changeEvent(QEvent *e)
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


void SaveSyncFileDialog::SetGraphs(GRAPHSRC g)
{

   GraphSrc = g;

   for(int i=0;i<GraphSrc.size();i++)
   {
      QString str;
      str=GraphToString(GraphSrc[i]);
      m_ui->listWidget_Sources->addItem(str);
   }
   // Select the first graph
   m_ui->listWidget_Sources->setCurrentRow(0);

}


/**
  \brief handles selection of a row in the graphs list box. Updates the oder-of-storage list box
**/
void SaveSyncFileDialog::on_listWidget_Sources_currentRowChanged(int currentRow)
{
   // clear
   m_ui->listWidget_Order->clear();
   // fill
   for(int i=0;i<GraphSrc[currentRow].size();i++)
      m_ui->listWidget_Order->addItem(QString("%1").arg(GraphSrc[currentRow][i]));
}



void SaveSyncFileDialog::on_pushButton_Down_clicked()
{
   // Find selected row. If non null, then swap entries in GraphSrc, update listbox
   int g = m_ui->listWidget_Sources->currentRow();
   int r = m_ui->listWidget_Order->currentRow();
   if(r==GraphSrc[g].size()-1)
      return;
   int t= GraphSrc[g][r];
   GraphSrc[g][r]=GraphSrc[g][r+1];
   GraphSrc[g][r+1]=t;
   on_listWidget_Sources_currentRowChanged(g);
   m_ui->listWidget_Order->setCurrentRow(r+1);
}

void SaveSyncFileDialog::on_pushButton_Up_clicked()
{
   // Find selected row. If non null, then swap entries in GraphSrc, update listbox
   int g = m_ui->listWidget_Sources->currentRow();
   int r = m_ui->listWidget_Order->currentRow();
   if(r==0)
      return;
   int t= GraphSrc[g][r];
   GraphSrc[g][r]=GraphSrc[g][r-1];
   GraphSrc[g][r-1]=t;
   on_listWidget_Sources_currentRowChanged(g);
   m_ui->listWidget_Order->setCurrentRow(r-1);
}

GRAPHSRC SaveSyncFileDialog::GetSaveOrder()
{
   return GraphToSave;
}

/**
  \brief Close without saving.
**/
void SaveSyncFileDialog::on_pushButton_3_clicked()
{
   done(0);
}

/**
  \brief Save the selected linked graph
**/
void SaveSyncFileDialog::on_pushButton_clicked()
{
   GraphToSave.clear();
   GraphToSave.push_back(GraphSrc[m_ui->listWidget_Sources->currentRow()]);
   done(1);
}

/**
  \brief Save all the linked graphs
**/
void SaveSyncFileDialog::on_pushButton_2_clicked()
{
   GraphToSave = GraphSrc;
   done(2);
}
