﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#if _MSC_VER >= 1600
//#pragma execution_character_set("utf-8")
//#endif

#include <QMainWindow>
#include <uchardet.h>
#include "ui_mainwindow.h"
#include "dialog.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected slots:
    void onActionAbout();
    void onClickedAddFiles();
	void onGetFilter(QString sFilter);
    void onClickedRemoveListItem();
    void onClickedClean();
    void onClickedStart();
    void ucharDet();//文件编码检测
	void dragEnterEvent(QDragEnterEvent *event) override; //鼠标拖入
	void dropEvent(QDropEvent *event) override;//鼠标放下
private:
    Ui::MainWindow *ui;
    QDialog* m_dialog;
	QString m_fileFilter;//导入目录时的文件过滤器

    bool import2ListWidget(const QString& str, const QString& filter="");
    int findFile(const QString& path);//遍历文件目录
};
#endif // MAINWINDOW_H
