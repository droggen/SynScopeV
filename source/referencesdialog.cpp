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

#include <QLabel>
#include <QCheckBox>
#include <QSpacerItem>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>

#include "referencesdialog.h"
#include "ui_referencesdialog.h"

#include "cio.h"

ReferencesDialog::ReferencesDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ReferencesDialog)
{
   m_ui->setupUi(this);

   NumSignalSource=0;
   NumVideoSource=0;

   gridLayout_Link = new QGridLayout(m_ui->LinkUIFrame);



   int success;



   /*success=connect(m_ui->listWidget,SIGNAL(itemActivated(QListWidgetItem *)),this,SLOT(on_itemActivated(QListWidgetItem *)));
   success=connect(m_ui->listWidget,SIGNAL(itemChanged(QListWidgetItem *)),this,SLOT(on_itemChanged(QListWidgetItem *)));
   success=connect(m_ui->listWidget,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(on_itemClicked(QListWidgetItem *)));
   success=connect(m_ui->listWidget,SIGNAL(itemDoubleClicked(QListWidgetItem *)),this,SLOT(on_itemDoubleClicked(QListWidgetItem *)));
   success=connect(m_ui->listWidget,SIGNAL(itemEntered(QListWidgetItem *)),this,SLOT(on_itemEntered(QListWidgetItem *)));
   success=connect(m_ui->listWidget,SIGNAL(itemPressed(QListWidgetItem *)),this,SLOT(on_itemPressed(QListWidgetItem *)));*/

   success=connect(m_ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(on_currentRowChanged(int)));






}

ReferencesDialog::~ReferencesDialog()
{
    delete m_ui;
    delete gridLayout_Link;
}

void ReferencesDialog::changeEvent(QEvent *e)
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

void ReferencesDialog::on_pushButton_Add_clicked()
{
   emit AddReference();
}

void ReferencesDialog::on_pushButton_Remove_clicked()
{
   if(m_ui->listWidget->currentRow()!=-1)
      emit RemoveReference(m_ui->listWidget->currentRow());
}
QString ReferencesDialog::IDX2Txt(int idx)
{
   if(idx<NumVideoSource)
      return QString("V%1").arg(idx);
   return QString("S%1").arg(idx-NumVideoSource);

}
void ReferencesDialog::SetReferencePoints(REFERENCEPOINTS &ref)
{
   //m_ui->tableWidget->setRowCount(ref.size());
   //m_ui->tableWidget->setColumnCount(4);

   m_ui->listWidget->clear();

   /*for(int i=0;i<ref.size();i++)
   {
      for(int j=0;j<4;j++)
      {
         QString t;
         t=QString("%1").arg(ref[i][j]);
         QTableWidgetItem *newItem = new QTableWidgetItem(t);
         m_ui->tableWidget->setItem(i,j,newItem);
      }
   }*/

   // Do the same on the list widget
   for(int i=0;i<ref.size();i++)
   {
      QString str;
      //str.sprintf("%s:%d\t->\t%s:%d",IDX2Txt(ref[i][0]),ref[i][1],IDX2Txt(ref[i][2]),ref[i][3]);
      str=QString("%1:%2\t->\t%3:%4").arg(IDX2Txt(ref[i][0])).arg(ref[i][1]).arg(IDX2Txt(ref[i][2])).arg(ref[i][3]);
      m_ui->listWidget->addItem(str);
   }
}




