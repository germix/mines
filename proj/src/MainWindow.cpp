#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MineBoard.h"
#include "AboutDialog.h"

#include <QSettings>
#ifdef USE_CUSTOM
#include "CustomDifficultyDialog.h"
#endif

struct DIFFICULTY_DATA
{
	int numCols;
	int numRows;
	int numMines;
}difficultyData[] =
{
	{ COLS_BEGINNER,	ROWS_BEGINNER,	MINES_BEGINNER	},
	{ COLS_MEDIUM,		ROWS_MEDIUM,	MINES_MEDIUM	},
	{ COLS_EXPERT,		ROWS_EXPERT,	MINES_EXPERT	},
};

#define SETTINGS_APPLICATION "Mine"
#define SETTINGS_ORGANIZATION "Germix"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
	ui(new Ui::MainWindow), difficulty(DIFFICULTY_NONE)
{
    ui->setupUi(this);

	QSettings s(SETTINGS_ORGANIZATION, SETTINGS_APPLICATION);

	languages.init(ui->menu_Languages, "translations", "mines", s.value("Language").toString());

	// ...
#if 0	//!!!
	QString s;
	s = windowTitle();
	s += " [ ";
	s += __DATE__;
	s += " - ";
	s += __TIME__;
	s += " ] ";
	setWindowTitle(s);
#endif
	// ...
	board = new MineBoard(this);
	board->show();
	ui->gridLayout->addWidget(board, 1, 0, 1, 3);

	// ...
#ifdef USE_CUSTOM
#else
	ui->actionGameCustom->setVisible(false);
#endif

#if 0	//!!!
	iconDead		 = QIcon(QPixmap::fromImage(QImage(":/faces/Sad.png")));
	iconSmile		 = QIcon(QPixmap::fromImage(QImage(":/faces/Smile.png")));
	iconSurprise	 = QIcon(QPixmap::fromImage(QImage(":/faces/Surprise.png")));
	iconWinner		 = QIcon(QPixmap::fromImage(QImage(":/faces/Cool.png")));
#else
	iconDead		 = QIcon(QPixmap::fromImage(QImage(":/faces2/Sad.png")));
	iconSmile		 = QIcon(QPixmap::fromImage(QImage(":/faces2/Smile.png")));
	iconSurprise	 = QIcon(QPixmap::fromImage(QImage(":/faces2/Surprise.png")));
	iconWinner		 = QIcon(QPixmap::fromImage(QImage(":/faces2/Laugh.png")));
#endif
	connect(board, SIGNAL(onTimeChanged(int)), this, SLOT(slotBoard_timeChanged(int)));
	connect(board, SIGNAL(onFaceChanged(MineBoard::FACE)), this, SLOT(slotBoard_faceChanged(MineBoard::FACE)));
	connect(board, SIGNAL(onStatusChanged(MineBoard::STATUS)), this, SLOT(slotBoard_statusChanged(MineBoard::STATUS)));

	connect(ui->toolButton, SIGNAL(clicked()), this, SLOT(slotActions()));
	connect(ui->actionGameNew, SIGNAL(triggered()), this, SLOT(slotActions()));
	connect(ui->actionGameExit, SIGNAL(triggered()), this, SLOT(slotActions()));
	connect(ui->actionGameBeginner, SIGNAL(triggered()), this, SLOT(slotActions()));
	connect(ui->actionGameMedium, SIGNAL(triggered()), this, SLOT(slotActions()));
	connect(ui->actionGameExpert, SIGNAL(triggered()), this, SLOT(slotActions()));
#ifdef USE_CUSTOM
	connect(ui->actionGameCustom, SIGNAL(triggered()), this, SLOT(slotActions()));
#endif
	connect(ui->actionHelpAbout, SIGNAL(triggered()), this, SLOT(slotActions()));

	//
	// Restaurar estado anterior
	//
	restoreGeometry(s.value("WindowGeometry").toByteArray());
	restoreState(s.value("WindowState").toByteArray());

	QActionGroup* ag = new QActionGroup(this);
	ag->addAction(ui->actionGameBeginner);
	ag->addAction(ui->actionGameMedium);
	ag->addAction(ui->actionGameExpert);
#ifdef USE_CUSTOM
	ag->addAction(ui->actionGameCustom);

	setDifficulty(s.value("LastDifficulty", DIFFICULTY_BEGINNER).toInt(), &s);
#else
	setDifficulty(s.value("LastDifficulty", DIFFICULTY_BEGINNER).toInt());
#endif
}

