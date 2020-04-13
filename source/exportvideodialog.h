#ifndef EXPORTVIDEODIALOG_H
#define EXPORTVIDEODIALOG_H

#include <QtGui/QDialog>
#include "data.h"

namespace Ui {
    class ExportVideoDialog;
}

class ExportVideoDialog : public QDialog {
    Q_OBJECT
public:
    ExportVideoDialog(const PLOTVIEWPARAM &p,QWidget *parent = 0);
    ~ExportVideoDialog();

    PLOTVIEWPARAM getParam();

    /*unsigned getWidth();
    unsigned getHeight();
    unsigned getBitrate();
    unsigned getGop();
    unsigned getSignalStep();
    bool getTitle();
    bool getDouble();*/

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ExportVideoDialog *m_ui;
};

bool GetVideoParameters(PLOTVIEWPARAM &param);

#endif // EXPORTVIDEODIALOG_H
