/*
--- ���� ---
Array�� �ϸ� ���ϰ�����, �׷� ��� Memory �ʰ��� ���ٴ� �ڻ���� ���� ����
Class�� ������.

���簢���� Cell, �簢���� �̷�� ���� Face, �� �������� Node�� ������.

--- ��ü �帧 ���� ---
�� �׸� Cell�̶� �θ���
�׸� �̷�� ��/��/��/���� ���� Face�� �θ���.
�׸��� �� �������� Node�� �θ���.

���Ǹ� ����, ��ü �׸� Cell���� ������ �� �ִ�.( Rectangle->initCell[i][j] )
Cell���� Face �� Node�� ������ �� �ִ�.
( Cell-> LFace,  DFace,  RFace,  UFace );
( Cell->LUNode, LDNode, RDNode, RUNode );

Face���� �������ִ� Cell�� Node�� ������ �� �ִ�.
( Face-> sNode, eNode [startNode, endNode�� ���Ӹ�] )

--- ��� ������ ���õ� �κ� ---
�� �κп� ����, ���Ϸ� ����� ��, ���� ��µǴ� �κп� ���� ������ �ʿ��ϴ�.
Ư�� Face�� ����� �� sNode, eNode, sCell, eCell ������ ����ϴ� �κ��� �ִµ�,
�⿪��(?)�� �׻� �׸��鼭 ����ϴ� ������ �ڻ�Բ� ���� ���ص�⸦ �ٶ���.

���� �׸� ��, (0, 0)�� ����� �κ��� start�� �ϰ�, (width, height)�� �������� end�� �����Ѵ�.
����� ��, �� �κ��� NULL�� ��쿡�� ������ �Ųٷ� ����ϵ��� �����Ͽ���.
���� ���, Face(��)�� ���� �����غ���
(0, 0) ~ (100, 0)�� �մ� Face�� �ִٰ� ������ ��,
�� Face�� sNode�� (0, 0)�̰�, eNode�� (100, 0)�̴�.

������, ����ϱ� ���� ��ǥ������ ������ ������ �� �ۼ��Ͽ���.


��ü ���� ����� ����
1. ����, ���� ũ�⿡ ���� �Է��� �޴´�.
2. ����/���� GCD�� ���ؼ�, �� GCD�� �µ��� ���� �ɰ���.
3. �� Cell�� ���� ������ �Է��Ѵ�.
*/

#include <iostream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <cstdio>
#include <queue>
#include <set>
#include <map>

#pragma warning(disable:4996)
using namespace std;
class CellShape; // ��
class Cell; // ��
class CellRectangle; // ��
class CellTriangle; // ��
class Face;
class Node;
class Rectangle;
typedef pair<double, double> pdd; // ��
typedef pair<double, int> pdi;
typedef pair<pair<int, int>, pair<int, int> > fi;
typedef pair<int, Face*> detface; //�� delete
typedef pair<int, Cell*> detcell; //�� delete
typedef pair<int, CellShape*> node_of_cell; //��:tail

vector<Node*> nodeVector;
vector<Cell*> cellVector;
vector<Face*> faceVector;
vector<Face*> wallVector; // 0208
vector<fi>wall; //< <�ﰢ������ �簢������ , ������ id > < snode, enode> >

Rectangle *initRectangle;
vector<pdd> mininode[2]; //�� //vector<CellShape*> minimalCellVector_rec;
vector<CellShape*> minimalCellVector_tri; // �� : �� ���� CellRectangle�� CellTriangle ���� 2017-01-12
map< pdd, pair< int, void* > > mininodePlace; // �� : mininode�� ������ face�� node�� �ּҸ� �����Ѵ�.( 0 : Node, 1 : Face) 2017-01-12
map< pdd, map < pdd, CellShape* > > mininodeCell; // �� : map 2017-01-12
vector<detface> detfaceVector;
vector<detcell> detcellVector;
//vector<int> detinitcellVector; // �� : 2017_03_17

vector< pair<pdd, pdd> > inputLine; // �� : �Է¹޴� ������ ���� 2017_02_08
int initCellMaxID = 0; // �� : initCell�� ���� ���� ������, ������ initCell���� ���� 2017_02_19
double  S_N = 0.000000001;
FILE *fp; // ���� �ϰ� ���ο��� �ν��Ͻ�ȭ
class Node {
public:
	double x, y;
	int id;
	static int nodeCount; // node ������ count
	int inout; // �� : 
			   /* attribute
			   -2: line (������)
			   -1: inner (������ ����)
			   0 : outflow  (�� ������)
			   1 : inlet    (�� ����)
			   2 : top      (�� ��)
			   3 : bottom   (�� �Ʒ�)
			   4 : interrior(�߰� Node��)
			   5 : unknown (��Ȯ��)
			   */
public:
	Node() {
		x = -99999, y = -99999;
		id = ++nodeCount;
		inout = 5; // �� : �ʱ�ȭ 2017_02_13
	}
	Node(double x, double y) {
		this->x = x; this->y = y;
		id = ++nodeCount;
		inout = 5; // �� : inout �ʱ�ȭ, �⺻������ 5�� �Ǿ����� 2017_02_05
	}
	~Node() {
		nodeCount--;
	}
	void setPoint(double x, double y) {
		this->x = x;
		this->y = y;
	}
	double getX() {
		return x;
	}
	double getY() {
		return y;
	}
	int getID() {
		return id;
	}
	int getCount() {
		return nodeCount;
	}
	int getInout() { // �� : inout �߰��� ���� �Լ� �߰�2017_02_05
		return inout;
	}
	bool operator==(Node* n) { // �� : node�� node�� �� 2017-01-12
		if (x == n->getX()) {
			if (y == n->getY()) {
				if (id == n->getID())
					return true;
			}
		}
		return false;
	}
	node_of_cell findtailCell(pdd n); // �� : tail ����  2017-03-07
	CellShape* findCell(Node* n); // �� : Node�� Node�϶� mininodeCell ���� 2017-01-12
	CellShape* findCell(Face* f); // �� : Node�� face�϶� mininodeCell ���� 2017-01-12
};
class Face {
public:
	Node *sNode, *eNode, *centerNode;
	CellShape *sCell, *eCell; // �� : Cell => CellShape
	Face *Child1, *Child2, *curParent;
	int id, attribute;
	int depth;
	int printID, myPrintID;
	bool childOrNot;
	/* attribute
	0 : outflow  (�� ������)
	1 : inlet    (�� ����)
	2 : top      (�� ��)
	3 : bottom   (�� �Ʒ�)
	4 : interrior(�߰� Face��) */
	static int faceCount;
	static int maxDepth;
public:
	Face() {
		sNode = eNode = centerNode = NULL;
		sCell = eCell = NULL;
		Child1 = Child2 = curParent = NULL;
		id = ++faceCount;
		attribute = 4; // default = interrior
		depth = 0;
		printID = 0;
		childOrNot = false;
	}
	Face(Node *sNode, Node *eNode) {
		centerNode = NULL;
		this->sNode = sNode; this->eNode = eNode;
		sCell = eCell = NULL;
		Child1 = Child2 = curParent = NULL;
		id = ++faceCount;
		attribute = 4; // default = interrior
		depth = 0;
		printID = 0;
		childOrNot = false;
	}
	~Face() { // delete face
		faceCount--;
		faceVector.erase(faceVector.begin() + id - 1);
	}
	void setAttribute(int attribute) {
		this->attribute = attribute;
	}
	void setStartCell(CellShape *cell) { // �� : Cell => CellShape 2017-01-12
		this->sCell = cell;
	}
	void setEndCell(CellShape *cell) { // �� : Cell => CellShape 2017-01-12
		this->eCell = cell;
	}
	void setStartNode(Node *node) {
		this->sNode = node;
	}
	void setEndNode(Node *node) {
		this->eNode = node;
	}
	void setCenterNode(Node *node) {
		centerNode = node;
	}
	void setChilds(Face *c1, Face *c2) {
		Child1 = c1;
		Child2 = c2;

		int childDepth = depth + 1;
		c1->depth = childDepth;	c1->setParent(this);
		c2->depth = childDepth;	c2->setParent(this);

		if (childDepth > maxDepth) {
			maxDepth = childDepth;
		}
	}
	void setID(int newID) {
		id = newID;
	}
	void setDepth(int depth) {
		this->depth = depth;
	}
	void setPrintID(int newID) {
		this->printID = newID;
	}
	void setMyPrintID(int newID) {
		myPrintID = newID;
	}
	void setParent(Face *parent) {
		curParent = parent;
	}
	int getMyPrintID() {
		return myPrintID;
	}
	int getMaxDepth() {
		return maxDepth;
	}
	int getPrintID() {
		return this->printID;
	}
	int getID() {
		if (id>0)return id;
		return NULL;
	}
	int getAttribute() {
		return attribute;
	}
	int getCount() {
		return faceCount;
	}
	Node* getStartNode() {
		return sNode;
	}
	Node* getEndNode() {
		return eNode;
	}
	Node* getCenterNode() {
		return centerNode;
	}
	CellShape* getStartCell() { // �� : CellShape�� ��ȯ�� ����  2017-01-12
		return sCell;
	}
	CellShape* getEndCell() { // �� : CellShape�� ��ȯ�� ����  2017-01-12
		return eCell;
	}
	Face* getChild1() {
		return Child1;
	}
	Face* getChild2() {
		return Child2;
	}
	int getDepth() {
		return depth;
	}
	bool isParent() {
		return ((Child1 != NULL) && (Child2 != NULL));
	}
	bool isChild() {
		return curParent != NULL;
	}
	bool operator==(Face& f) { // �� : face�� face�� ��ġ �� 2017-01-12
		if (this->sNode == f.getStartNode()) {
			if (this->eNode == f.getEndNode()) {
				if (this->id == f.getID()) {
					if (this->depth == f.getDepth())
						return true;
				}
			}
		}
		return false;
	}
	CellShape* findCell(Node* n); // �� : face�� Node�϶� mininodeCell ���� 2017-01-12
	CellShape* findCell(Face* f); // �� : face�� face�϶� mininodeCell ���� 2017-01-12
};
class CellShape { // �� : Cell �⺻��
public:
	double cellSize;
	int id, depth;
	static int cellCount;
	static int triCount; //0208
	bool iscut;

	int getID() {
		if (id>0)return id;
		return NULL;
	}
	void setID(int newID) {
		this->id = newID;
	}
};
class Cell : public CellShape { // �� : �����Ͽ��� 2017-01-12
public:
	Face *LFace, *DFace, *RFace, *UFace;
	Node *LUNode, *LDNode, *RDNode, *RUNode;
	CellShape *LUCell, *LDCell, *RDCell, *RUCell;
	void setChildNodes(Node *L, Node *D, Node *R, Node *U, Node *C, int partialtype) { // ��: tailó��_���� 0307
		if (partialtype == 0 || partialtype == 3) {
			((Cell*)LUCell)->setLUNode(LUNode);	 ((Cell*)LUCell)->setLDNode(L);
			((Cell*)LUCell)->setRDNode(C);		 ((Cell*)LUCell)->setRUNode(U);
		}
		if (partialtype == 0 || partialtype == 2) {
			((Cell*)LDCell)->setLUNode(L);		 ((Cell*)LDCell)->setLDNode(LDNode);
			((Cell*)LDCell)->setRDNode(D);		 ((Cell*)LDCell)->setRUNode(C);
		}
		if (partialtype != 3) {
			((Cell*)RDCell)->setLUNode(C);		 ((Cell*)RDCell)->setLDNode(D);
			((Cell*)RDCell)->setRDNode(RDNode);	 ((Cell*)RDCell)->setRUNode(R);
		}
		if (partialtype != 2) {
			((Cell*)RUCell)->setLUNode(U);		 ((Cell*)RUCell)->setLDNode(C);
			((Cell*)RUCell)->setRDNode(R);		 ((Cell*)RUCell)->setRUNode(RUNode);
		}
	}
	void setNewFaces(Face *from, Face *child1, Face *child2, Node *Start, Node *Center, Node *End) {
		if (!from->isParent()) {
			child1 = new Face(Start, Center);
			child2 = new Face(Center, End);
			faceVector.push_back(child1);
			faceVector.push_back(child2);
			child1->setAttribute(LFace->attribute);
			/*
			�� Lface �� attribute�� ��������
			*/
			child2->setAttribute(LFace->attribute);
			from->setChilds(child1, child2);
			from->setCenterNode(Center);
		}
		else {
			child1 = from->Child1;
			child2 = from->Child2;
		}
		child1->curParent = from;
		child2->curParent = from;
	}
	void setupMininodePlace(double x, double y) { // �� : dotpoint�Լ��� �ʹ� ������� �� ���Ƽ� �и����׽��ϴ�. 2017-01-12
												  // mininodePlace[make_pair(x, y)] = pair< 0 || 1, void*>; // �� : map[x,y] = 0�� Node�� �ּ� �Ǵ� 1�� Face�� �ּ�.
												  // 0 : Node, 1 : Face
		if (LDNode->getX() == x) { // �� : x, y�� ���� LFace�� ������
			if (LDNode->getY() == y)
				mininodePlace[make_pair(x, y)] = make_pair(0, LDNode);
			else if (LUNode->getY() == y)
				mininodePlace[make_pair(x, y)] = make_pair(0, LUNode);
			else
				mininodePlace[make_pair(x, y)] = make_pair(1, LFace);

		}
		else if (LDNode->getY() == y) { // �� : x, y��  DFace�� ������
			if (RDNode->getX() == x)
				mininodePlace[make_pair(x, y)] = make_pair(0, RDNode);
			else
				mininodePlace[make_pair(x, y)] = make_pair(1, DFace);
		}
		else if (RDNode->getX() == x) { // �� : x, y�� ���� RFace�� ������
			if (RUNode->getY() == y)
				mininodePlace[make_pair(x, y)] = make_pair(0, RUNode);
			else
				mininodePlace[make_pair(x, y)] = make_pair(1, RFace);
		}
		else if (RUNode->getY() == y) { // �� : x, y�� ���� UFace�� ������
			if (LUNode->getX() == x)
				mininodePlace[make_pair(x, y)] = make_pair(0, LUNode);
			else
				mininodePlace[make_pair(x, y)] = make_pair(1, UFace);
		}
	}
public:
	Cell() {
		LUCell = LDCell = RDCell = RUCell = NULL;
		LUNode = LDNode = RDNode = RUNode = NULL;
		LFace = DFace = RFace = UFace = NULL;
		id = ++cellCount;
		depth = 0;
		iscut = false;
	}
	~Cell() {//��delete
		cellCount--;
		cellVector.erase(cellVector.begin() + id - 1);
	}
	bool getiscut() {
		return iscut;
	}
	void setiscut(bool cut) {
		iscut = cut;
	}
	int getNodeCount() {
		return LUNode->getCount();
	}
	int getCellCount() {
		return cellCount;
	}
	int getFaceCount() {
		return LFace->getCount();
	}
	int getID() {//��delete
		if (id>0)return id;
		return NULL;
	}
	Node* getLUNode() {
		return LUNode;
	}
	Node* getLDNode() {
		return LDNode;
	}
	Node* getRDNode() {
		return RDNode;
	}
	Node* getRUNode() {
		return RUNode;
	}
	bool isParent() {
		return (LUCell != NULL);
	}
	double getSize() {
		return this->cellSize;
	}
	int getDepth() {
		return depth;
	}
	void setDepth(int newDepth) {
		depth = newDepth;
	}
	void setID(int newID) {
		this->id = newID;
	}
	void setLUNode(Node *node) {
		LUNode = node;
	}
	void setLDNode(Node *node) {
		LDNode = node;
	}
	void setRDNode(Node *node) {
		RDNode = node;
	}
	void setRUNode(Node *node) {
		RUNode = node;
	}
	void setSize(double size) {
		cellSize = size;
	}
	void dotPoint(double x, double y, int layer, int depth) { // �� : LUCell, LDCell, RUCell, RDCell ��� CellShape�� ��ȯ, Cell�� ����Լ��� ���� �ʿ�� ((Cell*) __) ��ġ 2017-01-12
		const int OUTFLOW = 0, INLET = 1, TOP = 2, BOTTOM = 3, INTERRIOR = 4;
		/*
		���ɰ��� ���� ������ �ñ�
		*/
		if (LUCell == NULL) {	// ���� �� Cell�� �ɰ����� �ʾ�����, �ɰ���! 
								//( �ɰ������� LUCell, LDCell, RDCell, RUCell�� NULL�� �ƴҰŴϱ� ���� �ϳ��� üũ�Ѱ��� )
								// ���� Cell(Default)�� ���� �����صд�.
			LUCell = new Cell();	((Cell*)LUCell)->setDepth(depth + 1);
			LDCell = new Cell();	((Cell*)LDCell)->setDepth(depth + 1);
			RDCell = new Cell();	((Cell*)RDCell)->setDepth(depth + 1);
			RUCell = new Cell();	((Cell*)RUCell)->setDepth(depth + 1);
			cellVector.push_back(((Cell*)LUCell));	cellVector.push_back(((Cell*)LDCell));
			cellVector.push_back(((Cell*)RDCell));	cellVector.push_back(((Cell*)RUCell));

			// ���� ��������� Node�� ���� �����Ѵ�.
			Node *LeftCenter, *DownCenter, *RightCenter, *UpCenter, *cellCenter;
			if (!LFace->isParent()) { // parent�� �ƴϸ� �ɰ�������������
				LeftCenter = new Node(((LUNode->x + LDNode->x) / 2.0), ((LUNode->y + LDNode->y) / 2.0));
				nodeVector.push_back(LeftCenter);
			}
			else {
				LeftCenter = LFace->centerNode;
			}
			if (!DFace->isParent()) {
				DownCenter = new Node((this->LDNode->x + this->RDNode->x) / 2.0, (this->LDNode->y + this->RDNode->y) / 2.0);
				nodeVector.push_back(DownCenter);
			}
			else {
				DownCenter = DFace->centerNode;
			}
			if (!RFace->isParent()) {
				RightCenter = new Node((this->RDNode->x + this->RUNode->x) / 2.0, (this->RDNode->y + this->RUNode->y) / 2.0);
				nodeVector.push_back(RightCenter);
			}
			else {
				RightCenter = RFace->centerNode;
			}
			if (!UFace->isParent()) {
				UpCenter = new Node((this->RUNode->x + this->LUNode->x) / 2.0, (this->RUNode->y + this->LUNode->y) / 2.0);
				nodeVector.push_back(UpCenter);
			}
			else {
				UpCenter = UFace->centerNode;
			}
			cellCenter = new Node((this->LDNode->x + this->RUNode->x) / 2.0, (this->LDNode->y + this->RUNode->y) / 2.0);
			nodeVector.push_back(cellCenter);

			// ���� Cell�� Node�� �����Ѵ�.
			setChildNodes(LeftCenter, DownCenter, RightCenter, UpCenter, cellCenter, 0);

			// ���� Cell Size �� ����.
			double ChildSize = cellSize / 2.0;
			((Cell*)LUCell)->setSize(ChildSize);		((Cell*)LDCell)->setSize(ChildSize);
			((Cell*)RDCell)->setSize(ChildSize);		((Cell*)RUCell)->setSize(ChildSize);

			// ���� Face 12���� ����̴ϴ�.
			Face *LU = 0, *LD = 0, *DL = 0, *DR = 0, *RD = 0, *RU = 0;
			Face *UR = 0, *UL = 0, *IL = 0, *ID = 0, *IR = 0, *IU = 0;

			// Cell ���� �� ���� ��������� Face�� �ܺ� 8�� ���� ����
			// LD LU
			if (!LFace->isParent()) {
				LD = new Face(LDNode, LeftCenter);	faceVector.push_back(LD);
				LU = new Face(LeftCenter, LUNode);	faceVector.push_back(LU);
				LD->setAttribute(LFace->attribute);
				LU->setAttribute(LFace->attribute);
				LFace->setChilds(LD, LU);
				LFace->setCenterNode(LeftCenter);
			}
			else {
				LD = LFace->Child1;
				LU = LFace->Child2;
			}
			// DL DR
			if (!DFace->isParent()) {
				DL = new Face(LDNode, DownCenter);	faceVector.push_back(DL);
				DR = new Face(DownCenter, RDNode);	faceVector.push_back(DR);
				DL->setAttribute(DFace->attribute);
				DR->setAttribute(DFace->attribute);
				DFace->setChilds(DL, DR);
				DFace->setCenterNode(DownCenter);
			}
			else {
				DL = DFace->Child1;
				DR = DFace->Child2;
			}
			// RD RU
			if (!RFace->isParent()) {
				RD = new Face(RDNode, RightCenter);	faceVector.push_back(RD);
				RU = new Face(RightCenter, RUNode);	faceVector.push_back(RU);
				RD->setAttribute(RFace->attribute);
				RU->setAttribute(RFace->attribute);
				RFace->setChilds(RD, RU);
				RFace->setCenterNode(RightCenter);
			}
			else {
				RD = RFace->Child1;
				RU = RFace->Child2;
			}
			// UL UR
			if (!UFace->isParent()) {
				UL = new Face(LUNode, UpCenter);	faceVector.push_back(UL);
				UR = new Face(UpCenter, RUNode);	faceVector.push_back(UR);
				UL->setAttribute(UFace->attribute);
				UR->setAttribute(UFace->attribute);
				UFace->setChilds(UL, UR);
				UFace->setCenterNode(UpCenter);
			}
			else {
				UL = UFace->Child1;
				UR = UFace->Child2;
			}

			IL = new Face(LeftCenter, cellCenter);	IL->setAttribute(INTERRIOR);	IL->setDepth(UL->depth);	faceVector.push_back(IL);
			ID = new Face(DownCenter, cellCenter);	ID->setAttribute(INTERRIOR);	ID->setDepth(UL->depth);	faceVector.push_back(ID);
			IR = new Face(cellCenter, RightCenter);	IR->setAttribute(INTERRIOR);	IR->setDepth(UL->depth);	faceVector.push_back(IR);
			IU = new Face(cellCenter, UpCenter);		IU->setAttribute(INTERRIOR);	IU->setDepth(UL->depth);	faceVector.push_back(IU);

			// �� ���� cell�� face�� ����Ű����
			((Cell*)LUCell)->LFace = LU;	((Cell*)LUCell)->DFace = IL;	((Cell*)LUCell)->RFace = IU;	((Cell*)LUCell)->UFace = UL;
			((Cell*)LDCell)->LFace = LD;	((Cell*)LDCell)->DFace = DL;	((Cell*)LDCell)->RFace = ID;	((Cell*)LDCell)->UFace = IL;
			((Cell*)RDCell)->LFace = ID;	((Cell*)RDCell)->DFace = DR;	((Cell*)RDCell)->RFace = RD;	((Cell*)RDCell)->UFace = IR;
			((Cell*)RUCell)->LFace = IU;	((Cell*)RUCell)->DFace = IR;	((Cell*)RUCell)->RFace = RU;	((Cell*)RUCell)->UFace = UR;

			// �� Face�� startCell / endCell ���� // �� : Face�� sCell�� eCell�� CellShape�� �����Ͽ��� ������ �ؿ� �ڵ嵵 �ణ �����߽��ϴ�.  2017-01-12

			// LU, LD 
			//const int OUTFLOW = 0, INLET = 1, TOP = 2, BOTTOM = 3, INTERRIOR = 4;
			LU->setEndCell(((Cell*)LUCell));	LD->setEndCell(((Cell*)LDCell));
			if (LFace->attribute != INLET) {
				if (((Cell*)(LFace->sCell))->LUCell == NULL) {
					LU->setStartCell(LFace->sCell);
					LD->setStartCell(LFace->sCell);
				}
				else {
					LU->setStartCell(((Cell*)(LFace->sCell))->RUCell);
					LD->setStartCell(((Cell*)(LFace->sCell))->RDCell);
				}
			}
			// DL, DR
			DL->setStartCell(((Cell*)LDCell));	DR->setStartCell(((Cell*)RDCell));
			if (DFace->attribute != BOTTOM) {
				if (((Cell*)(DFace->eCell))->LUCell == NULL) {
					DL->setEndCell(DFace->eCell);
					DR->setEndCell(DFace->eCell);
				}
				else {
					DL->setEndCell(((Cell*)(DFace->eCell))->LUCell);
					DR->setEndCell(((Cell*)(DFace->eCell))->RUCell);
				}
			}
			// RD, RU
			RD->setStartCell(((Cell*)RDCell));	RU->setStartCell(((Cell*)RUCell));
			if (RFace->attribute != OUTFLOW) {
				if (((Cell*)(RFace->eCell))->LUCell == NULL) {
					RD->setEndCell(RFace->eCell);
					RU->setEndCell(RFace->eCell);
				}
				else {
					RD->setEndCell(((Cell*)(RFace->eCell))->LDCell);
					RU->setEndCell(((Cell*)(RFace->eCell))->LUCell);
				}
			}
			// UR, UL
			UR->setEndCell(((Cell*)RUCell));	UL->setEndCell(((Cell*)LUCell));
			if (UFace->attribute != TOP) {
				if (((Cell*)(UFace->sCell))->LUCell == NULL) {
					UR->setStartCell(UFace->sCell);
					UL->setStartCell(UFace->sCell);
				}
				else {
					UR->setStartCell(((Cell*)(UFace->sCell))->RDCell);
					UL->setStartCell(((Cell*)(UFace->sCell))->LDCell);
				}
			}
			// INNER Faces
			IL->setStartCell(((Cell*)LUCell));	IL->setEndCell(((Cell*)LDCell));
			ID->setStartCell(((Cell*)LDCell));	ID->setEndCell(((Cell*)RDCell));
			IR->setStartCell(((Cell*)RUCell));	IR->setEndCell(((Cell*)RDCell));
			IU->setStartCell(((Cell*)LUCell));	IU->setEndCell(((Cell*)RUCell));
		}

		// layer spread // �� : SpreadCell Ÿ�� CellShape�� �����ϰ� �׿����� �ؿ� �ڵ� �����߽��ϴ�.  2017-01-12
		if (layer > 0) {
			// [0] [1] [2] [3] [4] [5] [6] [7]
			// LU,  L, LD,  D, RD,  R, RU,  U 
			CellShape *SpreadCell[8] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };
			bool isLeft = (LFace->attribute == INLET);
			bool isBottom = (DFace->attribute == BOTTOM);
			bool isRight = (RFace->attribute == OUTFLOW);
			bool isTop = (UFace->attribute == TOP);

			if (!isLeft) {
				SpreadCell[1] = LFace->sCell;
				if (!isTop)	SpreadCell[0] = ((Cell*)SpreadCell[1])->UFace->sCell;
				if (!isBottom)	SpreadCell[2] = ((Cell*)SpreadCell[1])->DFace->eCell;
			}
			if (!isBottom) {
				SpreadCell[3] = DFace->eCell;
				if (!isRight)	SpreadCell[4] = ((Cell*)SpreadCell[3])->RFace->eCell;
			}
			if (!isRight) {
				SpreadCell[5] = RFace->eCell;
				if (!isTop)	SpreadCell[6] = ((Cell*)SpreadCell[5])->UFace->sCell;
			}
			if (!isTop) SpreadCell[7] = UFace->sCell;
			// spread by layer
			for (int i = 0; i < 8; i++) {
				if (SpreadCell[i] != NULL) {
					((Cell*)SpreadCell[i])->dotPoint(x, y, layer - 1, 0);
				}
			}
		}

