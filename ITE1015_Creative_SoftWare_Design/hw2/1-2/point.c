#include <stdio.h>

typedef struct point {
	int xpos;
	int ypos;
}Point;

Point getScale2xPoint(const Point* p1);
void swapPoint(Point *p1, Point *p2);



int main() {
	Point p1,p2;

	scanf("%d %d", &p1.xpos, &p1.ypos);

	
	printf("Calling getScale2xPoint()\n");
	p2 = getScale2xPoint(&p1);
	printf("P1 : %d %d\n", p1.xpos, p1.ypos);
	printf("P2 : %d %d\n", p2.xpos, p2.ypos);

	printf("Calling getScale2xPoint()\n");
	swapPoint(&p1,&p2);
	printf("P1 : %d %d\n", p1.xpos, p1.ypos);
	printf("P2 : %d %d\n", p2.xpos, p2.ypos);




}

Point getScale2xPoint(const Point * p)
{
	Point p_2;
	p_2.xpos = p->xpos*2;
	p_2.ypos = p->ypos *2;
	return p_2;
}

void swapPoint(Point * p1, Point * p2)
{
	int temp[2];
	temp[0] = p1->xpos;
	temp[1] = p1->ypos;

	p1->xpos = p2->xpos;
	p1->ypos = p2->ypos;
	p2->xpos = temp[0];
	p2->ypos = temp[1];
	
}

