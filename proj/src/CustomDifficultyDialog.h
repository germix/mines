#ifndef CUSTOMDIFFICULTYDIALOG_H
#define CUSTOMDIFFICULTYDIALOG_H
#ifdef USE_CUSTOM

#include <QDialog>

namespace Ui {
class CustomDifficultyDialog;
}

class CustomDifficultyDialog : public QDialog
{
	Q_OBJECT
	Ui::CustomDifficultyDialog* ui;
public:
	explicit CustomDifficultyDialog(int cols, int rows, int mines, QWidget* parent = 0);
	~CustomDifficultyDialog();
public:
	int getCols() const;
	int getRows() const;
	int getMines() const;
private:
	void accept();
private slots:
	void slotSpinBox_valueChanged(int value);
};

#endif
#endif // CUSTOMDIFFICULTYDIALOG_H