		// depth �������鼭 �� �ɰ��� �κ�
		if (depth > 0) {  // �� : depth�� 0�� �� �ɰ��� �ɷ� �˰� �־��µ� �׷��� �Ǹ� mininode�� ���ݰ� �ٶ� depth�� 0�� �� �ɰ��� �ʴ� ������ �߽��ϴ�.  
			Node *CellCenterNode = ((Cell*)LUCell)->RDNode;
			double centerX = CellCenterNode->x;
			double centerY = CellCenterNode->y;
			if (x > centerX) {
				if (y > centerY) {
					if (depth > 1)
						((Cell*)RUCell)->dotPoint(x, y, layer, depth - 1);
					else
						((Cell*)RUCell)->setupMininodePlace(x, y);
				}
				else {
					if (depth > 1)
						((Cell*)RDCell)->dotPoint(x, y, layer, depth - 1);
					else
						((Cell*)RDCell)->setupMininodePlace(x, y);
				}
			}
			else {
				if (y > centerY) {
					if (depth > 1)
						((Cell*)LUCell)->dotPoint(x, y, layer, depth - 1);
					else
						((Cell*)LUCell)->setupMininodePlace(x, y);
				}
				else {
					if (depth > 1)
						((Cell*)LDCell)->dotPoint(x, y, layer, depth - 1);
					else
						((Cell*)LDCell)->setupMininodePlace(x, y);
				}
			}
		}
	}
	void printCell() {
		// �׳� �����ڰ� Ȯ���Ϸ��� ������ �κ�
		cout << this->id << " cell information --\n";
		cout << "  node  \n";
		cout << "LU = " << this->LUNode->id;
		cout << " LD = " << this->LDNode->id;
		cout << " RD = " << this->RDNode->id;
		cout << " RU = " << this->RUNode->id << endl;

		cout << "  face  \n";
		cout << "L = " << this->LFace->id;
		cout << " D = " << this->DFace->id;
		cout << " R = " << this->RFace->id;
		cout << " U = " << this->UFace->id << endl;
		cout << endl;

		// deldeldel
		if (LFace->Child1 != NULL) {
			if (LFace->Child1->Child1 != NULL) {
				cout << "!!!!!!!!!!!!!!!!!!!!!!!! ";
				cout << LFace->id << " == " << LFace->Child1->id << " == (" << LFace->Child1->Child1->id << " " << LFace->Child1->Child2->id << ")\n";
			}
			if (LFace->Child2->Child1 != NULL) {
				cout << "!!!!!!!!!!!!!!!!!!!!!!!! ";
				cout << LFace->id << " == " << LFace->Child2->id << " == (" << LFace->Child2->Child1->id << " " << LFace->Child2->Child2->id << ")\n";
			}
		}
		if (DFace->Child1 != NULL) {
			if (DFace->Child1->Child1 != NULL) {
				cout << "!!!!!!!!!!!!!!!!!!!!!!!! ";
				cout << DFace->id << " == " << DFace->Child1->id << " == (" << DFace->Child1->Child1->id << " " << DFace->Child1->Child2->id << ")\n";
			}
			if (DFace->Child2->Child1 != NULL) {
				cout << "!!!!!!!!!!!!!!!!!!!!!!!! ";
				cout << DFace->id << " == " << DFace->Child2->id << " == (" << DFace->Child2->Child1->id << " " << DFace->Child2->Child2->id << ")\n";
			}
		}
		if (RFace->Child1 != NULL) {
			if (RFace->Child1->Child1 != NULL) {
				cout << "!!!!!!!!!!!!!!!!!!!!!!!! ";
				cout << RFace->id << " == " << RFace->Child1->id << " == (" << RFace->Child1->Child1->id << " " << RFace->Child1->Child2->id << ")\n";
			}
			if (RFace->Child2->Child1 != NULL) {
				cout << "!!!!!!!!!!!!!!!!!!!!!!!! ";
				cout << RFace->id << " == " << RFace->Child2->id << " == (" << RFace->Child2->Child1->id << " " << RFace->Child2->Child2->id << ")\n";
			}
		}
		if (UFace->Child1 != NULL) {
			if (UFace->Child1->Child1 != NULL) {
				cout << "!!!!!!!!!!!!!!!!!!!!!!!! ";
				cout << UFace->id << " == " << UFace->Child1->id << " == (" << UFace->Child1->Child1->id << " " << UFace->Child1->Child2->id << ")\n";
			}
			if (UFace->Child2->Child1 != NULL) {
				cout << "!!!!!!!!!!!!!!!!!!!!!!!! ";
				cout << UFace->id << " == " << UFace->Child2->id << " == (" << UFace->Child2->Child1->id << " " << UFace->Child2->Child2->id << ")\n";
			}
		}
		system("pause");
	}
	void confirmAll() { // �� : Face�� eCell�� sCell�� CellShape�� �����ؼ� �� �κе� ������ �ʿ��� �����߽��ϴ�.  2017-01-12
						// �׳� �����ڰ� Ȯ���Ϸ��� ����� �� �κ�
		if (LUCell != NULL) {
			if (((Cell*)LUCell)->LUCell != NULL) {
				cout << "Start Left : " << ((Cell*)LUCell)->LFace->id << "    ";
				cout << ((Cell*)LUCell)->LFace->Child2->id << " " << ((Cell*)LUCell)->LFace->Child1->id << " __ " <<
					((Cell*)LUCell)->DFace->Child1->id << " " << ((Cell*)LUCell)->DFace->Child2->id << " __ " <<
					((Cell*)LUCell)->RFace->Child1->id << " " << ((Cell*)LUCell)->RFace->Child2->id << " __ " <<
					((Cell*)LUCell)->UFace->Child2->id << " " << ((Cell*)LUCell)->UFace->Child1->id << " __    " <<
					((Cell*)((Cell*)LUCell)->LUCell)->DFace->id << " " << ((Cell*)((Cell*)LUCell)->LDCell)->RFace->id << " " <<
					((Cell*)((Cell*)LUCell)->RDCell)->UFace->id << " " << ((Cell*)((Cell*)LUCell)->RUCell)->LFace->id << "\n\n";
			}
			if (((Cell*)LDCell)->LUCell != NULL) {
				cout << "Start Left : " << ((Cell*)LDCell)->LFace->id << "    ";
				cout << ((Cell*)LDCell)->LFace->Child2->id << " " << ((Cell*)LDCell)->LFace->Child1->id << " __ " <<
					((Cell*)LDCell)->DFace->Child1->id << " " << ((Cell*)LDCell)->DFace->Child2->id << " __ " <<
					((Cell*)LDCell)->RFace->Child1->id << " " << ((Cell*)LDCell)->RFace->Child2->id << " __ " <<
					((Cell*)LDCell)->UFace->Child2->id << " " << ((Cell*)LDCell)->UFace->Child1->id << " __    " <<
					((Cell*)((Cell*)LDCell)->LUCell)->DFace->id << " " << ((Cell*)((Cell*)LDCell)->LDCell)->RFace->id << " " <<
					((Cell*)((Cell*)LDCell)->RDCell)->UFace->id << " " << ((Cell*)((Cell*)LDCell)->RUCell)->LFace->id << "\n\n";
			}
			if (((Cell*)RDCell)->RDCell != NULL) {
				cout << "Start Left : " << ((Cell*)RDCell)->LFace->id << "    ";
				cout << ((Cell*)RDCell)->LFace->Child2->id << " " << ((Cell*)RDCell)->LFace->Child1->id << " __ " <<
					((Cell*)RDCell)->DFace->Child1->id << " " << ((Cell*)RDCell)->DFace->Child2->id << " __ " <<
					((Cell*)RDCell)->RFace->Child1->id << " " << ((Cell*)RDCell)->RFace->Child2->id << " __ " <<
					((Cell*)RDCell)->UFace->Child2->id << " " << ((Cell*)RDCell)->UFace->Child1->id << " __    " <<
					((Cell*)((Cell*)RDCell)->LUCell)->DFace->id << " " << ((Cell*)((Cell*)RDCell)->LDCell)->RFace->id << " "
					<< ((Cell*)((Cell*)RDCell)->RDCell)->UFace->id << " " << ((Cell*)((Cell*)RDCell)->RUCell)->LFace->id << "\n\n";
			}
			if (((Cell*)RUCell)->RUCell != NULL) {
				cout << "Start Left : " << ((Cell*)RUCell)->LFace->id << "    ";
				cout << ((Cell*)RUCell)->LFace->Child2->id << " " << ((Cell*)RUCell)->LFace->Child1->id << " __ " <<
					((Cell*)RUCell)->DFace->Child1->id << " " << ((Cell*)RUCell)->DFace->Child2->id << " __ " <<
					((Cell*)RUCell)->RFace->Child1->id << " " << ((Cell*)RUCell)->RFace->Child2->id << " __ " <<
					((Cell*)RUCell)->UFace->Child2->id << " " << ((Cell*)RUCell)->UFace->Child1->id << " __    " <<
					((Cell*)((Cell*)RUCell)->LUCell)->DFace->id << " " << ((Cell*)((Cell*)RUCell)->LDCell)->RFace->id << " " <<
					((Cell*)((Cell*)RUCell)->RDCell)->UFace->id << " " << ((Cell*)((Cell*)RUCell)->RUCell)->LFace->id << "\n\n";
			}

		}
		else {
			cout << "NO CHILD\n";
		}
	}
	bool checkNode(Node* n) { // �� : Node�� ���ڷ� �ָ� Cell�� ���ڸ� ������ �ִ��� Ȯ���ϴ� �Լ� 2017-01-12
		if (LUNode == n || LDNode == n || RUNode == n || RDNode == n)
			return true;
		return false;
	}
	bool checkFace(Face* f) { // �� : Face�� ���ڷ� �ָ� Cell�� ���ڸ� ������ �ִ��� Ȯ���ϴ� �Լ� 2017-01-12
		if (LFace == f || UFace == f || RFace == f || DFace == f)
			return true;
		return false;
	}
	void splitCell(int layer, int depth) { // �� : Cell split ����, depth �߰� 2017-01-19
		const int OUTFLOW = 0, INLET = 1, TOP = 2, BOTTOM = 3, INTERRIOR = 4;
		if (LUCell == NULL) {	// ���� �� Cell�� �ɰ����� �ʾ�����, �ɰ���! 
								//( �ɰ������� LUCell, LDCell, RDCell, RUCell�� NULL�� �ƴҰŴϱ� ���� �ϳ��� üũ�Ѱ��� )
								// ���� Cell(Default)�� ���� �����صд�.
			LUCell = new Cell();	((Cell*)LUCell)->setDepth(depth + 1);
			LDCell = new Cell();	((Cell*)LDCell)->setDepth(depth + 1);
			RDCell = new Cell();	((Cell*)RDCell)->setDepth(depth + 1);
			RUCell = new Cell();	((Cell*)RUCell)->setDepth(depth + 1);
			cellVector.push_back(((Cell*)LUCell));	cellVector.push_back(((Cell*)LDCell));
			cellVector.push_back(((Cell*)RDCell));	cellVector.push_back(((Cell*)RUCell));

			// ���� ��������� Node�� ���� �����Ѵ�.
			Node *LeftCenter, *DownCenter, *RightCenter, *UpCenter, *cellCenter;
			if (!LFace->isParent()) { // parent�� �ƴϸ� �ɰ�������������
				LeftCenter = new Node(((LUNode->x + LDNode->x) / 2.0), ((LUNode->y + LDNode->y) / 2.0));
				nodeVector.push_back(LeftCenter);
			}
			else {
				LeftCenter = LFace->centerNode;
			}
			if (!DFace->isParent()) {
				DownCenter = new Node((this->LDNode->x + this->RDNode->x) / 2.0, (this->LDNode->y + this->RDNode->y) / 2.0);
				nodeVector.push_back(DownCenter);
			}
			else {
				DownCenter = DFace->centerNode;
			}
			if (!RFace->isParent()) {
				RightCenter = new Node((this->RDNode->x + this->RUNode->x) / 2.0, (this->RDNode->y + this->RUNode->y) / 2.0);
				nodeVector.push_back(RightCenter);
			}
			else {
				RightCenter = RFace->centerNode;
			}
			if (!UFace->isParent()) {
				UpCenter = new Node((this->RUNode->x + this->LUNode->x) / 2.0, (this->RUNode->y + this->LUNode->y) / 2.0);
				nodeVector.push_back(UpCenter);
			}
			else {
				UpCenter = UFace->centerNode;
			}
			cellCenter = new Node((this->LDNode->x + this->RUNode->x) / 2.0, (this->LDNode->y + this->RUNode->y) / 2.0);
			nodeVector.push_back(cellCenter);

			// ���� Cell�� Node�� �����Ѵ�.
			setChildNodes(LeftCenter, DownCenter, RightCenter, UpCenter, cellCenter, 0);

			// ���� Cell Size �� ����.
			double ChildSize = cellSize / 2.0;
			((Cell*)LUCell)->setSize(ChildSize);		((Cell*)LDCell)->setSize(ChildSize);
			((Cell*)RDCell)->setSize(ChildSize);		((Cell*)RUCell)->setSize(ChildSize);

			// ���� Face 12���� ����̴ϴ�.
			Face *LU = 0, *LD = 0, *DL = 0, *DR = 0, *RD = 0, *RU = 0;
			Face *UR = 0, *UL = 0, *IL = 0, *ID = 0, *IR = 0, *IU = 0;

			// Cell ���� �� ���� ��������� Face�� �ܺ� 8�� ���� ����
			// LD LU
			if (!LFace->isParent()) {
				LD = new Face(LDNode, LeftCenter);	faceVector.push_back(LD);
				LU = new Face(LeftCenter, LUNode);	faceVector.push_back(LU);
				LD->setAttribute(LFace->attribute);
				LU->setAttribute(LFace->attribute);
				LFace->setChilds(LD, LU);
				LFace->setCenterNode(LeftCenter);
			}
			else {
				LD = LFace->Child1;
				LU = LFace->Child2;
			}
			// DL DR
			if (!DFace->isParent()) {
				DL = new Face(LDNode, DownCenter);	faceVector.push_back(DL);
				DR = new Face(DownCenter, RDNode);	faceVector.push_back(DR);
				DL->setAttribute(DFace->attribute);
				DR->setAttribute(DFace->attribute);
				DFace->setChilds(DL, DR);
				DFace->setCenterNode(DownCenter);
			}
			else {
				DL = DFace->Child1;
				DR = DFace->Child2;
			}
			// RD RU
			if (!RFace->isParent()) {
				RD = new Face(RDNode, RightCenter);	faceVector.push_back(RD);
				RU = new Face(RightCenter, RUNode);	faceVector.push_back(RU);
				RD->setAttribute(RFace->attribute);
				RU->setAttribute(RFace->attribute);
				RFace->setChilds(RD, RU);
				RFace->setCenterNode(RightCenter);
			}
			else {
				RD = RFace->Child1;
				RU = RFace->Child2;
			}
			// UL UR
			if (!UFace->isParent()) {
				UL = new Face(LUNode, UpCenter);	faceVector.push_back(UL);
				UR = new Face(UpCenter, RUNode);	faceVector.push_back(UR);
				UL->setAttribute(UFace->attribute);
				UR->setAttribute(UFace->attribute);
				UFace->setChilds(UL, UR);
				UFace->setCenterNode(UpCenter);
			}
			else {
				UL = UFace->Child1;
				UR = UFace->Child2;
			}

			IL = new Face(LeftCenter, cellCenter);	IL->setAttribute(INTERRIOR);	IL->setDepth(UL->depth);	faceVector.push_back(IL);
			ID = new Face(DownCenter, cellCenter);	ID->setAttribute(INTERRIOR);	ID->setDepth(UL->depth);	faceVector.push_back(ID);
			IR = new Face(cellCenter, RightCenter);	IR->setAttribute(INTERRIOR);	IR->setDepth(UL->depth);	faceVector.push_back(IR);
			IU = new Face(cellCenter, UpCenter);		IU->setAttribute(INTERRIOR);	IU->setDepth(UL->depth);	faceVector.push_back(IU);

			// �� ���� cell�� face�� ����Ű����
			((Cell*)LUCell)->LFace = LU;	((Cell*)LUCell)->DFace = IL;	((Cell*)LUCell)->RFace = IU;	((Cell*)LUCell)->UFace = UL;
			((Cell*)LDCell)->LFace = LD;	((Cell*)LDCell)->DFace = DL;	((Cell*)LDCell)->RFace = ID;	((Cell*)LDCell)->UFace = IL;
			((Cell*)RDCell)->LFace = ID;	((Cell*)RDCell)->DFace = DR;	((Cell*)RDCell)->RFace = RD;	((Cell*)RDCell)->UFace = IR;
			((Cell*)RUCell)->LFace = IU;	((Cell*)RUCell)->DFace = IR;	((Cell*)RUCell)->RFace = RU;	((Cell*)RUCell)->UFace = UR;

			// �� Face�� startCell / endCell ���� // �� : Face�� sCell�� eCell�� CellShape�� �����Ͽ��� ������ �ؿ� �ڵ嵵 �ణ �����߽��ϴ�.  2017-01-12

			// LU, LD 

			LU->setEndCell(((Cell*)LUCell));	LD->setEndCell(((Cell*)LDCell));
			if (LFace->attribute != INLET) {
				if (((Cell*)(LFace->sCell))->LUCell == NULL) {
					LU->setStartCell(LFace->sCell);
					LD->setStartCell(LFace->sCell);
				}
				else {
					LU->setStartCell(((Cell*)(LFace->sCell))->RUCell);
					LD->setStartCell(((Cell*)(LFace->sCell))->RDCell);
				}
			}
			// DL, DR
			DL->setStartCell(((Cell*)LDCell));	DR->setStartCell(((Cell*)RDCell));
			if (DFace->attribute != BOTTOM) {
				if (((Cell*)(DFace->eCell))->LUCell == NULL) {
					DL->setEndCell(DFace->eCell);
					DR->setEndCell(DFace->eCell);
				}
				else {
					DL->setEndCell(((Cell*)(DFace->eCell))->LUCell);
					DR->setEndCell(((Cell*)(DFace->eCell))->RUCell);
				}
			}
			// RD, RU
			RD->setStartCell(((Cell*)RDCell));	RU->setStartCell(((Cell*)RUCell));
			if (RFace->attribute != OUTFLOW) {
				if (((Cell*)(RFace->eCell))->LUCell == NULL) {
					RD->setEndCell(RFace->eCell);
					RU->setEndCell(RFace->eCell);
				}
				else {
					RD->setEndCell(((Cell*)(RFace->eCell))->LDCell);
					RU->setEndCell(((Cell*)(RFace->eCell))->LUCell);
				}
			}
			// UR, UL
			UR->setEndCell(((Cell*)RUCell));	UL->setEndCell(((Cell*)LUCell));
			if (UFace->attribute != TOP) {
				if (((Cell*)(UFace->sCell))->LUCell == NULL) {
					UR->setStartCell(UFace->sCell);
					UL->setStartCell(UFace->sCell);
				}
				else {
					UR->setStartCell(((Cell*)(UFace->sCell))->RDCell);
					UL->setStartCell(((Cell*)(UFace->sCell))->LDCell);
				}
			}
			// INNER Faces
			IL->setStartCell(((Cell*)LUCell));	IL->setEndCell(((Cell*)LDCell));
			ID->setStartCell(((Cell*)LDCell));	ID->setEndCell(((Cell*)RDCell));
			IR->setStartCell(((Cell*)RUCell));	IR->setEndCell(((Cell*)RDCell));
			IU->setStartCell(((Cell*)LUCell));	IU->setEndCell(((Cell*)RUCell));
		}

		// layer spread // �� : Cell split �� layer��� �߰�.  2017-01-22
		if (layer > 0) {
			// [0] [1] [2] [3] [4] [5] [6] [7]
			// LU,  L, LD,  D, RD,  R, RU,  U 
			CellShape *SpreadCell[8] = { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL };
			bool isLeft = (LFace->attribute == INLET);
			bool isBottom = (DFace->attribute == BOTTOM);
			bool isRight = (RFace->attribute == OUTFLOW);
			bool isTop = (UFace->attribute == TOP);

			if (!isLeft) {
				SpreadCell[1] = LFace->sCell;
				if (!isTop)	SpreadCell[0] = ((Cell*)SpreadCell[1])->UFace->sCell;
				if (!isBottom)	SpreadCell[2] = ((Cell*)SpreadCell[1])->DFace->eCell;
			}
			if (!isBottom) {
				SpreadCell[3] = DFace->eCell;
				if (!isRight)	SpreadCell[4] = ((Cell*)SpreadCell[3])->RFace->eCell;
			}
			if (!isRight) {
				SpreadCell[5] = RFace->eCell;
				if (!isTop)	SpreadCell[6] = ((Cell*)SpreadCell[5])->UFace->sCell;
			}
			if (!isTop) SpreadCell[7] = UFace->sCell;
			// spread by layer
			for (int i = 0; i < 8; i++) {
				if (SpreadCell[i] != NULL) {
					((Cell*)SpreadCell[i])->splitCell(layer - 1, 0);
				}
			}
		}

		// depth �������鼭 �� �ɰ��� �κ�
		if (depth > 1) {
			((Cell*)RUCell)->splitCell(layer, depth - 1);
			((Cell*)RDCell)->splitCell(layer, depth - 1);
			((Cell*)LUCell)->splitCell(layer, depth - 1);
			((Cell*)LDCell)->splitCell(layer, depth - 1);
		}
	}

};
class CellRectangle : public CellShape { // �� : child�� ������ �ʴ´�.
public:
	Node *LUNode, *LDNode, *RDNode, *RUNode;
	CellRectangle() {
		LUNode = LDNode = RDNode = RUNode = NULL;
		iscut = false;
	}
};
class CellTriangle : public CellShape { // �� : child�� ������ �ʴ´�.
public: //0208
	Node *HONode, *VONode, *WONode; // opposite angle, �� ���̽��� �밢�� ��ġ�� ��
	CellTriangle() {
		HONode = VONode = WONode = NULL;
		id = ++triCount;
		iscut = false;
	}
	CellTriangle(Node *HO, Node *VO, Node *WO) {
		HONode = HO;	VONode = VO;	WONode = WO;
		id = ++triCount;
		iscut = false;
	}
	~CellTriangle() {
		triCount--;
	}
	int getTriCount() {
		return triCount;
	}
};
class Rectangle {
public:
	double LDx, LDy, RUx, RUy;
	Cell **initCell;
	double initCellSize;
	int widthCount, heightCount;

public:
	Rectangle(double LDX, double LDY, double Cellsize, int WidthCount, int HeightCount) {//��_initcellsize����
		LDx = LDX;	LDy = LDY;
		RUx = LDX + WidthCount*Cellsize;	RUy = LDY + HeightCount*Cellsize;//��_initcellsize����

		initCellSize = Cellsize;//��_initcellsize����

		widthCount = WidthCount;//��_initcellsize����
		heightCount = HeightCount;//��_initcellsize����

		Node **initNode = new Node*[widthCount + 1];
		for (int w = 0; w <= widthCount; w++) {
			double plusX = LDX + (w * initCellSize);
			initNode[w] = new Node[heightCount + 1];
			for (int h = 0; h <= heightCount; h++) {
				double plusY = LDY + (h * initCellSize);
				initNode[w][h].setPoint(plusX, plusY);
				nodeVector.push_back(&initNode[w][h]);
			}
		}

		const int OUTFLOW = 0, INLET = 1, TOP = 2, BOTTOM = 3, INTERRIOR = 4;
		initCell = new Cell*[widthCount];
		for (int i = 0; i<widthCount; i++) {
			initCell[i] = new Cell[heightCount];
			for (int j = 0; j<heightCount; j++) {
				initCell[i][j].setSize(initCellSize);
				cellVector.push_back(&initCell[i][j]);

				initCell[i][j].setLUNode(&initNode[i][j + 1]);
				initCell[i][j].setLDNode(&initNode[i][j]);
				initCell[i][j].setRDNode(&initNode[i + 1][j]);
				initCell[i][j].setRUNode(&initNode[i + 1][j + 1]);

				// Left Face
				if (i == 0) { // Left : inlet
					initCell[i][j].LFace = new Face(initCell[i][j].LDNode, initCell[i][j].LUNode);
					initCell[i][j].LFace->setAttribute(INLET);
					faceVector.push_back(initCell[i][j].LFace);
				}
				else {
					initCell[i][j].LFace = initCell[i - 1][j].RFace;
				}

				// Bottom Face
				if (j == 0) {
					initCell[i][j].DFace = new Face(initCell[i][j].LDNode, initCell[i][j].RDNode);
					initCell[i][j].DFace->setAttribute(BOTTOM);
					faceVector.push_back(initCell[i][j].DFace);
				}
				else {
					initCell[i][j].DFace = initCell[i][j - 1].UFace;
				}

				// Right Face
				initCell[i][j].RFace = new Face(initCell[i][j].RDNode, initCell[i][j].RUNode);
				if (i == widthCount - 1) {
					initCell[i][j].RFace->setAttribute(OUTFLOW);
				}
				faceVector.push_back(initCell[i][j].RFace);

				// Top Face
				initCell[i][j].UFace = new Face(initCell[i][j].LUNode, initCell[i][j].RUNode);
				if (j == heightCount - 1) {
					initCell[i][j].UFace->setAttribute(TOP);
				}
				faceVector.push_back(initCell[i][j].UFace);

				if (initCell[i][j].LFace->attribute != INLET)
					initCell[i][j].LFace->setStartCell(&initCell[i - 1][j]);
				initCell[i][j].LFace->setEndCell(&initCell[i][j]);

				initCell[i][j].DFace->setStartCell(&initCell[i][j]);
				if (initCell[i][j].DFace->attribute != BOTTOM)
					initCell[i][j].DFace->setEndCell(&initCell[i][j - 1]);

				initCell[i][j].RFace->setStartCell(&initCell[i][j]);
				if (initCell[i][j].RFace->attribute != OUTFLOW)
					initCell[i][j].RFace->setEndCell(&initCell[i + 1][j]);

				if (initCell[i][j].UFace->attribute != TOP)
					initCell[i][j].UFace->setStartCell(&initCell[i][j + 1]);
				initCell[i][j].UFace->setEndCell(&initCell[i][j]);
			}
		}
	}
	int getNodeCount() {
		return initCell[0][0].getLDNode()->getCount();
	}
	int getCellCount() {
		return initCell[0][0].getCellCount();
	}
	int getFaceCount() {
		return initCell[0][0].getFaceCount();
	}
	void printAll() {
		for (int w = 0; w<widthCount; w++) {
			for (int h = 0; h<heightCount; h++) {
				cout << "cell[" << w << "][" << h << "]\n";
				initCell[w][h].printCell();
			}
		}
	}
	void confirmAll() {
		for (int w = 0; w<widthCount; w++) {
			for (int h = 0; h<heightCount; h++) {
				cout << "cell[" << w << "][" << h << "]\n";
				initCell[w][h].confirmAll();
			}
		}
	}
	void dotPoint(double x, double y, int layer = 0, int depth = 0) { //�� : ��輱���ó�� ����
		int flag = 0;// flag 0: �⺻ 1: x��迡 ������ 2: y ��迡 ������ 3: �Ѵ� ������
		int findX = 1, findY = 1;
		while (true) {
			if (((findX * initCellSize) + LDx) > x) {
				break;
			}
			else if (((findX * initCellSize) + LDx) == x) flag = 1;

			findX++;
		}
		while (true) {
			if (((findY * initCellSize) + LDy) > y) {
				break;
			}
			else if (((findY * initCellSize) + LDy) == y) {
				if (flag == 1) flag = 3;
				flag = 2;
			}
			findY++;
		}
		initCell[findX - 1][findY - 1].dotPoint(x, y, layer, depth);
		if ((flag == 1 || flag == 3) && findX > 1) initCell[findX - 2][findY - 1].dotPoint(x, y, layer, depth);
		else if ((flag == 2 || flag == 3) && findY > 1) initCell[findX - 1][findY - 2].dotPoint(x, y, layer, depth);


	}
	void splitCell(double, double, double, double, int, int, int); // �� : Cell split �Լ� �Դϴ�. �� �� ���� layer, depth�� �Է¹޽��ϴ�. (x1, y1, x2, y2, layer, depth) 2017-01-22
};

