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

#include "resampledialog.h"
#include "ui_resampledialog.h"

ResampleDialog::ResampleDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ResampleDialog)
{
    m_ui->setupUi(this);
}

ResampleDialog::~ResampleDialog()
{
    delete m_ui;
}

void ResampleDialog::changeEvent(QEvent *e)
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

int ResampleDialog::getMethod()
{
   if(m_ui->radioButton_M_NN->isChecked())
      return 0;
   return 1;
}
int ResampleDialog::getStrategy()
{
   if(m_ui->radioButton_S_ROP->isChecked())
      return 2;
   if(m_ui->radioButton_S_RO->isChecked())
      return 1;
   return 0;
}
void ResampleDialog::SetC()
{
   m_ui->radioButton_M_NN->setChecked(true);
   m_ui->radioButton_M_UD->setEnabled(false);
}
void ResampleDialog::SetMatlab()
{
}

