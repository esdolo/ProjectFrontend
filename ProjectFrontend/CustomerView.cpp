#include "stdafx.h"
#include "CustomerView.h"

CustomerView::CustomerView(QWidget *parent)
	: QDialog(parent),
	pCustomer(dynamic_cast<Customer*>(MainLogic::GetInstance()->pUser)),
	pWaitor(nullptr)
{
	ui.setupUi(this);
	switch (pCustomer->GetState())
	{
	case CSSTATE::Absent:
		PhaseSeatPick();
		break;
	case CSSTATE::Make:
		PhaseOrderMake();
		break;
	case CSSTATE::Eat:
		PhaseMealEat();
		break;
	default:
		break;
	}
	ui.lblTotal->setText("�ܼƣ�0Ԫ");
	nTotal = 0;
}

CustomerView::~CustomerView()
{
	pCustomer = nullptr;
	MainLogic::GetInstance()->pUser = nullptr;
}

void CustomerView::paintEvent(QPaintEvent * event)
{
	ui.btnAddWater->setDisabled(pWaitor == nullptr);
	ui.btnHasten->setDisabled(pWaitor == nullptr);
	ui.btnPackUp->setDisabled(pWaitor == nullptr);
	ui.btnSubmit->setDisabled(nTotal == 0);
	ui.btnFinish->setDisabled(ui.pbGeneral->value() != 100);
	QDialog::paintEvent(event);
}

void CustomerView::SubmitOrder()
{
	auto ret = QMessageBox::information(this, "֧��ȷ��",
		QString("����֧��%0Ԫ����ȷ��֧����").arg(pCustomer->m_itNow->second.CheckOut()),
		QMessageBox::Yes | QMessageBox::No);
	if (ret == QMessageBox::Yes)
	{
		pCustomer->m_itNow->second.Assign();
		for (auto & s : pCustomer->m_itNow->second.GetFoodMap())
		{
			pCustomer->m_statusComment[s.first.GetId()];
		}
		PhaseMealEat();
	}
}

void CustomerView::PhaseSeatPick()
{
	auto st = pCustomer->GetTableNum();
	if (st != -1)
	{
		MainLogic::GetInstance()->arrSeatVacance[st] = 1;
		st = -1;
	}
	HideAll();
	MyResize(850, 950);
	ui.gbSeatPick->show();
	ui.lblHint->show();
	ui.lblInfo->show();
	nSeat = MainLogic::GetInstance()->arrSeatVacance.size();
	ui.lblInfo->setText("��ӭ���٣��𾴵�" + str2qstr(pCustomer->GetName()) + "�˿�");
	PrepareSeats();
	if (IsNoVacance())
	{
		ui.lblHint->setText("�ܱ�Ǹ������û�п�����λ���������ĵȴ�����");
	}
	else
	{
		ui.lblHint->setText("�������·���\"ѡ�����\"����ѡ��λ��");
	}
}

void CustomerView::PhaseOrderMake()
{
	HideAll();
	MyResize(800, 750);
	ui.gbMenu->show();
	ui.lblInfo->show();
	PrepareMenu();
	ui.lblInfo->setText(QString("���ã��𾴵Ĺ˿�%0����������Ϊ%1").
		arg(str2qstr(pCustomer->GetName())).
		arg(pCustomer->GetTableNum() + 1));
	ui.lblTotal->setText(QString("�ܼƣ�%0Ԫ").arg(nTotal));
}

void CustomerView::PhaseMealEat()
{
	HideAll();
	MyResize(1000, 600);
	ui.gbOrder->show();
	ui.gbInstruction->show();
	ui.lblInfo->show();
	auto idWaitor = pCustomer->MakeOrder();
	if (idWaitor != "")
	{
		pWaitor = &MainLogic::s_currentWaitors[idWaitor];
		pCustomer->m_waitorComment[idWaitor];
		ui.lblWaitor->setText("����Ա" + str2qstr(idWaitor) + "Ϊ������");
	}
	else
	{
		pWaitor = nullptr;
		ui.lblWaitor->setText("���޷���ԱΪ������");
	}
	ui.lblInfo->setText(QString("���ã��𾴵Ĺ˿�%0����������Ϊ%1").
		arg(str2qstr(pCustomer->GetName())).
		arg(pCustomer->GetTableNum() + 1));
	PrepareOrder();
}