int Node::nodeCount = 0;
int Face::faceCount = 0;
int Face::maxDepth = 0;
int Cell::cellCount = 0;
//int CellRectangle::rectCount = 0;
int CellTriangle::triCount = 0; //0208

void DescribeMesh() { // �� : Cell => CellShape 2017-01-12 //0208
	fprintf(fp, "(0 \"Grid:\")\n\n");

	fprintf(fp, "(0 \"Dimensions:\")\n");
	fprintf(fp, "(2 2)\n\n");

	fprintf(fp, "(12 (0 1 %x 0))\n", cellVector.size() + minimalCellVector_tri.size()); // Cell Count
	fprintf(fp, "(13 (0	1 %x 0))\n", faceVector.size() + wallVector.size()); // Face Count 
	fprintf(fp, "(10 (0 1 %x 0 2))\n\n", initRectangle->getNodeCount()); // Node Count

	int myID = 1;
	double printSize = cellVector[0]->getSize();
	int cellParentCount = 0;
	for (int i = 0; i< cellVector.size(); i++) {
		if (cellVector[i]->isParent()) {
			cellParentCount++;
		}
	}

	fprintf(fp, "(12 (%x 1 %x 1 3))\n", myID++, cellVector.size() - cellParentCount); // Cells (if have child, it should be changed)
	if (cellParentCount != 0) {
		fprintf(fp, "(12 (%x %x %x 20 3))\n", myID++, 1 + cellVector.size() - cellParentCount, cellVector.size());
	}
	if (minimalCellVector_tri.size() != 0) {
		fprintf(fp, "(12 (%x %x %x 1 1))\n", myID++, cellVector.size() + 1, cellVector.size() + minimalCellVector_tri.size());
	}
	fprintf(fp, "\n");

	//triangle ����

	int faceMaxDepth = faceVector[0]->getMaxDepth();
	int **FACount = new int*[faceMaxDepth + 1];
	for (int i = 0; i<faceMaxDepth + 1; i++) {
		const int ColumnCount = 10;
		FACount[i] = new int[ColumnCount];
		for (int j = 0; j<ColumnCount; j++) FACount[i][j] = 0;
	}

	// for get Count attributes
	const int OUTFLOW = 0, INLET = 1, TOP = 2, BOTTOM = 3, INTERRIOR = 4;
	int findDepth = faceVector[0]->getDepth();
	for (vector<Face*>::iterator it = faceVector.begin(); it != faceVector.end(); it++) {
		if ((*it)->depth != findDepth) {
			findDepth = (*it)->depth;
		}
		int att = (*it)->attribute;
		int plusAlpha = 0;
		if ((*it)->isParent()) plusAlpha = 5;
		if (att == INTERRIOR) {
			FACount[findDepth][0 + plusAlpha]++;
		}
		else if (att == BOTTOM) {
			FACount[findDepth][1 + plusAlpha]++;
		}
		else if (att == TOP) {
			FACount[findDepth][2 + plusAlpha]++;
		}
		else if (att == INLET) {
			FACount[findDepth][3 + plusAlpha]++;
		}
		else {
			FACount[findDepth][4 + plusAlpha]++;
		}
	}

	// start print faces
	queue< Face* > haveChild;
	int faceNumber = 1, lowCount = 0, di = faceVector[0]->depth;
	for (unsigned int k = 0; k<faceVector.size(); k++) {


		if (di != faceVector[k]->depth) {
			di = faceVector[k]->depth;
			lowCount = 0;

		}
		if (FACount[di][lowCount] == 0) {
			k--; lowCount++; continue;
		}
		int ParentChildNumber[2] = { faceNumber,FACount[di][lowCount] + faceNumber - 1 };
		fprintf(fp, "(13 (%x %x %x ", myID++, faceNumber, FACount[di][lowCount] + faceNumber - 1);
		if (faceVector[k]->isParent()) {
			fprintf(fp, "1f 2)(");
			faceVector[k]->myPrintID = (myID - 1);
		}
		else if (lowCount == 0) fprintf(fp, "2 2)(");//interior
		else if (lowCount <= 2) fprintf(fp, "7 2)(");//upp//lower
		else if (lowCount == 3) fprintf(fp, "a 2)(");//inlet
		else if (lowCount == 4) fprintf(fp, "24 2)(");//out

		faceNumber = FACount[di][lowCount] + faceNumber;
		for (int i = 0; i<FACount[di][lowCount]; i++) {
			if (faceVector[k]->isChild()) {
				faceVector[k]->myPrintID = (myID - 1);
			}
			if (faceVector[k]->isParent()) {
				faceVector[k]->myPrintID = (myID - 1);
				haveChild.push(faceVector[k]);
			}
			CellShape *s = faceVector[k]->sCell;
			CellShape *e = faceVector[k]->eCell;

			int sn = faceVector[k]->sNode->id;
			int en = faceVector[k]->eNode->id;
			int sfi = 0;
			int efi = 0;

			if (s != NULL) sfi = s->getID();//0208
			if (e != NULL) efi = e->getID();//0208

											/* test
											vector<int>::iterator initID;
											for(initID = detinitcellVector.begin(); initID < detinitcellVector.end(); initID++) {
											cout << "initCellID :" <<(*initID) << endl;
											if (sfi == (*initID)) {
											sfi = 0;
											}
											if (efi == (*initID)) {
											efi = 0;
											}
											}*/

			if (s == NULL || sfi == NULL) {//��delete
				fprintf(fp, "\n%x	%x	%x	%x", en, sn, efi, sfi);
			}
			else {
				fprintf(fp, "\n%x	%x	%x	%x", sn, en, sfi, efi);
			}
			k++;
		}
		fprintf(fp, "))\n\n");
		lowCount++;
		k--;
		if (!haveChild.empty()) {
			fprintf(fp, "(59 (%x %x %x %x)(", ParentChildNumber[0], ParentChildNumber[1], haveChild.front()->myPrintID, haveChild.front()->Child1->myPrintID);
			myID++;
			while (!haveChild.empty()) {
				fprintf(fp, "\n 2 %x %x", haveChild.front()->Child1->id, haveChild.front()->Child2->id);
				haveChild.pop();
			}
			fprintf(fp, "))\n\n");
		}
	} // end of faces

	  //print wall
	fprintf(fp, "(13 (%x %x %x 3 2)(", myID++, faceVector.size() + 1, faceVector.size() + wallVector.size());
	for (int i = 0; i < wallVector.size(); i++) {
		int sn = wallVector[i]->sNode->id;
		int en = wallVector[i]->eNode->id;
		int sfi = wallVector[i]->sCell->getID();
		fprintf(fp, "\n%x	%x	%x	0", sn, en, sfi);
	}
	fprintf(fp, "))\n\n");

	// print nodes
	int nodeCount = nodeVector.size();
	fprintf(fp, "(10 (%x 1 %x 1 2)\n(\n", myID++, nodeCount);
	for (int i = 0; i<nodeCount - 1; i++) {
		fprintf(fp, "%lf\t	%lf\n", nodeVector[i]->x, nodeVector[i]->y);
	}
	fprintf(fp, "%lf\t	%lf))\n", nodeVector[nodeCount - 1]->x, nodeVector[nodeCount - 1]->y);
	// end of nodes
}
int cnt = 0;
bool faceCompare(Face *f1, Face *f2) {
	//delete �� face �� ó��
	if (f1->isParent()) {
		if (f1->Child1->getID() == NULL)f1->Child1 = f1->Child2;
		else if (f1->Child2->getID() == NULL) f1->Child2 = f1->Child1;
	}
	if (f2->isParent()) {
		if (f2->Child1->getID() == NULL) f2->Child1 = f2->Child2;
		else if (f2->Child2->getID() == NULL) f2->Child2 = f2->Child1;
	}
	int f1Depth = f1->depth;
	int f2Depth = f2->depth;
	if (f1Depth > f2Depth) {
		return true;
	}
	else if (f1Depth == f2Depth) {
		if ((f1->isParent() && f2->isParent())) {
			if (f1->attribute > f2->attribute)
				return true;
			else if (f1->attribute == f2->attribute) {
				return f1->Child1->myPrintID > f2->Child1->myPrintID;
			}
			else
				return false;
		}
		else if ((!f1->isParent() && !f2->isParent())) {
			return f1->attribute > f2->attribute;
		}
		else {
			return !f1->isParent();
		}

	}
	else {
		return false;
	}
}
bool cellCompare(Cell *c1, Cell *c2) {

	if ((c1->isParent() && c2->isParent()) || (!c1->isParent() && !c2->isParent())) {
		return false;
	}
	else {
		return !c1->isParent();
	}
}
bool nodeCompare(Node *n1, Node *n2) { // �� : checkInner���� ���
	if (n1->getID() < n2->getID())
		return true;
	return false;
}
bool nodeCompareXY(Node *n1, Node *n2) { // �� : checkInner���� ���
	if (n1->getX() == n2->getX()) {
		if (n1->getY() > n2->getY())
			return true;
	}
	else {
		if (n1->getX() < n2->getX()) return true;
	}
	return false;
}
int findn(double point, double size, int SorL) {//��
	if (SorL == 0) {//small
		if (point >= 0) {
			int n = ((point / size) - (int)(point / size) == 0) ? point / size : point / size + 1;
			return n;
		}
		int n = point / size;
		return n;
	}
	//large
	if (point < 0) {
		int n = ((point / size) - (int)(point / size) == 0) ? point / size : point / size - 1;
		return n;
	}
	int n = point / size;
	return n;
}

