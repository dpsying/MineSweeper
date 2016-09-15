#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPaintEvent>
#include <QPainter>
#include <QTime>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainToolBar->hide();
   // ui->menuBar->hide();


    connect(ui->menu,SIGNAL(aboutToShow()),this,SLOT(OnMenu_NewGame()));
    connect(ui->menu_2,SIGNAL(aboutToShow()),this,SLOT(OnMenu_Settings()));
	connect(ui->action9_9_10, SIGNAL(triggered()), this, SLOT(OnMenu_Level1())); 
	connect(ui->action16_16_40, SIGNAL(triggered()), this, SLOT(OnMenu_Level2())); 
	connect(ui->action_16_30_99, SIGNAL(triggered()), this, SLOT(OnMenu_Level3()));
	
	m_FlagImage = QPixmap(":/new/image/flag.png");
	m_BombImage = QPixmap(":/new/image/bomb.png");
	
	m_nRows = 9;
	m_nColumes = 9;
	m_nMineCount = 10;
	NewGame();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitItems()
{
	//随机初始化雷
	m_Mines.clear();
	for(int i = 0; i<m_nMineCount; i++)
	{
		qsrand(QTime::currentTime().msec());
		int x = qrand()%m_nColumes;
		int y = qrand()%m_nRows;
		while(m_Mines.contains(QPoint(x,y)))
		{
			x = qrand()%m_nColumes;
			y = qrand()%m_nRows;
		}
		m_Mines.append(QPoint(x,y));
	}
	//建立2维数组保存所有元素位置，方便索引
	for(int i=0; i<m_nColumes; i++)
	{
		QVector<Item*> rowItems;
		for(int j=0; j<m_nRows; j++)
		{
			QPoint pos = QPoint(i,j);
			Item* pItem = new Item(pos);
			if(m_Mines.contains(pos))   //该位置是雷
			{
				pItem->m_bIsMine = true;
			}
			rowItems.append(pItem);		
		}
		m_items.append(rowItems);
	}
	//计算雷附近格子的数字
	for(int i=0; i<m_nColumes; i++)
	{
		for(int j=0; j<m_nRows; j++)
		{
			if (m_items[i][j]->m_bIsMine)
			{
				continue;
			}
			int nCountMines = 0;
			//求每个点附近的8个点的是雷的总数
			for (int m=-1;m<=1;m++)
			{
				for (int n=-1; n<=1;n++)
				{
					if (m==0 && n==0)
					{
						continue;
					}
					QPoint ptNew = QPoint(i+m,j+n);
					if (!PointInGameArea(ptNew))
					{
						continue;
					}

					if (m_items[i+m][j+n]->m_bIsMine)
					{
						nCountMines++;
					}
				}
			}
			m_items[i][j]->m_nNumber = nCountMines;
		}
	}
}

void MainWindow::ReleaseItems()
{
	for (int i=0; i<m_items.size(); i++)
	{
		for (int j=0;j<m_items[i].size(); j++)
		{
			if (m_items[i][j] != NULL)
			{
				delete m_items[i][j];
				m_items[i][j] = NULL;
			}
		}
	}
	m_items.clear();
}

void MainWindow::NewGame()
{
	m_bGameFail = false;
	resize(START_X*2 + m_nColumes*RECT_WIDTH  ,START_Y*2 + m_nRows*RECT_HEIGHT);

	ReleaseItems();
    InitItems();
}

void MainWindow::GameSuccess()
{
	//for (int i=0; i<m_items.size(); i++)
	//{
	//	for (int j=0;j<m_items[i].size(); j++)
	//	{
	//		if (m_items[i][j]->m_bIsMine)
	//		{
	//			m_items[i][j]->m_bMarked = true;
	//		}
	//		else
	//		{
	//			m_items[i][j]->m_bMarked = false;
	//			m_items[i][j]->m_bOpen = true;
	//		}
	//	}
	//}
}

void MainWindow::GameFail()
{
	m_bGameFail = true;
	for (int i=0; i<m_items.size(); i++)
	{
		for (int j=0;j<m_items[i].size(); j++)
		{
			if (m_items[i][j]->m_bIsMine)
			{
				m_items[i][j]->m_bMarked = true;
			}
			else
			{
				m_items[i][j]->m_bMarked = false;
				m_items[i][j]->m_bOpen = true;
			}
		}
	}
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    DrawChessboard();
    DrawItems();

    update();
}

