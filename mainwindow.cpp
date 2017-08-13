#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextCodec>
#include <QTextStream>
#include <QDebug>
#include <QDropEvent>
#include <QList>
#include <QUrl>
#include <QMimeData>
#include <QShortcut>
#include <QKeySequence>
#include <QAbstractItemView>

//#include <uchardet/uchardet.h>
#include <uchardet.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_dialog = new Dialog(this);
    //GUI初始化
	this->setAcceptDrops(true);


    this->setWindowTitle(tr("EncodingConverter"));
    this->setWindowIcon(QIcon("icon.png"));
    this->setWindowFlags(Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint);//禁用最大化按钮
    this->setFixedSize(this->width(), this->height());//设置固定大小
    ui->listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);//QListWidget多选
    ui->radioButton->setChecked(true);//默认点选“备份”
    //信号与槽
    connect(ui->actionAbout_Qt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionAbout_App, &QAction::triggered, this, &MainWindow::onActionAbout);
    connect(ui->puBtn_addDir, &QPushButton::clicked, this, &MainWindow::onClickedAddDir);
    connect(ui->puBtn_addFiles, &QPushButton::clicked, this, &MainWindow::onClickedAddFiles);
    connect(ui->puBtn_remove, &QPushButton::clicked, this, &MainWindow::onClickedRemove);
    ui->puBtn_remove->setShortcut(QKeySequence::Delete);
    connect(ui->puBtn_Clean, &QPushButton::clicked, this, &MainWindow::onClickedClean);
    connect(ui->puBtn_start, &QPushButton::clicked, this, &MainWindow::onClickedStart);
    connect(ui->listWidget, &QListWidget::itemSelectionChanged, this, &MainWindow::ucharDet);
    connect(m_dialog, SIGNAL(sigfilter(QString)), this, SLOT(onGetFilter(QString)));

	ui->statusBar->showMessage(tr("Ready!"), 3000);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onActionAbout()
{
	QMessageBox msgBox(this);
	msgBox.setWindowTitle(tr("About"));
    QString aboutMsg = "<p>EncodingConverter"
                      "Version 1.24 </p>"
                      "<p>This is an opensource application. Designed by <a href=\"https://github.com/ultrali\">@ultrali</a>."
                      "NOT GUARANTEE THE SAFETY OF THE PROGRAM, USE WITH CAUTION! \n</p>"
                      "<p>From: <a href=\"https://github.com/ultrali/EncodingConverter\">EncodingConverter</a></p>";
    msgBox.setInformativeText(aboutMsg);
    QPixmap pm(QLatin1String("icon.png"));
    if (!pm.isNull()){
        msgBox.setIconPixmap(pm);
    }
	msgBox.addButton(tr("OK"), QMessageBox::ActionRole);
	msgBox.exec();
}

void MainWindow::onClickedAddFiles()
{
    int count = 0;//新导入的
    ui->statusBar->showMessage(tr("[Info] Import Files"));
    QStringList files=QFileDialog::getOpenFileNames(this,
                                                    tr("Import Files"),
                                                    nullptr,
                                                    tr("C/C++ Source(*.h *.c *.cpp);;Text(*.txt);;All Files(*.*)"));
    if(files.length()!=0){
        QStringList::iterator it=files.begin();
        while(it!= files.end()){
            if(this->import2ListWidget(*it))
                count++;
            it++;
        }
    }
    ui->statusBar->showMessage(QString::number(count) +
                               tr(" new files imported. ") +QString::number(files.length()-count)+tr(" ignored.") , 3000);
}

