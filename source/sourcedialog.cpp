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

#include <stdio.h>

#include "sourcedialog.h"
#include "ui_sourcedialog.h"


SourceDialog::SourceDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SourceDialog)
{
    m_ui->setupUi(this);
}

SourceDialog::~SourceDialog()
{
    delete m_ui;
}

void SourceDialog::changeEvent(QEvent *e)
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

void SourceDialog::SetSignalData(std::vector<SIGNALDATA> &SignalData)
{
   // Fill the content of the table.
   m_ui->tableWidget->setRowCount(SignalData.size());
   for(unsigned i=0;i<SignalData.size();i++)
   {     
      m_ui->tableWidget->setItem(i,0,new QTableWidgetItem(SignalData[i].filename));
      QString str;
      str.sprintf("%d",SignalData[i].data.size());
      m_ui->tableWidget->setItem(i,1,new QTableWidgetItem(str));
      str.sprintf("%d",SignalData[i].data[0].size());
      m_ui->tableWidget->setItem(i,2,new QTableWidgetItem(str));

      // Fill the headers
      str.sprintf("Source %d",i);
      m_ui->tableWidget->setVerticalHeaderItem(i,new QTableWidgetItem(str));
   }


}



