#ifndef LAB3_VORONOI_VORONOI_H
#define LAB3_VORONOI_VORONOI_H

#include <iostream>
#include <queue>
#include <set>
#include <cmath>

using namespace std;
const double width = 1000, height = 1000;

struct point {
    double y;
    double x;
    point(double x_ = 0, double y_ = 0);;
};

struct arc;
struct seg;

struct event {
    double y;
    point p;
    arc *a;
    bool valid;
    bool site = false;
    event(double y_, point p_, arc *a_, bool is_site = false);;
};

struct cmp {
    bool operator()(event *a, event *b) { return a->y > b->y; }
};

struct arc {
    point p;
    arc *prev, *next;
    event *e;

    seg *s0, *s1;

    arc(point pp, arc *a = nullptr, arc *b = nullptr);;
};


struct seg {
    point l, r;
    point start, end;
    bool done;
    seg(point p);;
    //mark as "done."
    void finish(point p);
    void current(point p);
    void setl(point l_);
    void setr(point r_);
};


struct voronoi {
    arc *root = nullptr;
    priority_queue<event*, vector<event*>, cmp> events;
    vector<seg*> output;

    double next();

    void site_event();

    void circle_event();

    void insert_in_beach_line(point p);

    bool circle(point a, point b, point c, double *y, point *o);

    void check_circle_event(arc *i, double x0);

    bool intersect(point p, arc *i, point *res = nullptr);

    point intersection(point p0, point p1, double dir);

    void finish_edges();

    void current_edges(double sweep);

};


#endif //LAB3_VORONOI_VORONOI_H
