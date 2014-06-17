#include "undosolve.h"

#include "canvas.h"
#include "sudoku.h"

UndoSolve::UndoSolve(Sudoku *sudoku, Canvas *canvas, const QString &before, const QString &after) :
	m_sudoku(sudoku),
	m_canvas(canvas),
	m_before(before),
	m_after(after)
{
}

void UndoSolve::undo()
{
	m_sudoku->fromString(m_before);
	m_canvas->update();
}

void UndoSolve::redo()
{
	m_sudoku->fromString(m_after);
	m_canvas->update();
}
