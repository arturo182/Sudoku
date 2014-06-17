#include "sudoku.h"

#include <QDebug>
#include <QStringList>
#include <QMessageBox>

Sudoku::Sudoku() :
	m_rating(0)
{
	clear();
}

int Sudoku::rating() const
{
	return m_rating;
}

void Sudoku::setField(const int &x, const int &y, const int &i)
{
	m_fields[x + y * 9] = i;
}

int Sudoku::field(const int &x, const int &y) const
{
	return m_fields[x + y * 9];
}

void Sudoku::setDefault(const int &x, const int &y, const int &i)
{
	m_defaults[x + y * 9] = i;
	setField(x, y, i);
}

bool Sudoku::isDefault(const int &x, const int &y) const
{
	return (m_defaults[x + y * 9] > 0);
}

bool Sudoku::isValid(const int &x, const int &y, const int &i) const
{
	return options(x, y).contains(i);
}

int Sudoku::emptyCount() const
{
	return m_fields.count(0);
}

void Sudoku::fromString(const QString &str, bool defaults)
{
	QStringList lines = str.split("\n");
	for(int y = 0; y < qMin(9, lines.count()); ++y) {
		QStringList fields = lines[y].split(" ");
		for(int x = 0; x < qMin(9, fields.count()); ++x) {
			setField(x, y, fields[x].toInt());
		}
	}

	if(defaults) {
		m_defaults = m_fields;
	}

	m_rating = 0;
}

void Sudoku::clear()
{
	clearFields();
	clearDefaults();
}

void Sudoku::clearFields()
{
	m_fields = QVector<int>(81, 0).toList();
}

void Sudoku::clearDefaults()
{
	m_defaults = QVector<int>(81, 0).toList();
}

void Sudoku::generate(const Difficulty &dif)
{
	int start;
	switch(dif) {
		case EASY:
			start = 30;
		break;

		case MEDIUM:
			start = 25;
		break;

		case HARD:
			start = 22;
		break;
	}

	int tries = 0;
	while(true) {
		++tries;

		clear();
		while(81 - emptyCount() < start) {
			int x = qrand() % 9;
			int y = qrand() % 9;
			int i = qrand() % 9;

			if(isValid(x, y, i)) {
				setField(x, y, i);
			}
		}

		Sudoku tmp;
		tmp.fromString(toString());
		tmp.solve();
		if(tmp.emptyCount() == 0) {
			qDebug() << tries << "tries," << tmp.rating() << "rating";

			break;
		}
	}

	m_defaults = m_fields;
}

QList<int> Sudoku::options(const int &x, const int &y) const
{
	QList<int> options = QList<int>() << 1 << 2 << 3 << 4 << 5 << 6 << 7 << 8 << 9;

	//row && cols
	for(int i = 0; i < 9; ++i) {
		options.removeOne(field(i, y));
		options.removeOne(field(x, i));
	}

	//square
	const int squareRow = 3 * (x / 3);
	const int squareCol = 3 * (y / 3);

	for(int i = 0; i < 3; ++i) {
		for(int j = 0; j < 3; ++j) {
			if((squareRow + i != x) && (squareCol + j != y)) {
				options.removeOne(field(squareRow + i, squareCol + j));
			}
		}
	}

	return options;
}

bool Sudoku::solve()
{
	if(emptyCount() != 0) {
		int prev;
		int curr = emptyCount();

		do {
			solveSingles();
			solveUniques();

			prev = curr;
			curr = emptyCount();
		} while(curr < prev);
	}

	if(emptyCount() != 0) {
		return solveBacktracking();
	}

	return true;
}

void Sudoku::solveSingles()
{
	bool hasWork = true;
	while(hasWork) {
		hasWork = false;

		for(int x = 0; x < 9; ++x) {
			for(int y = 0; y < 9; ++y) {
				if(field(x, y) == 0) {
					QList<int> opts = options(x, y);
					if(opts.count() == 1) {
						setField(x, y, opts[0]);

						m_rating += 2;
						hasWork = true;
					}
				}
			}
		}

		if(!hasWork) {
			++m_rating;
		}
	}
}

