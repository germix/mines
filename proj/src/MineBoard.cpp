#include "MineBoard.h"
#include <time.h>
#include <QPainter>
#include <QMouseEvent>


typedef enum
{
	MPRESS_BMP, ONE_BMP, TWO_BMP, THREE_BMP, FOUR_BMP, FIVE_BMP, SIX_BMP,
	SEVEN_BMP, EIGHT_BMP, BOX_BMP, FLAG_BMP, QUESTION_BMP, EXPLODE_BMP,
	WRONG_BMP, MINE_BMP, QPRESS_BMP
} MINEBMP_OFFSET;

///////////////////////////////////////////////////////////////////////////////////////////////////

MineBoard::MineBoard(QWidget* Parent) : QWidget(Parent)
{
//	setMouseTracking(true);
	// ...
	images = QImage(":/mines.bmp");
	if(images.isNull())
	{
		qDebug("Couldn't mines image loaded");
	}
	numCols = COLS_BEGINNER;
	numRows = ROWS_BEGINNER;
	numMines = MINES_BEGINNER;

	mark = true;

	status = STATUS_WAITING;
	timerId = 0;
	pressInfo.x = 0;
	pressInfo.y = 0;
	createBoard();
}
MineBoard::~MineBoard()
{
}
void		MineBoard::newGame()
{
	createBoard();
}
void		MineBoard::newGame(int cols, int rows, int mines)
{
	numCols = cols;
	numRows = rows;
	numMines = mines;
	checkLevel();
	createBoard();
}
QSize		MineBoard::sizeHint() const
{
	return QSize(width, height);
}
QSize		MineBoard::minimumSizeHint() const
{
	return QSize(width, height);
}
void		MineBoard::timerEvent(QTimerEvent* e)
{
	Q_UNUSED(e);
	time++;
	emit onTimeChanged(time);
}
void		MineBoard::paintEvent(QPaintEvent* e)
{
	Q_UNUSED(e);
	QPainter p(this);
	int row;
	int col;

	// Dibujar minas
	for(row = 1; row <= numRows; row++)
	{
		for(col = 1; col <= numCols; col++)
		{
			drawMine(p, col, row, false);
		}
	}
}
void		MineBoard::mouseMoveEvent(QMouseEvent* e)
{
	Qt::MouseButton button;

	if(e->buttons() & Qt::MiddleButton)
		button = Qt::MiddleButton;
	else if(e->buttons() & Qt::LeftButton && e->buttons() & Qt::RightButton)
		button = Qt::MiddleButton;
	else if(e->buttons() & Qt::LeftButton)
		button = Qt::LeftButton;
	else
		return;

	testBoard(e->x(), e->y(), button, true);
}
void		MineBoard::mousePressEvent(QMouseEvent* e)
{
	Qt::MouseButton button;

	button = e->button();
	if(button == Qt::LeftButton)
	{
		if(e->buttons() & Qt::RightButton)
			button = Qt::MiddleButton;
	}
	else if(button == Qt::RightButton)
	{
		if(e->buttons() & Qt::LeftButton)
		{
			button = Qt::MiddleButton;
			pressInfo.x = 0;
			pressInfo.y = 0;
		}
	}
	testBoard(e->x(), e->y(), button, (e->type() == QEvent::MouseButtonPress));
}
void		MineBoard::mouseReleaseEvent(QMouseEvent* e)
{
	Qt::MouseButton button;

	button = e->button();
	if(button == Qt::LeftButton)
	{
		if(e->buttons() & Qt::RightButton)
			button = Qt::MiddleButton;
	}
	else if(button == Qt::RightButton)
	{
		if(e->buttons() & Qt::LeftButton)
			button = Qt::MiddleButton;
	}
	testBoard(e->x(), e->y(), button, (e->type() == QEvent::MouseButtonPress));
}
void		MineBoard::checkLevel()
{
	if(numRows < ROWS_BEGINNER)
		numRows = ROWS_BEGINNER;

	if(numRows > MAX_BOARD_ROWS)
		numRows = MAX_BOARD_ROWS;

	if(numCols < COLS_BEGINNER)
		numCols = COLS_BEGINNER;

	if(numCols > MAX_BOARD_COLS)
		numCols = MAX_BOARD_COLS;

#if 0
	if(numMines < MINES_BEGINNER)
		numMines = MINES_BEGINNER;
#else
	if(numMines < 1)
		numMines = 1;
#endif

	if(numMines > numCols * numRows - 1)
		numMines = numCols * numRows - 1;
}
void		MineBoard::createBoard()
{
	if(timerId)
	{
		killTimer(timerId);
		timerId = 0;
	}
	numFlags = 0;
	boxesLeft = numCols * numRows - numMines;

	createBoxes();

	width = numCols * MINE_WIDTH;
	height = numRows * MINE_HEIGHT;

	time = 0;
	face = FACE_SMILE;
	status = STATUS_WAITING;

	setMinimumSize(sizeHint());
	setMaximumSize(sizeHint());
	update();

	emit onTimeChanged(time);
	emit onFaceChanged(face);
	emit onStatusChanged(status);
}
void		MineBoard::createBoxes()
{
	int i, j;
	int col, row;

	srand((unsigned int)::time(NULL));

	// ...
	for(col = 0; col <= numCols + 1; col++)
	{
		for(row = 0; row <= numRows + 1; row++)
		{
			boxes[col][row].isMine		 = false;
			boxes[col][row].isPressed		 = false;
			boxes[col][row].boxType		 = BOXTYPE_NORMAL;
			boxes[col][row].numMines		 = 0;
		}
	}
	// Crear minas
	i = 0;
	while(i < numMines)
	{
		col = (int)(numCols * (float)rand() / RAND_MAX + 1);
		row = (int)(numRows * (float)rand() / RAND_MAX + 1);

		if(!boxes[col][row].isMine)
		{
			i++;
			boxes[col][row].isMine = true;
		}
	}
	// Indicar al resto de las cajas
	// el número de minas que les rodea
	for(col = 1; col < numCols + 1; col++)
	{
		for(row = 1; row < numRows + 1; row++)
		{
			for(i = -1; i <= 1; i++)
			{
				for(j = -1; j <= 1; j++)
				{
					if(boxes[col + i][row + j].isMine)
					{
						boxes[col][row].numMines++;
					}
				}
			}
		}
	}
}
void		MineBoard::drawMine(QPainter& p, int col, int row, bool isPressed)
{
	Q_UNUSED(isPressed);
	BOX_STRUCT* box;
	MINEBMP_OFFSET offset = BOX_BMP;

	if(col == 0 || col > numCols || row == 0 || row > numRows)
		return;

	box = &boxes[col][row];
	if(status == STATUS_GAMEOVER)
	{
		if(box->isMine)
		{
			switch(box->boxType)
			{
				case BOXTYPE_FLAG:
					offset = FLAG_BMP;
					break;
				case BOXTYPE_COMPLETE:
					offset = EXPLODE_BMP;
					break;
				case BOXTYPE_NORMAL:
				case BOXTYPE_QUESTION:
					offset = MINE_BMP;
					break;
			}
		}
		else
		{
			switch(box->boxType)
			{
				case BOXTYPE_QUESTION:
					offset = QUESTION_BMP;
					break;
				case BOXTYPE_FLAG:
					offset = WRONG_BMP;
					break;
				case BOXTYPE_NORMAL:
					offset = BOX_BMP;
					break;
				case BOXTYPE_COMPLETE:
					// Nada
					break;
				default:
					qDebug("Unknown BoxType during game over in drawMine\n");
					break;
			}
		}
	}
	else
	{
		// STATUS_WAITING ó STATUS_PLAYING
		switch(box->boxType)
		{
			case BOXTYPE_QUESTION:
//				if(!isPressed)
				if(!box->isPressed)
					offset = QUESTION_BMP;
				else
					offset = QPRESS_BMP;
				break;
			case BOXTYPE_FLAG:
				offset = FLAG_BMP;
				break;
			case BOXTYPE_NORMAL:
//				if(!isPressed)
				if(!box->isPressed)
					offset = BOX_BMP;
				else
					offset = MPRESS_BMP;
				break;
			case BOXTYPE_COMPLETE:
				// Nada
				break;
			default:
				qDebug("Unknown BoxType while playing in drawMine\n");
				break;
		}
	}
	if(box->boxType == BOXTYPE_COMPLETE && !box->isMine)
	{
		offset = (MINEBMP_OFFSET)box->numMines;
	}
	p.drawImage(
				(col - 1) * MINE_WIDTH,
				(row - 1) * MINE_HEIGHT,
				images,
				0, offset * MINE_HEIGHT, MINE_WIDTH, MINE_HEIGHT);
}
void		MineBoard::testFace(int x, int y, Qt::MouseButton button, bool isPressed)
{
	Q_UNUSED(x);
	Q_UNUSED(y);
	if(status == STATUS_PLAYING || status == STATUS_WAITING)
	{
		if(isPressed && (button == Qt::LeftButton || button == Qt::MiddleButton))
			face = FACE_SURPRISE;
		else
			face = FACE_SMILE;
	}
	else if(status == STATUS_GAMEOVER)
		face = FACE_DEAD;
	else if(status == STATUS_WON)
		face = FACE_WINNER;
}
void		MineBoard::testMines(int x, int y, Qt::MouseButton button, bool isPressed)
{
#ifdef Q_DEBUG
	qDebug("MineBoard::testMines");
#endif
	int col = (x / MINE_WIDTH) + 1;
	int row = (y / MINE_HEIGHT) + 1;

	if(button == Qt::LeftButton)
	{
		if(isPressed)
		{
//			if(pressInfo.x != Col || pressInfo.y != Row)
			{
				unpressBox(pressInfo.x, pressInfo.y);
				pressInfo.x = col;
				pressInfo.y = row;
				unpressBox(pressInfo.x, pressInfo.y);
			}
		}
		else
		{
//			if(pressInfo.x != col || pressInfo.y != row)
				unpressBox(pressInfo.x, pressInfo.y);

			pressInfo.x = 0;
			pressInfo.y = 0;

			if(boxes[col][row].boxType != BOXTYPE_FLAG)
			{
				status = STATUS_PLAYING;
			}
			completeBox(col, row);
		}
	}
	else if(button == Qt::RightButton)
	{
#ifdef Q_DEBUG
		qDebug("Qt::RightButton");
#endif
		if(isPressed)
		{
			addFlag(col, row);
			status = STATUS_PLAYING;
		}
	}
	else if(button == Qt::MiddleButton)
	{
#ifdef Q_DEBUG
		qDebug("Qt::MiddleButton");
#endif
//		if(pressInfo.x != col || pressInfo.y != row)
		{
			unpressBoxes(pressInfo.x, pressInfo.y);
		}
		pressInfo.x = 0;
		pressInfo.y = 0;
		if(isPressed)
		{
			pressBoxes(col, row);
		}
		else
		{
			completeBoxes(col, row);
		}
	}
}
void		MineBoard::testBoard(int x, int y, Qt::MouseButton button, bool isPressed)
{
	if(status == STATUS_GAMEOVER || status == STATUS_WON)
		return;
	// ...
//	FACE oldFace = face;
	STATUS oldStatus = status;

	if(status != STATUS_GAMEOVER && status != STATUS_WON)
	{
		testMines(x, y, button, isPressed);
	}
	else
	{
		unpressBoxes(pressInfo.x, pressInfo.y);
		pressInfo.x = 0;
		pressInfo.y = 0;
	}
	if(boxesLeft == 0)
	{
		status = STATUS_WON;

		for(int col = 0; col <= numCols + 1; col++)
		{
			for(int row = 0; row <= numRows + 1; row++)
			{
				if(boxes[col][row].isMine)
				{
					boxes[col][row].boxType = BOXTYPE_FLAG;
				}
			}
		}
		numFlags = numMines;
	}
	testFace(x, y, button, isPressed);
	// ...
	update();
	// ...
	if(status == STATUS_PLAYING && oldStatus != STATUS_PLAYING)
	{
		time++;
		emit onTimeChanged(time);
		timerId = startTimer(1000);
	}
	if(status == STATUS_GAMEOVER || status == STATUS_WON)
	{
		killTimer(timerId);
		timerId = 0;
	}
	emit onFaceChanged(face);
	emit onStatusChanged(status);
}
void		MineBoard::addFlag(int col, int row)
{
	if(boxes[col][row].boxType != BOXTYPE_COMPLETE)
	{
		switch(boxes[col][row].boxType)
		{
			case BOXTYPE_FLAG:
				if(mark)
					boxes[col][row].boxType = BOXTYPE_QUESTION;
				else
					boxes[col][row].boxType = BOXTYPE_NORMAL;

				numFlags--;
				break;
			case BOXTYPE_QUESTION:
				boxes[col][row].boxType = BOXTYPE_NORMAL;
				break;
			default:
				boxes[col][row].boxType = BOXTYPE_FLAG;
				numFlags++;
				break;
		}
	}
}
void		MineBoard::pressBox(int col, int row)
{
#if 1	//!!!
	boxes[col][row].isPressed = true;
#else
	QPainter p(this);
	drawMine(p, col, row, true);
#endif
}
void		MineBoard::pressBoxes(int col, int row)
{
	int i, j;

	for(i = -1; i <= 1; i++)
	{
		for(j = -1; j <= 1; j++)
		{
			boxes[col + i][row + j].isPressed = true;
			pressBox(col + i, row + j);
		}
	}
	for(i = -1; i <= 1; i++)
	{
		for(j = -1; j <= 1; j++)
		{
			if(!boxes[pressInfo.x + i][pressInfo.y + j].isPressed)
			{
				unpressBox(pressInfo.x + i, pressInfo.y + j);
			}
		}
	}
	for(i = -1; i <= 1; i++)
	{
		for(j = -1; j <= 1; j++)
		{
			boxes[col + i][row + j].isPressed = false;
			pressBox(col + i, row + j);
		}
	}
	pressInfo.x = col;
	pressInfo.y = row;
}
void		MineBoard::unpressBox(int col, int row)
{
#ifdef Q_DEBUG
	qDebug("MineBoard::unpressBox");
#endif
#if 1	//!!!
	boxes[col][row].isPressed = false;
#else
	QPainter p(this);
	drawMine(p, col, row, false);
#endif
}
void		MineBoard::unpressBoxes(int col, int row)
{
#ifdef Q_DEBUG
	qDebug("MineBoard::unpressBoxes");
#endif
	for(int i = -1; i <= 1; i++)
	{
		for(int j = -1; j <= 1; j++)
		{
			unpressBox(col + i, row + j);
		}
	}
}
void		MineBoard::completeBox(int col, int row)
{
	if(boxes[col][row].boxType != BOXTYPE_COMPLETE &&
			boxes[col][row].boxType != BOXTYPE_FLAG &&
			col > 0 && col < numCols + 1 && row > 0 && row < numRows + 1)
	{
		boxes[col][row].boxType = BOXTYPE_COMPLETE;

		if(boxes[col][row].isMine)
		{
			face = FACE_DEAD;
			status = STATUS_GAMEOVER;
		}
		else if(status != STATUS_GAMEOVER)
		{
			boxesLeft--;
		}
		if(boxes[col][row].numMines == 0 )
		{
			for(int i = -1; i <= 1; i++)
				for(int j = -1; j <= 1; j++)
					completeBox(col + i, row + j);
		}
	}
}
void		MineBoard::completeBoxes(int col, int row)
{
	int i, j;
	uint flags = 0;

	if(boxes[col][row].boxType == BOXTYPE_COMPLETE)
	{
		for(i = -1; i <= 1; i++)
		{
			for(j = -1; j <= 1; j++)
			{
				if(boxes[col + i][row + j].boxType == BOXTYPE_FLAG)
					flags++;
			}
		}
		if(flags == boxes[col][row].numMines)
		{
			for(i = -1; i <= 1; i++)
			{
				for(j = -1; j <= 1; j++)
				{
					if(boxes[col + i][row + j].boxType != BOXTYPE_FLAG)
						completeBox(col + i, row + j);
				}
			}
		}
	}
}


