#ifndef SUDOKU_H
#define SUDOKU_H

#include <QList>

class Sudoku
{
	public:
		enum Difficulty
		{
			EASY = 0 ,
			MEDIUM,
			HARD
		};

	public:
		Sudoku();

		int rating() const;

		void setField(const int &x, const int &y, const int &i);
		int field(const int &x, const int &y) const;

		void setDefault(const int &x, const int &y, const int &i);
		bool isDefault(const int &x, const int &y) const;
		bool isValid(const int &x, const int &y, const int &i) const;
		int emptyCount() const;

		void fromString(const QString &str, bool defaults = false);
		QString toString() const;

		void clear();
		void clearFields();
		void clearDefaults();
		void generate(const Difficulty &dif);
		bool solve();

		QList<int> options(const int &x, const int &y) const;

	private:
		void solveSingles();
		void solveUniques();
		bool solveBacktracking();

	private:
		QList<int> m_fields;
		QList<int> m_defaults;
		int m_rating;
};

#endif // SUDOKU_H
