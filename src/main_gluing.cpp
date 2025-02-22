#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include "bdd.h"

constexpr uint32_t log2(uint32_t x)
{
 return x < 3 ? 1 : 1 + log2((x + 1) / 2);
}

std::ostream* out_stream;
const std::size_t N = 9;
const std::size_t M = 4;
const std::size_t ROW_LENGTH = 3;
const int LOG_N = log2(N);

const std::size_t VAR_NUM = N * M * LOG_N;
char var[VAR_NUM];
std::ofstream out;
enum Neighbour { UP, TOP_LEFT };

struct PropertyOfObject
{
 PropertyOfObject(int propertyNumber, int objectNumber, int propertyValue) :
	propertyNumber_(propertyNumber), objectNumber_(objectNumber), propertyValue_(propertyValue) {}

 int propertyNumber_;
 int objectNumber_;
 int propertyValue_;
};

void init(bdd p[M][N][N]) // ���������� �� 0 �� 2^logN
{
 for (int i = 0; i < M; ++i)
 {
	for (int j = 0; j < N; ++j)
	{
	  for (int k = 0; k < N; ++k)
	  {
		p[i][j][k] = bddtrue;
		for (int l = 0; l < LOG_N; ++l)
		{
		  p[i][j][k] &= ((k >> l) & 1) ? bdd_ithvar((j * LOG_N * M) + l + LOG_N * i) : bdd_nithvar((j * LOG_N * M) + l + LOG_N * i);
		}
	  }
	}
 }
}

bdd cond1(const bdd p[M][N][N], const PropertyOfObject propertyOfObject)
{
 return p[propertyOfObject.propertyNumber_][propertyOfObject.objectNumber_][propertyOfObject.propertyValue_];
}

bdd cond2(const bdd p[M][N][N], const PropertyOfObject propertyOfObject1, const PropertyOfObject propertyOfObject2)
{
 bdd tree = bddtrue;
 for (int i = 0; i < N; ++i)
 {
	bdd a = p[propertyOfObject1.propertyNumber_][i][propertyOfObject1.propertyValue_];
	bdd b = p[propertyOfObject2.propertyNumber_][i][propertyOfObject2.propertyValue_];
	tree &= ((!a) & (!b)) | (a & b);
 }
 return tree;
}

bdd cond3(const bdd p[M][N][N], const Neighbour neighbour, const PropertyOfObject currentProperty,
 PropertyOfObject neihbourProperty)
{
 bdd tree = bddtrue;
 switch (neighbour)
 {
 case UP:
 {
	for (int i = 0; i < N; ++i) //�������� ������ ���
	{
	  if (i != 0 && i != 1 && i != 2) {
		tree &= !(p[currentProperty.propertyNumber_][i][currentProperty.propertyValue_] ^ p[neihbourProperty.propertyNumber_][i - ROW_LENGTH][neihbourProperty.propertyValue_]);
	  }
	  else {
		tree &= !(p[currentProperty.propertyNumber_][i][currentProperty.propertyValue_]);
	  }
	}
	break;
 }
 case TOP_LEFT:
 {
	for (int i = 0; i < N; ++i)
	{
	  if (i % ROW_LENGTH == 0 && i != 0) { //������� (3 � 6 �������)
		tree &= !(p[currentProperty.propertyNumber_][i][currentProperty.propertyValue_] ^ p[neihbourProperty.propertyNumber_][i - 1][neihbourProperty.propertyValue_]);
	  }
	  else if (i != 1 && i != 2 && i % ROW_LENGTH != 0) //�������� ����� ������� � ��� ������� �������
	  {
		tree &= !(p[currentProperty.propertyNumber_][i][currentProperty.propertyValue_] ^ p[neihbourProperty.propertyNumber_][i - ROW_LENGTH - 1][neihbourProperty.propertyValue_]);
	  }
	  else {
		tree &= !(p[currentProperty.propertyNumber_][i][currentProperty.propertyValue_]);
	  }
	}
	break;
 }
 }
 return tree;
}

bdd cond4(const bdd p[M][N][N], const PropertyOfObject currentProperty, const PropertyOfObject neihbourProperty)
{
 bdd treeTmp = bddfalse;
 std::vector<Neighbour> neighbours = { UP, TOP_LEFT };
 for (Neighbour neighbour : neighbours)
 {
	treeTmp |= cond3(p, neighbour, currentProperty, neihbourProperty);
 }
 return treeTmp;
}

void cond5(bdd& result, bdd p[M][N][N]) {
 for (size_t i = 0; i < M; ++i) {
	for (size_t j = 0; j < N - 1; ++j) {
	  for (size_t k = j + 1; k < N; ++k) {
		for (size_t n = 0; n < N; ++n) {
		  result &= (!p[i][j][n] | !p[i][k][n]);
		}
	  }
	}
 }
}