/**
  \brief Inits the signal linking user interface.
**/
void ReferencesDialog::InitRelationsUI(int numsignalsource,int numvideosource)
{
   // If the number of sources is unchanged, avoid re-initializing the GUI to keep CB states
   if(NumSignalSource==numsignalsource && NumVideoSource==numvideosource)
      return;
   NumSignalSource = numsignalsource;
   NumVideoSource = numvideosource;

   // Delete old link UI widgets
   for(int i=0;i<LinkWidgets.size();i++)
   {
      for(int j=0;j<LinkWidgets[i].size();j++)
      {
         if(LinkWidgets[i][j])
         {
            gridLayout_Link->removeWidget(LinkWidgets[i][j]);
            delete LinkWidgets[i][j];
            LinkWidgets[i][j]=0;
         }
      }
   }
   LinkWidgets.clear();

   // If the total number of sources is zero or one, return.
   if(NumSignalSource+NumVideoSource==0 || NumSignalSource+NumVideoSource==1)
   {
      m_ui->checkBox_EnableSignalLinking->setEnabled(false);
      return;
   }
   m_ui->checkBox_EnableSignalLinking->setEnabled(true);

   // Create the new link UI widgets
   LinkWidgets.resize(NumSignalSource+NumVideoSource,vector<QWidget*>(NumSignalSource+NumVideoSource,static_cast<QWidget*>(0)));

   // Set the title label
   LinkWidgets[0][0] = new QLabel("Link");
   LinkWidgets[0][0]->setSizePolicy( QSizePolicy::Maximum,QSizePolicy::Maximum);
   gridLayout_Link->addWidget(LinkWidgets[0][0],0,0,1,1);


   // Set the left labels
   for(int i=0;i<NumSignalSource+NumVideoSource-1;i++)
   {
      QString str;
      if(i<NumVideoSource)
         str.sprintf("V%d",i);
      else
         str.sprintf("S%d",i-NumVideoSource);
      LinkWidgets[1+i][0] = new QLabel(str);
      LinkWidgets[1+i][0]->setSizePolicy( QSizePolicy::Maximum,QSizePolicy::Maximum);
      gridLayout_Link->addWidget(LinkWidgets[1+i][0],1+i,0,1,1);
   }



   // Set the top labels
   for(int i=0;i<NumSignalSource+NumVideoSource-1;i++)
   {
      QString str;
      if(i+1<NumVideoSource)
         str.sprintf("V%d",i+1);
      else
         str.sprintf("S%d",i-NumVideoSource+1);
      LinkWidgets[0][1+i] = new QLabel(str);
      LinkWidgets[0][1+i]->setSizePolicy( QSizePolicy::Maximum,QSizePolicy::Maximum);
      gridLayout_Link->addWidget(LinkWidgets[0][1+i],0,1+i,1,1);
   }

   // Create the checkboxes
   for(int i=0;i<NumSignalSource+NumVideoSource-1;i++)
   {
      for(int j=0;j<NumSignalSource+NumVideoSource-1;j++)
      {
         if(j<i)
            continue;
         LinkWidgets[1+i][1+j] = new QCheckBox();
         gridLayout_Link->addWidget(LinkWidgets[1+i][1+j],1+i,1+j,1,1);
         LinkWidgets[1+i][1+j]->setSizePolicy( QSizePolicy::Maximum,QSizePolicy::Maximum);
         int success=connect(LinkWidgets[1+i][1+j],SIGNAL(stateChanged(int)),this,SLOT(on_LinkUIStateChanged(int)));
      }
   }

   /*printf("Dumping my stored widgets\n");
   for(int i=0;i<LinkWidgets.size();i++)
   {
      for(int j=0;j<LinkWidgets[i].size();j++)
      {
         printf("(%d-%d;%p) ",i,j,LinkWidgets[i][j]);
      }
      printf("\n");
   }

   printf("Dumping grid layout widgets\n");
   printf("Number of widgets: %d. %d x %d\n",gridLayout_Link->count(),gridLayout_Link->columnCount(),gridLayout_Link->rowCount());
   for(int i=0;i<gridLayout_Link->count();i++)
   {
      printf("%d %p. ",i,gridLayout_Link->itemAt(i)->widget());
      int r,c,rs,cs;
      gridLayout_Link->getItemPosition(i,&r,&c,&rs,&cs);
      printf("r,c: %d,%d rs,cs: %d,%d\n",r,c,rs,cs);
   }*/





   /*for(int i=0;i<numsource;i++)
   {
      gridLayout_Link->removeWidget(LinkWidgets[numsource/2][i]);
      delete LinkWidgets[numsource/2][i];
   }*/



}

/**
  \brief Reflects the available relations in the signal linking UI

  Also fills the equation widget.

**/
void ReferencesDialog::SetRelationsAvailable(RELATIONS r)
{
   // Need to iterate through all the relations.

  if(NumSignalSource+NumVideoSource==0 || NumSignalSource+NumVideoSource==1)
      return;

  // Disable all CBs... the relations available will enable the corresponding CBs afterwards
   for(int i=0;i<NumSignalSource+NumVideoSource-1;i++)
   {
      for(int j=0;j<NumSignalSource+NumVideoSource-1;j++)
      {
         if(j<i)
            continue;
         LinkWidgets[1+i][1+j]->setEnabled(false);
      }
   }


   // Iterate through all the relations
   //printf("Iterate through all the relations\n");
   for(int i=0;i<r.size();i++)
   {
      // A relation between r[i].x1 and r[i].x2 exists -> enable checkboxes.
      // can be [x1,x2] or [x2,x1]
      //printf("Relation %d is between x%d and x%d\n",i,r[i].x1,r[i].x2);
      int x1,x2;
      if(r[i].x1<r[i].x2)
      {
         x1=r[i].x1;
         x2=r[i].x2;
      }
      else
      {
         x1=r[i].x2;
         x2=r[i].x1;
      }
      //printf("Relation %d. wanting to set %d %d\n",i,1+x1,x2);
      LinkWidgets[1+x1][x2]->setEnabled(true);
   }

   // Ensures all disabled CBs (relation is not available) see their check cleared.
   for(int i=0;i<NumSignalSource+NumVideoSource-1;i++)
   {
      for(int j=0;j<NumSignalSource+NumVideoSource-1;j++)
      {
         if(j<i)
            continue;
         if(!LinkWidgets[1+i][1+j]->isEnabled())
            static_cast<QCheckBox*>(LinkWidgets[1+i][1+j])->setChecked(false);
      }
   }

   // Fill the equation widget
   m_ui->listWidget_Equations->clear();
   for(int i=0;i<r.size();i++)
      m_ui->listWidget_Equations->addItem(RelationToText(r[i]));
}


