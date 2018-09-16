#ifndef DIALOG_H
#define DIALOG_H

#include <QtCore/QObject>
#include <QDialog>
#include "ui_dialog.h"

/*
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")

namespace Ui {
class Dialog;
}
#endif
*/


class Dialog : public QDialog
{
    Q_OBJECT
public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();
protected slots:
    void onClickedButtonOk();
	void onClickedButtonCancel();
signals:
    void sigfilter(QString);//发送filter串
private:
    Ui::Dialog *ui;
};
#endif // DIALOG_H
