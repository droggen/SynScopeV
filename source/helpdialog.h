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

#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QtGui/QDialog>
#include <QLabel>

namespace Ui {
    class HelpDialog;
}

class HelpDialog : public QDialog {
    Q_OBJECT
public:
    HelpDialog(QWidget *parent = 0);
    ~HelpDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::HelpDialog *m_ui;

    QLabel *label;

private slots:
    //void on_pushButton_clicked();
    //void linkActivated(QString);

private slots:

private slots:

private slots:
    void on_buttonBox_accepted();
};

#endif // HELPDIALOG_H