void findminnode(double x0, double y0, double x1, double y1, double size, int updown) {//��_01_22 ���� 													   // input2.txt�� ����� mininode�� ������ �ּ� cell������� Ŀ�� findmininodeCell���� cell�� ã�� ���ϴ� ������ ����.
	double gradient = (y1 - y0) / (x1 - x0);
	double sx, lx, sy, ly;
	sx = min(x0, x1);
	lx = max(x0, x1);
	sy = min(y0, y1);
	ly = max(y0, y1);
	if (gradient == 0) {
		for (int i = findn(sx, size, 0); i <= findn(lx, size, 1); i++) {
			mininode[updown].push_back(make_pair(i*size, y0));
		}
	}
	else {
		for (int i = findn(sx, size, 0); i <= findn(lx, size, 1); i++) {
			mininode[updown].push_back(make_pair(i*size, gradient* (i*size - x0) + y0));

		}
		for (int i = findn(sy, size, 0); i <= findn(ly, size, 1); i++) {
			double x = (i*size - y0) / gradient + x0;
			mininode[updown].push_back(make_pair(x, i*size));
		}
	}
	//8�ڸ��� ���� ���� ����
	sort(mininode[updown].begin(), mininode[updown].end());
	vector<pdd>::iterator posy = unique(mininode[updown].begin(), mininode[updown].end());
	mininode[updown].erase(posy, mininode[updown].end());
}
bool innerpoint(Cell* c, pdd n) {
	if (c->getLDNode()->x <= n.first && n.first <= c->getRDNode()->x) {
		if (c->getLDNode()->y <= n.second && n.second <= c->getLUNode()->y) return true;
	}
	return false;
}

node_of_cell Node::findtailCell(pdd n) { // �� : tail ����  2017-03-07
	vector<Cell*>::iterator it = cellVector.begin();
	for (it; it < cellVector.end(); it++) {
		if ((*it)->getLUNode() == this) {
			if (innerpoint((*it), n)) return make_pair(1, (*it));
		}
		else if ((*it)->getLDNode() == this) {
			if (innerpoint((*it), n)) return make_pair(2, (*it));
		}
		else if ((*it)->getRUNode() == this) {
			if (innerpoint((*it), n)) return make_pair(3, (*it));
		}
		else if ((*it)->getRDNode() == this) {
			if (innerpoint((*it), n)) return make_pair(4, (*it));
		}
	}
	printf("\nerror_findtialCell_Node_Node\n");
	return make_pair(0, (*cellVector.begin()));
}

CellShape* Node::findCell(Node* n) { // �� : Node�� Node�϶� mininodeCell ���� 2017-01-12
	vector<Cell*>::iterator it = cellVector.begin();
	for (it; it < cellVector.end(); it++) {
		if ((*it)->getLUNode() == n) {
			if ((*it)->checkNode(this))
				return (*it);
		}
		else if ((*it)->getLDNode() == n) {
			if ((*it)->checkNode(this))
				return (*it);
		}
		else if ((*it)->getRUNode() == n) {
			if ((*it)->checkNode(this))
				return (*it);
		}
		else if ((*it)->getRDNode() == n) {
			if ((*it)->checkNode(this))
				return (*it);
		}
	}
	printf("\nerror_findCell_Node_Node\n");
	return NULL;
}
CellShape* Node::findCell(Face* f) { // �� : Node�� face�϶� mininodeCell ���� 2017-01-12
	if (f->eCell) {
		if (((Cell*)(f->eCell))->checkNode(this))
			return f->eCell;
	}
	if (f->sCell) {
		if (((Cell*)(f->sCell))->checkNode(this))
			return f->sCell;
	}
	else
		printf("\nerror_findCell_Node_Face\n");
	return NULL;
}
CellShape* Face::findCell(Node* n) { // �� : face�� Node�϶� mininodeCell ���� 2017-01-12
	if (this->eCell) {
		if (((Cell*)(this->eCell))->checkNode(n))
			return this->eCell;
	}
	if (this->sCell) {
		if (((Cell*)(this->sCell))->checkNode(n))
			return this->sCell;
	}
	else
		printf("\nerror_findCell_Face_Node\n");
	return NULL;
}
CellShape* Face::findCell(Face* f) { // �� : face�� face�϶� mininodeCell ���� 2017-01-12
	if (this->eCell) {
		if (((Cell*)(this->eCell))->checkFace(f))
			return this->eCell;
	}
	if (this->sCell) {
		if (((Cell*)(this->sCell))->checkFace(f))
			return this->sCell;
	}
	else
		printf("\nerror_findCell_Face_Face\n");
	return NULL;
}

bool setupmininodeCell(pdd p1, pdd p2) { // �� : mininodePlace�� ���� mininodeCell�� ���� 2017-01-12
	pair< int, void* > pre = mininodePlace[p1]; // main�� �ݺ������� p1�� mininode[i-1]
	pair< int, void* > now = mininodePlace[p2]; // main�� �ݺ������� p2�� mininode[i]
	if (pre.first == 0) {//node
		if (now.first == 0)
			mininodeCell[p1][p2] = ((Node*)pre.second)->findCell((Node*)now.second);
		else if (now.first == 1)
			mininodeCell[p1][p2] = ((Node*)pre.second)->findCell((Face*)now.second);
		else {
			printf("\nerror_setupmininodeCell_0\n");
			return false;
		}
	}
	else if (pre.first == 1) {//face
		if (now.first == 0)
			mininodeCell[p1][p2] = ((Face*)pre.second)->findCell((Node*)now.second);
		else if (now.first == 1)
			mininodeCell[p1][p2] = ((Face*)pre.second)->findCell((Face*)now.second);
		else {
			printf("\nerror_setupmininodeCell_1\n");
			return false;
		}
	}
	if (mininodeCell[p1][p2] == NULL)
		printf("\nerror_setupmininodeCell_2\n");
	//else printf("\nmininodeCell[%lf, %lf][%lf, %lf] = %d\n", p1.first, p1.second, p2.first, p2.second, ((Cell*)mininodeCell[p1][p2])->id); // �� : Ȯ�ο�
	return true;
}
void Rectangle::splitCell(double x1, double y1, double x2, double y2, int layer, int splitDepth, int depth) { // �� : Cell split �Լ� �Դϴ�. �� �ΰ��� layer, splitDepth, depth�� �Է¹޽��ϴ�. 2017-02-20
	int findX1 = 1, findY1 = 1;
	int findX2 = 1, findY2 = 1;
	int flag = 0; // flag 0: �⺻ 1: x��迡 ������ 2: y ��迡 ������ 3: �Ѵ� ������ // �� : ������ ���� �� ���� ��迡 ��ĥ �� ��ġ 2017-01-22

	if (splitDepth > depth) {
		cout << "split Cell�� depth�� main�� depth�� �ʰ��Ͽ� main�� depth�� �����ϰڽ��ϴ�." << endl;
		splitDepth = depth;
	}
	while (true) {
		if (((findX1 * initCellSize) + LDx) > x1) {
			break;
		}
		else if (((findX1 * initCellSize) + LDx) == x1) flag = 1; // �� : ������ ���� �� ���� ��迡 ��ĥ �� ��ġ 2017-01-22

		findX1++;
	}
	while (true) {
		if (((findY1 * initCellSize) + LDy) > y1) {
			break;
		}
		else if (((findY1 * initCellSize) + LDy) == y1) { // �� : ������ ���� �� ���� ��迡 ��ĥ �� ��ġ 2017-01-22
			if (flag == 1) flag = 3;
			else flag = 2;
		}
		findY1++;
	}

	while (true) {
		if (initCell[findX2 - 1] == NULL) { // �� : boundary error ���� 2017_01_31
			findX2--;
			break;
		}
		if (((findX2 * initCellSize) + LDx) > x2) {
			break;
		}
		findX2++;
	}
	while (true) {
		if (initCell[findY2 - 1] == NULL) { // �� : boundary error ���� 2017_01_31
			findY2--;
			break;
		}if (((findY2 * initCellSize) + LDy) > y2) {
			break;
		}
		findY2++;
	}

	if (flag == 1 || flag == 3 || findX1 > 2) // �� : ���� ���� ���� 2017-01-22
		findX1--;
	if (flag == 2 || flag == 3 || findY1 > 2) // �� : ���� ���� ���� 2017-01-22
		findY1--;

	for (int i = findX1; i < findX2 + 1; i++) {
		for (int j = findY1; j < findY2 + 1; j++) {
			initCell[i - 1][j - 1].splitCell(layer, splitDepth); // �� : depth �߰�, Class Cell�� ����Լ� splitCell ��� 2017-01-22
		}
	}
}

double ccw(pdd a, pdd b) { //��_01_22
	return a.first*b.second - a.second*b.first;
}
double ccw(pdd a, pdd b, pdd p) {//��_01_22 :���� ap �� �������� ���� ������ ���� �ؿ� ������ ��� �������� ���� 0 ���
	return ccw(make_pair(a.first - p.first, a.second - p.second), make_pair(b.first - p.first, b.second - p.second));
}
double distancePTP(pdd a, pdd b) { // �� : ���� ������ �Ÿ��� ������ ���ϴ� �Լ� 2017_02_13
	return (((b.first - a.first) * (b.first - a.first)) + ((b.second - a.second) * (b.second - a.second)));
}
double ccwAddition(pdd a, pdd b, pdd p) { // �� : ���� ccw ���� ��ȯ���� 0�� �� ������ �߻��Ͽ� �Ϻ� ������ �Լ��� ����. 2017_02_13
	double temp = ccw(make_pair(a.first - p.first, a.second - p.second), make_pair(b.first - p.first, b.second - p.second));
	if (temp > 0)
		return 1;
	else if (temp < 0)
		return -1;
	else { // �� : ���� ������ ���Ⱑ ���� ��� ���� �������� �ִٸ� 0�� ��ȯ�ϰ� �׷��� �ʴٸ� 1�� ��ȯ 2017_02_13
		pdd middlePoint = make_pair((a.first + b.first) / 2, (a.second + b.second) / 2);  // �� : �� a�� �� b�� �߰���. 2017_02_13
		double length = sqrt(distancePTP(a, b));
		if (distancePTP(p, middlePoint) > length)
			return 1;
	}
	return 0;
}
vector<pdd> centermaking(vector<pdd> centernode, pdd cnode, Cell* c, double mincellsize) { //��_cutcell����:
	Face *  cface = (Face*)mininodePlace[cnode].second;
	//0 : left , 1: down 2: right, 3: up 4: center
	if (cface == c->LFace) {
		centernode[0] = cnode;
		centernode[2] = make_pair(cnode.first + mincellsize, cnode.second);
	}
	else if (cface == c->RFace) {
		centernode[2] = cnode;
		centernode[0] = make_pair(cnode.first - mincellsize, cnode.second);
	}
	else if (cface == c->DFace) {
		centernode[1] = cnode;
		centernode[3] = make_pair(cnode.first, cnode.second + mincellsize);
	}
	else if (cface == c->UFace) {
		centernode[3] = cnode;
		centernode[1] = make_pair(cnode.first, cnode.second - mincellsize);
	}
	else { cout << " error : centermaking _cell �ȿ� face�� ����" << endl; }
	return centernode;
}

