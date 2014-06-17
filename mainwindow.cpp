#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "sudoku.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QUndoStack>
#include <QDebug>
#include <QLabel>
#include <QFile>
#include "undosolve.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	m_ui(new Ui::MainWindow),
	m_undoStack(new QUndoStack(this)),
	m_sudoku(new Sudoku),
	m_timeLabel(new QLabel("00:00:00")),
	m_fillLabel(new QLabel("81/81")),
	m_gameTime(0)
{
	m_ui->setupUi(this);
	m_ui->canvas->setSudoku(m_sudoku);
	m_ui->canvas->setUndoStack(m_undoStack);

	m_ui->statusBar->addWidget(m_timeLabel);
	m_ui->statusBar->addPermanentWidget(m_fillLabel);

	m_gameTimer.setInterval(100);
	connect(&m_gameTimer, SIGNAL(timeout()), SLOT(tick()));

	connect(m_undoStack, SIGNAL(canUndoChanged(bool)), m_ui->undoAction, SLOT(setEnabled(bool)));
	connect(m_undoStack, SIGNAL(canRedoChanged(bool)), m_ui->redoAction, SLOT(setEnabled(bool)));

	connect(m_ui->undoAction, SIGNAL(triggered()), m_undoStack, SLOT(undo()));
	connect(m_ui->redoAction, SIGNAL(triggered()), m_undoStack, SLOT(redo()));


	newEmptyGame();
}

MainWindow::~MainWindow()
{
	delete m_ui;
	delete m_sudoku;
}

void MainWindow::loadGame()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Otwórz sudoku", QString(), "Pliki tekstowe (*.txt)");
	if(!fileName.isEmpty()) {
		QFile file(fileName);
		if(file.open(QFile::ReadOnly | QFile::Text)) {
			QString content = file.readAll();
			m_sudoku->clear();
			m_sudoku->fromString(content, true);
			m_ui->canvas->update();

			file.close();
		}
	}

	m_undoStack->clear();
	resetTimer();
}

void MainWindow::saveGame()
{
	QString fileName = QFileDialog::getSaveFileName(this, "Otwórz sudoku", QString(), "Pliki tekstowe (*.txt)");
	if(!fileName.isEmpty()) {
		QFile file(fileName);
		if(file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text)) {
            file.write(m_sudoku->toString().toLatin1());

			file.close();
		}
	}
}

void MainWindow::newEasyGame()
{
	m_undoStack->clear();
	m_sudoku->generate(Sudoku::EASY);
	m_ui->canvas->update();

	resetTimer();
}

void MainWindow::newMediumGame()
{
	m_undoStack->clear();
	m_sudoku->generate(Sudoku::MEDIUM);
	m_ui->canvas->update();

	resetTimer();
}

void MainWindow::newHardGame()
{
	m_undoStack->clear();
	m_sudoku->generate(Sudoku::HARD);
	m_ui->canvas->update();

	resetTimer();
}

void MainWindow::newEmptyGame()
{
	m_undoStack->clear();
	m_sudoku->clear();
	m_ui->canvas->update();

	resetTimer();
}

void MainWindow::solveGame()
{
	QString before = m_sudoku->toString();

	m_sudoku->solve();
	m_ui->canvas->update();

	qDebug() << "rating" << m_sudoku->rating();
	m_undoStack->push(new UndoSolve(m_sudoku, m_ui->canvas, before, m_sudoku->toString()));

	if(m_sudoku->emptyCount() > 0) {
		QMessageBox::warning(this, "Brak rozwiązań", "To Sudoku nie ma poprawnych rozwiązań :(");
	} else if(m_sudoku->emptyCount() == 0) {
		m_gameTimer.stop();

		QMessageBox::information(this, "Gratualcje", "Rozwiązałeś Sudoku!");
	}
}

void MainWindow::rateGame()
{
	Sudoku s;
	s.fromString(m_sudoku->toString());
	s.solve();

	QMessageBox::information(this, "Ocena Sudoku", "Ocena tego Sudoku to: " + QString::number(s.rating()));
}

void MainWindow::giveHint()
{
	if(m_sudoku->emptyCount() > 0) {
		Sudoku s;
		s.fromString(m_sudoku->toString());
		if(s.solve()) {
			int x;
			int y;

			do {
				x = qrand() % 9;
				y = qrand() % 9;
			} while(m_sudoku->field(x, y) != 0);

			m_sudoku->setField(x, y, s.field(x, y));

			update();
		}
	}
}

void MainWindow::tick()
{
	++m_gameTime;

	int seconds = m_gameTime / 10;
	int minutes = seconds / 60;
	int hours = minutes / 60;
	minutes %= 60;
	seconds %= 60;

	m_timeLabel->setText(QString("%1%2:%3%4:%5%6").arg(hours < 10 ? "0" : "").arg(hours).arg(minutes < 10 ? "0" : "").arg(minutes).arg(seconds < 10 ? "0" : "").arg(seconds));
	m_fillLabel->setText(QString("%1/81").arg(m_sudoku->emptyCount()));

	if(m_sudoku->emptyCount() == 0) {
		m_gameTimer.stop();

		QMessageBox::information(this, "Gratualcje", "Rozwiązałeś Sudoku!");
	}
}

void MainWindow::resetTimer()
{
	m_gameTime = 0;
	m_gameTimer.start();
	m_timeLabel->setText("00:00:00");
}
