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

struct point * getHeights(struct point pts [], struct line l, int plen) {
	struct point out [plen];
	for (int i = 0; i<plen; i++) {
		pts[i].lineHeight = heightFromLine(pts[i], l);
		out[i] = pts[i];
	}
	return out;
}

int checkIntercept(struct line lone, struct line ltwo) {
	float m1 = (lone.two.y - lone.one.y) / (lone.two.x - lone.one.x);
	float m2 = (ltwo.two.y - ltwo.one.y) / (ltwo.two.x - ltwo.one.x);

	if (m1 == m2) return 0;

	float c1 = lone.one.y - (m1 * lone.one.x);
	float c2 = ltwo.one.y - (m2 * ltwo.one.x);

	float minx = min(min(min(lone.one.x, lone.two.x), ltwo.one.x), ltwo.two.x);
	float miny = min(min(min(lone.one.y, lone.two.y), ltwo.one.y), ltwo.two.y);

	float maxx = max(max(max(lone.one.x, lone.two.x), ltwo.one.x), ltwo.two.x);
	float maxy = max(max(max(lone.one.y, lone.two.y), ltwo.one.y), ltwo.two.y);
	
	float xint = (c1 - c2) / (m2 - m1);
	float yint = m1*xint + c1;
	if ((xint > minx) && (xint >= maxx) && (yint < maxy) && (yint > maxy)){
		return 1;
 	} else {
		 return 0;
	 }

}

struct line * convexHull(struct point pin [], struct line lin [], int plen, int llen) {
	struct line lout [llen + 2];
	struct line nl1;
	struct line nl2;
	for (int i = 0; i < llen; i++) lout[i] = lin[i];
	for (int i = 0; i < llen; i++) {
		getHeights(pin,lin[i], plen);
		struct point * sorted = sort(pin, plen, lhgtr);
		int gotPoint = 0;
		int index = 1;
		do {
			nl1.two = pin[index];
			nl1.one = lin[i].one;
			nl2.two = pin[index];
			nl2.one = lin[i].two;
			int collide = 0;
			for (int j = 0; j < llen; j++) {
				collide += checkIntercept(nl1, lin[j]);
				collide += checkIntercept(nl2, lin[j]);
			}
			if (collide == 0) {
				gotPoint = 1;
			} else {
				index++;
			}
		} while (gotPoint == 0);
		i = llen;
	}

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
		printf("%s", toprint);
		printf("\n");
	}
	//char buff [10];
	//sprintf(buff, "%d", RAND_MAX);
	//printf(buff);
	printf("thank you kanye, very cool!\n");
	
	return 0;
}

