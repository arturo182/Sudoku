#ifndef UNDOSOLVE_H
#define UNDOSOLVE_H

#include <QUndoCommand>

class Sudoku;
class Canvas;

class UndoSolve : public QUndoCommand
{
	public:
		UndoSolve(Sudoku *sudoku, Canvas *canvas, const QString &before, const QString &after);

		void undo();
		void redo();

	private:
		Sudoku *m_sudoku;
		Canvas *m_canvas;
		QString m_before;
		QString m_after;
};

#endif // UNDOSOLVE_H