void MainWindow::mousePressEvent(QMouseEvent * e)
{
	QPoint pt;
	pt.setX( (e->pos().x() - START_X ) / RECT_WIDTH);
	pt.setY( (e->pos().y() - START_X ) / RECT_HEIGHT);
	if (!PointInGameArea(pt))
	{
		return;
	}
	Item* pItem = m_items[pt.x()][pt.y()];

	if(e->button()==Qt::LeftButton)
	{
		if(!pItem->m_bMarked && !pItem->m_bOpen)
		{
			if (pItem->m_bIsMine)
			{
				//QMessageBox::information(NULL,  "GAME OVER","FAIL!", QMessageBox::Yes , QMessageBox::Yes);
				GameFail();
				return;
			}
			else
			{
				pItem->m_bOpen = true;
				if (pItem->m_nNumber == 0)
				{
					OpenEmptyItem(pt);
				}
			
				if (FindAll())
				{
					QMessageBox::information(NULL,  "GAME OVER","SUCCESS!", QMessageBox::Yes , QMessageBox::Yes);
					//GameSuccess();
					return;
				}
			}
		}
	}
	else if(e->button()==Qt::RightButton)
	{
		if (pItem->m_bMarked)
		{
			pItem->m_bMarked = false;
		}
		else if (!pItem->m_bOpen)
		{
			pItem->m_bMarked = true;

			if (FindAll())
			{
				QMessageBox::information(NULL,  "GAME OVER","SUCCESS!", QMessageBox::Yes , QMessageBox::Yes);
				//GameSuccess();
				return;
			}
		}
	}
}


void MainWindow::OnMenu_NewGame()
{
    NewGame();
}
void MainWindow::OnMenu_Settings()
{

}

void MainWindow::DrawChessboard()
{
    QPainter painter(this);
    painter.setBrush(Qt::lightGray);
    painter.drawRect( 0,0,width(),height());
}

void MainWindow::DrawItems()
{
    QPainter painter(this);
    painter.setBrush(Qt::lightGray);
    painter.setPen(QPen(QColor(Qt::black),2));

    for(int i=0; i<m_nColumes; i++)
    {
        for(int j=0; j<m_nRows; j++)
        {
            DrawItem(painter,m_items[i][j]);
        }
    }
}
void MainWindow::DrawItem(QPainter& painter,Item* pItem)
{
    if(pItem->m_bMarked)
    {
		if (m_bGameFail)
		{
			//游戏结束，显示为雷
			QRect rcSrc(0,0,m_FlagImage.width(),m_FlagImage.height());
			QRect rcTarget(START_X + pItem->m_pos.x()*RECT_WIDTH + 2,START_Y + pItem->m_pos.y()*RECT_HEIGHT + 2,RECT_WIDTH-4,RECT_HEIGHT-4);
			painter.drawPixmap(rcTarget,m_BombImage,rcSrc);
		}
		else
		{
			//游戏没结束，显示为旗子
			QRect rcSrc(0,0,m_FlagImage.width(),m_FlagImage.height());
			QRect rcTarget(START_X + pItem->m_pos.x()*RECT_WIDTH + 2,START_Y + pItem->m_pos.y()*RECT_HEIGHT + 2,RECT_WIDTH-4,RECT_HEIGHT-4);
			painter.drawPixmap(rcTarget,m_FlagImage,rcSrc);
		}
		painter.setBrush(Qt::transparent);
		painter.drawRect( START_X + pItem->m_pos.x()*RECT_WIDTH,START_Y + pItem->m_pos.y()*RECT_HEIGHT,RECT_WIDTH,RECT_HEIGHT);
		return;
	}
	else if (pItem->m_bOpen)
	{
		if(pItem->m_nNumber == 0)
		{
			painter.setBrush(Qt::white);
		}
		else
		{
			QFont font;
			font.setPointSize(20);
			font.setFamily(("msyh"));
			font.setBold(true);

			//float f = pItem->m_nNumber/8;
			//painter.setBrush(QColor(f*255,f*255,0));
			painter.setBrush(Qt::white);
			painter.drawRect( START_X + pItem->m_pos.x()*RECT_WIDTH,START_Y + pItem->m_pos.y()*RECT_HEIGHT,RECT_WIDTH,RECT_HEIGHT);

			painter.setBrush(Qt::black);
			painter.setFont(font);
			painter.drawText( START_X + pItem->m_pos.x()*RECT_WIDTH  + 8,START_Y + pItem->m_pos.y()*RECT_HEIGHT + 22,QString("%1").arg(pItem->m_nNumber));
			return;
		}
	}
	else
	{
		painter.setBrush(Qt::green);

		//调试使用：显示各个雷
		//if (pItem->m_bIsMine)
		//{
		//	painter.setBrush(Qt::red);
		//	painter.drawRect( START_X + pItem->m_pos.x()*RECT_WIDTH,START_Y + pItem->m_pos.y()*RECT_HEIGHT,10,10);
		//	return;
		//}
	}
    painter.drawRect( START_X + pItem->m_pos.x()*RECT_WIDTH,START_Y + pItem->m_pos.y()*RECT_HEIGHT,RECT_WIDTH,RECT_HEIGHT);
}



