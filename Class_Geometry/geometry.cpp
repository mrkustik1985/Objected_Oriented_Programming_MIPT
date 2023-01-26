#include "geometry.h"

// class Point

Point::Point(const double& x, const double& y) : x(x), y(y) {}

bool Point::operator==(const Point& po) const {
    return (std::abs(x - po.x) < eps) && (std::abs(y - po.y) < eps);
}

bool Point::operator!=(const Point& po) const {
    return !(*this == po);
}

double Point::dist(const Point& po) const {
    return sqrt((po.x - x) * (po.x - x) + (po.y - y) * (po.y - y));
}

void Point::rotate(double angle) {
    angle = angle * M_PI / 180;
    double xold = x;
    double yold = y;
    x = xold * cos(angle) - yold * sin(angle);
    y = xold * sin(angle) + yold * cos(angle);
    x = std::abs(x) < eps ? 0 : x;
    y = std::abs(y) < eps ? 0 : y;
}

void Point::reflect(const Point& center) {
    x += 2 * (center.x - x);
    y += 2 * (center.y - y);
}

void Point::reflect(const Line& axis) {
    reflect(axis.cross(axis.normal(*this)));
}

// Class Line

Line::Line(double a, double b, double c) : a(a), b(b), c(c) {}

Line::Line(const Point& a, const Point& b) {
    if (b.x == a.x) {
        *this = {-1, 0, a.x};
    } else {
        *this = Line(a, (b.y - a.y) / (b.x - a.x));
    }
}

Line::Line(const Point& p, const double& a) : Line(a, p.y - (p.x * a)) {}

Line::Line(const double& k, const double& b) : Line(k, -1, b) {}

bool Line::operator==(const Line& another) const {
    bool fl1 = false;
    bool fl2 = false;
    //std::cerr << another.a << ' ' << another.b << ' ' << another.c << " !!!! "<< std::endl;
    if (a == 0) {
        fl1 = another.isPoint(Point(1, -c / b));
    } else {
        fl1 = another.isPoint(Point(-c / a, 0));
    }
    if (b == 0) {
        fl2 = another.isPoint(Point(-c / a, 1));
    } else if (c != 0) {
        fl2 = another.isPoint(Point(0, -c / b));
    } else {
        fl2 = another.isPoint(Point(1, -a / b));
    }
    return fl1 && fl2;
}

bool Line::operator!=(const Line& another) const {
    return !(*this == another);
}

Line Line::normal(const Point& po) const {
    if (std::abs(a) < eps) {
        return Line(po, Point(po.x, po.y - 1));
    }
    double k = b / a;
    return Line(po, k);
}

Point Line::cross(const Line& ln) const {
    return Point((b * ln.c - c * ln.b) / (a * ln.b - ln.a * b),
                 (ln.a * c - ln.c * a) / (a * ln.b - ln.a * b));
}

bool Line::isPoint(const Point& po) const {
    return std::abs(a * po.x + b * po.y + c) < eps;
}

// class ellipse

Ellipse::Ellipse(const Point& f1, const Point& f2, double d)
    : f1(f1), f2(f2), r(d) {}
Ellipse::~Ellipse() {}

std::pair<Point, Point> Ellipse::focuses() const {
    return {f1, f2};
}

double Ellipse::eccentricity() const {
    return c() / a();
}

Point Ellipse::center() const {
    return Point((f1.x + f2.x) / 2, (f1.y + f2.y) / 2);
}

std::pair<Line, Line> Ellipse::directrices() const {
    Line x0(f1, f2);
    Point help(f1.x - center().x, f1.y - center().y);
    double helpcoord = a() / eccentricity() / c();
    help.x *= helpcoord;
    help.y *= helpcoord;
    Point left(center().x + help.x, center().y + help.y);
    Point right(center().x - help.x, center().y - help.y);
    return {x0.normal(left), x0.normal(right)};
}

double Ellipse::perimeter() const {
    return M_PI * (3 * (a() + b()) - sqrt((3 * a() + b()) * (a() + 3 * b())));
}

double Ellipse::area() const {
    return M_PI * a() * b();
}

