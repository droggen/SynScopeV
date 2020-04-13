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

#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QClipboard>
#include "matlabcodedialog.h"
#include "ui_matlabcodedialog.h"

MatlabCodeDialog::MatlabCodeDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::MatlabCodeDialog)
{
    m_ui->setupUi(this);
}

MatlabCodeDialog::~MatlabCodeDialog()
{
    delete m_ui;
}

void MatlabCodeDialog::changeEvent(QEvent *e)
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

void MatlabCodeDialog::setCode(QString _code)
{
   code=_code;
   m_ui->textEdit_Code->setPlainText(code);
}

void MatlabCodeDialog::on_pushButton_Close_clicked()
{
   close();
}

void MatlabCodeDialog::on_pushButton_Save_clicked()
{
   QString t="Save matlab resample code ";
   QString fileName = QFileDialog::getSaveFileName(this, t,QString(),"Matlab code (*.m)");
   if(!fileName.isNull())
   {
      QFile file(fileName);
      if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
      {
         // Stream writer...
         QTextStream out(&file);

         out << code;
         file.close();
      }
      else
      {
         QMessageBox::critical(this,t+"error", "Cannot write to file");
      }
   }
}

void MatlabCodeDialog::on_pushButton_Copy_clicked()
{
   QClipboard *cb = QApplication::clipboard();
   cb->clear(QClipboard::Clipboard);
   cb->setText(code,QClipboard::Clipboard);
}