void cuting(Cell* c, pdd fnode, pdd snode, double mincellsize, int updown, double gradient) {//������
	vector<pdd> centernode = vector<pdd>(5);//0 : left , 1: down 2: right, 3: up 4: center
	centernode = centermaking(centernode, fnode, c, mincellsize);
	centernode = centermaking(centernode, snode, c, mincellsize);
	centernode[4] = make_pair(centernode[1].first, centernode[0].second);

	int deletenode = 0; // LD=3 LU=6 UR=11 DR=8

	c->LUCell = new Cell();	((Cell*)c->LUCell)->setDepth(c->depth + 1);
	c->LDCell = new Cell();	((Cell*)c->LDCell)->setDepth(c->depth + 1);
	c->RDCell = new Cell();	((Cell*)c->RDCell)->setDepth(c->depth + 1);
	c->RUCell = new Cell();	((Cell*)c->RUCell)->setDepth(c->depth + 1);
	cellVector.push_back(((Cell*)c->LUCell));	cellVector.push_back(((Cell*)c->LDCell));
	cellVector.push_back(((Cell*)c->RDCell));	cellVector.push_back(((Cell*)c->RUCell));

	// ���� ��������� Node�� ���� �����Ѵ�.
	//0 : left , 1: down 2: right, 3: up 4: center
	Node *LeftCenter, *DownCenter, *RightCenter, *UpCenter, *cellCenter;
	if (!c->LFace->isParent()) {
		LeftCenter = new Node(centernode[0].first, centernode[0].second);
		nodeVector.push_back(LeftCenter);
	}
	else LeftCenter = c->LFace->centerNode;

	if (!c->DFace->isParent()) {
		DownCenter = new Node(centernode[1].first, centernode[1].second);
		nodeVector.push_back(DownCenter);
	}
	else DownCenter = c->DFace->centerNode;

	if (!c->RFace->isParent()) {
		RightCenter = new Node(centernode[2].first, centernode[2].second);
		nodeVector.push_back(RightCenter);
	}
	else RightCenter = c->RFace->centerNode;

	if (!c->UFace->isParent()) {
		UpCenter = new Node(centernode[3].first, centernode[3].second);
		nodeVector.push_back(UpCenter);
	}
	else UpCenter = c->UFace->centerNode;

	cellCenter = new Node(centernode[4].first, centernode[4].second);
	nodeVector.push_back(cellCenter);

	// ���� Cell�� Node�� �����Ѵ�.
	c->setChildNodes(LeftCenter, DownCenter, RightCenter, UpCenter, cellCenter, 0);

	// ���� Face 12���� ����̴ϴ�.
	Face *LU = 0, *LD = 0, *DL = 0, *DR = 0, *RD = 0, *RU = 0;
	Face *UR = 0, *UL = 0, *IL = 0, *ID = 0, *IR = 0, *IU = 0;

	// Cell ���� �� ���� ��������� Face�� �ܺ� 8�� ���� ����
	// LD LU

	if (!c->LFace->isParent()) {
		LD = new Face(c->LDNode, LeftCenter);	faceVector.push_back(LD);
		LU = new Face(LeftCenter, c->LUNode);	faceVector.push_back(LU);
		if (updown*gradient < 0) {
			if (updown > 0) {
				detfaceVector.push_back(make_pair(LD->id, LD));
				LD = LU;
				deletenode += 2;
			}
			else {
				detfaceVector.push_back(make_pair(LU->id, LU));
				LU = LD;
				deletenode += 2;
			}
		}
		LD->setAttribute(c->LFace->attribute);
		LU->setAttribute(c->LFace->attribute);
		c->LFace->setChilds(LD, LU);
		c->LFace->setCenterNode(LeftCenter);
	}
	else {
		if (updown*gradient < 0) deletenode += 2;
		LD = c->LFace->Child1;
		LU = c->LFace->Child2;
	}

	// DL DR
	if (!c->DFace->isParent()) {
		DL = new Face(c->LDNode, DownCenter);	faceVector.push_back(DL);
		DR = new Face(DownCenter, c->RDNode);	faceVector.push_back(DR);
		if (updown > 0) {
			if (gradient > 0) {
				detfaceVector.push_back(make_pair(DR->id, DR));
				DR = DL;
				deletenode += 1;
			}
			else {
				detfaceVector.push_back(make_pair(DL->id, DL));
				DL = DR;
				deletenode += 1;
			}
		}
		DL->setAttribute(c->DFace->attribute);
		DR->setAttribute(c->DFace->attribute);
		c->DFace->setChilds(DL, DR);
		c->DFace->setCenterNode(DownCenter);
	}
	else {
		if (updown > 0) deletenode += 1;
		DL = c->DFace->Child1;
		DR = c->DFace->Child2;
	}

	// RD RU
	if (!c->RFace->isParent()) {
		RD = new Face(c->RDNode, RightCenter);	faceVector.push_back(RD);
		RU = new Face(RightCenter, c->RUNode);	faceVector.push_back(RU);
		if (gradient*updown > 0) {
			if (updown > 0) {
				detfaceVector.push_back(make_pair(RD->id, RD));
				RD = RU;
				deletenode += 7;
			}
			else {
				detfaceVector.push_back(make_pair(RU->id, RU));
				RU = RD;
				deletenode += 7;
			}
		}
		RD->setAttribute(c->RFace->attribute);
		RU->setAttribute(c->RFace->attribute);
		c->RFace->setChilds(RD, RU);
		c->RFace->setCenterNode(RightCenter);
	}
	else {
		if (gradient*updown > 0) deletenode += 7;
		RD = c->RFace->Child1;
		RU = c->RFace->Child2;
	}

	// UL UR
	if (!c->UFace->isParent()) {
		UL = new Face(c->LUNode, UpCenter);	faceVector.push_back(UL);
		UR = new Face(UpCenter, c->RUNode);	faceVector.push_back(UR);
		if (updown < 0) {
			if (gradient > 0) {
				detfaceVector.push_back(make_pair(UL->id, UL));
				UL = UR;
				deletenode += 4;
			}
			else {//���� �����κ�
				detfaceVector.push_back(make_pair(UR->id, UR));
				UR = UL;
				deletenode += 4;
			}
		}
		UL->setAttribute(c->UFace->attribute);
		UR->setAttribute(c->UFace->attribute);
		c->UFace->setChilds(UL, UR);
		c->UFace->setCenterNode(UpCenter);
	}
	else {
		if (updown < 0)deletenode += 4;
		UL = c->UFace->Child1;
		UR = c->UFace->Child2;
	}


	IL = new Face(LeftCenter, cellCenter);	IL->setAttribute(4);	IL->setDepth(UL->depth);	faceVector.push_back(IL);
	ID = new Face(DownCenter, cellCenter);	ID->setAttribute(4);	ID->setDepth(UL->depth);	faceVector.push_back(ID);
	IR = new Face(cellCenter, RightCenter);	IR->setAttribute(4);	IR->setDepth(UL->depth);	faceVector.push_back(IR);
	IU = new Face(cellCenter, UpCenter);	IU->setAttribute(4);	IU->setDepth(UL->depth);	faceVector.push_back(IU);

	// �� ���� cell�� face�� ����Ű����
	((Cell*)c->LUCell)->LFace = LU;	((Cell*)c->LUCell)->DFace = IL;	((Cell*)c->LUCell)->RFace = IU;	((Cell*)c->LUCell)->UFace = UL;
	((Cell*)c->LDCell)->LFace = LD;	((Cell*)c->LDCell)->DFace = DL;	((Cell*)c->LDCell)->RFace = ID;	((Cell*)c->LDCell)->UFace = IL;
	((Cell*)c->RDCell)->LFace = ID;	((Cell*)c->RDCell)->DFace = DR;	((Cell*)c->RDCell)->RFace = RD;	((Cell*)c->RDCell)->UFace = IR;
	((Cell*)c->RUCell)->LFace = IU;	((Cell*)c->RUCell)->DFace = IR;	((Cell*)c->RUCell)->RFace = RU;	((Cell*)c->RUCell)->UFace = UR;

	// �� Face�� startCell / endCell ���� // �� : Face�� sCell�� eCell�� CellShape�� �����Ͽ��� ������ �ؿ� �ڵ嵵 �ణ �����߽��ϴ�.  2017-01-12

	// LU, LD 
	if (deletenode != 6)LU->setEndCell(((Cell*)c->LUCell));
	if (deletenode != 3)LD->setEndCell(((Cell*)c->LDCell));
	if (c->LFace->attribute != 1) {
		if (deletenode != 6 && LU->getStartCell() == NULL)LU->setStartCell(c->LFace->sCell);
		if (deletenode != 3 && LD->getStartCell() == NULL)LD->setStartCell(c->LFace->sCell);
	}
	// DL, DR
	if (deletenode != 3)DL->setStartCell(((Cell*)c->LDCell));
	if (deletenode != 8)DR->setStartCell(((Cell*)c->RDCell));
	if (c->DFace->attribute != 3) {
		if (deletenode != 3 && DL->getEndCell() == NULL)DL->setEndCell(c->DFace->eCell);
		if (deletenode != 8 && DR->getEndCell() == NULL)DR->setEndCell(c->DFace->eCell);
	}
	// RD, RU
	if (deletenode != 8)RD->setStartCell(((Cell*)c->RDCell));
	if (deletenode != 11)RU->setStartCell(((Cell*)c->RUCell));
	if (c->RFace->attribute != 0) {
		if (deletenode != 8 && RD->getEndCell() == NULL)RD->setEndCell(c->RFace->eCell);
		if (deletenode != 11 && RU->getEndCell() == NULL)RU->setEndCell(c->RFace->eCell);
	}
	// UR, UL
	if (deletenode != 11)UR->setEndCell(((Cell*)c->RUCell));
	if (deletenode != 6)UL->setEndCell(((Cell*)c->LUCell));
	if (c->UFace->attribute != 2) {
		if (deletenode != 11 && UR->getStartCell() == NULL)UR->setStartCell(c->UFace->sCell);
		if (deletenode != 6 && UL->getStartCell() == NULL)UL->setStartCell(c->UFace->sCell);
	}
	// INNER Faces
	IL->setStartCell(((Cell*)c->LUCell));	IL->setEndCell(((Cell*)c->LDCell));
	ID->setStartCell(((Cell*)c->LDCell));	ID->setEndCell(((Cell*)c->RDCell));
	IR->setStartCell(((Cell*)c->RUCell));	IR->setEndCell(((Cell*)c->RDCell));
	IU->setStartCell(((Cell*)c->LUCell));	IU->setEndCell(((Cell*)c->RUCell));

	//������ node inout���� 2017_03_14 ��
	if (deletenode == 3) {//LD
						  //delete
		detcellVector.push_back(make_pair(c->LDCell->id, (Cell*)c->LDCell));
		//�ﰢ������
		c->LDCell = new CellTriangle(DownCenter, LeftCenter, cellCenter);
		((Cell*)c->LUCell)->DFace->eCell = c->LDCell;
		((Cell*)c->RDCell)->LFace->sCell = c->LDCell;
		minimalCellVector_tri.push_back(c->LDCell);
		wall.push_back(make_pair(make_pair(1, c->LDCell->id - 1), make_pair(c->LFace->centerNode->id - 1, c->DFace->centerNode->id - 1)));
		//�� : �߰� 2017_03_14
		LeftCenter->inout = -2;
		DownCenter->inout = -2;
	}
	else if (deletenode == 6) {//LU
							   //delete
		detcellVector.push_back(make_pair(c->LUCell->id, (Cell*)c->LUCell));
		//�ﰢ������
		c->LUCell = new CellTriangle(UpCenter, LeftCenter, cellCenter);
		((Cell*)c->LDCell)->UFace->sCell = c->LUCell;
		((Cell*)c->RUCell)->LFace->sCell = c->LUCell;
		minimalCellVector_tri.push_back(c->LUCell);
		wall.push_back(make_pair(make_pair(1, c->LUCell->id - 1), make_pair(c->UFace->centerNode->id - 1, c->LFace->centerNode->id - 1)));
		//�� : �߰� 2017_03_14
		LeftCenter->inout = -2;
		UpCenter->inout = -2;
	}
	else if (deletenode == 11) {//UR
								//delete
		detcellVector.push_back(make_pair(c->RUCell->id, (Cell*)c->RUCell));
		//�ﰢ������
		c->RUCell = new CellTriangle(UpCenter, RightCenter, cellCenter);
		((Cell*)c->RDCell)->UFace->sCell = c->RUCell;
		((Cell*)c->LUCell)->RFace->eCell = c->RUCell;
		minimalCellVector_tri.push_back(c->RUCell);
		wall.push_back(make_pair(make_pair(1, c->RUCell->id - 1), make_pair(c->RFace->centerNode->id - 1, c->UFace->centerNode->id - 1)));
		//�� : �߰� 2017_03_14
		RightCenter->inout = -2;
		UpCenter->inout = -2;
	}
	else if (deletenode == 8) {//DR
							   //delete
		detcellVector.push_back(make_pair(c->RDCell->id, (Cell*)c->RDCell));
		//�ﰢ������
		c->RDCell = new CellTriangle(UpCenter, LeftCenter, cellCenter);
		((Cell*)c->LDCell)->RFace->eCell = c->RDCell;
		((Cell*)c->RUCell)->DFace->eCell = c->RDCell;
		minimalCellVector_tri.push_back(c->RDCell);
		wall.push_back(make_pair(make_pair(1, c->RDCell->id - 1), make_pair(c->DFace->centerNode->id - 1, c->RFace->centerNode->id - 1)));
		//�� : �߰� 2017_03_14
		RightCenter->inout = -2;
		DownCenter->inout = -2;
	}

}
int cutface(pdd CenterPoint, Face* face, int child1or2) { //child1or2 = child1,2�� � ���� �츮�°�
	Node * Centernode;
	Centernode = new Node(CenterPoint.first, CenterPoint.second);
	nodeVector.push_back(Centernode);
	Face * child;
	if (child1or2 == 1) child = new Face(face->sNode, Centernode);
	else child = new Face(Centernode, face->eNode);
	faceVector.push_back(child);
	child->setAttribute(face->attribute);
	face->setChilds(child, child);
	face->setCenterNode(Centernode);
	child->setStartCell(face->getStartCell());
	child->setEndCell(face->getEndCell());
	//�߰� 2017_03_15 ��
	Centernode->inout = -2; // ������ ��
	return Centernode->id;
}
void eraseRect(Cell* c, pdd fnode, pdd snode, int wopoint) { // �簢�� ó��
															 // wopoint 1 = DFace , 2= RFace , 4=LFace , 5 = UFace
	int wallsid, walleid;
	int norf_f = mininodePlace[fnode].first;
	int norf_s = mininodePlace[snode].first;
	Face* face_f = NULL;
	Face* face_s = NULL;
	if (norf_f == 1) face_f = (Face*)mininodePlace[fnode].second;
	if (norf_s == 1) face_s = (Face*)mininodePlace[snode].second;

	//DFace�̸� Dface�� �������� ���� centerNode�� ���� �ְ� ���� �ٸ� ���� 2017_03_14 ��
	if (wopoint == 1) {//DFACE
		if (norf_f == 1) { //face�̸�
			if (!face_f->isParent()) wallsid = cutface(fnode, face_f, 2);// child�� ������ �ɰ��� �߰����� wall�� snode
			else {
				wallsid = face_f->centerNode->id; //�ɰ��������� centernode =  wall�� snode
												  //�߰� 2017_03_15 ��
				face_f->centerNode->inout = -2; // �������� ��
			}
			c->LFace->eCell = NULL;
		}
		else {// node�� �ش� node�� wall�� snode
			wallsid = ((Node*)mininodePlace[fnode].second)->id;
			//�߰� 2017_03_15 ��
			((Node*)mininodePlace[fnode].second)->inout = -2;// �������� ��
		}

		if (norf_s == 1) {
			if (!face_s->isParent()) walleid = cutface(snode, face_s, 2);
			else {
				walleid = face_s->centerNode->id;
				//�߰� 2017_03_15 ��
				face_s->centerNode->inout = -2; // �������� ��
			}
			c->RFace->sCell = NULL;
		}
		else {
			walleid = ((Node*)mininodePlace[snode].second)->id;
			//�߰� 2017_03_15 ��
			((Node*)mininodePlace[snode].second)->inout = -2;// �������� ��
		}
		c->DFace->sCell = NULL;
	}
	else if (wopoint == 2) {//RFace
		if (norf_f == 1) { //face�̸�
			if (!face_f->isParent()) wallsid = cutface(fnode, face_f, 1);// child�� ������ �ɰ��� �߰����� wall�� snode
			else {
				wallsid = face_f->centerNode->id; //�ɰ��������� centernode =  wall�� snode
												  //�߰� 2017_03_15 ��
				face_f->centerNode->inout = -2; // �������� ��
			}
			c->DFace->sCell = NULL;
		}
		else {// node�� �ش� node�� wall�� snode
			wallsid = ((Node*)mininodePlace[fnode].second)->id;
			//�߰� 2017_03_15 ��
			((Node*)mininodePlace[fnode].second)->inout = -2; // �������� ��
		}

		if (norf_s == 1) {
			if (!face_s->isParent()) walleid = cutface(snode, face_s, 1);
			else {
				walleid = face_s->centerNode->id;

				//�߰� 2017_03_15 ��
				face_s->centerNode->inout = -2; // �������� ��
			}
			c->UFace->eCell = NULL;
		}
		else {
			walleid = ((Node*)mininodePlace[snode].second)->id;
			//�߰� 2017_03_15 ��
			((Node*)mininodePlace[snode].second)->inout = -2; // �������� ��
		}
		c->RFace->sCell = NULL;
	}
	else if (wopoint == 4) {//LFace
		if (norf_f == 1) { //face�̸�
			if (!face_f->isParent()) wallsid = cutface(fnode, face_f, 2);// child�� ������ �ɰ��� �߰����� wall�� snode
			else {
				wallsid = face_f->centerNode->id; //�ɰ��������� centernode =  wall�� snode

												  //�߰� 2017_03_15 ��
				face_f->centerNode->inout = -2; // �������� ��
			}
			c->UFace->eCell = NULL;
		}
		else {// node�� �ش� node�� wall�� snode
			wallsid = ((Node*)mininodePlace[fnode].second)->id;
			//�߰� 2017_03_15 ��
			((Node*)mininodePlace[fnode].second)->inout = -2; // �������� ��
		}

		if (norf_s == 1) {
			if (!face_s->isParent()) walleid = cutface(snode, face_s, 2);
			else {
				walleid = face_s->centerNode->id;
				//�߰� 2017_03_15 ��
				face_s->centerNode->inout = -2; // �������� ��
			}
			c->DFace->sCell = NULL;
		}
		else {
			walleid = ((Node*)mininodePlace[snode].second)->id;
			//�߰� 2017_03_15 ��
			((Node*)mininodePlace[snode].second)->inout = -2; // �������� ��
		}
		c->LFace->eCell = NULL;
	}
	else if (wopoint == 5) {//UFace
		if (norf_f == 1) { //face�̸�
			if (!face_f->isParent()) wallsid = cutface(fnode, face_f, 1);// child�� ������ �ɰ��� �߰����� wall�� snode
			else {
				wallsid = face_f->centerNode->id; //�ɰ��������� centernode =  wall�� snode
												  //�߰� 2017_03_15 ��
				face_s->centerNode->inout = -2; // �������� ��
			}
			c->RFace->sCell = NULL;
		}
		else {// node�� �ش� node�� wall�� snode
			wallsid = ((Node*)mininodePlace[fnode].second)->id;
			//�߰� 2017_03_15 ��
			((Node*)mininodePlace[fnode].second)->inout = -2; // �������� ��
		}

		if (norf_s == 1) {
			if (!face_s->isParent()) walleid = cutface(snode, face_s, 1);
			else {
				walleid = face_s->centerNode->id;
				//�߰� 2017_03_15 ��
				face_s->centerNode->inout = -2; // �������� ��
			}
			c->LFace->eCell = NULL;
		}
		else {
			walleid = ((Node*)mininodePlace[snode].second)->id;
			//�߰� 2017_03_15 ��
			((Node*)mininodePlace[snode].second)->inout = -2; // �������� ��
		}
		c->UFace->eCell = NULL;
	}
	else cout << "Eroor : eraseRect() " << endl;

	wall.push_back(make_pair(make_pair(0, c->id - 1), make_pair(wallsid - 1, walleid - 1)));
}

void eraseTri(Cell* c, pdd fnode, pdd snode, int wopoint) {//�ﰢ��ó��
														   //wopoint 0 = LUnode ,1 = RUnode , 2 = LDnode , 3 = RDnode
	int norf_f = mininodePlace[fnode].first;
	int norf_s = mininodePlace[snode].first;
	//���� �簢cell ����
	detcellVector.push_back(make_pair(c->id, c));

	if (wopoint == 0) {//LU
					   //�ﰢ���� HFace, VFace ������ HO,VO ����
		Node* LeftCenter;	Node* UPCenter;
		Face* LeftFace;		Face* UpFace;
		if (norf_f == 1) {
			if (!c->LFace->isParent()) { //child �����ٸ� �ɰ���.
				LeftCenter = new Node(fnode.first, fnode.second);
				nodeVector.push_back(LeftCenter);
				Face * LU = new Face(LeftCenter, c->LUNode);
				LU->setAttribute(c->LFace->attribute);
				LU->setStartCell(c->LFace->sCell);
				faceVector.push_back(LU);
				c->LFace->setChilds(LU, LU);
				c->LFace->setCenterNode(LeftCenter);
				LeftFace = ((Cell*)c->LFace->sCell)->RFace->Child1;
			}
			else {
				LeftCenter = c->LFace->centerNode;
				LeftFace = c->LFace->Child2;
			}
		}
		else {
			LeftCenter = c->LDNode;
			LeftFace = c->LFace;
		}

		if (norf_s == 1) {
			if (!c->UFace->isParent()) { //child �����ٸ� �ɰ���.
				UPCenter = new Node(snode.first, snode.second);
				nodeVector.push_back(UPCenter);
				Face * UL = new Face(c->LUNode, UPCenter);
				UL->setAttribute(c->UFace->attribute);
				UL->setStartCell(c->UFace->sCell);
				faceVector.push_back(UL);
				c->UFace->setChilds(UL, UL);
				c->UFace->setCenterNode(UPCenter);
				UpFace = ((Cell*)c->UFace->sCell)->DFace->Child1;
			}
			else {
				UPCenter = c->UFace->centerNode;
				UpFace = c->UFace->Child1;
			}
		}
		else {
			UPCenter = c->RUNode;
			UpFace = c->UFace;

		}
		// �ﰢ������
		CellTriangle* tri = new CellTriangle(LeftCenter, UPCenter, c->LUNode);
		minimalCellVector_tri.push_back(tri);
		LeftFace->eCell = tri;
		UpFace->eCell = tri;
		wall.push_back(make_pair(make_pair(1, tri->id - 1), make_pair(LeftCenter->id - 1, UPCenter->id - 1)));
		//�߰� 2017_03_15 ��
		tri->HONode->inout = -2;
		tri->VONode->inout = -2;
	}
	else if (wopoint == 1) {//RU
							//�ﰢ���� HFace, VFace ������ HO,VO ����
		Node* RightCenter;	Node* UPCenter;
		Face* RightFace;		Face* UpFace;
		if (norf_f == 1) {
			if (!c->UFace->isParent()) { //child �����ٸ� �ɰ���.
				UPCenter = new Node(fnode.first, fnode.second);
				nodeVector.push_back(UPCenter);
				Face * UR = new Face(UPCenter, c->RUNode);
				UR->setAttribute(c->UFace->attribute);
				UR->setStartCell(c->UFace->sCell);
				faceVector.push_back(UR);
				c->UFace->setChilds(UR, UR);
				c->UFace->setCenterNode(UPCenter);
				UpFace = ((Cell*)c->UFace->sCell)->DFace->Child1;
			}
			else {
				UPCenter = c->UFace->centerNode;
				UpFace = c->UFace->Child2;
			}
		}
		else {
			UPCenter = c->LUNode;
			UpFace = c->UFace;
		}

		if (norf_s == 1) {
			if (!c->RFace->isParent()) { //child �����ٸ� �ɰ���.
				RightCenter = new Node(snode.first, snode.second);
				nodeVector.push_back(RightCenter);
				Face * RU = new Face(RightCenter, c->RUNode);
				RU->setAttribute(c->RFace->attribute);
				RU->setEndCell(c->RFace->eCell);
				faceVector.push_back(RU);
				c->RFace->setChilds(RU, RU);
				c->RFace->setCenterNode(RightCenter);
				RightFace = ((Cell*)c->RFace->eCell)->LFace->Child1;
			}
			else {
				RightCenter = c->RFace->centerNode;
				RightFace = c->RFace->Child2;
			}
		}
		else {
			RightCenter = c->RDNode;
			RightFace = c->RFace;

		}
		// �ﰢ������
		CellTriangle* tri = new CellTriangle(RightCenter, UPCenter, c->RUNode);
		minimalCellVector_tri.push_back(tri);
		RightFace->sCell = tri;
		UpFace->eCell = tri;
		wall.push_back(make_pair(make_pair(1, tri->id - 1), make_pair(UPCenter->id - 1, RightCenter->id - 1)));
		//�߰� 2017_03_15 ��
		tri->HONode->inout = -2;
		tri->VONode->inout = -2;
	}
	else if (wopoint == 2) {//LD
							//�ﰢ���� HFace, VFace ������ HO,VO ����
		Node* LeftCenter;	Node* DownCenter;
		Face* LeftFace;		Face* DownFace;
		if (norf_f == 1) {
			if (!c->DFace->isParent()) { //child �����ٸ� �ɰ���.
				DownCenter = new Node(fnode.first, fnode.second);
				nodeVector.push_back(DownCenter);
				Face * DL = new Face(c->LDNode, DownCenter);
				DL->setAttribute(c->DFace->attribute);
				DL->setEndCell(c->DFace->eCell);
				faceVector.push_back(DL);
				c->DFace->setChilds(DL, DL);
				c->DFace->setCenterNode(DownCenter);
				DownFace = ((Cell*)c->DFace->eCell)->UFace->Child1;
			}
			else {
				DownCenter = c->DFace->centerNode;
				DownFace = c->DFace->Child1;
			}
		}
		else {
			DownCenter = c->RDNode;
			DownFace = c->DFace;
		}

		if (norf_s == 1) {
			if (!c->LFace->isParent()) { //child �����ٸ� �ɰ���.
				LeftCenter = new Node(snode.first, snode.second);
				nodeVector.push_back(LeftCenter);
				Face * LD = new Face(c->LDNode, LeftCenter);
				LD->setAttribute(c->LFace->attribute);
				LD->setStartCell(c->LFace->sCell);
				faceVector.push_back(LD);
				c->LFace->setChilds(LD, LD);
				c->LFace->setCenterNode(LeftCenter);
				LeftFace = ((Cell*)c->LFace->sCell)->RFace->Child1;
			}
			else {
				LeftCenter = c->LFace->centerNode;
				LeftFace = c->LFace->Child1;
			}
		}
		else {
			LeftCenter = c->LUNode;
			LeftFace = c->LFace;

		}
		// �ﰢ������
		CellTriangle* tri = new CellTriangle(LeftCenter, DownCenter, c->LDNode);
		minimalCellVector_tri.push_back(tri);
		LeftFace->eCell = tri;
		DownFace->sCell = tri;
		wall.push_back(make_pair(make_pair(1, tri->id - 1), make_pair(DownCenter->id - 1, LeftCenter->id - 1)));
		//�߰� 2017_03_15 ��
		tri->HONode->inout = -2;
		tri->VONode->inout = -2;
	}
	else if (wopoint == 3) {//RD
							//�ﰢ���� HFace, VFace ������ HO,VO ����
		Node* RightCenter;	Node* DownCenter;
		Face* RightFace;	Face* DownFace;
		if (norf_f == 1) {
			if (!c->RFace->isParent()) { //child �����ٸ� �ɰ���.
				RightCenter = new Node(fnode.first, fnode.second);
				nodeVector.push_back(RightCenter);
				Face * RD = new Face(c->RDNode, RightCenter);
				RD->setAttribute(c->RFace->attribute);
				RD->setEndCell(c->RFace->eCell);
				faceVector.push_back(RD);
				c->RFace->setChilds(RD, RD);
				c->RFace->setCenterNode(RightCenter);
				RightFace = ((Cell*)c->RFace->eCell)->LFace->Child1;
			}
			else {
				RightCenter = c->RFace->centerNode;
				RightFace = c->RFace->Child1;
			}
		}
		else {
			RightCenter = c->RUNode;
			RightFace = c->RFace;
		}

		if (norf_s == 1) {
			if (!c->DFace->isParent()) { //child �����ٸ� �ɰ���.
				DownCenter = new Node(snode.first, snode.second);
				nodeVector.push_back(DownCenter);
				Face * DR = new Face(DownCenter, c->RDNode);
				DR->setAttribute(c->DFace->attribute);
				DR->setEndCell(c->DFace->eCell);
				faceVector.push_back(DR);
				c->DFace->setChilds(DR, DR);
				c->DFace->setCenterNode(DownCenter);
				DownFace = ((Cell*)c->DFace->eCell)->UFace->Child1;
			}
			else {
				DownCenter = c->DFace->centerNode;
				DownFace = c->DFace->Child2;
			}
		}
		else {
			DownCenter = c->LDNode;
			DownFace = c->DFace;

		}
		// �ﰢ������
		CellTriangle* tri = new CellTriangle(RightCenter, DownCenter, c->RDNode);
		minimalCellVector_tri.push_back(tri);
		RightFace->sCell = tri;
		DownFace->sCell = tri;
		wall.push_back(make_pair(make_pair(1, tri->id - 1), make_pair(RightCenter->id - 1, DownCenter->id - 1)));
		//�߰� 2017_03_15 ��
		tri->HONode->inout = -2;
		tri->VONode->inout = -2;
	}
	else cout << "Error : eraseTRI()" << endl;

}

