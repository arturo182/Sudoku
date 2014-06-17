#include "canvas.h"

#include "undofield.h"
#include "sudoku.h"

#include <QUndoStack>
#include <QPainter>
#include <QDebug>
#include <QtGui>

Canvas::Canvas(QWidget *parent) :
	QWidget(parent),
	m_pressX(-1),
	m_pressY(-1),
	m_hints(true)
{
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
}

void Canvas::setSudoku(Sudoku *sudoku)
{
	m_sudoku = sudoku;
}

void Canvas::setUndoStack(QUndoStack *undoStack)
{
	m_undoStack = undoStack;
}

void Canvas::setShowHints(bool hints)
{
	m_hints = hints;

	update();
}

void Canvas::paintEvent(QPaintEvent *event)
{
	QPainter p(this);
	p.setRenderHint(QPainter::Antialiasing);

	QRadialGradient background(p.window().center(), p.window().width() * 0.75, p.window().center());
	background.setColorAt(0.0, QColor("#5c8dcb"));
	background.setColorAt(1.0, QColor("#07318f"));
	p.fillRect(p.window(), background);

	p.setFont(QFont("Verdana", 20, QFont::Bold));

	const float margin = 20;
	const float cell = (width() - 2 * margin) / 9.0;

	const int mouseX = (mapFromGlobal(QCursor::pos()).x() - margin) / cell;
	const int mouseY = (mapFromGlobal(QCursor::pos()).y() - margin) / cell;

	p.translate(margin, margin);
	for(int x = 0; x < 9; ++x) {
		for(int y = 0; y < 9; ++y) {
			QRectF rect(x * cell + (x / 3) * 1.5, y * cell + (y / 3) * 1.5, cell * 0.95, cell * 0.95);

			//bg
			p.setPen(Qt::transparent);
			p.setBrush(Qt::white);

			p.drawRoundedRect(rect, 7, 7);

			if((m_pressX == -1) && (m_pressY == -1)) {
				if((x == mouseX) && (y == mouseY)) {
					p.setBrush(QColor("#ffc600"));
					p.drawRoundedRect(rect.adjusted(2.5, 2.5, -2.5, -2.5), 7, 7);
				} else if((x == mouseX) || (y == mouseY)) {
					p.setBrush(QColor("#d2e5fc"));
					p.drawRoundedRect(rect.adjusted(2.5, 2.5, -2.5, -2.5), 7, 7);
				}
			}

			if(m_sudoku->field(x, y) > 0) {
				if(m_sudoku->isDefault(x, y)) {
					p.setPen(QColor("#0066d4"));
				} else {
					p.setPen(Qt::black);
				}

				p.drawText(rect, QString::number(m_sudoku->field(x, y)), QTextOption(Qt::AlignCenter));
			} else {
				if(m_hints) {
					const int r = cell * 0.1;

					p.setPen(Qt::transparent);
					QList<int> options = m_sudoku->options(x, y);
					for(int i = 0; i < 9; ++i) {
						if(options.contains(i + 1)) {
							p.setBrush(QColor("#0066d4"));
						} else {
							p.setBrush(QColor("#b2d1f2"));
						}

						p.drawEllipse(QPointF(12 + rect.left() + (i % 3) * r * 2.5, 12 + rect.top() + (i / 3) * r * 2.5), r, r);
					}
				}
			}
		}
	}

	if((m_pressX > -1) && (m_pressY > -1)) {
		p.fillRect(rect().adjusted(-margin, -margin, 0, 0), QColor(0, 0, 0, 64));

		const QList<int> options = m_sudoku->options(m_pressX, m_pressY) << m_sudoku->field(m_pressX, m_pressY);
		const float left = qBound(0.0, (m_pressX - 1) * cell + ((m_pressX - 1) / 3) * 1.5, width() - cell * 3.0 - margin * 2);
		const float top  = qBound(0.0, (m_pressY - 1) * cell + ((m_pressY - 1) / 3) * 1.5, height() - cell * 3.0 - margin * 2);

		QRectF rect(left, top, cell  * 3, cell * 3);

		p.setPen(QPen(QColor("#07318f"), 3.0));
		p.setBrush(Qt::white);

		p.drawRoundedRect(rect, 7, 7);

		const int mouseX = qFloor((mapFromGlobal(QCursor::pos()).x() - left - margin) / cell);
		const int mouseY = qFloor((mapFromGlobal(QCursor::pos()).y() - top - margin) / cell);

		for(int i = 0; i < 9; ++i) {
			QRectF cellRect(rect.left() + (i % 3) * cell, rect.top() + (i / 3) * cell, cell, cell);

			if((i % 3 == mouseX) && (i / 3 == mouseY)) {
				p.setPen(Qt::transparent);
				p.setBrush(QColor("#ffc600"));
				p.drawRoundedRect(cellRect.adjusted(3, 3, -3, -3), 7, 7);
			}

			if(options.contains(i + 1)) {
				p.setPen(QColor("#0066d4"));
			} else {
				p.setPen(QColor("#b2d1f2"));
			}

			p.drawText(cellRect, QString::number(i + 1), QTextOption(Qt::AlignCenter));
		}
	}
	p.resetTransform();
}