bool Ellipse::operator==(const Shape& shape) const {
    const Ellipse* newEl = dynamic_cast<const Ellipse*>(&shape);
    if (newEl == nullptr || std::abs(area() - shape.area()) > eps) {
        return false;
    }
    const Ellipse& newEll = *newEl;
    if (newEll.f1 == f1 && newEll.f2 == f2 && std::abs(r - newEll.r) < eps) {
        return true;
    }
    if (newEll.f2 == f1 && newEll.f1 == f2 && std::abs(r - newEll.r) < eps) {
        return true;
    }
    return false;
}
bool Ellipse::operator==(const Ellipse& ellipse) const {
    if (std::abs(area() - ellipse.area()) > eps) {
        return false;
    };
    if (ellipse.f1 == f1 && ellipse.f2 == f2 && std::abs(r - ellipse.r) < eps) {
        return true;
    }
    if (ellipse.f2 == f1 && ellipse.f1 == f2 && std::abs(r - ellipse.r) < eps) {
        return true;
    }
    return false;
}

bool Ellipse::operator!=(const Shape& shape) const {
    return !(*this == shape);
}
bool Ellipse::operator!=(const Ellipse& ellipse) const {
    return !(*this == ellipse);
}

bool Ellipse::isCongruentTo(const Shape& shape) const {
    const Ellipse* pointer = dynamic_cast<const Ellipse*>(&shape);
    if (pointer == nullptr) {
        return false;
    }
    const Ellipse& another = *pointer;
    double area1 = area(), area2 = shape.area();
    return (std::abs(r - another.r) < eps) && (std::abs(area1 - area2) < eps);
}

bool Ellipse::isSimilarTo(const Shape& other) const {
    const Ellipse* pointer = dynamic_cast<const Ellipse*>(&other);
    if (pointer == nullptr) {
        return false;
    }
    const Ellipse& another = *pointer;
    double distOther =
        sqrt((another.f1.x - another.f2.x) * (another.f1.x - another.f2.x) +
             (another.f1.y - another.f2.y) * (another.f1.y - another.f2.y));
    double dist =
        sqrt((f1.x - f2.x) * (f1.x - f2.x) + (f1.y - f2.y) * (f1.y - f2.y));
    double koef = dist / distOther - r / another.r;
    return std::abs(koef) < eps;
}

bool Ellipse::containsPoint(const Point& po) const {
    return f1.dist(po) + f2.dist(po) < eps + r;
}

void Ellipse::rotate(const Point& center, double angle) {
    Point nw1(f1.x - center.x, f1.y - center.y);
    Point nw2(f2.x - center.x, f2.y - center.y);
    nw1.rotate(angle);
    nw2.rotate(angle);
    f1.x = nw1.x + center.x;
    f1.y = nw1.y + center.y;
    f2.x = nw2.x + center.x;
    f2.y = nw2.y + center.y;
}

void Ellipse::reflect(const Point& center) {
    f1.reflect(center);
    f2.reflect(center);
}

void Ellipse::reflect(const Line& axis) {
    f1.reflect(axis);
    f2.reflect(axis);
}

void Ellipse::scale(const Point& center, double coefficient) {
    Point nw1(f1.x - center.x, f1.y - center.y);
    Point nw2(f2.x - center.x, f2.y - center.y);
    f1.x = nw1.x * coefficient + center.x;
    f1.y = nw1.y * coefficient + center.y;
    f2.x = nw2.x * coefficient + center.x;
    f2.y = nw2.y * coefficient + center.y;
    r *= coefficient;
}
double Ellipse::a() const {
    return r / 2;
}
double Ellipse::b() const {
    return sqrt(pow(a(), 2) - pow(c(), 2));
}
double Ellipse::c() const {
    return f1.dist(f2) / 2;
}

// class Circle

Circle::Circle(const Point& x, double r) : Ellipse(x, x, r * 2) {}

double Circle::radius() const {
    return r / 2;
}

Circle::~Circle() {}

// class Polygon

Polygon::Polygon(const std::vector<Point>& vec) {
    vertices = vec;
}

Polygon::Polygon(const Point& p) {
    vertices.push_back(p);
}

