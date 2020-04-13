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

#ifndef SAVESYNCFILEDIALOG_H
#define SAVESYNCFILEDIALOG_H

#include <QtGui/QDialog>
#include <vector>

#include "data.h"

namespace Ui {
    class SaveSyncFileDialog;
}


typedef vector<vector<int> > GRAPHSRC;

class SaveSyncFileDialog : public QDialog {
    Q_OBJECT
public:
    SaveSyncFileDialog(QWidget *parent = 0);
    ~SaveSyncFileDialog();

    void SetGraphs(GRAPHSRC g);
    GRAPHSRC GetSaveOrder();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SaveSyncFileDialog *m_ui;

    GRAPHSRC GraphSrc,GraphToSave;

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_Down_clicked();
    void on_pushButton_Up_clicked();
    void on_listWidget_Sources_currentRowChanged(int currentRow);
};

#endif // SAVESYNCFILEDIALOG_H