void MainWindow::onClickedAddDir()
{
	//显示过滤器设置对话框
	m_dialog->exec();

    int count = 0;//新导入的
    ui->statusBar->showMessage(tr("[Info] Import Dir"));
    QString res = QFileDialog::getExistingDirectory(this,
                                                    tr("Import Dir"),
                                                    nullptr,
                                                    QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if(!res.isEmpty()){
        count = this->findFile(res);
    }
    ui->statusBar->showMessage(QString::number(count)+ tr(" new files imported."));
}

void MainWindow::onClickedRemove()
{
    int count = 0;
    QList<QListWidgetItem*> items = ui->listWidget->selectedItems();
    if(items.size() != 0){
        QListWidgetItem* it;
        foreach (it, items) {
			int row = ui->listWidget->row(it); //获取item所在行号(从0开始)
			ui->listWidget->takeItem(row); //拿下item，不在受qt管理
			delete it; //永久删除item
            count++;
        }
    }

    ui->statusBar->showMessage(QString::number(count) + tr(" files removed."), 3000);
}

void MainWindow::onClickedClean()
{
    ui->listWidget->clear();
}

void MainWindow::onClickedStart()
{
    if(ui->comboBox_srcEncode->currentText() == ui->comboBox_dstEncode->currentText()){
        ui->statusBar->showMessage(tr("Do Nothing."), 3000);
        return;
    }
    int rows = ui->listWidget->count();
    if(ui->radioButton->isChecked()){
        //需备份
        for(int i=0;i<rows;i++){
            QString fileName = ui->listWidget->item(i)->text();
            QFile::copy(fileName, fileName+".bak");
        }
        ui->statusBar->showMessage(QString::number(rows) + tr(" file backup."), 3000);
    }

    //开始转换
    for(int i=0;i<rows;i++){
		//读取文件内容
		QString fileName = ui->listWidget->item(i)->text();
		QByteArray contentBytes;//文件所有内容
		QString data;//全部文本内容
		QFile file(fileName);
		if (!file.open(QIODevice::ReadOnly)) {
			ui->statusBar->showMessage(tr("[Error] Open file \"") + fileName + tr("\" failed."));
			qDebug() << tr("[Error] Open file \"") + fileName + tr("\" failed.");
			continue;
		}
		contentBytes = file.readAll();

		//行尾转换
		if (ui->comboBox_dstLinetype->currentIndex() == 1) {
			//1, → LF
			contentBytes.replace("\r", "\n");
			contentBytes.replace("\n\n", "\n");
		}
		else if (ui->comboBox_dstLinetype->currentIndex() == 2) {
			//2, → CR
			contentBytes.replace("\n", "\r");
			contentBytes.replace("\r\r", "\r");
		}
		else {
			//3, → CRLF
			contentBytes.replace("\r", "\n");
			contentBytes.replace("\n\n", "\n");
			contentBytes.replace("\n", "\r\n");
		}
		


		//编码转换
        //src_codec => Unicode QString
		QTextCodec* pSrcCodec = QTextCodec::codecForName(ui->comboBox_srcEncode->currentText().toStdString().c_str());
		if (nullptr != pSrcCodec) {
			data = pSrcCodec->toUnicode(contentBytes);
		}
        else {
            qDebug()<<"[Error] Not Support Codec!";
            return;
        }
        file.close();


        // Unicode QString ==> dst_codec
        if(!file.open(QIODevice::WriteOnly|QIODevice::Truncate)){
            ui->statusBar->showMessage(tr("[Error] Open file \"")+fileName+tr("\" failed."));
            qDebug()<<tr("[Error] Open file \"")+fileName+tr("\" failed.");
            continue;
        }

		QTextCodec* pDstCodec;
		if (ui->comboBox_dstEncode->currentText() == "UTF-8 BOM") {
			pDstCodec = QTextCodec::codecForName("UTF-8");
			file.write(QByteArray::fromHex("EFBBBF"));
		}
		else {
			pDstCodec = QTextCodec::codecForName(ui->comboBox_dstEncode->currentText().toStdString().c_str());
		}
		if (nullptr != pDstCodec) {
			file.write(pDstCodec->fromUnicode(data));
		}
        else {
            qDebug()<<"[Error] Not Support Codec!";
            return;
        }

        file.close();
    }

    ui->statusBar->showMessage(tr("[Info] Transcoding is complete."), 3000);
}


void MainWindow::onGetFilter(QString sFilter)
{
	m_fileFilter = sFilter;
}


bool MainWindow::import2ListWidget(const QString & str, const QString& filter)
{
    if(str.isEmpty())
        return false;

    if(!filter.isEmpty()){
        //有过滤规则
        QStringList filterLs = filter.split("|", QString::SkipEmptyParts);
        QFileInfo file(str);
        QStringList::const_iterator it;
        for(it= filterLs.begin();it!= filterLs.end();it++){
            if("*." + file.suffix() == *it)
                break;
        }
        if(it == filterLs.end())
            return false;
    }

    //先查看是否已存在表中
    QList<QListWidgetItem*> res = ui->listWidget->findItems(str, Qt::MatchFixedString);
    if(res.length() != 0)
        return false;

    QListWidgetItem* item = new QListWidgetItem;
    item->setText(str);
    ui->listWidget->addItem(item);
    return true;
}

int MainWindow::findFile(const QString& path)
{
    QDir dir(path);
    int count = 0;
    if(!dir.exists())
        return 0;
    dir.setFilter(QDir::Dirs|QDir::Files);
    dir.setSorting(QDir::DirsFirst);//将目录放在前面
    QFileInfoList list = dir.entryInfoList();
    int i=0;
    do{
        QFileInfo fileInfo = list.at(i);
        if(fileInfo.fileName()=="." || fileInfo.fileName()==".."){
            i++;
            continue;
        }
        if(fileInfo.isDir()){
            count += this->findFile(fileInfo.filePath());
        }else{
            if(this->import2ListWidget(fileInfo.filePath(), m_fileFilter))
                count++;
        }
        i++;
    }while(i< list.size());
    return count;
}


void MainWindow::ucharDet()
{
    ui->statusBar->showMessage(tr("[Info] Identifying file ..."));
    
#define BUFFER_SIZE 65536
	QByteArray contentBuf;
    uchardet_t ud;


	//防止item已被移除
	if (nullptr == ui->listWidget->currentItem()) {
		return;
	}
	QFile fin(ui->listWidget->currentItem()->text());
	if (false == fin.open(QIODevice::ReadOnly)) {
		ui->statusBar->showMessage(tr("[Error] Read file failed!"));
		return;
	}
	contentBuf = fin.read(BUFFER_SIZE);
	fin.close();

    /* 通过样本字符分析文本编码 */
    ud = uchardet_new();
	/* 如果样本字符不够，那么有可能导致分析失败 */
    if(uchardet_handle_data(ud, contentBuf.data(), contentBuf.size()) != 0)
    {
        ui->statusBar->showMessage(tr("[Info] Identification failed!\n"));
    }
    uchardet_data_end(ud);
    QString res = QString(uchardet_get_charset(ud));
    if(res == ""){
        res = "ASCII/unknown";
    }

	//更改字符集下拉框
	int index = ui->comboBox_srcEncode->findText(res, Qt::MatchFixedString);
	if (-1 != index) {
		ui->comboBox_srcEncode->setCurrentIndex(index);
	}
	if (res == "UTF-8") {
		ui->comboBox_dstEncode->setCurrentIndex(0);//GB
		ui->comboBox_dstLinetype->setCurrentIndex(0);//CRLF
	}
	else if (res == "GB18030") {
		ui->comboBox_dstEncode->setCurrentIndex(2);//utf-8 with bom
		ui->comboBox_dstLinetype->setCurrentIndex(1);//LF
	}
	
    uchardet_delete(ud);


	//识别行尾
	if (contentBuf.contains("\r\n")) {
		ui->statusBar->showMessage(tr("[Get] File encoding(%1), with end-of-line(%2)").arg(res).arg("CRLF"), 5000);
		ui->comboBox_srcLinetype->setCurrentIndex(0);
	}
	else if (contentBuf.contains('\n')) {
		ui->statusBar->showMessage(tr("[Get] File encoding(%1), with end-of-line(%2)").arg(res).arg("LF"), 5000);
		ui->comboBox_srcLinetype->setCurrentIndex(1);
	}
	else if (contentBuf.contains('\r')) {
		ui->statusBar->showMessage(tr("[Get] File encoding(%1), with end-of-line(%2)").arg(res).arg("CR"), 5000);
		ui->comboBox_srcLinetype->setCurrentIndex(2);
	}
	else {
		//无换行
		ui->statusBar->showMessage(tr("[Get] File encoding(%1), with end-of-line(%2)").arg(res).arg("None"), 5000);
		ui->comboBox_srcLinetype->setCurrentIndex(3);
	}
}


void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("text/uri-list")) {
		//接受事件
		event->acceptProposedAction();
	}
}


void MainWindow::dropEvent(QDropEvent *event)
{
	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.isEmpty())
		return;

	foreach(QUrl url, urls) {
		if (url.isLocalFile()) {
			QString file_name = url.toLocalFile();
			QFileInfo fi(file_name);
			if (fi.isFile()) {
				//判断为文件
				this->import2ListWidget(file_name);
				continue;
			}
			else if (fi.isDir()) {
				//为目录
				m_dialog->exec();
				int count = this->findFile(file_name);
				ui->statusBar->showMessage(tr("[Info] %1 new files imported!").arg(count), 3000);
				continue;
			}
		}
		ui->statusBar->showMessage(tr("[Info] Not support object(%1)!").arg(url.url()), 3000);
	}
}