void ReferencesDialog::on_LinkUIStateChanged(int state)
{
   int r,c,rs,cs;
   int idx;

   // Identify which relation is activated
   QObject *s = sender();
   idx=gridLayout_Link->indexOf(static_cast<QWidget*>(s));
   gridLayout_Link->getItemPosition(idx,&r,&c,&rs,&cs);


   //printf("idx %d. r,c: %d,%d rs,cs: %d,%d\n",idx,r,c,rs,cs);

   // Convert the idx into a relation pair.
   int x1,x2;
   x1 = r-1;
   x2 = c;

   //printf("maps to relation %d-%d\n",x1,x2);

   // Now should go through all the relations, and update (make 'uncheckable' and somehow highlight) those that are conflicting.

   // Collect all active relations
   RELATIONS ar=GetRelationsActive();


   // Disable/Enable conflicting options.
   for(int i=0;i<LinkWidgets.size()-1;i++)      // Row
   {
      for(int j=0;j<LinkWidgets.size()-1;j++)   // Column
      {
         if(j<i)
            continue;
         // Check if relation i,
         bool conflict = IsRelationConflicting(ar,i,j+1);
         //printf("%d-%d: %d\n",i,j+1,conflict);
         QPalette palette;
         if(conflict && static_cast<QCheckBox*>(LinkWidgets[1+i][1+j])->isChecked()==false)
         {
            QColor color(Qt::red);
            palette.setColor(QPalette::Base, color);
            static_cast<QCheckBox*>(LinkWidgets[1+i][1+j])->setPalette(palette);
            static_cast<QCheckBox*>(LinkWidgets[1+i][1+j])->setCheckable(false);
         }
         else
         {
            static_cast<QCheckBox*>(LinkWidgets[1+i][1+j])->setPalette(palette);
            static_cast<QCheckBox*>(LinkWidgets[1+i][1+j])->setCheckable(true);
         }
      }
   }

   emit ChangedActiveRelations();

}

/**
  \brief Returns the selected "active" relations to link sources

  \warning The regression values are not provided in the RELATION structure. Only the fiels x1 and x2 are valid.
**/
RELATIONS ReferencesDialog::GetRelationsActive()
{
   // Collect all active relations
   RELATIONS ar;

   if(NumSignalSource+NumVideoSource==0 || NumSignalSource+NumVideoSource==1)
      return ar;

   for(unsigned i=0;i<LinkWidgets.size()-1;i++)
   {
      for(unsigned j=0;j<LinkWidgets.size()-1;j++)
      {
         if(j<i)
            continue;
         if(static_cast<QCheckBox*>(LinkWidgets[1+i][1+j])->isChecked())
         {
            RELATION r;
            r.x1=i;
            r.x2=j+1;
            ar.push_back(r);
            // Store both directions.
            r.x1=j+1;
            r.x2=i;
            ar.push_back(r);
         }
      }
   }

   /*printf("Dump relations\n");
   for(int i=0;i<ar.size();i++)
   {
      printf("%d-%d\n",ar[i].x1,ar[i].x2);
   }*/
   return ar;
}



void ReferencesDialog::on_checkBox_EnableSignalLinking_toggled(bool checked)
{
    emit ChangedActiveRelations();
}

bool ReferencesDialog::GetLinkState()
{
   return m_ui->checkBox_EnableSignalLinking->isChecked();
}


/*
   Signals from the list widget
*/

/**
  \brief this is the working shit!
**/
void ReferencesDialog::on_currentRowChanged ( int currentRow)
{
   if(currentRow!=-1)
      emit ReferenceSelected(currentRow);
}


/**
  \brief Clear reference points
**/

void ReferencesDialog::on_pushButton_Clear_clicked()
{
   emit RemoveReference(-1);
}

/**
  \brief Clear (uncheck) all the signal links, including global enable.
**/
void ReferencesDialog::ClearLinks()
{
   if(NumSignalSource+NumVideoSource==0 || NumSignalSource+NumVideoSource==1)
      return;
   for(int i=0;i<NumSignalSource+NumVideoSource-1;i++)
   {
      for(int j=0;j<NumSignalSource+NumVideoSource-1;j++)
      {
         if(j<i)
            continue;
         static_cast<QCheckBox*>(LinkWidgets[1+i][1+j])->setChecked(false);
      }
   }
   m_ui->checkBox_EnableSignalLinking->setChecked(false);
   emit ChangedActiveRelations();
}
