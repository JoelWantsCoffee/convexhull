#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#define clear() printf("\033[H\033[J")

struct point {
	float x;
	float y;
	float lineHeight;
};

struct line {
	struct point one;
	struct point two;
};

struct outline {
	struct line * lines;
	int len;
};

struct point * next(struct point in [], struct point out [], int lenin, int lenout, int (*ptr)(struct point, struct point));

struct point * next(struct point in [], struct point out [], int lenin, int lenout, int (*ptr)(struct point, struct point)) {	
	if (lenin <= 0) {
		return out;
	} else {
		int maxIndex = 0;
		for (int i = 0; i<lenin; i++) {
			if (ptr(in[i], in[maxIndex]) == 1) {
				maxIndex = i;
			}
		}
		struct point nextin [lenin - 1];
		for (int i = 0; i<lenin; i++) {
			if (i < maxIndex) nextin[i] = in[i];
			if (i > maxIndex) nextin[i-1] = in[i];
		}
		struct point nextout [lenout + 1];
		for (int i = 0; i<lenout; i++) {
			nextout[i] = out[i];
		}
		nextout[lenout] = in[maxIndex];
		return next(nextin, nextout, lenin - 1, lenout + 1, ptr);
	}
}

float min(float a, float b) {
	if (a < b) {
		return a;
	} else {
		return b;
	}
} 


float max(float a, float b) {
	if (a > b) {
		return a;
	} else {
		return b;
	}
} 

struct point * sort(struct point in [], int len, int (*ptr)(struct point, struct point)) {
	struct point out [0];
	return next(in, out, len, 0, ptr);
}

int xgtr(struct point in1, struct point in2) {
	return (in1.x < in2.x);
}

int ygtr(struct point in1, struct point in2) {
	return (in1.y < in2.y);
}

int lhgtr(struct point in1, struct point in2) {
	return (in1.lineHeight < in2.lineHeight);
}

float heightFromLine(struct point pin, struct line lin) {
	//calculate line equation
	float m = (lin.two.y - lin.one.y) / (lin.two.x - lin.one.x);
	float c = lin.one.y - (lin.one.x * m);
	return abs(pin.y - (m*pin.x + c));
}

void getHeights(struct point pts [], struct line l, int plen) {
	struct point out [plen];
	for (int i = 0; i<plen; i++) {
		pts[i].lineHeight = heightFromLine(pts[i], l);
		out[i] = pts[i];
	}
}

int withinRange(float xin, float yin, float xmin, float xmax, float ymin, float ymax) {
	if (xin > xmin) {
		if (xin < xmax) {
			if (yin > ymin) {
				if (yin < ymax) {
					return 1;
				}
			}
		}
	}
	return 0;
}

int checkIntercept(struct line lone, struct line ltwo, int (*func)(float, float, float, float, float, float)) {
	float m1 = (lone.two.y - lone.one.y) / (lone.two.x - lone.one.x);
	float m2 = (ltwo.two.y - ltwo.one.y) / (ltwo.two.x - ltwo.one.x);

	if (lone.two.x == lone.one.x) m1 = 1000000;
	if (ltwo.two.x == ltwo.one.x) m2 = 1000000;

	if (m1 == m2) m1 += 0.001;

	float c1 = lone.one.y - (m1 * lone.one.x);
	float c2 = ltwo.one.y - (m2 * ltwo.one.x);

	float minx = min(min(min(lone.one.x, lone.two.x), ltwo.one.x), ltwo.two.x);
	float miny = min(min(min(lone.one.y, lone.two.y), ltwo.one.y), ltwo.two.y);

	float maxx = max(max(max(lone.one.x, lone.two.x), ltwo.one.x), ltwo.two.x);
	float maxy = max(max(max(lone.one.y, lone.two.y), ltwo.one.y), ltwo.two.y);

	float xint = (c1 - c2) / (m2 - m1);
	float yint = m1*xint + c1;
	
	if (func(xint, yint, minx, maxx, miny, maxy) == 1){
		return 1;
 	} else {
		return 0;
	 }
}

int inside(struct point pin, struct line lin [], int llen) {
	int coltop = 0;
	int colbottom = 0;
	int colleft = 0;
	int colright = 0;
	//check top
	for (int i = 0; i<llen; i++) {
		float ymax = max(lin[i].one.y, lin[i].two.y) + 1;
		float ymin = min(lin[i].one.y, lin[i].two.y) - 1;
		float xmax = max(lin[i].one.x, lin[i].two.x) + 1;
		float xmin = min(lin[i].one.x, lin[i].two.x) - 1;

		struct line top;
		top.one = pin;
		top.two.x = pin.x;
		top.two.y = ymax;

		struct line bottom;
		bottom.one = pin;
		bottom.two.x = pin.x;
		bottom.two.y = ymin;

		struct line left;
		left.one = pin;
		left.two.x = xmin;
		left.two.y = pin.y;

		struct line right;
		right.one = pin;
		right.two.x = xmax;
		right.two.y = pin.y;

		//printf("%d\n", checkIntercept(top, lin[i], withinRange));

		coltop += checkIntercept(top, lin[i], withinRange);
		colbottom += checkIntercept(bottom, lin[i], withinRange);
		colleft += checkIntercept(left, lin[i], withinRange);
		colright += checkIntercept(right, lin[i], withinRange);
	}

	if (coltop > 0) {
		if (colbottom > 0) {
			if (colleft > 0) {
				if (colright > 0) return 1;
			}
		}
	}
	return 0;
}

