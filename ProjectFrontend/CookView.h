#pragma once

#include <QDialog>
#include "ui_CookView.h"

class CookView : public QDialog
{
	Q_OBJECT

public:
	CookView(QWidget *parent = Q_NULLPTR);
	~CookView();
protected:
	virtual void paintEvent(QPaintEvent *event) override;
private slots:
	void CookOne();
	void OnSelect(int nRow);
private:
	void PrepareTodo();
	void PrepareDone();
private:
	Ui::CookView ui;
	Cook *pCook;
	QStandardItemModel *mdlDone;
	deque<pair<Cuisine, Order*>>::iterator itThis;
};
