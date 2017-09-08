#pragma once

#include <QDialog>
#include "ui_ManagerView.h"

class ManagerView : public QDialog
{
	Q_OBJECT

public:
	ManagerView(QWidget *parent = Q_NULLPTR);
	~ManagerView();
private:
	void PrepareCook();
	void PrepareWaitor();
private slots :
	void BindCook(QString qstrCook);
	void BindWaitor(QString qstrWaitor);
private:
	Ui::ManagerView ui;
	string idCook;
	string idWaitor;
	Manager *pManager;
	QStandardItemModel *mdlCook;
	QStandardItemModel *mdlWaitor;
	QStandardItemModel *mdlComm;
	QStringList lstService;
};