struct outline convexHull(struct point pin [], struct line lin [], int plen, int llen);

struct outline convexHull(struct point pin [], struct line lin [], int plen, int llen) {
	if (plen <= 0) {
		struct outline out;
		out.lines = lin;
		out.len = llen;
		return out;
	}
	struct point pout [plen - 1];
	struct line nl1;
	struct line nl2;
	int omitLine;
	int omitPoint;
	for (int i = 0; i < llen; i++) {
		getHeights(pin,lin[i], plen);
		struct point * sorted = sort(pin, plen, lhgtr);
		int gotPoint = 0;
		int index = 1;
		for (int k = plen - 1; k >= 0; k--) {
			nl1.two = pin[k];
			nl1.one = lin[i].one;
			nl2.two = pin[k];
			nl2.one = lin[i].two;
			int collide = 0;
			for (int j = 0; j < llen; j++) {
				collide += checkIntercept(nl1, lin[j], withinRange);
				collide += checkIntercept(nl2, lin[j], withinRange);
			}
			if (collide == 0) {
				gotPoint = 1;
				omitLine = i;
				omitPoint = k;
			}
		}
		if (gotPoint == 1) i = llen;
	}

	int loutlen;
	
	if (llen == 1) {
		loutlen = llen + 2;
	} else {
		loutlen = llen + 1;
	}
	
	struct line lout [loutlen];

	if (llen == 1) {
		lout[0] = lin[0];
	} else {
		for (int i = 0; i < llen - 1; i++) {
			if (i < omitLine) lout[i] = lin[i];
			if (i > omitLine) lout[i-1] = lin[i];
		}
	}

	lout[loutlen - 2] = nl1;
	lin[loutlen - 1] = nl2;

	for (int i = 0; i < plen - 1; i++) {
		if (i < omitPoint) pout[i] = pin[i];
		if (i > omitPoint) pout[i-1] = pin[i];
	}

	int pointsRemoved = 0;

	for (int i = 0; i < plen - 1; i++) {
		if (inside(pout[i], lout, llen + 1) == 1) pointsRemoved++;
	}

	int poutlen = plen - (1 + pointsRemoved);

	struct point poutreal [poutlen];

	pointsRemoved = 0;

	for (int i = 0; i < plen - 1; i++) {
		if (inside(pout[i], lout, llen + 1) == 1) {
			pointsRemoved++;
		} else {
			poutreal[i - pointsRemoved] = pout[i];
		}
	}
	printf("ding %d\n", poutlen);
	return convexHull(poutreal, lout, poutlen, llen + 1);
}

struct outline doConvexHull(struct point pin [], int plen) {
	struct line mid [1];
	struct point * sorted = sort(pin, plen, xgtr);
	mid[0].one = sorted[0];
	mid[0].two = sorted[plen - 1];
	struct point removed [plen - 2];

	for (int i = 1; i<plen - 1; i++) {
		removed[i-1] = sorted[i];
	}
	return convexHull(removed, mid, plen - 2, 1);
}

int main() {
	clear();

	printf("-----------let it begin-----------\n");	
	struct point ps [100];	
	
	for (int i = 0; i<100; i++) {
		float x = (rand() % 100);

		float y = (rand() % 100);
		ps[i].x = x;
		ps[i].y = y;
	}
	struct point * sorted;
	//int (*xgtp)(struct point, struct point) = xgtr;
	sorted = sort(ps, 100, ygtr); 

	struct line mid;

	mid.one = sorted[0];
	mid.two = sorted[99];

	struct point * sortedtwo = sort(sorted, 100, lhgtr);
	struct outline outside = doConvexHull(ps, 100);

	printf("yeah so I finnished the recursion\n");
	
	for (int i = 0; i<100; i++) {
		char tempx [2];
		char tempy [2];
		int intx = (sortedtwo[i].x);
		int inty = (sortedtwo[i].y);
		sprintf(tempx, "%d", intx);
		sprintf(tempy, "%d", inty);
		char toprint [5];
		toprint[0] = tempx[0];
		toprint[1] = tempx[1];
		toprint[2] = ',';
		toprint[3] = tempy[0];
		toprint[4] = tempy[1];
		printf("(%s)", toprint);
	}
	printf("\n");
	for (int i = 0; i<outside.len; i++) {
		printf("([%f,", outside.lines[i].one.x);
		printf("%f],[", outside.lines[i].one.y);
		printf("%f,", outside.lines[i].two.x);
		printf("%f])", outside.lines[i].two.y);
	}

	//char buff [10];
	//sprintf(buff, "%d", RAND_MAX);
	//printf(buff);
	printf("thank you kanye, very cool!\n");
	return 0;
}
/*([59.000000,19.000000],[19.000000,-155871792741920049840212934656.000000])([3.000000,-26491894557786430745996310209363968.000000],[3.000000,19.000000])([3.000000,-155896273489767246081000734720.000000],[92.000000,17.000000])([98.000000,80.000000],[17.000000,92.000000])([17.000000,92.000000],[17.000000,-155855321127627800517707563008.000000])([1.000000,80.000000],[17.000000,92.000000])([92.000000,500.000000],[21.000000,19.000000])([-2147483648.000000,21.000000],[44.000000,19.000000])([19.000000,73.000000],[21.000000,19.000000])*/