bool isshape(pdd fnode, pdd snode, int updown, double mincellsize) {//��_01_22 : ����Ǻ� �Լ� up = 1, down=-1
	Cell * c = (Cell *)mininodeCell[fnode][snode];
	if (c->getiscut() == true) {
		//�̹��ɰ��� �簢��
		printf(" \n >>>>Please increase the depth of this programe <<<<< \n \t : There is two of line in one minicell ! \n ");
		return false;
	}
	else {
		if (c != NULL) { //�����̾ null��ȯ�� �ƴѰ��
			vector<Node*> cnode;
			cnode.push_back(c->getLUNode());
			cnode.push_back(c->getRUNode());
			cnode.push_back(c->getLDNode());
			cnode.push_back(c->getRDNode());
			double gradient = (fnode.second - snode.second) / (fnode.first - snode.first);
			int wopoint = 0;
			int fpoint = 0;
			int ccwresult[3] = { 0,0,0 };
			for (int i = 0; i < 4; i++) {
				//cout << "node i : " << cnode[i]->x << " " << cnode[i]->y << endl;
				double r = ccw(fnode, make_pair(cnode[i]->x, cnode[i]->y), snode);
				//cout << "r : " << r << endl;
				if (r == 0) {
					ccwresult[1]++;
					fpoint += i;
				}
				else if (updown*r > 0) ccwresult[0]++;
				else {
					ccwresult[2]++;
					wopoint += i;
				}
			}
			//cout << "ccw result : " << ccwresult[0] << " " << ccwresult[1] << " " << ccwresult[2] << endl;
			if (ccwresult[0] == 1 && ccwresult[1] == 0 && ccwresult[2] == 3) { //������
				cuting(c, fnode, snode, mincellsize, updown, gradient);
				c->setiscut(true);
				//cout << " ������ " << endl;
				return true;
			}
			else if (ccwresult[2] == 2 && (ccwresult[0] + ccwresult[1] == 2)) {//�簢��
				if (updown == 1) eraseRect(c, fnode, snode, wopoint);
				else eraseRect(c, snode, fnode, wopoint);
				c->setiscut(true);
				//cout << " �簢�� " << endl;
				return true;
			}
			else if (ccwresult[2] == 1 && (ccwresult[0] + ccwresult[1] == 3)) {//�ﰢ��
				if (updown == 1) eraseTri(c, fnode, snode, wopoint);
				else eraseTri(c, snode, fnode, wopoint);
				c->setiscut(true);
				//cout << " �ﰢ�� " << endl;
				return true;
			}
			else if (ccwresult[2] == 0 && ccwresult[1] == 2 && ccwresult[0] == 2) {//����
				if (fpoint == 1) {//uface
					if (updown == -1) {
						Face* wf = new Face(c->RUNode, c->LUNode);
						wf->setStartCell(c);
						wf->setEndCell(NULL);
						wf->setAttribute(c->UFace->attribute);
						faceVector.push_back(wf);
						c->UFace->Child1 = wf;
						c->UFace->Child2 = wf;
						//�߰� ���� 03_23
						c->RUNode->inout = -2;
						c->LUNode->inout = -2;
					}
					else {
						Face* wf = new Face(c->LUNode, c->RUNode);
						wf->setStartCell(c->UFace->sCell);
						wf->setEndCell(NULL);
						wf->setAttribute(c->UFace->attribute);
						faceVector.push_back(wf);
						c->UFace->Child1 = wf;
						c->UFace->Child2 = wf;
						//�߰� ���� 03_23
						c->RUNode->inout = -2;
						c->LUNode->inout = -2;
					}

					return true;
				}
				else if (fpoint == 5) {//dface
					if (updown == -1) {
						Face* wf = new Face(c->LDNode, c->RDNode);
						wf->setStartCell(c->DFace->eCell);
						wf->setEndCell(NULL);
						wf->setAttribute(c->UFace->attribute);
						faceVector.push_back(wf);
						c->DFace->Child1 = wf;
						c->DFace->Child2 = wf;
						//�߰� ���� 03_23
						c->RUNode->inout = -2;
						c->LUNode->inout = -2;
					}
					else {
						Face* wf = new Face(c->RDNode, c->LDNode);
						wf->setStartCell(c);
						wf->setEndCell(NULL);
						wf->setAttribute(c->UFace->attribute);
						faceVector.push_back(wf);
						c->DFace->Child1 = wf;
						c->DFace->Child2 = wf;
						//�߰� ���� 03_23
						c->RUNode->inout = -2;
						c->LUNode->inout = -2;
					}
					return true;
				}
				else {
					cout << "error: fpoint " << endl;
				}
			}
			else printf("error : ccwresult �Ǻ� ����");
		}
		else {
			cout << "error: ccwresult null point " << endl;
		}
	}
	return false;
}
/*void checkInner(Cell* c, pdd p1, pdd p2) { // �� : Cell�� Node���� �������� ������� ��ġ�� Ȯ���ϰ� face�� �����Ѵ�. 2017_02_05
// �� : ������ �̷�� ���е��� �ݽð�������� �־��� 2017_02_11
int flag;
if (c->LUNode->inout == 2 || c->LUNode->inout == -1) { // �� : Node�� Ȯ�ε��� ���� �͸� Ȯ�� 2017_02_05
flag = ccwAddition(p1, p2, make_pair(c->LUNode->getX(), c->LUNode->getY()));
if (flag == 1)
c->LUNode->inout = -1;
else if (flag == -1)
c->LUNode->inout = 1;
else
c->LUNode->inout = 0;
}
if (c->LDNode->inout == 2 || c->LDNode->inout == -1) {
flag = ccwAddition(p1, p2, make_pair(c->LDNode->getX(), c->LDNode->getY()));
if (flag == 1)
c->LDNode->inout = -1;
else if (flag == -1)
c->LDNode->inout = 1;
else
c->LDNode->inout = 0;
}
if (c->RUNode->inout == 2 || c->RUNode->inout == -1) {
flag = ccwAddition(p1, p2, make_pair(c->RUNode->getX(), c->RUNode->getY()));
if (flag == 1)
c->RUNode->inout = -1;
else if (flag == -1)
c->RUNode->inout = 1;
else
c->RUNode->inout = 0;
}
if (c->RDNode->inout == 2 || c->RDNode->inout == -1) {
flag = ccwAddition(p1, p2, make_pair(c->RDNode->getX(), c->RDNode->getY()));
if (flag == 1)
c->RDNode->inout = -1;
else if (flag == -1)
c->RDNode->inout = 1;
else
c->RDNode->inout = 0;
}
}*/

void checkInner(double LeftDownX, double LeftDownY, double initRCellsize, int Xcount, int Ycount, bool flag) {
	//nodeVector�� ����, begin()�� ���� �ֻ��
	sort(nodeVector.begin(), nodeVector.end(), nodeCompareXY);
	vector <Node*>::iterator it, itT;
	double RightUpX = LeftDownX + (Xcount * initRCellsize);
	double RightUpY = LeftDownY + (Ycount * initRCellsize);
	int inout = 4;
	/* attribute
	-2: line (������)
	-1: inner (������ ����)
	0 : outflow  (�� ������)
	1 : inlet    (�� ����)
	2 : top      (�� ��)
	3 : bottom   (�� �Ʒ�)
	4 : interrior(�߰� Node��)
	5 : unknown (��Ȯ��)
	*/
	itT = nodeVector.end();
	for (it = nodeVector.begin(); it < nodeVector.end(); it++) {
		// �����ڸ� ó��

		if (fabs((*it)->getY() - LeftDownY) < S_N) {
			(*it)->inout = 3; // bottom
		}
		else if (fabs((*it)->getY() - RightUpY) < S_N) {
			(*it)->inout = 2; // top
		}
		else if (fabs((*it)->getX() - LeftDownX) < S_N) {
			(*it)->inout = 1; // inlet
		}
		else if (fabs((*it)->getX() - RightUpX) < S_N) {
			(*it)->inout = 0; // outlet
		}

		/* initCell�� face ó��
		if (flag) {
		if ((*it)->getX() == airpoile.begin()->first && (*it)->getY() == airpoile.begin()->second)
		inout = -2;
		}*/

		if ((*it)->getInout() == 5)
			(*it)->inout = 4;
		else if ((*it)->getInout() == 3)
			itT = nodeVector.end();
		else if ((*it)->getInout() == -2) {
			if (itT == nodeVector.end())
				itT = it;
			else {
				itT++;
				for (itT; itT < nodeVector.end(); itT++) {
					if (itT == it)
						break;
					else {
						(*itT)->inout = -1;
					}
				}
				itT = nodeVector.end();
			}
		}

	}
}
void deleteInnerFace() {
	// �� : face ���� 2017_02_05 // ��輱�� ���� �ǵ帮�� �ʴ´�.

	/* test_jae_detface : fluent���� #f error �߻�
	vector<Face*>::iterator itf; // �� : ���� 2017_02_19

	for (itf = faceVector.begin(); itf < faceVector.end(); itf++) {
	if ((*itf)->attribute == 4) {
	if ((*itf)->eCell != NULL && (*itf)->sCell != NULL) {
	if ((*itf)->eNode->inout == -1 && (*itf)->sNode->inout == -1 && (*itf)->eCell->getID() > initCellMaxID && (*itf)->sCell->getID() > initCellMaxID) {
	detfaceVector.push_back(make_pair((*itf)->getID(), (*itf)));
	//
	if ((*itf)->isChild()) {
	if ((*itf)->curParent->Child1 == (*itf))
	(*itf)->curParent->Child1 = NULL;
	if ((*itf)->curParent->Child2 == (*itf))
	(*itf)->curParent->Child2 = NULL;
	}//

	}
	if ((*itf)->eNode->inout == -1 && (*itf)->sNode->inout == 0 && (*itf)->eCell->getID() > initCellMaxID && (*itf)->sCell->getID() > initCellMaxID) {
	detfaceVector.push_back(make_pair((*itf)->getID(), (*itf)));
	//
	if ((*itf)->isChild()) {
	if ((*itf)->curParent->Child1 == (*itf))
	(*itf)->curParent->Child1 = NULL;
	if ((*itf)->curParent->Child2 == (*itf))
	(*itf)->curParent->Child2 = NULL;
	}//
	}
	if ((*itf)->eNode->inout == 0 && (*itf)->sNode->inout == -1 && (*itf)->eCell->getID() > initCellMaxID && (*itf)->sCell->getID() > initCellMaxID) {
	detfaceVector.push_back(make_pair((*itf)->getID(), (*itf)));
	//
	if ((*itf)->isChild()) {
	if ((*itf)->curParent->Child1 == (*itf))
	(*itf)->curParent->Child1 = NULL;
	if ((*itf)->curParent->Child2 == (*itf))
	(*itf)->curParent->Child2 = NULL;
	}//
	}
	}
	}
	}
	*/

	/* test_jae_0_0
	vector<Face*>::iterator itf; // �� : ���� 2017_02_19
	for (itf = faceVector.begin(); itf < faceVector.end(); itf++) {
	if ((*itf)->eNode->inout == -1 && (*itf)->sNode->inout == -1) {
	(*itf)->eCell == NULL;
	(*itf)->sCell == NULL;
	}
	if ((*itf)->eNode->inout == -1 && (*itf)->sNode->inout == 0) {
	(*itf)->eCell == NULL;
	(*itf)->sCell == NULL;
	}
	if ((*itf)->eNode->inout == 0 && (*itf)->sNode->inout == -1) {
	(*itf)->eCell == NULL;
	(*itf)->sCell == NULL;
	}
	}*/

	vector<Cell*>::iterator itc;
	for (itc = cellVector.begin(); itc < cellVector.end(); itc++) {
		/*
		if ((*itc)->RUNode->inout == 0 || (*itc)->RUNode->inout == -1) { // �� : ���� 2017_02_28
		if ((*itc)->LUNode->inout == 0 || (*itc)->LUNode->inout == -1) {
		if ((*itc)->LDNode->inout == 0 || (*itc)->LDNode->inout == -1) {
		if ((*itc)->RDNode->inout == 0 || (*itc)->RDNode->inout == -1) {
		// �� : initCell�� �����ؼ� �ȵǴ� ������ ���� 2017_02_19
		if ((*itc)->getID() > initCellMaxID)
		detcellVector.push_back(make_pair((*itc)->getID(), (*itc)));
		}
		}
		}
		}
		*
		if ((*itc)->RUNode->inout != 1 && (*itc)->LUNode->inout != 1 && (*itc)->LDNode->inout != 1 && (*itc)->RDNode->inout != 1) {
		if ((*itc)->RUNode->inout == 2 || (*itc)->LUNode->inout == 2 || (*itc)->LDNode->inout == 2 || (*itc)->RDNode->inout == 2)
		cout << "error_checkInner_inout_2\n";
		if ((*itc)->getID() > initCellMaxID)
		detcellVector.push_back(make_pair((*itc)->getID(), (*itc)));
		}*/
		/* attribute
		-2: line (������)
		-1: inner (������ ����)
		0 : outflow  (�� ������)
		1 : inlet    (�� ����)
		2 : top      (�� ��)
		3 : bottom   (�� �Ʒ�)
		4 : interrior(�߰� Node��, ������ �ٱ�)
		5 : unknown (��Ȯ��)
		*/
		if (((*itc)->RUNode->inout == -2 || (*itc)->RUNode->inout == -1) && ((*itc)->RDNode->inout == -2 || (*itc)->RDNode->inout == -1) && ((*itc)->LUNode->inout == -2 || (*itc)->LUNode->inout == -1) && ((*itc)->LDNode->inout == -2 || (*itc)->LDNode->inout == -1)) {
			//if ( (*itc)->RUNode->inout == -1 &&(*itc)->RDNode->inout == -1 && (*itc)->LUNode->inout == -1 &&  (*itc)->LDNode->inout == -1) {
			if ((*itc)->RUNode->inout == 5 || (*itc)->LUNode->inout == 5 || (*itc)->LDNode->inout == 5 || (*itc)->RDNode->inout == 5)
				cout << "error_checkInner_inout_5\n";
			if ((*itc)->getID() > initCellMaxID)
				detcellVector.push_back(make_pair((*itc)->getID(), (*itc)));
			//else				detinitcellVector.push_back((*itc)->getID());
		}
	}
}