void Sudoku::solveUniques()
{
	bool hasWork = true;
	while(hasWork) {
		hasWork = false;

		//cols
		for(int x = 0; x < 9; ++x) {
			QVector<int> places(9, 0);

			for(int y = 0; y < 9; ++y) {
				if(field(x, y) == 0) {
					QList<int> opts = options(x, y);
					for(int i = 0; i < opts.count(); ++i) {
						++places[opts[i] - 1];
					}
				}
			}

			for(int i = 0; i < 9; ++i) {
				if(places[i] == 1) {
					for(int j = 0; j < 9; ++j) {
						if(field(x, j) == 0) {
							QList<int> opts = options(x, j);
							if(opts.contains(i + 1)) {
								setField(x, j, i + 1);

								m_rating += 2;
								hasWork = true;
							}
						}
					}
				}
			}
		}

		//rows
		for(int y = 0; y < 9; ++y) {
			QVector<int> places(9, 0);

			for(int x = 0; x < 9; ++x) {
				if(field(x, y) == 0) {
					QList<int> opts = options(x, y);
					for(int i = 0; i < opts.count(); ++i) {
						++places[opts[i] - 1];
					}
				}
			}

			for(int i = 0; i < 9; ++i) {
				if(places[i] == 1) {

					for(int j = 0; j < 9; ++j) {
						if(field(j, y) == 0) {
							QList<int> opts = options(j, y);
							if(opts.contains(i + 1)) {
								setField(j, y, i + 1);

								m_rating += 2;
								hasWork = true;
							}
						}
					}
				}
			}
		}

		//squares
		for(int k = 0; k < 9; ++k) {
			int xb = 3 * ((int) k / 3);
			int yb = 3 * ( k % 3);
			QVector<int> places(9, 0);

			for(int i = 0; i < 9; ++i) {
				int x = xb + i % 3;
				int y = yb + (int)i / 3;

				if(field(x, y) == 0) {
					QList<int> opts = options(x, y);
					for(int j = 0; j < opts.count(); ++j) {
						++places[opts[j] - 1];
					}
				} else {
					places[field(x, y) - 1] = 10;
				}
			}

			for(int i = 0; i < 9; ++i) {
				if(places[i] == 1) {
					for(int k = 0; k < 9; ++k) {
						int x = xb + k % 3;
						int y = yb + (int)k / 3;

						if(field(x, y) == 0) {
							QList<int> opts = options(x, y);
							if(opts.contains(i + 1)) {
								setField(x, y, i + 1);

								m_rating += 2;
								hasWork = true;
							}
						}
					}
				}
			}
		}


		if(!hasWork) {
			++m_rating;
		}
	}
}

bool Sudoku::solveBacktracking()
{
	m_rating += 10;

	for(int y = 0; y < 9; ++y) {
		for(int x = 0; x < 9; ++x) {
			if(field(x, y) == 0) {
				QList<int> opts = options(x, y);

				if(opts.count() == 0) {
					return false;
				}

				for(int z = 0; z < opts.count(); ++z) {
					Sudoku tmp;
					tmp.fromString(toString());
					tmp.setField(x, y, opts[z]);

					if(tmp.solve()) {
						int rating = m_rating + tmp.rating();

						clearFields();
						fromString(tmp.toString());
						m_rating += rating;

						return true;
					}
				}

				return false;
			}
		}
	}

	return false;
}

QString Sudoku::toString() const
{
	QString str;
	for(int y = 0; y < 9; ++y) {
		for(int x = 0; x < 9; ++x) {
			str += QString::number(field(x, y));
			str += " ";
		}

		str.chop(1);
		str += "\n";
	}

	return str;
}