Polygon::Polygon(std::initializer_list<Point>& v) : vertices(v) {}

Polygon::~Polygon() {}

int Polygon::verticesCount() const {
    return vertices.size();
}

std::vector<Point> Polygon::getVertices() const {
    return vertices;
}
bool Polygon::isConvex() const {
    size_t n = vertices.size();
    bool f1 = false;
    bool f2 = false;
    for (size_t i = 0; i < n; ++i) {
        double vx1 = vertices[(i - 1 + n) % n].x - vertices[i].x;
        double vy1 = vertices[(i - 1 + n) % n].y - vertices[i].y;
        double vx2 = vertices[(i + 1) % n].x - vertices[i].x;
        double vy2 = vertices[(i + 1) % n].y - vertices[i].y;
        double prom = vx1 * vy2 - vy1 * vx2;
        if (prom < eps) {
            f1 = true;
        } else {
            f2 = true;
        }
    }
    return !f1 || !f2;
}

double Polygon::perimeter() const {
    double ans = 0;
    size_t n = vertices.size();
    for (size_t i = 0; i < n; ++i) {
        ans += vertices[i].dist(vertices[(i + 1) % n]);
    }
    return ans;
}

double Polygon::area() const {
    double ans = 0;
    size_t n = vertices.size();
    for (size_t i = 0; i < vertices.size(); i++) {
        ans += vertices[i].x * vertices[(i + 1) % n].y;
    }
    for (size_t i = 0; i < vertices.size(); i++) {
        ans -= vertices[i].y * vertices[(i + 1) % n].x;
    }
    return std::abs(ans) / 2;
}

bool Polygon::operator==(const Shape& another) const {
    const Polygon* newEl = dynamic_cast<const Polygon*>(&another);
    if (newEl == nullptr || std::abs(area() - another.area()) > eps) {
        return false;
    }
    const Polygon& newPol = *newEl;
    if (newPol.vertices.size() != vertices.size()) {
        return false;
    }
    size_t n = vertices.size();
    for (size_t s = 0; s < vertices.size(); s++) {
        bool fl = true;
        for (size_t i = 0; i < vertices.size(); i++) {
            if (vertices[(i + s) % n] != newPol[i]) {
                fl = false;
                break;
            }
        }
        if (fl) {
            return true;
        }
    }
    for (size_t s = 0; s < vertices.size(); s++) {
        bool fl = true;
        for (size_t i = 0; i < vertices.size(); i++) {
            if (vertices[(i + s) % n] != newPol[n - i - 1]) {
                fl = false;
                break;
            }
        }
        if (fl) {
            return true;
        }
    }
    return false;
}

bool Polygon::operator!=(const Shape& other) const {
    return !(*this == other);
}

bool Polygon::isSimilarTo(const Shape& other) const {
    const Polygon* newEl = dynamic_cast<const Polygon*>(&other);
    if (newEl == nullptr) {
        return false;
    }
    const Polygon& newPo = *newEl;
    size_t n = vertices.size();
    if (n != newPo.vertices.size() || isConvex() != newPo.isConvex()) {
        return false;
    }
    double dp = sqrt(other.area() / area());
    for (size_t s = 0; s < n; s++) {
        bool fl = true;
        double dper = dp;
        for (size_t i = 0; i < n; i++) {
            double d = newPo[(i + s + 1) % n].dist(newPo[(i + s) % n]) /
                       vertices[(i + 1) % n].dist(vertices[i]);
            if (std::abs(d - dper) < eps) {
                continue;
            }
            fl = false;
            break;
        }
        if (fl) {
            return true;
        }
    }
    for (size_t s = 0; s < n; s++) {
        bool fl = true;
        double dper = dp;
        for (size_t i = 0; i < n; i++) {
            double d = newPo[(i + s + 1) % n].dist(newPo[(i + s) % n]) /
                       vertices[n - i - 1].dist(vertices[(n - i) % n]);
            if (std::abs(d - dper) < eps) {
                continue;
            }
            fl = false;
            break;
        }
        if (fl) {
            return true;
        }
    }
    return false;
}

bool Polygon::isCongruentTo(const Shape& other) const {
    return (std::abs(area() - other.area()) < eps) && (isSimilarTo(other));
}

