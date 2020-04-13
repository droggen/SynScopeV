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

#ifndef MATLABCODEDIALOG_H
#define MATLABCODEDIALOG_H

#include <QtGui/QDialog>

namespace Ui {
    class MatlabCodeDialog;
}

class MatlabCodeDialog : public QDialog {
    Q_OBJECT
public:
    MatlabCodeDialog(QWidget *parent = 0);
    ~MatlabCodeDialog();

    void setCode(QString _code);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MatlabCodeDialog *m_ui;
    QString code;

private slots:
    void on_pushButton_Copy_clicked();
    void on_pushButton_Save_clicked();
    void on_pushButton_Close_clicked();
};

#endif // MATLABCODEDIALOG_H
