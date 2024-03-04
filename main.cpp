#include "voronoi.h"
#include <SFML/Graphics.hpp>
#include <iostream>




double y(double x, point f, double sw) {
    return  0.5 * (x - f.x) * (x - f.x) / (f.y - sw) + (f.y + sw) / 2;
}

void Parabola(sf::RenderWindow &w, arc *ar, double sw, double a, double b) {
    vector<sf::Vertex> vertices;
    a = max(a, 0.);
    b = min(b, width);
    while (a <= b) {

        vertices.emplace_back(sf::Vertex(sf::Vector2f(a, y(a, ar->p, sw))));
        a += 1;
    }
    w.draw(&vertices[0], vertices.size(), sf::LinesStrip);
}

int main() {
    point p;
    srand(time(0));
    int num = 1000;
    voronoi v;
    vector<sf::CircleShape> Points(num);
    for (int i = 0; i < num; i++){
        p.y = int(rand()) % int(height);
        p.x = int(rand()) % int(width);
        Points[i].setRadius(6);
        Points[i].setPosition(p.x - 6, p.y - 6);
        v.events.push(new event(p.y, p, nullptr, true));
    }

    bool cleaned = false;
    bool redraw = true;
    bool delaunay = false;

    sf::RenderWindow window(sf::VideoMode(width, height), "Voronoi diagram");
    vector<vector<sf::Vertex>> diagram;
    vector<vector<sf::Vertex>> delone;
    double sweep = 0;
    while (window.isOpen()) {
        sf::Event even{};

        while (window.pollEvent(even)) {
            if (even.type == sf::Event::Closed)
                window.close();

        }
        window.clear(sf::Color(0));
        for (const sf::CircleShape& point : Points) {
            window.draw(point);
        }

        if (!v.events.empty()) {
            sweep = v.next();
        }

        if (v.events.empty() && !cleaned) {
            v.finish_edges();
            cleaned = true;
        }

        if (redraw || !v.events.empty()) {
            v.current_edges(sweep);
            diagram.clear();
            for (auto edge: v.output) {
                double x1 = edge->start.x;
                double y1 = edge->start.y;
                double x2 = edge->end.x;
                double y2 = edge->end.y;
                vector<sf::Vertex> e(2);
                e[0] = (sf::Vertex(sf::Vector2f(x1, y1)));
                e[1] = (sf::Vertex(sf::Vector2f(x2, y2)));
                diagram.push_back(e);
            }
            redraw = !cleaned;

            for(arc* i = v.root;i != nullptr;i = i->next) {
                double i1 = 0, i2 = width;
                if (i->s0 != nullptr)
                    i1 = i->s0->end.x;
                if (i->s1 != nullptr)
                    i2 = i->s1->end.x;
                if (!delaunay)
                    Parabola(window, i, sweep, i1, i2);
            }
        }

        if (!delaunay) {
            vector<sf::Vertex> sweep_line(2);
            sweep_line[0] = sf::Vertex(sf::Vector2f(0, sweep));
            sweep_line[1] = sf::Vertex(sf::Vector2f(width, sweep));
            window.draw(&sweep_line[0], 2, sf::LinesStrip);

            for (auto &i : diagram) {
                window.draw(&i[0], 2, sf::LinesStrip);
            }
        }

        if (delaunay && !redraw) {
            delone.clear();
            for (auto edge: v.output) {
                double y1 = edge->l.y;
                double x1 = edge->l.x;
                double y2 = edge->r.y;
                double x2 = edge->r.x;
                vector<sf::Vertex> e(2);
                e[0] = (sf::Vertex(sf::Vector2f(x1, y1)));
                e[1] = (sf::Vertex(sf::Vector2f(x2, y2)));
                delone.push_back(e);
            }
            for (auto & i : delone) {
                window.draw(&i[0], 2, sf::LinesStrip);
            }
        }

        window.display();
        if (!delaunay)
            sf::sleep(sf::microseconds(1000000 / num));
    }


}