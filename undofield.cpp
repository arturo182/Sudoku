#include "undofield.h"

#include "canvas.h"
#include "sudoku.h"

#include <QDebug>

UndoField::UndoField(Sudoku *sudoku, Canvas *canvas, const int &x, const int &y, const int &prev, const int &curr) :
	m_sudoku(sudoku),
	m_canvas(canvas),
	m_x(x),
	m_y(y),
	m_prev(prev),
	m_curr(curr)
{
}

void UndoField::undo()
{
	m_sudoku->setField(m_x, m_y, m_prev);
	m_canvas->update();
}

void UndoField::redo()
{
	m_sudoku->setField(m_x, m_y, m_curr);
	m_canvas->update();
}