void MainWindow::OpenEmptyItem(QPoint pt)
{
	//对于空白元素，有上下左右4个方向挨着的空白元素，就打开并继续查找空白元素
	QVector<QPoint> directions;
	directions.push_back(QPoint(-1,0));
	directions.push_back(QPoint(1,0));
	directions.push_back(QPoint(0,-1));
	directions.push_back(QPoint(0,1));
	for (int i=0; i<directions.size(); i++)
	{
		QPoint ptNew = pt + directions[i];
		if (!PointInGameArea(ptNew))
		{
			continue;
		}
		Item* pItem = m_items[ptNew.x()][ptNew.y()];
		if (!pItem->m_bIsMine && !pItem->m_bOpen && !pItem->m_bMarked && pItem->m_nNumber == 0)
		{
			pItem->m_bOpen = true;

			//对于找到的空白元素，在它的8个方向上有数字元素就打开
			QVector<QPoint> directions2 = directions;
			directions2.push_back(QPoint(-1,-1));
			directions2.push_back(QPoint(1,1));
			directions2.push_back(QPoint(1,-1));
			directions2.push_back(QPoint(-1,1));
			for (int j=0; j<directions2.size(); j++)
			{
				QPoint ptNew2 = ptNew + directions2[j];
				if(!PointInGameArea(ptNew2))
				{
					continue;
				}
				Item* pItem2 = m_items[ptNew2.x()][ptNew2.y()];
				if (!pItem2->m_bIsMine && !pItem2->m_bOpen && !pItem2->m_bMarked && pItem2->m_nNumber > 0)
				{
					pItem2->m_bOpen = true;
				}
			}
			//递归查找上下左右4个方向的空白元素
			OpenEmptyItem(ptNew);
		}
	}
}

void MainWindow::OnMenu_Level1()
{
	m_nRows = 9;
	m_nColumes = 9;
	m_nMineCount = 10;
	NewGame();
}

void MainWindow::OnMenu_Level2()
{
	m_nRows = 16;
	m_nColumes = 16;
	m_nMineCount = 40;  
	NewGame();
}

void MainWindow::OnMenu_Level3()
{
	m_nRows = 16;
	m_nColumes = 30;
	m_nMineCount = 99;  
	NewGame();
}

bool MainWindow::FindAll()
{
	bool bFindAll = true;
	for (int i=0; i<m_items.size(); i++)
	{
		for (int j=0;j<m_items[i].size(); j++)
		{
			Item* pItem = m_items[i][j];
			if (pItem->m_bIsMine)
			{
				if (!pItem->m_bMarked)
				{
					bFindAll = false;
				}
			}
			else
			{
				if (!pItem->m_bOpen)
				{
					bFindAll = false;
				}
			}
		}
	}
	return bFindAll;
}

bool MainWindow::PointInGameArea(QPoint pt)
{
	if(pt.x()>=0 && pt.x()< m_nColumes && pt.y()>=0 && pt.y()< m_nRows)
	{
		return true;
	}
	return false;
}