void CustomerView::OnPackUp()
{
	pWaitor->m_deqTodo.push_back(SRVS::Package);
	QMessageBox::information(this, "֪ͨ", "��Ϣ ��� �ѷ��͸�����Ա��");
}

void CustomerView::OnAddWater()
{
	pWaitor->m_deqTodo.push_back(SRVS::Water);
	QMessageBox::information(this, "֪ͨ", "��Ϣ ��ˮ �ѷ��͸�����Ա��");
}

void CustomerView::OnHasten()
{
	pWaitor->m_deqTodo.push_back(SRVS::Hasten);
	QMessageBox::information(this, "֪ͨ", "��Ϣ �ߵ� �ѷ��͸�����Ա��");
}

void CustomerView::OnCommentDish()
{
	AddComment addc(IDGENERATOR::ID_CUISINE ,pCustomer, this);
	addc.exec();
}

void CustomerView::OnCommentWaitor()
{
	AddComment addc(IDGENERATOR::ID_WAITOR, pCustomer, this);
	addc.exec();
}


void CustomerView::OnFinish()
{
	pCustomer->Finish();
	pCustomer->m_itNow = MainLogic::s_currentOrders.end();
	pCustomer->m_statusComment.clear();
	accept();
}

void CustomerView::PrepareSeats()
{
	if (matSeat.isEmpty())
	{
		for (int i = 0; i < nSeat; ++i)
		{
			matSeat.push_back(new QPushButton(ui.gbSeatPick));
		}
	}
	int nCol = round(sqrt(nSeat));
	int nSize, nDiv;
	if (nSeat > 64)
	{
		nSize = 65;
		nDiv = 75;
	}
	else
	{
		nSize = 80;
		nDiv = 90;
	}
	for (int i = 0; i < nSeat; ++i)
	{
		matSeat[i]->setGeometry(25 + (i % nCol) * nDiv, 25 + (i / nCol) * nDiv, nSize, nSize);
		matSeat[i]->setText(QString("����%0").arg(i + 1));
		matSeat[i]->show();
		matSeat[i]->setDisabled(MainLogic::GetInstance()->arrSeatVacance[i] == 0);
		connect(matSeat[i], &QPushButton::clicked, 
			//The slots by lambda expression
			[=]()
			{
				SeatClicked(i);
			});
	}
}


bool CustomerView::IsNoVacance()
{
	for (auto &t : MainLogic::GetInstance()->arrSeatVacance)
	{
		if (t == 1)
		{
			return false;
		}
	}
	return true;
}

void CustomerView::SeatClicked(int num)
{
	pCustomer->GetSeat(num, qstr2str(QDate::currentDate().toString("yyyy/MM/dd")));
	MainLogic::GetInstance()->arrSeatVacance[num] = 0;
	PhaseOrderMake();
}

void CustomerView::HideAll()
{
	//Hide all widgets
	ui.gbSeatPick->hide();
	ui.lblHint->hide();
	ui.lblInfo->hide();
	ui.gbMenu->hide();
	ui.gbInstruction->hide();
	ui.gbOrder->hide();
}

void CustomerView::Subtotal(int nCost, int nRow, int nCol)
{
	ui.tbMenuDisp->setItem(nRow, nCol, new QTableWidgetItem(QString::number(nCost)));
}