void cond6(bdd& result, bdd p[M][N][N]) { //� ��� �� ����� ���� ��� �������
 for (size_t i = 0; i < N; ++i) {
	bdd tempBdd = bddtrue;
	for (size_t j = 0; j < M; ++j) {
	  bdd eatempBdd = bddfalse;
	  for (size_t k = 0; k < N; ++k) {	//���������� �� ���� ��������� 
		eatempBdd |= p[j][i][k];
	  }
	  tempBdd &= eatempBdd;
	}
	result &= tempBdd;
 }
}

void print()
{
 for (uint32_t i = 0; i < N; ++i)
 {
	(*out_stream) << i << ": ";
	for (uint32_t j = 0; j < M; ++j)
	{
	  uint32_t J = i * M * LOG_N + j * LOG_N;
	  uint32_t num = 0;
	  for (uint32_t k = 0; k < LOG_N; ++k)
	  {
		num += (uint32_t)(var[J + k] << k);
	  }
	  (*out_stream) << num << ' ';
	}
	(*out_stream) << '\n';
 }
 (*out_stream) << std::endl;
}

void build(char* varset, unsigned n, unsigned I)
{
 if (I == n - 1) {
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

void printOutTable(std::string input) {
 if (input == "LINE") {
	for (std::size_t i = 0; i < 7; i++) {
	  std::cout << "*: * * * *" << '\n';
	}
	std::cout << "7: 1 4 5 6" << '\n';
	std::cout << "*: * * * *" << '\n';
 }
 else if (input == "SOLUTION") {
	for (std::size_t i = 0; i < 9; i++) {
	  std::cout << i << ": 2 3 4 5" << '\n';
	}
 }
}

int main() {
 bdd_init(500000, 70000);
 bdd_setvarnum(VAR_NUM);
 bdd p[M][N][N];
 init(p);
 bdd tree = bddtrue;

 size_t index = 0;

 for (size_t i = 0; i < M; ++i) {
	for (size_t j = 0; j < N; ++j) {
	  for (size_t k = 0; k < N; ++k) {

		p[i][j][k] = bddtrue;

		for (size_t n = 0; n < LOG_N; ++n) {
		  index = LOG_N * M * j + LOG_N * i + n;
		  p[i][j][k] &= (((k >> n) & 1) ? bdd_ithvar(index) : bdd_nithvar(index));
		}
	  }
	}
 }

 cond6(tree, p);

 //tree &= cond1(p, PropertyOfObject(2, 0, 6)); //+ comm
 //tree &= cond1(p, PropertyOfObject(2, 2, 7)); //+ 
 //tree &= cond1(p, PropertyOfObject(0, 3, 5)); //+ comm
 tree &= cond1(p, PropertyOfObject(1, 3, 8)); //+
 //tree &= cond1(p, PropertyOfObject(3, 3, 0)); //+

 //��������������
 tree &= cond1(p, PropertyOfObject(2, 4, 0)); //+ 
 //tree &= cond1(p, PropertyOfObject(0, 5, 7)); //+ comm
 tree &= cond1(p, PropertyOfObject(3, 6, 8)); //+ 
 //tree &= cond1(p, PropertyOfObject(0, 7, 8)); //+
 tree &= cond1(p, PropertyOfObject(2, 7, 5)); //+ 
 tree &= cond1(p, PropertyOfObject(0, 8, 0)); //+ 
 tree &= cond1(p, PropertyOfObject(2, 6, 1)); //+


 tree &= cond2(p, PropertyOfObject(0, -1, 2), PropertyOfObject(3, -1, 4)); //+
 tree &= cond2(p, PropertyOfObject(1, -1, 6), PropertyOfObject(2, -1, 4)); //+
 tree &= cond2(p, PropertyOfObject(1, -1, 7), PropertyOfObject(0, -1, 3)); //+
 tree &= cond2(p, PropertyOfObject(2, -1, 3), PropertyOfObject(3, -1, 7)); //+
 tree &= cond2(p, PropertyOfObject(1, -1, 1), PropertyOfObject(3, -1, 2)); //+
 tree &= cond2(p, PropertyOfObject(3, -1, 6), PropertyOfObject(2, -1, 8)); //+



 //std::cout << bdd_satcount(tree) << " solution(s):\n";

 //tree &= cond3(p, Neighbour::UP, PropertyOfObject(2, -1, 0), PropertyOfObject(1, -1, 0));

 tree &= cond3(p, Neighbour::TOP_LEFT, PropertyOfObject(0, -1, 7), PropertyOfObject(0, -1, 3));

 tree &= cond3(p, Neighbour::TOP_LEFT, PropertyOfObject(0, -1, 4), PropertyOfObject(1, -1, 2)); //+

 tree &= cond3(p, Neighbour::UP, PropertyOfObject(1, -1, 5), PropertyOfObject(2, -1, 4)); //+

 tree &= cond3(p, Neighbour::UP, PropertyOfObject(3, -1, 3), PropertyOfObject(3, -1, 1)); //+


 //std::cout << bdd_satcount(tree) << " solution(s):\n";

 tree &= cond4(p, PropertyOfObject(1, -1, 8), PropertyOfObject(0, -1, 2)); //+
 tree &= cond4(p, PropertyOfObject(1, -1, 3), PropertyOfObject(3, -1, 0)); //+
 tree &= cond4(p, PropertyOfObject(1, -1, 4), PropertyOfObject(0, -1, 6)); //+
 tree &= cond4(p, PropertyOfObject(2, -1, 2), PropertyOfObject(3, -1, 4)); //+
 tree &= cond4(p, PropertyOfObject(3, -1, 5), PropertyOfObject(3, -1, 7)); //+

 //std::cout << bdd_satcount(tree) << " solution(s):\n";

 

 cond5(tree, p);

 //tree &= cond1(p, PropertyOfObject(2, 0, 6)); //+ 
 //tree &= cond1(p, PropertyOfObject(2, 2, 7)); //+
 //tree &= cond1(p, PropertyOfObject(0, 3, 5)); //+
 //tree &= cond1(p, PropertyOfObject(1, 3, 8)); //+
 ////tree &= cond1(p, PropertyOfObject(3, 3, 0)); //+

 ////��������������
 //tree &= cond1(p, PropertyOfObject(2, 4, 0)); //+ 
 //tree &= cond1(p, PropertyOfObject(0, 5, 7)); //+ 
 ////tree &= cond1(p, PropertyOfObject(3, 6, 8)); //+ 
 ////tree &= cond1(p, PropertyOfObject(0, 7, 8)); //+
 //tree &= cond1(p, PropertyOfObject(2, 7, 5)); //+ 
 //tree &= cond1(p, PropertyOfObject(0, 8, 0)); //+ 
 //tree &= cond1(p, PropertyOfObject(2, 6, 1)); //+


 //tree &= cond2(p, PropertyOfObject(0, -1, 2), PropertyOfObject(3, -1, 4)); //+
 //tree &= cond2(p, PropertyOfObject(1, -1, 6), PropertyOfObject(2, -1, 4)); //+
 //tree &= cond2(p, PropertyOfObject(1, -1, 7), PropertyOfObject(0, -1, 3)); //+
 //tree &= cond2(p, PropertyOfObject(2, -1, 3), PropertyOfObject(3, -1, 7)); //+
 //tree &= cond2(p, PropertyOfObject(1, -1, 1), PropertyOfObject(3, -1, 2)); //+
 //tree &= cond2(p, PropertyOfObject(3, -1, 6), PropertyOfObject(2, -1, 8)); //+


 ////std::cout << bdd_satcount(tree) << " solution(s):\n";


 //tree &= cond3(p, Neighbour::TOP_LEFT, PropertyOfObject(0, -1, 4), PropertyOfObject(1, -1, 2)); //+

 //tree &= cond3(p, Neighbour::UP, PropertyOfObject(1, -1, 5), PropertyOfObject(2, -1, 4)); //+

 //tree &= cond3(p, Neighbour::UP, PropertyOfObject(3, -1, 3), PropertyOfObject(3, -1, 1)); //+


 ////std::cout << bdd_satcount(tree) << " solution(s):\n";

 //tree &= cond4(p, PropertyOfObject(1, -1, 8), PropertyOfObject(0, -1, 2)); //+
 //tree &= cond4(p, PropertyOfObject(1, -1, 3), PropertyOfObject(3, -1, 0)); //+
 //tree &= cond4(p, PropertyOfObject(1, -1, 4), PropertyOfObject(0, -1, 6)); //+
 //tree &= cond4(p, PropertyOfObject(2, -1, 2), PropertyOfObject(3, -1, 4)); //+
 //tree &= cond4(p, PropertyOfObject(3, -1, 5), PropertyOfObject(3, -1, 7)); //+

 ////std::cout << bdd_satcount(tree) << " solution(s):\n";


 //cond5(tree, p);

 std::ofstream out("out.txt");

 auto satCount = bdd_satcount(tree);
 out << satCount << "\n";
 out_stream = &out;
 if (satCount)
 {
	bdd_allsat(tree, fun);
 }
}