#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

class QUndoStack;
class QLabel;
class Sudoku;

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

	private slots:
		void loadGame();
		void saveGame();
		void newEasyGame();
		void newMediumGame();
		void newHardGame();
		void newEmptyGame();
		void solveGame();
		void rateGame();
		void giveHint();
		void tick();

	private:
		void resetTimer();

	private:
		Ui::MainWindow *m_ui;
		QUndoStack *m_undoStack;
		Sudoku *m_sudoku;
		QLabel *m_timeLabel;
		QLabel *m_fillLabel;
		QTimer m_gameTimer;
		int m_gameTime;
};

#endif // MAINWINDOW_H