MainWindow::~MainWindow()
{
	QSettings s(SETTINGS_ORGANIZATION, SETTINGS_APPLICATION);
	s.clear();
	s.setValue("Language", languages.language());
	s.setValue("WindowState", saveState());
	s.setValue("WindowGeometry", saveGeometry());
	s.setValue("LastDifficulty", difficulty);
#ifdef USE_CUSTOM
	if(difficulty == DIFFICULTY_CUSTOM)
	{
		s.setValue("CustomCols", board->getNumCols());
		s.setValue("CustomRows", board->getNumRows());
		s.setValue("CustomMines", board->getNumMines());
	}
#endif
    delete ui;
}
void MainWindow::changeEvent(QEvent* e)
{
	if(e != NULL)
	{
		switch(e->type())
		{
#if 1
			case QEvent::LocaleChange:
				{
					QString locale = QLocale::system().name();
					locale.truncate(locale.lastIndexOf('_'));
					languages.load(locale);
				}
				break;
#endif
			case QEvent::LanguageChange:
				ui->retranslateUi(this);
				break;
			default:
				break;
		}
	}
	QMainWindow::changeEvent(e);
}
void MainWindow::updateSize()
{
	//
	// Ajustar tamaño de la ventana
	//
	QSize s;
	s = ui->centralWidget->minimumSizeHint();
	ui->centralWidget->setMinimumSize(s);
	ui->centralWidget->setMaximumSize(s);

	// ...
	s = minimumSizeHint();
	setMinimumSize(s);
	setMaximumSize(s);
}
void MainWindow::setDifficulty(int d, QSettings* s)
{
#ifdef USE_CUSTOM
	if(d == DIFFICULTY_CUSTOM)
	{
		int numCols = board->getNumCols();
		int numRows = board->getNumRows();
		int numMines = board->getNumMines();
		if(s)
		{
			numCols = s->value("CustomCols", numCols).toInt();
			numRows = s->value("CustomRows", numRows).toInt();
			numMines = s->value("CustomMines", numMines).toInt();
			difficulty = DIFFICULTY_CUSTOM;
			ui->actionGameCustom->setChecked(true);

			board->newGame(numCols, numRows, numMines);
		}
		else
		{
			CustomDifficultyDialog dlg(numCols, numRows, numMines);

			if(dlg.exec() == QDialog::Accepted)
			{
				difficulty = DIFFICULTY_CUSTOM;
				ui->actionGameCustom->setChecked(true);

				board->newGame(dlg.getCols(), dlg.getRows(), dlg.getMines());
			}
		}
	}
	else
#endif
	{
		board->newGame(difficultyData[d].numCols,
					   difficultyData[d].numRows,
					   difficultyData[d].numMines);
		difficulty = d;
		switch(d)
		{
			case DIFFICULTY_BEGINNER:	ui->actionGameBeginner->setChecked(true);	break;
			case DIFFICULTY_MEDIUM:		ui->actionGameMedium->setChecked(true);		break;
			case DIFFICULTY_EXPERT:		ui->actionGameExpert->setChecked(true);		break;
		}
	}
}
void MainWindow::slotActions()
{
	QObject* action = qobject_cast<QObject*>(sender());

	if(action == ui->actionGameNew || action == ui->toolButton)
	{
		board->newGame();
	}
	else if(action == ui->actionGameExit)
	{
		close();
	}
	else if(action == ui->actionGameBeginner)
	{
		setDifficulty(DIFFICULTY_BEGINNER);
	}
	else if(action == ui->actionGameMedium)
	{
		setDifficulty(DIFFICULTY_MEDIUM);
	}
	else if(action == ui->actionGameExpert)
	{
		setDifficulty(DIFFICULTY_EXPERT);
	}
#ifdef USE_CUSTOM
	else if(action == ui->actionGameCustom)
	{
		setDifficulty(DIFFICULTY_CUSTOM);
	}
#endif
	else if(action == ui->actionHelpAbout)
	{
		AboutDialog().exec();
	}
}
void MainWindow::slotBoard_timeChanged(int time)
{
	ui->lcdNumberTimer->display(time);
}
void MainWindow::slotBoard_faceChanged(MineBoard::FACE face)
{
	switch(face)
	{
		case MineBoard::FACE_DEAD:
			ui->toolButton->setIcon(iconDead);
			break;
		case MineBoard::FACE_SMILE:
			ui->toolButton->setIcon(iconSmile);
			break;
		case MineBoard::FACE_SURPRISE:
			ui->toolButton->setIcon(iconSurprise);
			break;
		case MineBoard::FACE_WINNER:
			ui->toolButton->setIcon(iconWinner);
			break;
	}
}
void MainWindow::slotBoard_statusChanged(MineBoard::STATUS status)
{
	ui->lcdNumberMines->display(board->getNumMines() - board->getNumFlags());
	if(status == MineBoard::STATUS_WAITING)
	{
		updateSize();
	}
}


