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

#ifndef REFERENCESDIALOG_H
#define REFERENCESDIALOG_H

#include <QtGui/QDialog>
#include <QTableWidgetItem>
#include <QListWidgetItem>
#include <vector>

#include "data.h"

namespace Ui {
    class ReferencesDialog;
}

class ReferencesDialog : public QDialog {
    Q_OBJECT
public:
    ReferencesDialog(QWidget *parent = 0);
    ~ReferencesDialog();

    void SetReferencePoints(REFERENCEPOINTS &ref);
    void InitRelationsUI(int numsignalsource,int numvideosource=0);
    void SetRelationsAvailable(RELATIONS r);
    RELATIONS GetRelationsActive();
    bool GetLinkState();

    void ClearLinks();

signals:
    void AddReference();
    void RemoveReference(int idx);           // idx>=0 indicates selected row. idx=-1 indicates clear all references.
    void ReferenceSelected(int idx);
    void ChangedActiveRelations();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ReferencesDialog *m_ui;

    int NumSignalSource,NumVideoSource;
    QGridLayout *gridLayout_Link;
    vector<vector<QWidget*> > LinkWidgets;

    QString IDX2Txt(int idx);






private slots:
   void on_pushButton_Clear_clicked();
   void on_checkBox_EnableSignalLinking_toggled(bool checked);
   void on_pushButton_Remove_clicked();
   void on_pushButton_Add_clicked();

   /*void on_itemActivated ( QListWidgetItem * item );
   void on_itemChanged ( QListWidgetItem * item );
   void on_itemClicked ( QListWidgetItem * item );
   void on_itemDoubleClicked ( QListWidgetItem * item );
   void on_itemEntered ( QListWidgetItem * item );
   void on_itemPressed ( QListWidgetItem * item );*/
   void on_currentRowChanged ( int currentRow );



    void on_LinkUIStateChanged(int state);      // Receives link UI checkbox signals
};

#endif // REFERENCESDIALOG_H
