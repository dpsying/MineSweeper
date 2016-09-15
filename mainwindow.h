#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "item.h"

#include <QMainWindow>




namespace Ui {
class MainWindow;
}


#define RECT_WIDTH      30
#define RECT_HEIGHT     30

#define START_X         100
#define START_Y         100


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

	void InitItems();
	void ReleaseItems();

    void NewGame();						
	void GameSuccess();
	void GameFail();

	void OpenEmptyItem(QPoint pt);			//点击空白元素(相邻雷数为0)时，递归查找相邻的空白元素，以及空白元素附近的数字元素(数字是雷数)
	bool FindAll();
	bool PointInGameArea(QPoint pt);		//判断坐标是否超过游戏区域
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);

private slots:
    void OnMenu_NewGame();
    void OnMenu_Settings();
	void OnMenu_Level1();
	void OnMenu_Level2();
	void OnMenu_Level3();
private:
    void DrawChessboard();
    void DrawItems();
    void DrawItem(QPainter& painter,Item* pItem);
private:
    Ui::MainWindow *ui;
	QPixmap m_FlagImage;				//小红旗图片
	QPixmap m_BombImage;

	int m_nRows;						//行数
	int m_nColumes;					//列数
	int m_nMineCount;					//雷数
    QVector<QPoint> m_Mines;			//雷点
	QVector<QVector<Item*>> m_items;	//所有元素
	bool m_bGameFail;					//是否是游戏失败，失败了需要显示雷
};

#endif // MAINWINDOW_H