void CustomerView::PrepareMenu()
{
	ui.tbMenuDisp->setColumnCount(6);
	ui.tbMenuDisp->setHorizontalHeaderItem(0, new QTableWidgetItem("��ƷID"));			//Maybe turn to pics later
	ui.tbMenuDisp->setHorizontalHeaderItem(1, new QTableWidgetItem("��Ʒ����"));
	ui.tbMenuDisp->setHorizontalHeaderItem(2, new QTableWidgetItem("�۸�Ԫ/�ݣ�"));
	ui.tbMenuDisp->setHorizontalHeaderItem(3, new QTableWidgetItem("�������ݣ�"));
	ui.tbMenuDisp->setHorizontalHeaderItem(4, new QTableWidgetItem("С�ƣ�Ԫ��"));
	ui.tbMenuDisp->setHorizontalHeaderItem(5, new QTableWidgetItem("�鿴����"));
	int nRow = 0;
	for (auto &dish : MainLogic::s_currentMenu)
	{
		ui.tbMenuDisp->setRowCount(1 + nRow);
		ui.tbMenuDisp->setItem(nRow, 0, new QTableWidgetItem(str2qstr(dish.first)));
		ui.tbMenuDisp->setItem(nRow, 1, new QTableWidgetItem(str2qstr(dish.second.GetName())));
		ui.tbMenuDisp->setItem(nRow, 2, new QTableWidgetItem(QString::number(dish.second.GetPrice())));
		QSpinBox *sbQuant = new QSpinBox(ui.tbMenuDisp);
		sbQuant->setMinimum(0);
		connect(sbQuant, static_cast<void(QSpinBox:: *)(int)>(&QSpinBox::valueChanged),
			//The slots by lambda expression
			[=](int nVal)
		{
			pCustomer->m_itNow->second.Adjust(dish.second, nVal);
			auto cost = pCustomer->m_itNow->second.m_mapBill[dish.first];
			Subtotal(cost, nRow, 4);
			nTotal = pCustomer->m_itNow->second.CheckOut();
			ui.lblTotal->setText(QString("�ܼƣ�%0Ԫ").arg(nTotal));
		});
		ui.tbMenuDisp->setCellWidget(nRow, 3, sbQuant);
		ui.tbMenuDisp->setItem(nRow, 4, new QTableWidgetItem("0"));
		QPushButton *btnComment = new QPushButton(ui.tbMenuDisp);
		btnComment->setText("�鿴����");
		connect(btnComment, &QPushButton::clicked,
			//The slots by lambda expression
			[=]() 
		{
			BrowseComment bc(dish.first, this);
			bc.exec();
		});
		ui.tbMenuDisp->setCellWidget(nRow, 5, btnComment);
		++nRow;
	}
}

void CustomerView::MyResize(int w, int h)
{
	setMaximumSize(w, h);
	setMinimumSize(w, h);
	resize(w, h);
}

void CustomerView::PrepareOrder()
{
	ui.tbOrder->setColumnCount(3);
	ui.tbOrder->setHorizontalHeaderItem(0, new QTableWidgetItem("��Ʒ"));
	ui.tbOrder->setHorizontalHeaderItem(1, new QTableWidgetItem("�ѵ��"));
	ui.tbOrder->setHorizontalHeaderItem(2, new QTableWidgetItem("���ϲ�"));
	int nRow = 0;
	auto foodOrdered = pCustomer->m_itNow->second.GetFoodMap();
	auto all = 0, served = 0;
	for (auto &fo : foodOrdered)
	{
		auto servedFood = fo.second - pCustomer->m_itNow->second.m_mapFoodToDo[fo.first];
		ui.tbOrder->setRowCount(1 + nRow);
		ui.tbOrder->setItem(nRow, 0, new QTableWidgetItem(str2qstr(fo.first.GetName())));
		ui.tbOrder->setItem(nRow, 1, new QTableWidgetItem(QString::number(fo.second)));
		ui.tbOrder->setItem(nRow, 2, new QTableWidgetItem(QString::number(servedFood)));
		++nRow;
		all += fo.second;
		served += servedFood;
	}
	ui.pbGeneral->setValue(100 * served / all);
}