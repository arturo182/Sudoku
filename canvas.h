#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>

class QUndoStack;
class Sudoku;

class Canvas : public QWidget
{
	Q_OBJECT

	public:
		 Canvas(QWidget *parent = 0);

		 void setSudoku(Sudoku *sudoku);
		 void setUndoStack(QUndoStack *undoStack);

	public slots:
		 void setShowHints(bool hints);

	protected:
		void paintEvent(QPaintEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void keyPressEvent(QKeyEvent *event);

	private:
		Sudoku *m_sudoku;
		QUndoStack *m_undoStack;
		int m_pressX;
		int m_pressY;
		bool m_hints;

};

#endif // CANVAS_H
