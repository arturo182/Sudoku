#ifndef UNDOFIELD_H
#define UNDOFIELD_H

#include <QUndoCommand>

class Sudoku;
class Canvas;

class UndoField : public QUndoCommand
{
	public:
		UndoField(Sudoku *sudoku, Canvas *canvas, const int &x, const int &y, const int &prev, const int &curr);

		void undo();
		void redo();

	private:
		Sudoku *m_sudoku;
		Canvas *m_canvas;
		int m_x;
		int m_y;
		int m_prev;
		int m_curr;

};

#endif // UNDOFIELD_H
