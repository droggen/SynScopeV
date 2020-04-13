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

#include <QFile>
#include <QDir>

#include "helpdialog.h"
#include "ui_helpdialog.h"

#include <stdio.h>



#include "cio.h"

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::HelpDialog)
{
    m_ui->setupUi(this);

   QFrame *mainFrame = new QFrame();
   //mainFrame->setFrameStyle(QFrame::NoFrame);
   //mainFrame->setFrameShadow(QFrame::Raised);
   m_ui->scrollArea->setWidget(mainFrame);
   QVBoxLayout *signalVerticalLayout = new QVBoxLayout();
   mainFrame->setLayout(signalVerticalLayout);



    label = new QLabel();
    label->setWordWrap(true);
    label->setOpenExternalLinks(true);


//    m_ui->scrollArea->setWidget(label);

    signalVerticalLayout->addWidget(label);
    label->setFixedWidth(550);

//    connect(label,SIGNAL(linkActivated(QString)),this,SLOT(linkActivated(QString)));

   QFile *file=new QFile(":/help.html");
   file->open(QIODevice::ReadOnly | QIODevice::Text);

   QByteArray filedata = file->readAll();

   label->setText(QString(filedata));


}

HelpDialog::~HelpDialog()
{
    delete m_ui;
}

void HelpDialog::changeEvent(QEvent *e)
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




void HelpDialog::on_buttonBox_accepted()
{
   close();
}
