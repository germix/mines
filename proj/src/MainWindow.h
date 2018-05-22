#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}
#include "MineBoard.h"
#include "Languages.h"

enum DIFFICULTY
{
	DIFFICULTY_NONE = -1,
	DIFFICULTY_BEGINNER,
	DIFFICULTY_MEDIUM,
	DIFFICULTY_EXPERT,
	DIFFICULTY_CUSTOM,
};

class QSettings;

class MainWindow : public QMainWindow
{
    Q_OBJECT
	Ui::MainWindow*	ui;

	MineBoard*		board;

	QIcon			iconDead;
	QIcon			iconSmile;
	QIcon			iconSurprise;
	QIcon			iconWinner;

	int				difficulty;

	Languages		languages;
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
private:
	void changeEvent(QEvent* e);
private:
	void updateSize();
	void setDifficulty(int d, QSettings *s = 0);
private slots:
	void slotActions();
	void slotBoard_timeChanged(int time);
	void slotBoard_faceChanged(MineBoard::FACE face);
	void slotBoard_statusChanged(MineBoard::STATUS status);
public:
};

#endif // MAINWINDOW_H
