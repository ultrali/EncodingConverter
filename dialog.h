#ifndef DIALOG_H
#define DIALOG_H

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <QDialog>
namespace Ui {
class Dialog;
}
class Dialog : public QDialog
{
    Q_OBJECT
public:
    explicit Dialog(QWidget *parent = 0);
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
