#ifdef USE_CUSTOM
#include "CustomDifficultyDialog.h"
#include "ui_CustomDifficultyDialog.h"
#include "MineBoard.h"

CustomDifficultyDialog::CustomDifficultyDialog(int cols, int rows, int mines, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::CustomDifficultyDialog)
{
	ui->setupUi(this);

	setMinimumSize(sizeHint());
	setMaximumSize(sizeHint());

	ui->spinCols->setMinimum(COLS_BEGINNER);
	ui->spinCols->setMaximum(MAX_BOARD_COLS);

	ui->spinRows->setMinimum(ROWS_BEGINNER);
	ui->spinRows->setMaximum(MAX_BOARD_ROWS);

#if 0
	ui->spinMines->setMinimum(MINES_BEGINNER);
#else
	ui->spinMines->setMinimum(1);
#endif
	ui->spinMines->setMaximum((MAX_BOARD_COLS*MAX_BOARD_ROWS)-1);

	ui->spinCols->setValue(cols);
	ui->spinRows->setValue(rows);
	ui->spinMines->setValue(mines);

	connect(ui->btnOk, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->spinCols, SIGNAL(valueChanged(int)), this, SLOT(slotSpinBox_valueChanged(int)));
	connect(ui->spinRows, SIGNAL(valueChanged(int)), this, SLOT(slotSpinBox_valueChanged(int)));
}

CustomDifficultyDialog::~CustomDifficultyDialog()
{
	delete ui;
}
void CustomDifficultyDialog::accept()
{
	QDialog::accept();
}
int CustomDifficultyDialog::getCols() const
{
	return ui->spinCols->value();
}
int CustomDifficultyDialog::getRows() const
{
	return ui->spinRows->value();
}
int CustomDifficultyDialog::getMines() const
{
	return ui->spinMines->value();
}
void CustomDifficultyDialog::slotSpinBox_valueChanged(int value)
{
	Q_UNUSED(value);
	int max = ui->spinCols->value()*ui->spinRows->value();
	if((max-1) < ui->spinMines->value())
	{
		ui->spinMines->setValue(max-1);
	}
}

#endif