bool Polygon::containsPoint(const Point& point) const {
    double fl = 0;
    size_t n = vertices.size();
    for (size_t i = 0; i < n; i++) {
        double vx1 = vertices[i].x - point.x;
        double vy1 = vertices[i].y - point.y;
        double vx2 = vertices[(i + 1) % n].x - point.x;
        double vy2 = vertices[(i + 1) % n].y - point.y;
        double vect = vx1 * vy2 - vy1 * vx2;
        double scal = vx1 * vx2 + vy1 * vy2;
        fl += atan2(vect, scal);
    }
    return std::abs(std::abs(fl) - 2 * M_PI) < eps;
}

void Polygon::rotate(const Point& center, double angle) {
    for (size_t i = 0; i < vertices.size(); i++) {
        Point nw(vertices[i].x - center.x, vertices[i].y - center.y);
        nw.rotate(angle);
        nw.x += center.x;
        nw.y += center.y;
        vertices[i] = nw;
        vertices[i].x = std::abs(vertices[i].x) < eps ? 0 : vertices[i].x;
        vertices[i].y = std::abs(vertices[i].y) < eps ? 0 : vertices[i].y;
    }
}

void Polygon::reflect(const Point& center) {
    for (size_t i = 0; i < vertices.size(); i++) {
        vertices[i].reflect(center);
    }
}

void Polygon::reflect(const Line& axis) {
    for (size_t i = 0; i < vertices.size(); i++) {
        vertices[i].reflect(axis);
    }
}

void Polygon::scale(const Point& center, double coefficient) {
    for (size_t i = 0; i < vertices.size(); i++) {
        double nx = coefficient * (vertices[i].x - center.x) + center.x;
        double ny = coefficient * (vertices[i].y - center.y) + center.y;
        vertices[i].x = nx;
        vertices[i].y = ny;
    }
}

double Polygon::helpArea(const Point& x, const Point& y, const Point& z) {
    double d1 = x.dist(y);
    double d2 = x.dist(z);
    double d3 = y.dist(z);
    double p = (d1 + d2 + d3) / 2;
    return sqrt(std::abs(p) * std::abs(p - d1) * std::abs(p - d2) *
                std::abs(p - d3));
}

const Point& Polygon::operator[](int id) const {
    return vertices[id];
}

// class Rectangle

Rectangle::Rectangle(std::vector<Point>& vec) : Polygon(vec) {}
Rectangle::Rectangle(const Point& a, const Point& b, double c) {
    if (c < 1) {
        c = 1 / c;
    }
    double angleBeta = atan(c);
    double angleAlpha = M_PI - 2 * angleBeta;
    double dist = a.dist(b) / 2;
    double sizeSideB =
        sqrt(2 * dist * dist - 2 * dist * dist * cos(angleAlpha));
    double sizeSideD =
        sqrt(2 * dist * dist - 2 * dist * dist * cos(2 * angleBeta));
    Point mid((a.x + b.x) / 2 - a.x, (a.y + b.y) / 2 - a.y);
    Point midHelp = mid;
    double koefB = mid.dist(Point(0, 0)) / sizeSideB;
    double koefD = mid.dist(Point(0, 0)) / sizeSideD;
    mid.rotate(angleBeta * 180 / M_PI);
    Point B(a.x + mid.x / koefB, a.y + mid.y / koefB);
    midHelp.rotate(-(M_PI / 2 - angleBeta) * 180 / M_PI);
    Point D(a.x + midHelp.x / koefD, a.y + midHelp.y / koefD);
    vertices.push_back(a);
    vertices.push_back(B);
    vertices.push_back(b);
    vertices.push_back(D);
}

Rectangle::~Rectangle() {}

Point Rectangle::center() const {
    return Point((vertices[0].x + vertices[2].x) / 2,
                 (vertices[0].y + vertices[2].y) / 2);
}

std::pair<Line, Line> Rectangle::diagonals() const {
    return {Line(vertices[1], vertices[3]), Line(vertices[0], vertices[2])};
}

// class Square

