//Vladislav Boboshko@33504/3
#include "bdd.h"
#include <fstream>

using namespace std;

#define N_VAR 144	// число булевых переменных
#define N 9			// число объектов
#define M 4			// число свойств
#define LOG_N 4
const unsigned k = 12;

ofstream out;

void fun(char* varset, int size); // функция, используемая для вывода решений

void restrict4(bdd& dst,bdd p1[][N],int idx1,bdd p2[][N],int idx2) { //ограничение 4: отношение соседства
	bdd temp1 = bddtrue;
	bdd temp2 = bddtrue;
	for (unsigned i = 0; i < N - 1; i++) temp1 &= !( p2[i][idx2] ^ p1[i + 1][idx1] );
	for (unsigned i = 0; i < N - 1; i++) temp2 &= !( p1[i][idx1] ^ p2[i + 1][idx2] );
	dst &= temp1 | temp2;
}

int main(void)
{
	// инициализация
	bdd_init(1000000, 10000);	// выделение памяти под строки таблицы ( начальное число вершин и статический кэш для операции)
	bdd_setvarnum(N_VAR);	// установка числа булевых пер. для решения зад.

	// ->--- вводим функцию p(k, i, j) следующим образом ( pk[i][j] ):
	bdd p1[N][N];		// массивы, кодирующие свойства,
	bdd p2[N][N];		// pn[N][N], где n – номер свойства,
	bdd p3[N][N];			// N – количество объектов
	bdd p4[N][N];

	unsigned I = 0;
	for (unsigned i = 0; i < N; i++)
	{
		for (unsigned j = 0; j < N; j++)
		{
			p1[i][j] = bddtrue;
			for (unsigned k = 0; k < LOG_N; k++) p1[i][j] &= ((j >> k) & 1) ? bdd_ithvar(I + k) : bdd_nithvar(I + k);
			p2[i][j] = bddtrue;
			for (unsigned k = 0; k < LOG_N; k++) p2[i][j] &= ((j >> k) & 1) ? bdd_ithvar(I + LOG_N + k) : bdd_nithvar(I + LOG_N + k);
			p3[i][j] = bddtrue;
			for (unsigned k = 0; k < LOG_N; k++) p3[i][j] &= ((j >> k) & 1) ? bdd_ithvar(I + LOG_N * 2 + k) : bdd_nithvar(I + LOG_N * 2 + k);
			p4[i][j] = bddtrue;
			for (unsigned k = 0; k < LOG_N; k++) p4[i][j] &= ((j >> k) & 1) ? bdd_ithvar(I + LOG_N * 3 + k) : bdd_nithvar(I + LOG_N * 3 + k);
		}
		I += LOG_N*M;
	}

	bdd task = bddtrue; // булева функция, определяющая решение, начальное значение true

	// ограничение по умолчанию - 6-е
	//свойства принимают значения только из
	//заданных множеств (значение свойств должно быть меньше N)
	for (unsigned i = 0; i < N; i++)
	{
		bdd temp1 = bddfalse;
		bdd temp2 = bddfalse;
		bdd temp3 = bddfalse;
		bdd temp4 = bddfalse;
		for (unsigned j = 0; j < N; j++)
		{
			temp1 |= p1[i][j];
			temp2 |= p2[i][j];
			temp3 |= p3[i][j];
			temp4 |= p4[i][j];
		}
		task &= temp1 & temp2 & temp3 & temp4;
	}

	// ограничение типа 1 n1=2
	task &= p4[2][2];
	task &= p3[0][1];

	// ограничение типа 2 n2=5
	for (unsigned i = 0; i < N; i++) {
		task &= !(p3[i][0] ^ p4[i][3]);
		task &= !(p1[i][3] ^ p4[i][1]);
		task &= !(p2[i][2] ^ p3[i][0]);
		task &= !(p2[i][0] ^ p4[i][6]);
		task &= !(p1[i][4] ^ p4[i][0]);
	}

	// ограничение типа 3 n3=4
	task &= !p4[8][0] & !p3[0][3];
	for (unsigned i = 0; i < N - 1; i++) {
		task &= !(p4[i][0] ^ p3[i + 1][3]);
	}

	task &= !p4[N][5] & !p4[0][0];
	for (unsigned i = 0; i < N - 1; i++) {
		task &= !(p4[i][5] ^ p4[i + 1][0]);
	}

	task &= !p2[0][2] & !p1[8][2];
	for (unsigned i = 0; i < N - 1; i++) {
		task &= !(p1[i][2] ^ p2[i + 1][2]);
	}

	task &= !p3[0][5] & !p2[8][7];
	for (unsigned i = 0; i < N - 1; i++) {
		task &= !(p2[i][7] ^ p3[i + 1][5]);
	}

	restrict4(task, p1, 3, p2, 5);
	restrict4(task, p2, 3, p4, 3);
	restrict4(task, p4, 8, p2, 8);
	restrict4(task, p1, 3, p1, 1);

	// ->--- ограничение типа 7
	// Сумма свойств объектов с нечетной позицией не должна быть больше K
	for (unsigned i = 1; i < N; i += 2)
	{
		for (unsigned a = 0; a < N; ++a) {
			for (unsigned b = 0; b < N; ++b) {
				for (unsigned c = 0; c < N; ++c) {
					for (unsigned d = 0; d < N; ++d) {
						if (a + b + c + d > k) {
							task &= !(p1[i][a] & p2[i][b] & p3[i][c] & p4[i][d]);
						}
					}
				}
			}
		}
	}

	//Введены допольнительные ограничения для получения одного решения
	// ограничение типа 1

	task &= p2[3][2];
	task &= p4[8][1];
	task &= p2[2][6];
	task &= !p1[0][1];
	task &= !p2[0][5];

	//И пусть Владимир будет русским
	for (unsigned i = 0; i < N; i++) task &= !(p2[i][4] ^ p1[i][4]);

	// ->--- ограничение по-умолчанию 5
	//у двух различных объектов значения любого свойства не совпадают
	for (unsigned j = 0; j < N; j++) {
		for (unsigned i = 0; i < N - 1; i++) {
			for (unsigned k = i + 1; k < N; k++)
			{
				task &= p1[i][j] >> !p1[k][j];
				task &= p2[i][j] >> !p2[k][j];
				task &= p3[i][j] >> !p3[k][j];
				task &= p4[i][j] >> !p4[k][j];
				cout << j << endl;
			}
		}
	}

	// -<---

	// вывод результатов
	out.open("out.txt");
	unsigned satcount = (unsigned)bdd_satcount(task);
	out << satcount << " solutions:\n" << endl;
	cout << "OUT\n";
	cout << satcount << "=satcount\n";
	if (satcount) bdd_allsat(task, fun);
	out.close();
	bdd_done(); // завершение работы библиотеки
	//	return 0;
}
char var[N_VAR];
void print(void)
{
	for (unsigned i = 0; i < N; i++)
	{
		out << i << ": ";
		for (unsigned j = 0; j < M; j++)
		{
			unsigned J = i*M*LOG_N + j*LOG_N;
			unsigned num = 0;
			for (unsigned k = 0; k < LOG_N; k++)
				num += (unsigned)(var[J + k] << k);
			out << num << ' ';
		}
		out << endl;
	}
	out << endl;
}
void build(char* varset, unsigned n, unsigned I)
{
	if (I == n - 1)//n=1
	{
		if (varset[I] >= 0)
		{
			var[I] = varset[I];
			print();
			return;
		}
		var[I] = 0;
		print();
		var[I] = 1;
		print();
		return;
	}
	if (varset[I] >= 0)
	{
		var[I] = varset[I];
		build(varset, n, I + 1);
		return;
	}
	var[I] = 0;
	build(varset, n, I + 1);
	var[I] = 1;
	build(varset, n, I + 1);
}
void fun(char* varset, int size)
{
	build(varset, size, 0);
}
