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

#ifndef SOURCEDIALOG_H
#define SOURCEDIALOG_H

#include <QtGui/QDialog>
#include "data.h"

namespace Ui {
    class SourceDialog;
}

class SourceDialog : public QDialog {
    Q_OBJECT
public:
    SourceDialog(QWidget *parent = 0);
    ~SourceDialog();

   void SetSignalData(std::vector<SIGNALDATA> &SignalData);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SourceDialog *m_ui;
};

#endif // SOURCEDIALOG_H
