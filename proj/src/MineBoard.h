#ifndef QMINEBOARD_H
#define QMINEBOARD_H
#include <QWidget>

class MineBoard;
class QMineBoardPrivate;

#if 0
#define MAX_BOARD_COLS		30
#define MAX_BOARD_ROWS		24
#else
#define MAX_BOARD_COLS		50
#define MAX_BOARD_ROWS		50
#endif
//#define MINE_SIZE		16

#define MINE_WIDTH		16
#define MINE_HEIGHT		16

#define COLS_BEGINNER			9
#define ROWS_BEGINNER			9
#define MINES_BEGINNER			10

#define COLS_MEDIUM				16
#define ROWS_MEDIUM				16
#define MINES_MEDIUM			40

#define COLS_EXPERT				30
#define ROWS_EXPERT				16
#define MINES_EXPERT			99

class MineBoard : public QWidget
{
	Q_OBJECT
public:
	enum FACE
	{
		FACE_DEAD,
		FACE_SMILE,
		FACE_SURPRISE,
		FACE_WINNER,
	};
	enum STATUS
	{
		STATUS_WAITING,
		STATUS_PLAYING,
		STATUS_GAMEOVER,
		STATUS_WON,
	};
private:

	enum BOXTYPE
	{
		BOXTYPE_NORMAL,
		BOXTYPE_QUESTION,
		BOXTYPE_FLAG,
		BOXTYPE_COMPLETE,
	};
	struct BOX_STRUCT
	{
		bool	isMine;
		bool	isPressed;
		uint	boxType;
		uint	numMines;
	}boxes[MAX_BOARD_COLS + 2][MAX_BOARD_ROWS + 2];
	struct POINT
	{
		int x;
		int y;
	}pressInfo;

	QImage		images;

	int			width;
	int			height;
	int			numCols;
	int			numRows;
	int			numMines;
	int			numFlags;
	int			boxesLeft;

	bool		mark;

	ulong		time;

	FACE		face;
	STATUS		status;

	bool		isPressed;

	int			timerId;
public:
	MineBoard(QWidget* Parent);
	~MineBoard();
public:
	void	newGame();
	void	newGame(int cols, int rows, int mines);
	int		getNumCols() const
	{
		return numCols;
	}
	int		getNumRows() const
	{
		return numRows;
	}
	int		getNumMines() const
	{
		return numMines;
	}
	int		getNumFlags() const
	{
		return numFlags;
	}
signals:
	void	onTimeChanged(int time);
	void	onFaceChanged(MineBoard::FACE face);
	void	onStatusChanged(MineBoard::STATUS status);
private:
	QSize	sizeHint() const;
	QSize	minimumSizeHint() const;

	void	timerEvent(QTimerEvent* e);

	void	paintEvent(QPaintEvent* e);

	void	mouseMoveEvent(QMouseEvent* e);
	void	mousePressEvent(QMouseEvent* e);
	void	mouseReleaseEvent(QMouseEvent* e);
private:
	void	checkLevel();
	void	createBoard();
	void	createBoxes();
	void	drawMine(QPainter& p, int col, int row, bool isPressed);

	void	testFace(int x, int y, Qt::MouseButton button, bool isPressed);
	void	testMines(int x, int y, Qt::MouseButton button, bool isPressed);
	void	testBoard(int x, int y, Qt::MouseButton button, bool isPressed);
private:
	void	addFlag(int col, int row);

	void	pressBox(int col, int row);
	void	pressBoxes(int col, int row);

	void	unpressBox(int col, int row);
	void	unpressBoxes(int col, int row);

	void	completeBox(int col, int row);
	void	completeBoxes(int col, int row);
};

#endif // QMINEBOARD_H