Square::Square(std::vector<Point>& vec) : Rectangle(vec) {}
Square::Square(const Point& po1, const Point& po2) {
    Point mid((po1.x + po2.x) / 2, (po1.y + po2.y) / 2);
    Point copy(mid.x - po1.x, mid.y - po1.y);
    copy.rotate(90);
    Point b(mid.x + copy.x, mid.y + copy.y);
    Point d(mid.x - copy.x, mid.y - copy.y);
    vertices.push_back(po1);
    vertices.push_back(b);
    vertices.push_back(po2);
    vertices.push_back(d);
}
Square::~Square(){};
Circle Square::circumscribedCircle() const {
    Point center((vertices[0].x + vertices[2].x) / 2,
                 (vertices[0].y + vertices[2].y) / 2);
    return Circle(center, vertices[0].dist(vertices[2]) / 2);
}

Circle Square::inscribedCircle() const {
    Point center((vertices[0].x + vertices[2].x) / 2,
                 (vertices[0].y + vertices[2].y) / 2);
    return Circle(center, vertices[0].dist(vertices[1]) / 2);
}

// class Triangle

Triangle::Triangle(const Point& a, const Point& b, const Point& c)
    : Polygon(a, b, c) {}
Triangle::~Triangle() {}

Circle Triangle::circumscribedCircle() const {
    Point center = normalToCenter(0).cross(normalToCenter(1));
    double radius = center.dist(vertices[0]);
    return Circle(center, radius);
}

Circle Triangle::inscribedCircle() const {
    Point center = bisector(0).cross(bisector(1));
    double radius = (2 * area()) / perimeter();
    return Circle(center, radius);
}

Point Triangle::centroid() const {
    return median(0).cross(median(1));
}

Point Triangle::orthocenter() const {
    return normal(0).cross(normal(1));
}

Line Triangle::EulerLine() const {
    return Line(orthocenter(), circumscribedCircle().center());
}

Circle Triangle::ninePointsCircle() const {
    Point x1 = centerOfOpposite(0);
    Point x2 = centerOfOpposite(1);
    Point x3 = centerOfOpposite(2);
    Triangle midtriangle(x1, x2, x3);
    Circle help = midtriangle.circumscribedCircle();
    Point cross = normalToCenter(0).cross(normalToCenter(2));
    Point center((cross.x + orthocenter().x) / 2,
                 (cross.y + orthocenter().y) / 2);
    return Circle(center, help.radius());
}

Line Triangle::normalToCenter(int i) const {
    return line((i + vertices.size()) % vertices.size())
        .normal(centerOfOpposite((i + vertices.size()) % vertices.size()));
}

Line Triangle::line(int i) const {
    int now = (i + vertices.size()) % vertices.size();
    int next = (i + 1 + vertices.size()) % vertices.size();
    return Line(vertices[now], vertices[next]);
}

Line Triangle::median(int i) const {
    int now = (i + vertices.size()) % vertices.size();
    int nn = (i + 2 + vertices.size()) % vertices.size();
    return Line(vertices[nn], centerOfOpposite(now));
}

Line Triangle::normal(int i) const {
    int now = (i + vertices.size()) % vertices.size();
    int next = (i + 1 + vertices.size()) % vertices.size();
    return line(next).normal(vertices[now]);
}

Line Triangle::bisector(int i) const {
    Point now = vertices[i];
    Point pref = vertices[(i - 1 + vertices.size()) % vertices.size()];
    Point next = vertices[(i + 1) % vertices.size()];
    Point vectNow(next.x - now.x, next.y - now.y);
    Point vectPref(now.x - pref.x, now.y - pref.y);
    double leftDist = vectNow.dist(Point(0, 0));
    double rightDist = vectPref.dist(Point(0, 0));
    double dx = vectNow.x * rightDist - leftDist * vectPref.x;
    double dy = vectNow.y * rightDist - leftDist * vectPref.y;
    return Line(vertices[i], Point(vertices[i].x + dx, vertices[i].y + dy));
}

Point Triangle::centerOfOpposite(int i) const {
    int next = (i + 1) % vertices.size();
    return Point((vertices[i].x + vertices[next].x) / 2,
                 (vertices[i].y + vertices[next].y) / 2);
}