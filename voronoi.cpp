#include "voronoi.h"

point::point(double x_, double y_) :y(x_),x(y_){}

event::event(double y_, point p_, arc *a_, bool is_site) : y(y_), p(p_), a(a_), valid(true), site(is_site){}

arc::arc(point pp, arc *a, arc *b) : p(pp), prev(a), next(b), e(nullptr), s0(nullptr), s1(nullptr) {}

seg::seg(point p) : start(p), end(p), done(false) {}

void seg::finish(point p) {
    if (done) return;
    end = p;
    done = true;
}

void seg::current(point p) {
    end = p;
}

void seg::setl(point l_) {
    l = l_;
}

void seg::setr(point r_) {
    r = r_;
}


double voronoi::next() {
    if (!events.empty()) {
        double res = events.top()->y;
        if (events.top()->site)
            site_event();
        else
            circle_event();
        return res;
    }
    return -1;
}

void voronoi::site_event() {
    point p = events.top()->p;
    events.pop();
    insert_in_beach_line(p);
}

void voronoi::circle_event() {
    event *e = events.top();
    events.pop();

    if (e->valid) {
        seg *s = new seg(e->p);
        output.push_back(s);

        arc *a = e->a;
        if (a->prev) {
            a->prev->next = a->next;
            a->prev->s1 = s;
            s->setl(a->prev->p);
        }
        if (a->next) {
            a->next->prev = a->prev;
            a->next->s0 = s;
            s->setr(a->next->p);
        }

        if (a->s0)
            a->s0->finish(e->p);
        if (a->s1)
            a->s1->finish(e->p);

        if (a->prev)
            check_circle_event(a->prev, e->y);
        if (a->next)
            check_circle_event(a->next, e->y);
    }
    delete e;
}

void voronoi::insert_in_beach_line(point p) {
    if (!root) {
        root = new arc(p);
        return;
    }
    for (arc *i = root; i; i = i->next) {
        point z, zz;
        if (intersect(p, i, &z)) {
            if (i->next && !intersect(p, i->next, &zz)) {
                i->next->prev = new arc(i->p, i, i->next);
                i->next = i->next->prev;
            }
            else
                i->next = new arc(i->p, i);
            i->next->s1 = i->s1;


            i->next->prev = new arc(p, i, i->next);
            i->next = i->next->prev;

            i = i->next;

            seg* seg1 = new seg(z);
            output.push_back(seg1);
            seg1->setl(i->prev->p);
            seg1->setr(i->p);
            i->prev->s1 = i->s0 = seg1;

            seg* seg2 = new seg(z);
            output.push_back(seg2);
            seg2->setl(i->p);
            seg2->setr(i->next->p);
            i->next->s0 = i->s1 = seg2;

            check_circle_event(i, p.y);
            check_circle_event(i->prev, p.y);
            check_circle_event(i->next, p.y);
            return;
        }
    }
}

void voronoi::check_circle_event(arc *i, double x0) {
    //Invalidate any old event
    if (i->e && i->e->y != x0)
        i->e->valid = false;
    i->e = nullptr;

    if (!i->prev || !i->next)
        return;

    double y;
    point o;

    if (circle(i->prev->p, i->p, i->next->p, &y, &o) && y > x0) {
        i->e = new event(y, o, i);
        events.push(i->e);
    }
}

bool voronoi::circle(point a, point b, point c, double *y, point *o) {
    if ((b.y - a.y) * (c.x - a.x) - (c.y - a.y) * (b.x - a.x) > 0)
        return false;

    double A1 = 2 * (b.x - a.x);
    double B1 = 2 * (b.y - a.y);
    double C1 = b.x * b.x + b.y * b.y - a.x * a.x - a.y * a.y;
    double A2 = 2 * (c.x - b.x);
    double B2 = 2 * (c.y - b.y);
    double C2 = c.x * c.x + c.y * c.y - b.x * b.x - b.y * b.y;

    double det = A1 * B2 - A2 * B1;
    if (det == 0)
        return false;
    o->x = (C1 * B2 - C2 * B1) / det;
    o->y = (A1 * C2 - A2 * C1) / det;

    *y = o->y + sqrt( pow(a.y - o->y, 2) + pow(a.x - o->x, 2) );
    return true;
}

bool voronoi::intersect(point p, arc *i, point *res) {
    if (i->p.y == p.y)
        return false;

    double a,b;
    if (i->prev)
        a = intersection(i->prev->p, i->p, p.y).x;
    if (i->next)
        b = intersection(i->p, i->next->p, p.y).x;
    if ((!i->prev || a <= p.x) && (!i->next || p.x <= b)) {
        res->x = p.x;
        res->y = (i->p.y * i->p.y + (i->p.x - res->x) * (i->p.x - res->x) - p.y * p.y) / (2 * i->p.y - 2 * p.y);
        return true;
    }
    return false;
}

point voronoi::intersection(point p0, point p1, double dir) {
    point res, p = p0;

    if (p0.y == p1.y)
        res.x = (p0.x + p1.x) / 2;
    else if (p1.y == dir)
        res.x = p1.x;
    else if (p0.y == dir) {
        res.x = p0.x;
        p = p1;
    }
    else {
        double z0 = 2 * (p0.y - dir);
        double z1 = 2 * (p1.y - dir);

        double a = 1/z0 - 1/z1;
        double b = -2 * (p0.x / z0 - p1.x / z1);
        double c = (p0.x * p0.x + p0.y * p0.y - dir * dir) / z0 - (p1.x * p1.x + p1.y * p1.y - dir * dir) / z1;
        res.x = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
    }

    res.y = (p.y * p.y + (p.x - res.x) * (p.x - res.x) - dir * dir) / (2 * p.y - 2 * dir);
    return res;
}

void voronoi::finish_edges() {
    double l = 2 * height;
    for (arc *i = root; i->next; i = i->next)
        if (i->s1)
            i->s1->finish(intersection(i->p, i->next->p, l));
}

void voronoi::current_edges(double sweep) {
    if (root) {
        for (arc *i = root; i->next; i = i->next)
            if (i->s1)
                i->s1->current(intersection(i->p, i->next->p, sweep));
    }
}