/*
void cutInnerCell() {
// �� : ��� Cell�� ���� ���� ���� face ���� �۾�, ��輱�� ���� cut-Cell �۾��� ���� ���� ��Ų �� ���۵Ǿ���� 2017_02_08
vector<Cell*>::iterator it;
vector<pair<pdd, pdd> >::iterator itPdd;
for (it = cellVector.begin(); it < cellVector.end(); it++) {
for (itPdd = inputLine.begin(); itPdd < inputLine.end(); itPdd++) {
checkInner(*it, itPdd->first, itPdd->second);
}
}
deleteInnerFace();
}*/
bool manageEdge(vector<pdd> airpoile, double LeftDownX, double LeftDownY, double initRCellsize, int depth, int layer, double mincellsize) { // �� : ���κ� ó��
	cout << "manageEdge ����\n";
	//���κ��� x�� ��
	bool flag = false;
	Node *NB, *NE;
	vector<pdd>::iterator itB = airpoile.begin();
	vector<pdd>::iterator itE = airpoile.end();
	itE--;
	NB = new Node(itB->first, itB->second);
	NE = new Node(itE->first, itE->second);

	if (NB->getX() != NE->getX()) {
		cout << "edge�� X���� ���� ��ġ���� �ʽ��ϴ�.\n";
		return flag;
	}

	//cutInnerCell�� ���� airpolie �߰� manageEdge�� cutInnerCell���� �տ��� ���� �̺κ��� ���� ������
	inputLine.push_back(make_pair(*(itE), *(itB)));

	if (NB->getY() != NE->getY()) {
		cout << "manageEdge �ش�\n";
		//initCell�� face�� �ɸ� ���
		if (NB->getX() - (NB->getX() / initRCellsize) == 0) {
			cout << "initCell�� face�� �ɸ� ���\n";
			//splitCell - �̻��Ȳ ������ fluent�� Ȯ�� �ʿ�
			initRectangle->splitCell(NE->getX(), NE->getY(), NB->getX(), NB->getY(), layer, depth, depth);
			//inout = -2 �۾� // InnerCheck���� ����
			flag = true;
			//wall�߰�
			Face * wf = new Face(NB, NE);
			wallVector.push_back(wf);
		}

		//tailcutManager�� ��� �� ���̿� �����簢���� �ش� ���� ���� ���
		else if ((NB->getY() - NE->getY()) < mincellsize)
			cout << "tailcutManager�� ����\n";
		//���� �簢��
		else {
			cout << "���� �簢��\n";
			//mininode3 ���� - �������� ������
			//findmininode ����, updown�� 2
			findminnode(NE->getX(), NE->getY(), NB->getX(), NB->getY(), mincellsize, 2);
			//dotPoint �� setupmininodeCell
			if (!mininode[2].empty()) {
				for (int k = 0; k < mininode[2].size(); k++) {
					initRectangle->dotPoint(mininode[2][k].first, mininode[2][k].second, layer, depth);
					if (k != 0) {
						setupmininodeCell(mininode[2][k - 1], mininode[2][k]);
					}
				}
			}
			//isshapeȣ��, bool isshape()
			if (mininode[2].size() > 2) {
				for (int k = 0; k < mininode[2].size(); k++) {
					if (!isshape(mininode[2][k - 1], mininode[2][k], -1, mincellsize)) return flag;
				}
			}
		}
	}
	else
		cout << "manageEdge �ش����\n";
	return flag;

}
void tailcut_rect(Cell* c, pdd inode, pdd cnode, bool updown, int type) {// �� : tail ����  2017-03-07
																		 /*
																		 type 1 = Lface , type 2 = Uface , type 3 = Dface
																		 */

	c->LUCell = new Cell();	((Cell*)c->LUCell)->setDepth(c->depth + 1);
	c->LDCell = new Cell();	((Cell*)c->LDCell)->setDepth(c->depth + 1);
	c->RDCell = new Cell();	((Cell*)c->RDCell)->setDepth(c->depth + 1);
	c->RUCell = new Cell();	((Cell*)c->RUCell)->setDepth(c->depth + 1);
	cellVector.push_back(((Cell*)c->LUCell));	cellVector.push_back(((Cell*)c->LDCell));
	cellVector.push_back(((Cell*)c->RDCell));	cellVector.push_back(((Cell*)c->RUCell));

	// ���� ��������� Node�� ���� �����Ѵ�.
	//0 : left , 1: down 2: right, 3: up 4: center
	Node *LeftCenter, *DownCenter, *RightCenter, *UpCenter, *cellCenter;
	if (!c->LFace->isParent()) {
		if (type == 1) LeftCenter = new Node(inode.first, inode.second);
		else LeftCenter = new Node(c->LDNode->x, cnode.second);
		nodeVector.push_back(LeftCenter);
	}
	else LeftCenter = c->LFace->centerNode;

	if (!c->DFace->isParent()) {
		if (type == 3) DownCenter = new Node(inode.first, inode.second);
		else DownCenter = new Node(cnode.first, c->LDNode->y);
		nodeVector.push_back(DownCenter);
	}
	else DownCenter = c->DFace->centerNode;

	if (!c->RFace->isParent()) {
		RightCenter = new Node(c->RDNode->x, cnode.second);
		nodeVector.push_back(RightCenter);
	}
	else RightCenter = c->RFace->centerNode;

	if (!c->UFace->isParent()) {
		if (type == 2) UpCenter = new Node(inode.first, inode.second);
		else UpCenter = new Node(cnode.first, c->LUNode->y);
		nodeVector.push_back(UpCenter);
	}
	else UpCenter = c->UFace->centerNode;

	cellCenter = new Node(cnode.first, cnode.second);
	nodeVector.push_back(cellCenter);

	// ���� Cell�� Node�� �����Ѵ�.
	c->setChildNodes(LeftCenter, DownCenter, RightCenter, UpCenter, cellCenter, 0);

	//������ child �簢���� ���Ѵ�
	int deletecell_num = 0;
	if ((updown && type == 1) || (!updown && type == 3)) {//LDcell����
		deletecell_num = 1;
	}
	else if ((!updown&& type == 1) || (updown && type == 2)) {//LUcell����
		deletecell_num = 2;
	}
	else if (!updown && type == 2) {//RUcell����
		deletecell_num = 3;
	}
	else if (updown && type == 3) {//RDcell����
		deletecell_num = 4;
	}

	// ���� Face 12���� ����̴ϴ�.
	Face *LU = 0, *LD = 0, *DL = 0, *DR = 0, *RD = 0, *RU = 0;
	Face *UR = 0, *UL = 0, *IL = 0, *ID = 0, *IR = 0, *IU = 0;

	// Cell ���� �� ���� ��������� Face�� �ܺ� 8�� ���� ����
	// LD LU

	if (!c->LFace->isParent()) {
		LD = new Face(c->LDNode, LeftCenter);	faceVector.push_back(LD);
		LU = new Face(LeftCenter, c->LUNode);	faceVector.push_back(LU);
		if (deletecell_num == 1) {
			detfaceVector.push_back(make_pair(LD->id, LD));
			LD = LU;
		}
		else if (deletecell_num == 2) {
			detfaceVector.push_back(make_pair(LU->id, LU));
			LU = LD;
		}
		LD->setAttribute(c->LFace->attribute);
		LU->setAttribute(c->LFace->attribute);
		c->LFace->setChilds(LD, LU);
		c->LFace->setCenterNode(LeftCenter);
	}
	else {
		LD = c->LFace->Child1;
		LU = c->LFace->Child2;
	}

	// DL DR
	if (!c->DFace->isParent()) {
		DL = new Face(c->LDNode, DownCenter);	faceVector.push_back(DL);
		DR = new Face(DownCenter, c->RDNode);	faceVector.push_back(DR);
		if (deletecell_num == 1) {
			detfaceVector.push_back(make_pair(DL->id, DL));
			DL = DR;
		}
		else if (deletecell_num == 4) {
			detfaceVector.push_back(make_pair(DR->id, DR));
			DR = DL;
		}
		DL->setAttribute(c->DFace->attribute);
		DR->setAttribute(c->DFace->attribute);
		c->DFace->setChilds(DL, DR);
		c->DFace->setCenterNode(DownCenter);
	}
	else {
		DL = c->DFace->Child1;
		DR = c->DFace->Child2;
	}

	// RD RU
	if (!c->RFace->isParent()) {
		RD = new Face(c->RDNode, RightCenter);	faceVector.push_back(RD);
		RU = new Face(RightCenter, c->RUNode);	faceVector.push_back(RU);
		if (deletecell_num == 3) {
			detfaceVector.push_back(make_pair(RU->id, RU));
			RU = RD;
		}
		else if (deletecell_num == 4) {
			detfaceVector.push_back(make_pair(RD->id, RD));
			RD = RU;
		}
		RD->setAttribute(c->RFace->attribute);
		RU->setAttribute(c->RFace->attribute);
		c->RFace->setChilds(RD, RU);
		c->RFace->setCenterNode(RightCenter);
	}
	else {
		RD = c->RFace->Child1;
		RU = c->RFace->Child2;
	}

	// UL UR
	if (!c->UFace->isParent()) {
		UL = new Face(c->LUNode, UpCenter);	faceVector.push_back(UL);
		UR = new Face(UpCenter, c->RUNode);	faceVector.push_back(UR);
		if (deletecell_num == 2) {
			detfaceVector.push_back(make_pair(UL->id, UL));
			UL = UR;
		}
		else if (deletecell_num == 3) {
			detfaceVector.push_back(make_pair(UR->id, UR));
			UR = UL;
		}
		UL->setAttribute(c->UFace->attribute);
		UR->setAttribute(c->UFace->attribute);
		c->UFace->setChilds(UL, UR);
		c->UFace->setCenterNode(UpCenter);
	}
	else {
		UL = c->UFace->Child1;
		UR = c->UFace->Child2;
	}


	IL = new Face(LeftCenter, cellCenter);	IL->setAttribute(4);	IL->setDepth(UL->depth);	faceVector.push_back(IL);
	ID = new Face(DownCenter, cellCenter);	ID->setAttribute(4);	ID->setDepth(UL->depth);	faceVector.push_back(ID);
	IR = new Face(cellCenter, RightCenter);	IR->setAttribute(4);	IR->setDepth(UL->depth);	faceVector.push_back(IR);
	IU = new Face(cellCenter, UpCenter);	IU->setAttribute(4);	IU->setDepth(UL->depth);	faceVector.push_back(IU);

	// �� ���� cell�� face�� ����Ű����
	((Cell*)c->LUCell)->LFace = LU;	((Cell*)c->LUCell)->DFace = IL;	((Cell*)c->LUCell)->RFace = IU;	((Cell*)c->LUCell)->UFace = UL;
	((Cell*)c->LDCell)->LFace = LD;	((Cell*)c->LDCell)->DFace = DL;	((Cell*)c->LDCell)->RFace = ID;	((Cell*)c->LDCell)->UFace = IL;
	((Cell*)c->RDCell)->LFace = ID;	((Cell*)c->RDCell)->DFace = DR;	((Cell*)c->RDCell)->RFace = RD;	((Cell*)c->RDCell)->UFace = IR;
	((Cell*)c->RUCell)->LFace = IU;	((Cell*)c->RUCell)->DFace = IR;	((Cell*)c->RUCell)->RFace = RU;	((Cell*)c->RUCell)->UFace = UR;

	// �� Face�� startCell / endCell ���� // �� : Face�� sCell�� eCell�� CellShape�� �����Ͽ��� ������ �ؿ� �ڵ嵵 �ణ �����߽��ϴ�.  2017-01-12

	// LU, LD 
	if (updown) LU->setEndCell(((Cell*)c->LUCell));
	else LD->setEndCell(((Cell*)c->LDCell));
	if (c->LFace->attribute != 1) {
		if (deletecell_num != 2 && LU->getStartCell() == NULL)LU->setStartCell(c->LFace->sCell);
		if (deletecell_num != 1 && LD->getStartCell() == NULL)LD->setStartCell(c->LFace->sCell);
	}
	// DL, DR
	if (!updown) DL->setStartCell(((Cell*)c->LDCell));
	DR->setStartCell(((Cell*)c->RDCell));
	if (c->DFace->attribute != 3) {
		if (deletecell_num != 1 && DL->getEndCell() == NULL)DL->setEndCell(c->DFace->eCell);
		if (deletecell_num != 4 && DR->getEndCell() == NULL)DR->setEndCell(c->DFace->eCell);
	}
	// RD, RU
	RD->setStartCell(((Cell*)c->RDCell));
	RU->setStartCell(((Cell*)c->RUCell));
	if (c->RFace->attribute != 0) {
		if (deletecell_num != 4 && RD->getEndCell() == NULL)RD->setEndCell(c->RFace->eCell);
		if (deletecell_num != 3 && RU->getEndCell() == NULL)RU->setEndCell(c->RFace->eCell);
	}
	// UR, UL
	UR->setEndCell(((Cell*)c->RUCell));
	if (updown) UL->setEndCell(((Cell*)c->LUCell));
	if (c->UFace->attribute != 2) {
		if (deletecell_num != 3 && UR->getStartCell() == NULL)UR->setStartCell(c->UFace->sCell);
		if (deletecell_num != 2 && UL->getStartCell() == NULL)UL->setStartCell(c->UFace->sCell);
	}
	// INNER Faces
	IL->setStartCell(((Cell*)c->LUCell));	IL->setEndCell(((Cell*)c->LDCell));
	ID->setStartCell(((Cell*)c->LDCell));	ID->setEndCell(((Cell*)c->RDCell));
	IR->setStartCell(((Cell*)c->RUCell));	IR->setEndCell(((Cell*)c->RDCell));
	IU->setStartCell(((Cell*)c->LUCell));	IU->setEndCell(((Cell*)c->RUCell));

	if (deletecell_num == 1) {
		detcellVector.push_back(make_pair(c->LDCell->id, (Cell*)c->LDCell));
		wall.push_back(make_pair(make_pair(0, c->LUCell->id - 1), make_pair(LeftCenter->id - 1, cellCenter->id - 1)));
		wall.push_back(make_pair(make_pair(0, c->RDCell->id - 1), make_pair(cellCenter->id - 1, DownCenter->id - 1)));
	}
	else if (deletecell_num == 2) {
		detcellVector.push_back(make_pair(c->LUCell->id, (Cell*)c->LUCell));
		wall.push_back(make_pair(make_pair(0, c->LDCell->id - 1), make_pair(cellCenter->id - 1, LeftCenter->id - 1)));
		wall.push_back(make_pair(make_pair(0, c->RUCell->id - 1), make_pair(UpCenter->id - 1, cellCenter->id - 1)));
	}
	else if (deletecell_num == 3) {
		detcellVector.push_back(make_pair(c->RUCell->id, (Cell*)c->RUCell));
		wall.push_back(make_pair(make_pair(0, c->RDCell->id - 1), make_pair(RightCenter->id - 1, cellCenter->id - 1)));
		wall.push_back(make_pair(make_pair(0, c->LUCell->id - 1), make_pair(cellCenter->id - 1, UpCenter->id - 1)));
	}
	else if (deletecell_num == 4) {
		detcellVector.push_back(make_pair(c->RDCell->id, (Cell*)c->RDCell));
		wall.push_back(make_pair(make_pair(0, c->LDCell->id - 1), make_pair(DownCenter->id - 1, cellCenter->id - 1)));
		wall.push_back(make_pair(make_pair(0, c->RUCell->id - 1), make_pair(cellCenter->id - 1, RightCenter->id - 1)));
	}
	else {
		printf("Error : tail_cut_rect - deletenum : %d \n", deletecell_num);
	}
}
void tailcut_tri(Cell* c, pdd inode, pdd cnode, bool updown, int type) {// �� : tail ����  2017-03-07
																		/*
																		type 1 = RU ,RDCell�� �����ϴ� ���
																		type 2 = LD, RDCell�� �����ϴ� ���
																		type 3 = LU, RUCell�� �����ϴ� ���
																		type 4 = 1�� ���� ������ �̹� �ɰ�������(UFace�� Dface��)
																		*/

	if (type == 3) {
		c->LUCell = new Cell();	((Cell*)c->LUCell)->setDepth(c->depth + 1); cellVector.push_back(((Cell*)c->LUCell));
	}
	if (type == 2) {
		c->LDCell = new Cell();	((Cell*)c->LDCell)->setDepth(c->depth + 1); cellVector.push_back(((Cell*)c->LDCell));
	}
	if (type != 3) {
		c->RDCell = new Cell();	((Cell*)c->RDCell)->setDepth(c->depth + 1); cellVector.push_back(((Cell*)c->RDCell));
	}
	if (type != 2) {
		c->RUCell = new Cell();	((Cell*)c->RUCell)->setDepth(c->depth + 1); cellVector.push_back(((Cell*)c->RUCell));
	}

	// ���� ��������� Node�� ���� �����Ѵ�.
	//0 : left , 1: down 2: right, 3: up 4: center
	Node *LeftCenter = NULL, *DownCenter = NULL, *RightCenter = NULL, *UpCenter = NULL, *cellCenter = NULL;
	if (type == 2 || type == 3) {
		if (!c->LFace->isParent()) {
			LeftCenter = new Node(c->LDNode->x, cnode.second);
			nodeVector.push_back(LeftCenter);
		}
		else LeftCenter = c->LFace->centerNode;
	}
	if (type != 3) {
		if (!c->DFace->isParent()) {
			DownCenter = new Node(cnode.first, c->LDNode->y);
			nodeVector.push_back(DownCenter);
		}
		else DownCenter = c->DFace->centerNode;
	}

	if (!c->RFace->isParent()) {
		RightCenter = new Node(c->RDNode->x, cnode.second);
		nodeVector.push_back(RightCenter);
	}
	else RightCenter = c->RFace->centerNode;

	if (type != 2) {
		if (!c->UFace->isParent()) {
			UpCenter = new Node(cnode.first, c->LUNode->y);
			nodeVector.push_back(UpCenter);
		}
		else UpCenter = c->UFace->centerNode;
	}

	cellCenter = new Node(cnode.first, cnode.second);
	nodeVector.push_back(cellCenter);

	// ���� Cell�� Node�� �����Ѵ�.
	c->setChildNodes(LeftCenter, DownCenter, RightCenter, UpCenter, cellCenter, type);

	// ���� Face 12���� ����̴ϴ�.
	Face *LU = 0, *LD = 0, *DL = 0, *DR = 0, *RD = 0, *RU = 0;
	Face *UR = 0, *UL = 0, *IL = 0, *ID = 0, *IR = 0, *IU = 0;

	// Cell ���� �� ���� ��������� Face�� �ܺ� 8�� ���� ����
	// LD LU

	if (type != 1 && type != 4) {
		if (!c->LFace->isParent()) {
			LD = new Face(c->LDNode, LeftCenter);	faceVector.push_back(LD);
			LU = new Face(LeftCenter, c->LUNode);	faceVector.push_back(LU);
			if (type == 3 && !updown) {
				detfaceVector.push_back(make_pair(LD->id, LD));
				LD = LU;
			}
			else if (type == 2 && updown) {
				detfaceVector.push_back(make_pair(LU->id, LU));
				LU = LD;
			}
			LD->setAttribute(c->LFace->attribute);
			LU->setAttribute(c->LFace->attribute);
			c->LFace->setChilds(LD, LU);
			c->LFace->setCenterNode(LeftCenter);
		}
		else {
			LD = c->LFace->Child1;
			LU = c->LFace->Child2;
		}
	}

	// DL DR
	if (type != 3) {
		if (!c->DFace->isParent()) {
			DL = new Face(c->LDNode, DownCenter);	faceVector.push_back(DL);
			DR = new Face(DownCenter, c->RDNode);	faceVector.push_back(DR);
			if (type == 1 && updown) {
				detfaceVector.push_back(make_pair(DL->id, DL));
				DL = DR;
			}
			DL->setAttribute(c->DFace->attribute);
			DR->setAttribute(c->DFace->attribute);
			c->DFace->setChilds(DL, DR);
			c->DFace->setCenterNode(DownCenter);
		}
		else {
			DL = c->DFace->Child1;
			DR = c->DFace->Child2;
		}
	}
	// RD RU
	if (!c->RFace->isParent()) {
		RD = new Face(c->RDNode, RightCenter);	faceVector.push_back(RD);
		RU = new Face(RightCenter, c->RUNode);	faceVector.push_back(RU);
		if (type == 2 && !updown) {
			detfaceVector.push_back(make_pair(RU->id, RU));
			RU = RD;
		}
		else if (type == 3 && updown) {
			detfaceVector.push_back(make_pair(RD->id, RD));
			RD = RU;
		}
		RD->setAttribute(c->RFace->attribute);
		RU->setAttribute(c->RFace->attribute);
		c->RFace->setChilds(RD, RU);
		c->RFace->setCenterNode(RightCenter);
	}
	else {
		RD = c->RFace->Child1;
		RU = c->RFace->Child2;
	}

	// UL UR
	if (type != 2) {
		if (!c->UFace->isParent()) {
			UL = new Face(c->LUNode, UpCenter);	faceVector.push_back(UL);
			UR = new Face(UpCenter, c->RUNode);	faceVector.push_back(UR);
			if (type == 1 && !updown) {
				detfaceVector.push_back(make_pair(UL->id, UL));
				UL = UR;
			}
			UL->setAttribute(c->UFace->attribute);
			UR->setAttribute(c->UFace->attribute);
			c->UFace->setChilds(UL, UR);
			c->UFace->setCenterNode(UpCenter);
		}
		else {
			UL = c->UFace->Child1;
			UR = c->UFace->Child2;
		}
	}


	if (type != 1 && type != 4) { IL = new Face(LeftCenter, cellCenter);	IL->setAttribute(4);	IL->setDepth(RD->depth);	faceVector.push_back(IL); }
	if (type != 3) { ID = new Face(DownCenter, cellCenter);	ID->setAttribute(4);	ID->setDepth(RD->depth);	faceVector.push_back(ID); }
	IR = new Face(cellCenter, RightCenter);	IR->setAttribute(4);	IR->setDepth(RD->depth);	faceVector.push_back(IR);
	if (type != 2) { IU = new Face(cellCenter, UpCenter);	IU->setAttribute(4);	IU->setDepth(RD->depth);	faceVector.push_back(IU); }



	// �� ���� cell�� face�� ����Ű����
	if (type == 3) { ((Cell*)c->LUCell)->LFace = LU;	((Cell*)c->LUCell)->DFace = IL;	((Cell*)c->LUCell)->RFace = IU;	((Cell*)c->LUCell)->UFace = UL; }
	if (type == 2) { ((Cell*)c->LDCell)->LFace = LD;	((Cell*)c->LDCell)->DFace = DL;	((Cell*)c->LDCell)->RFace = ID;	((Cell*)c->LDCell)->UFace = IL; }
	if (type != 3) { ((Cell*)c->RDCell)->LFace = ID;	((Cell*)c->RDCell)->DFace = DR;	((Cell*)c->RDCell)->RFace = RD;	((Cell*)c->RDCell)->UFace = IR; }
	if (type != 2) { ((Cell*)c->RUCell)->LFace = IU;	((Cell*)c->RUCell)->DFace = IR;	((Cell*)c->RUCell)->RFace = RU;	((Cell*)c->RUCell)->UFace = UR; }

	// �� Face�� startCell / endCell ���� // �� : Face�� sCell�� eCell�� CellShape�� �����Ͽ��� ������ �ؿ� �ڵ嵵 �ణ �����߽��ϴ�.  2017-01-12

	// LU, LD 
	if (type != 1 && type != 4) {
		if (type == 3 || !updown) LU->setEndCell(((Cell*)c->LUCell));
		if (type == 2 || updown) LD->setEndCell(((Cell*)c->LDCell));
		if (c->LFace->attribute != 1) {
			if ((type == 3 || !updown) && LU->getStartCell() == NULL)LU->setStartCell(c->LFace->sCell);
			if ((type == 2 || updown > 0) && LD->getStartCell() == NULL)LD->setStartCell(c->LFace->sCell);
		}
	}
	// DL, DR
	if (type != 3) {
		if (type != 4 && (type == 2 || !updown)) DL->setStartCell(((Cell*)c->LDCell));
		DR->setStartCell(((Cell*)c->RDCell));
		if (c->DFace->attribute != 3) {
			if ((type != 4 && (type == 2 || !updown)) && DL->getEndCell() == NULL)DL->setEndCell(c->DFace->eCell);
			if (DR->getEndCell() == NULL)DR->setEndCell(c->DFace->eCell);
		}
	}
	// RD, RU
	if (!(type == 3 && updown)) RD->setStartCell(((Cell*)c->RDCell));
	if (!(type == 2 && !updown)) RU->setStartCell(((Cell*)c->RUCell));
	if (c->RFace->attribute != 0) {
		if (!(type == 3 && updown) && RD->getEndCell() == NULL)RD->setEndCell(c->RFace->eCell);
		if (!(type == 3 && updown) && RU->getEndCell() == NULL)RU->setEndCell(c->RFace->eCell);
	}
	// UR, UL
	if (type != 2) {
		UR->setEndCell(((Cell*)c->RUCell));
		if (type != 4 && (type == 2 || updown)) UL->setEndCell(((Cell*)c->LUCell));
		if (c->UFace->attribute != 2) {
			if (UR->getStartCell() == NULL)UR->setStartCell(c->UFace->sCell);
			if ((type != 4 && (type == 2 || updown)) && UL->getStartCell() == NULL)UL->setStartCell(c->UFace->sCell);
		}
	}

	// INNER Faces
	if (type == 3) {
		IL->setStartCell(((Cell*)c->LUCell));
		IU->setStartCell(((Cell*)c->LUCell));
	}
	if (type == 2) {
		IL->setEndCell(((Cell*)c->LDCell));
		ID->setStartCell(((Cell*)c->LDCell));
	}
	if (type != 3) {
		ID->setEndCell(((Cell*)c->RDCell));
		IR->setEndCell(((Cell*)c->RDCell));
	}
	if (type != 2) {
		IR->setStartCell(((Cell*)c->RUCell));
		IU->setEndCell(((Cell*)c->RUCell));
	}
	if (type == 1) {
		if (updown) {
			//�ﰢ������
			c->LUCell = new CellTriangle(cellCenter, c->LUNode, UpCenter);
			c->UFace->Child1->setEndCell(c->LUCell);
			IU->setStartCell(c->LUCell);
			minimalCellVector_tri.push_back(c->LUCell);
			wall.push_back(make_pair(make_pair(1, c->LUCell->id - 1), make_pair(c->LUNode->id - 1, cellCenter->id - 1)));
			wall.push_back(make_pair(make_pair(0, c->RDCell->id - 1), make_pair(cellCenter->id - 1, DownCenter->id - 1)));
			detfaceVector.push_back(make_pair(ID->id, ID));

		}
		else {
			c->LDCell = new CellTriangle(cellCenter, c->LDNode, DownCenter);
			c->DFace->Child1->setStartCell(c->LDCell);
			ID->setStartCell(c->LDCell);
			minimalCellVector_tri.push_back(c->LDCell);
			wall.push_back(make_pair(make_pair(1, c->LDCell->id - 1), make_pair(cellCenter->id - 1, c->LDNode->id - 1)));
			wall.push_back(make_pair(make_pair(0, c->RUCell->id - 1), make_pair(UpCenter->id - 1, cellCenter->id - 1)));
			detfaceVector.push_back(make_pair(IU->id, IU));
		}
		c->LFace->Child1 = ID;
		c->LFace->Child2 = IU;
		c->LFace->centerNode = cellCenter;
	}
	else if (type == 2) {
		if (!updown) {
			//�ﰢ������
			c->LUCell = new CellTriangle(c->LUNode, cellCenter, LeftCenter);
			c->LFace->Child2->setEndCell(c->LUCell);
			IL->setStartCell(c->LUCell);
			minimalCellVector_tri.push_back(c->LUCell);
			wall.push_back(make_pair(make_pair(1, c->LUCell->id - 1), make_pair(cellCenter->id - 1, c->LUNode->id - 1)));
			wall.push_back(make_pair(make_pair(0, c->RDCell->id - 1), make_pair(RightCenter->id - 1, cellCenter->id - 1)));
			detfaceVector.push_back(make_pair(IR->id, IR));
		}
		else {
			c->RUCell = new CellTriangle(c->RUNode, cellCenter, RightCenter);
			c->RFace->Child2->setStartCell(c->RUCell);
			IR->setStartCell(c->RUCell);
			minimalCellVector_tri.push_back(c->RUCell);
			wall.push_back(make_pair(make_pair(1, c->RUCell->id - 1), make_pair(c->RUNode->id - 1, cellCenter->id - 1)));
			wall.push_back(make_pair(make_pair(0, c->LDCell->id - 1), make_pair(cellCenter->id - 1, LeftCenter->id - 1)));
			detfaceVector.push_back(make_pair(IL->id, IL));
		}
		c->UFace->Child1 = IL;
		c->UFace->Child2 = IR;
		c->UFace->centerNode = cellCenter;
	}
	else if (type == 3) {
		if (updown) {
			//�ﰢ������
			c->LDCell = new CellTriangle(c->LDNode, cellCenter, LeftCenter);
			c->LFace->Child1->setEndCell(c->LDCell);
			IL->setEndCell(c->LDCell);
			minimalCellVector_tri.push_back(c->LDCell);
			wall.push_back(make_pair(make_pair(1, c->LDCell->id - 1), make_pair(c->LDNode->id - 1, cellCenter->id - 1)));
			wall.push_back(make_pair(make_pair(0, c->RUCell->id - 1), make_pair(cellCenter->id - 1, RightCenter->id - 1)));
			detfaceVector.push_back(make_pair(IR->id, IR));
		}
		else {
			c->RDCell = new CellTriangle(c->RDNode, cellCenter, RightCenter);
			c->RFace->Child1->setStartCell(c->RDCell);
			IR->setEndCell(c->RDCell);
			minimalCellVector_tri.push_back(c->RDCell);
			wall.push_back(make_pair(make_pair(1, c->RDCell->id - 1), make_pair(c->RDNode->id - 1, cellCenter->id - 1)));
			wall.push_back(make_pair(make_pair(0, c->LUCell->id - 1), make_pair(cellCenter->id - 1, LeftCenter->id - 1)));
			detfaceVector.push_back(make_pair(IL->id, IL));
		}
		c->DFace->Child1 = IL;
		c->DFace->Child2 = IR;
		c->DFace->centerNode = cellCenter;
	}
	else if (type == 4) {
		c->LFace->Child1 = ID;
		c->LFace->Child2 = IU;
		c->LFace->centerNode = cellCenter;
		//wall ó���Ұ��� ����
	}
	else {
		printf("Error : tail_cut_tri_ num : %d \n", type);
	}
}
void tailcutfour(Cell* c, pdd point) {//lface�ε��´ٴ� �����Ͽ�����
	c->LUCell = new Cell();	((Cell*)c->LUCell)->setDepth(c->depth + 1);
	c->LDCell = new Cell();	((Cell*)c->LDCell)->setDepth(c->depth + 1);
	c->RDCell = new Cell();	((Cell*)c->RDCell)->setDepth(c->depth + 1);
	c->RUCell = new Cell();	((Cell*)c->RUCell)->setDepth(c->depth + 1);
	cellVector.push_back(((Cell*)c->LUCell));	cellVector.push_back(((Cell*)c->LDCell));
	cellVector.push_back(((Cell*)c->RDCell));	cellVector.push_back(((Cell*)c->RUCell));

	// ���� ��������� Node�� ���� �����Ѵ�.
	Node *LeftCenter, *DownCenter, *RightCenter, *UpCenter, *cellCenter;
	LeftCenter = c->LFace->centerNode;

	if (!c->DFace->isParent()) {
		DownCenter = new Node(point.first, c->LDNode->y);
		nodeVector.push_back(DownCenter);
	}
	else {
		DownCenter = c->DFace->centerNode;
	}
	if (!c->RFace->isParent()) {
		RightCenter = new Node(c->RDNode->x, point.second);
		nodeVector.push_back(RightCenter);
	}
	else {
		RightCenter = c->RFace->centerNode;
	}
	if (!c->UFace->isParent()) {
		UpCenter = new Node(point.first, c->LUNode->y);
		nodeVector.push_back(UpCenter);
	}
	else {
		UpCenter = c->UFace->centerNode;
	}
	cellCenter = new Node(point.first, point.second);
	nodeVector.push_back(cellCenter);

	// ���� Cell�� Node�� �����Ѵ�.
	c->setChildNodes(LeftCenter, DownCenter, RightCenter, UpCenter, cellCenter, 0);

	// ���� Face 12���� ����̴ϴ�.
	Face *LU = 0, *LD = 0, *DL = 0, *DR = 0, *RD = 0, *RU = 0;
	Face *UR = 0, *UL = 0, *IL = 0, *ID = 0, *IR = 0, *IU = 0;

	// Cell ���� �� ���� ��������� Face�� �ܺ� 8�� ���� ����
	// LD LU
	LD = c->LFace->Child1;
	LU = c->LFace->Child2;
	// DL DR
	if (!c->DFace->isParent()) {
		DL = new Face(c->LDNode, DownCenter);	faceVector.push_back(DL);
		DR = new Face(DownCenter, c->RDNode);	faceVector.push_back(DR);
		DL->setAttribute(c->DFace->attribute);
		DR->setAttribute(c->DFace->attribute);
		c->DFace->setChilds(DL, DR);
		c->DFace->setCenterNode(DownCenter);
	}
	else {
		DL = c->DFace->Child1;
		DR = c->DFace->Child2;
	}
	// RD RU
	if (!c->RFace->isParent()) {
		RD = new Face(c->RDNode, RightCenter);	faceVector.push_back(RD);
		RU = new Face(RightCenter, c->RUNode);	faceVector.push_back(RU);
		RD->setAttribute(c->RFace->attribute);
		RU->setAttribute(c->RFace->attribute);
		c->RFace->setChilds(RD, RU);
		c->RFace->setCenterNode(RightCenter);
	}
	else {
		RD = c->RFace->Child1;
		RU = c->RFace->Child2;
	}
	// UL UR
	if (!c->UFace->isParent()) {
		UL = new Face(c->LUNode, UpCenter);	faceVector.push_back(UL);
		UR = new Face(UpCenter, c->RUNode);	faceVector.push_back(UR);
		UL->setAttribute(c->UFace->attribute);
		UR->setAttribute(c->UFace->attribute);
		c->UFace->setChilds(UL, UR);
		c->UFace->setCenterNode(UpCenter);
	}
	else {
		UL = c->UFace->Child1;
		UR = c->UFace->Child2;
	}

	IL = new Face(LeftCenter, cellCenter);	IL->setAttribute(4);	IL->setDepth(UL->depth);	faceVector.push_back(IL);
	ID = new Face(DownCenter, cellCenter);	ID->setAttribute(4);	ID->setDepth(UL->depth);	faceVector.push_back(ID);
	IR = new Face(cellCenter, RightCenter);	IR->setAttribute(4);	IR->setDepth(UL->depth);	faceVector.push_back(IR);
	IU = new Face(cellCenter, UpCenter);	IU->setAttribute(4);	IU->setDepth(UL->depth);	faceVector.push_back(IU);

	// �� ���� cell�� face�� ����Ű����
	((Cell*)c->LUCell)->LFace = LU;	((Cell*)c->LUCell)->DFace = IL;	((Cell*)c->LUCell)->RFace = IU;	((Cell*)c->LUCell)->UFace = UL;
	((Cell*)c->LDCell)->LFace = LD;	((Cell*)c->LDCell)->DFace = DL;	((Cell*)c->LDCell)->RFace = ID;	((Cell*)c->LDCell)->UFace = IL;
	((Cell*)c->RDCell)->LFace = ID;	((Cell*)c->RDCell)->DFace = DR;	((Cell*)c->RDCell)->RFace = RD;	((Cell*)c->RDCell)->UFace = IR;
	((Cell*)c->RUCell)->LFace = IU;	((Cell*)c->RUCell)->DFace = IR;	((Cell*)c->RUCell)->RFace = RU;	((Cell*)c->RUCell)->UFace = UR;

	// �� Face�� startCell / endCell ���� // �� : Face�� sCell�� eCell�� CellShape�� �����Ͽ��� ������ �ؿ� �ڵ嵵 �ణ �����߽��ϴ�.  2017-01-12

	const int OUTFLOW = 0, INLET = 1, TOP = 2, BOTTOM = 3, INTERRIOR = 4;

	// DL, DR
	DL->setStartCell(((Cell*)c->LDCell));	DR->setStartCell(((Cell*)c->RDCell));
	if (c->DFace->attribute != BOTTOM) {
		if (((Cell*)(c->DFace->eCell))->LUCell == NULL) {
			DL->setEndCell(c->DFace->eCell);
			DR->setEndCell(c->DFace->eCell);
		}
		else {
			DL->setEndCell(((Cell*)(c->DFace->eCell))->LUCell);
			DR->setEndCell(((Cell*)(c->DFace->eCell))->RUCell);
		}
	}
	// RD, RU
	RD->setStartCell(((Cell*)c->RDCell));	RU->setStartCell(((Cell*)c->RUCell));
	if (c->RFace->attribute != OUTFLOW) {
		if (((Cell*)(c->RFace->eCell))->LUCell == NULL) {
			RD->setEndCell(c->RFace->eCell);
			RU->setEndCell(c->RFace->eCell);
		}
		else {
			RD->setEndCell(((Cell*)(c->RFace->eCell))->LDCell);
			RU->setEndCell(((Cell*)(c->RFace->eCell))->LUCell);
		}
	}
	// UR, UL
	UR->setEndCell(((Cell*)c->RUCell));	UL->setEndCell(((Cell*)c->LUCell));
	if (c->UFace->attribute != TOP) {
		if (((Cell*)(c->UFace->sCell))->LUCell == NULL) {
			UR->setStartCell(c->UFace->sCell);
			UL->setStartCell(c->UFace->sCell);
		}
		else {
			UR->setStartCell(((Cell*)(c->UFace->sCell))->RDCell);
			UL->setStartCell(((Cell*)(c->UFace->sCell))->LDCell);
		}
	}
	// INNER Faces
	IL->setStartCell(((Cell*)c->LUCell));	IL->setEndCell(((Cell*)c->LDCell));
	ID->setStartCell(((Cell*)c->LDCell));	ID->setEndCell(((Cell*)c->RDCell));
	IR->setStartCell(((Cell*)c->RUCell));	IR->setEndCell(((Cell*)c->RDCell));
	IU->setStartCell(((Cell*)c->LUCell));	IU->setEndCell(((Cell*)c->RUCell));
}
void tailmanager(pdd startnode, pdd endnode, double mincellsize, int layer, int depth) {// �� : tail ����  2017-03-07
	pdd inode1 = mininode[0][mininode[0].size() - 1];
	pdd inode2 = mininode[1][mininode[1].size() - 1];

	if (inode1 != startnode) {
		pair<int, void*> temp = mininodePlace[inode1];
		if (temp.first == 0) {//node
			node_of_cell cs = ((Node*)temp.second)->findtailCell(startnode);
			if (cs.first != 0) {
				if (startnode.first == endnode.first) tailcut_tri((Cell*)cs.second, inode1, startnode, true, 1);
				else if (cs.first % 2) tailcut_tri((Cell*)cs.second, inode1, startnode, true, 2);
				else tailcut_tri((Cell*)cs.second, inode1, startnode, true, 3);
			}
		}
		else {//face
			Face *f = (Face*)mininodePlace[inode1].second;
			Cell * c = (Cell*)f->eCell;
			if (f == c->LFace) tailcut_rect(c, inode1, startnode, true, 1);
			else if (f == c->UFace) {
				if (startnode.second == endnode.second) tailcut_rect(c, inode1, startnode, true, 2); // ���������� ������ ����
				else tailcut_tri(c, inode1, startnode, true, 4);
			}
			else printf("error tail!\n");
		}
	}
	if (inode2 != endnode) {
		pair<int, void*> temp = mininodePlace[inode2];
		if (temp.first == 0) {//node
			node_of_cell cs = ((Node*)temp.second)->findtailCell(endnode);
			if (cs.first != 0) {
				if (startnode.first == endnode.first) tailcut_tri((Cell*)cs.second, inode2, endnode, false, 1);
				else if (cs.first % 2) tailcut_tri((Cell*)cs.second, inode2, endnode, false, 2);
				else tailcut_tri((Cell*)cs.second, inode2, endnode, false, 3);
			}
		}
		else {//face
			Face *f = (Face*)mininodePlace[inode2].second;
			Cell * c = (Cell*)f->eCell;
			/*
			if (f->getStartCell() == NULL) c = (Cell*)f->eCell;
			else c = (Cell*)f->sCell;*/
			if (f == c->LFace) tailcut_rect(c, inode2, endnode, false, 1);
			else if (f == c->DFace) {
				if (startnode.second == endnode.second) tailcut_rect(c, inode2, endnode, false, 3); // ���������� ������ ����
				else tailcut_tri(c, inode2, endnode, false, 4);
			}
			else printf("error tail!\n");
		}
	}

	//�����κ� ó��
	vector<Node*> tail_node;
	vector<pdi> result;
	for (int i = 0; i < nodeVector.size(); i++) {
		if (nodeVector[i]->x == startnode.first) 	result.push_back(make_pair(nodeVector[i]->y, i));
	}

	for (int i = findn(endnode.second, mincellsize, 0); i <= findn(startnode.second, mincellsize, 1); i++) {
		double y = i*mincellsize;
		if (y != endnode.second && y != startnode.second) {
			bool exist = false;
			for (int k = 0; k < result.size(); k++) {
				if (result[k].first == y) {
					tail_node.push_back(nodeVector[result[k].second]);
					initRectangle->dotPoint(startnode.first, y, layer, depth);
					exist = true;
					// �߰� 03_25
					nodeVector[result[k].second]->inout = -2;

				}
			}
			if (!exist) {
				Node * n = new Node(startnode.first, y);
				nodeVector.push_back(n);
				initRectangle->dotPoint(n->x, n->y, layer, depth);
				tail_node.push_back(n);
				// �߰� 03_25
				n->inout = -2;
			}
		}

	}


	if (inode1 == startnode && inode2 == endnode && inode1.first == inode2.first) {//����
		pair<int, void*> temp = mininodePlace[inode1];
		if (temp.first == 1) {
			Face * f = (Face *)temp.second;
			if (f->isParent()) {
				Cell * c = (Cell*)f->eCell;
				Face *nf = new Face(f->centerNode, f->sNode);
				nf->setAttribute(f->attribute);
				nf->setDepth(f->depth + 1);
				nf->sCell = f->eCell;
				f->Child1 = nf;
				faceVector.push_back(nf);
				tailcutfour(c, make_pair((c->LUNode->x + c->RUNode->x) / 2, f->centerNode->y));
				f->Child1->sCell = c->LDCell;
				f->Child2->eCell = c->LUCell;
			}
		}
		temp = mininodePlace[inode2];
		if (temp.first == 1) {
			Face * f = (Face *)temp.second;
			if (f->isParent()) {
				Cell * c = (Cell*)f->eCell;
				Face *nf = new Face(f->eNode, f->centerNode);
				nf->setAttribute(f->attribute);
				nf->setDepth(f->depth + 1);
				nf->sCell = f->eCell;
				f->Child2 = nf;
				faceVector.push_back(nf);
				tailcutfour(c, make_pair((c->LUNode->x + c->RUNode->x) / 2, f->centerNode->y));
				f->Child2->sCell = c->LUCell;
				f->Child1->eCell = c->LDCell;
			}
		}

	}

	else {

		for (int i = 1; i < tail_node.size(); i++) {
			pdd prev = make_pair(tail_node[i - 1]->x, tail_node[i - 1]->y);
			pdd nxt = make_pair(tail_node[i]->x, tail_node[i]->y);
			setupmininodeCell(prev, nxt);
			Cell * c = (Cell*)mininodeCell[prev][nxt];
			eraseRect(c, nxt, prev, 4);
		}
	}
}
bool cmp_vertical(const pdd &a, const pdd &b) {
	if (a.second == b.second) return a.first < b.first;
	return a.second < b.second;
}
int main() {
	double LeftDownX = -2, LeftDownY = -2;
	int layer = 2, depth = 10; //��
	double mincellsize = 0; //��
	int Xcount = 16, Ycount = 16; //��_initcellsize����
	double initRCellsize = 0.5; //��_initcellsize����
	FILE * inputfp;
	int n, uppern, lowern, tail; // �� inputfile �о�ͼ� ù�� ����
	double x, y; // ��ǥ
	vector<pdd> airpoile;
	vector<pdd> airpoile2;
	bool vertical_horizontal = false; // true = vertical , false = horizontal 
	//�� : �߰� : �̰� vertical_horizontal ��� false�� ä�� �����ȴ�. �� � ���� ���͵� horizontal�� �ν�
	bool y_increase;

	// �Է����� �о airpoile�� ����
	if ((inputfp = fopen("input.txt", "r")) == NULL) {
		cout << "input file�� ã���� �����ϴ� " << endl;
		return 0;
	}

	fscanf(inputfp, "%d %d %d %d", &n, &uppern, &lowern, &tail);
	for (int i = 0; i < n; i++) {
		fscanf(inputfp, "%lf %lf", &x, &y);
		airpoile.push_back(make_pair(x, y));
		airpoile2.push_back(make_pair(x + 2, y));
	}

	//�ʱ�ȭ
	initRectangle = new Rectangle(LeftDownX, LeftDownY, initRCellsize, Xcount, Ycount);//��_initcellsize����
	initCellMaxID = initRectangle->initCell[initRectangle->widthCount - 1][initRectangle->heightCount - 1].getID();
	mincellsize = initRectangle->initCellSize / pow((double)2, (double)depth); // �� : ���� ���� 2017_02_20
																			   // splitcell
																			   // initRectangle->splitCell(-2, -2, 3, 3, 1, 2, depth);
																			   // cout << "PHASE : splitcell �Ϸ�" << endl;
																			   // mininode ã��
	for (int i = 0; i < 1; i++) {
		//�ʱ�ȭ
		if (i != 0) {
			airpoile = airpoile2;
			mininode[0].clear();
			mininode[1].clear();
			inputLine.clear();
		}

		int ud = 0;

		double length_prev = 0;
		int index = 0;
		for (int i = 0; i < n - 1; i++) {
			if (i == uppern - 1) {
				index = 0;
				ud = 1;
			}
			findminnode(airpoile[i].first, airpoile[i].second, airpoile[i + 1].first, airpoile[i + 1].second, mincellsize, ud);
			inputLine.push_back(make_pair(airpoile[i], airpoile[i + 1]));
		}

		cout << "PHASE : findmininode �Ϸ�" << endl;

		// vertical ���� horizontal ���� Ȯ��
		if (vertical_horizontal) { // �������̸�
			sort(mininode[0].begin(), mininode[0].end(), cmp_vertical);
			sort(mininode[1].begin(), mininode[1].end(), cmp_vertical);
			y_increase = airpoile[0].second < airpoile[uppern - 1].second;
		}

		//dotpoint �� mininodecell ã��
		for (int i = 0; i < 2; i++) {
			if (!mininode[i].empty()) {
				for (int k = 0; k < mininode[i].size(); k++) {
					initRectangle->dotPoint(mininode[i][k].first, mininode[i][k].second, layer, depth);
					if (k != 0) {
						setupmininodeCell(mininode[i][k - 1], mininode[i][k]);
					}
				}
			}
		}
		cout << "PHASE : setupmininodeCell �Ϸ�" << endl;
		// isshape �ϸ鼭 delete�� �ع����� seupmininodecell�� �������ϴ� ��찡 ���ܼ� �и��� 
		if (!vertical_horizontal) {
			for (int i = 0; i < 2; i++) {
				if (!mininode[i].empty()) {
					for (int k = 1; k < mininode[i].size(); k++) {
						if (i == 0) {
							if (!isshape(mininode[i][k - 1], mininode[i][k], 1, mincellsize)) return 0;
						}
						else {
							if (!isshape(mininode[i][k - 1], mininode[i][k], -1, mincellsize)) return 0;
						}
					}
				}
			}
		}
		else {
			for (int i = 0; i < 2; i++) {
				if (!mininode[i].empty()) {
					for (int k = 1; k < mininode[i].size(); k++) {
						if ((y_increase && i == 1) || (!y_increase && i == 0)) {
							if (!isshape(mininode[i][k - 1], mininode[i][k], 1, mincellsize)) return 0;
						}
						else {
							if (!isshape(mininode[i][k - 1], mininode[i][k], -1, mincellsize)) return 0;
						}
					}
				}
			}
		}
		cout << "PHASE : Isshape �Ϸ�" << endl;

		if (tail == 1) { // �� : tail ����  2017-03-07
			tailmanager(airpoile[0], airpoile[n - 1], mincellsize, layer, depth);

			cout << "tailcut �Ϸ�" << endl;
		}
	} // �� : ������ : vertical_horizontal�� ��� false�� ä�� �����ȴ�. �߰��� �Ǵ��� ���� ��ȭ�� ����.
	//cutInnerCell();
	bool flag = false;
	cout << "PHASE : manageEdge �Ϸ�" << endl;

	// ��: 2017_03_10 cutInnerCell ccw�˰��� ���� �� vertical check �۾� �߰�
	// �� : cutInnerCell ����, ccw�˰��� ����, checkInner ����,  
	checkInner(LeftDownX, LeftDownY, initRCellsize, Xcount, Ycount, flag);

	sort(nodeVector.begin(), nodeVector.end(), nodeCompare); // 2017_03_16 ��
	deleteInnerFace();
	cout << "PHASE : cutinnetcell �Ϸ�" << endl;

	sort(detfaceVector.begin(), detfaceVector.end());

	vector<detface>::iterator pos = unique(detfaceVector.begin(), detfaceVector.end()); // �� : detfaceVector�� face�� �ߺ��Ǽ� ���� ������ �߻��Ͽ� �ڵ� �߰�
	detfaceVector.erase(pos, detfaceVector.end());										// �� : �ߺ��Ǵ� ���� ���� // 2017_02_13

	for (int i = detfaceVector.size() - 1; i >= 0; i--) {
		delete(detfaceVector[i].second);
	}

	for (int i = 0; i < wall.size(); i++) {
		Face* wf = new Face(nodeVector[wall[i].second.first], nodeVector[wall[i].second.second]);
		if (wall[i].first.first == 0) wf->sCell = cellVector[wall[i].first.second];
		if (wall[i].first.first == 1) wf->sCell = minimalCellVector_tri[wall[i].first.second];
		wallVector.push_back(wf);
	}

	sort(detcellVector.begin(), detcellVector.end());

	vector<detcell>::iterator posC = unique(detcellVector.begin(), detcellVector.end()); // �� : detcellVector�� face�� �ߺ��Ǽ� ���� ������ �߻��Ͽ� �ڵ� �߰�
	detcellVector.erase(posC, detcellVector.end());										// �� : �ߺ��Ǵ� ���� ���� // 2017_02_13

	for (int i = detcellVector.size() - 1; i >= 0; i--) {
		delete(detcellVector[i].second);
		detcellVector[i].second->setID(0);
	}

	sort(faceVector.begin(), faceVector.end(), faceCompare);
	sort(cellVector.begin(), cellVector.end(), cellCompare);

	for (unsigned int i = 0; i < faceVector.size(); i++) {
		if (faceVector[i]->id == 365)
			cout << endl;
		faceVector[i]->setID(i + 1);
	}

	for (unsigned int i = 0; i < cellVector.size(); i++) {
		cellVector[i]->setID(i + 1);
	}

	for (unsigned int i = 0; i < minimalCellVector_tri.size(); i++) {//0208
		minimalCellVector_tri[i]->setID(i + 1 + cellVector.size());
	}

	fp = fopen("twoairpoile10.msh", "w");
	DescribeMesh();
	fclose(inputfp);
	fclose(fp); //��_cutcell����
				//initRectangle->printAll();
				//initRectangle->confirmAll();
	return 0;
}