void Canvas::mousePressEvent(QMouseEvent *event)
{
	const float margin = 20;
	const float cell = (width() - 2 * margin) / 9.0;

	const int x = (mapFromGlobal(QCursor::pos()).x() - margin) / cell;
	const int y = (mapFromGlobal(QCursor::pos()).y() - margin) / cell;

	if((x > -1) && (y > -1) && (x < 9) && (y < 9)) {
		if(!m_sudoku->isDefault(x, y)) {
			if(event->button() == Qt::LeftButton) {
				m_pressX = x;
				m_pressY = y;
			} else {
				m_undoStack->push(new UndoField(m_sudoku, this, x, y, m_sudoku->field(x, y), 0));

				m_sudoku->setField(x, y, 0);
			}
		}
	}

	update();
}

void Canvas::mouseMoveEvent(QMouseEvent *event)
{
	update();
}

void Canvas::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() == Qt::LeftButton) {
		if((m_pressX > -1) && (m_pressY > -1) && (m_pressX < 9) && (m_pressY < 9)) {
			if(!m_sudoku->isDefault(m_pressX, m_pressY)) {
				const float margin = 20;
				const float cell = (width() - 2 * margin) / 9.0;
				const float left = qBound(0.0, (m_pressX - 1) * cell + ((m_pressX - 1) / 3) * 1.5, width() - cell * 3.0 - margin * 2);
				const float top  = qBound(0.0, (m_pressY - 1) * cell + ((m_pressY - 1) / 3) * 1.5, height() - cell * 3.0 - margin * 2);
				const int x = qFloor((mapFromGlobal(QCursor::pos()).x() - left - margin) / cell);
				const int y = qFloor((mapFromGlobal(QCursor::pos()).y() - top - margin) / cell);

				if(((x > -1) && (y > -1)) && ((x < 3) && (y < 3))) {
					const int i = x + y * 3 + 1;
					if(m_sudoku->isValid(m_pressX, m_pressY, i)) {
						m_undoStack->push(new UndoField(m_sudoku, this, m_pressX, m_pressY, m_sudoku->field(m_pressX, m_pressY), i));

						m_sudoku->setField(m_pressX, m_pressY, i);
					}
				}
			}
		}
	}

	m_pressX = -1;
	m_pressY = -1;

	update();
}

void Canvas::keyPressEvent(QKeyEvent *event)
{
	if((event->key() >= Qt::Key_0) && (event->key() <= Qt::Key_9)) {
		const float margin = 20;
		const float cell = (width() - 2 * margin) / 9.0;

		const int mouseX = (mapFromGlobal(QCursor::pos()).x() - margin) / cell;
		const int mouseY = (mapFromGlobal(QCursor::pos()).y() - margin) / cell;

		if((mouseX > -1) && (mouseY > -1) && (mouseX < 9) && (mouseY < 9)) {
			if(!m_sudoku->isDefault(mouseX, mouseY)) {
				const int i = event->key() - Qt::Key_0;

				if(m_sudoku->isValid(mouseX, mouseY, i) || (i == 0)) {
					m_undoStack->push(new UndoField(m_sudoku, this, mouseX, mouseY, m_sudoku->field(mouseX, mouseY), i));

					m_sudoku->setField(mouseX, mouseY, i);

					update();
				}
			}
		}
	}
}
