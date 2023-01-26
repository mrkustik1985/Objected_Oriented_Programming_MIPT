#include <cmath>
#include <initializer_list>
#include <iostream>
#include <vector>

const double eps = 1e-5;

// Struct Point
class Line;

struct Point {
    Point(const double& x, const double& y);
    bool operator==(const Point& po) const;

    bool operator!=(const Point& po) const;

    double dist(const Point& po) const;

    void rotate(double angle);

    void reflect(const Point& center);

    void reflect(const Line& /*axis*/);

    double x = 0;
    double y = 0;
};

// class Line
class Line {
  public:
    Line(double a, double b, double c);
    Line(const Point& a, const Point& b);
    Line(const Point& p, const double& a);
    Line(const double& k, const double& b);

    bool operator==(const Line& another) const;

    bool operator!=(const Line& another) const;

    Line normal(const Point& po) const;

    Point cross(const Line& ln) const;
    double a = 0;
    double b = 0;
    double c = 0;

    bool isPoint(const Point& po) const;
};
// class Shape
class Shape {
  public:
    virtual double perimeter() const = 0;
    virtual double area() const = 0;

    virtual bool operator==(const Shape&) const = 0;
    virtual bool operator!=(const Shape&) const = 0;

    virtual bool isCongruentTo(const Shape& another) const = 0;
    virtual bool isSimilarTo(const Shape& another) const = 0;
    virtual bool containsPoint(const Point& point) const = 0;

    virtual void rotate(const Point&, double) = 0;
    virtual void reflect(const Point&) = 0;
    virtual void reflect(const Line& axis) = 0;
    virtual void scale(const Point&, double) = 0;
    virtual ~Shape() = default;
};

// class ellipse

class Ellipse : public Shape {
  public:
    Ellipse(const Point& f1, const Point& f2, double d);
    ~Ellipse() override;

    std::pair<Point, Point> focuses() const;

    double eccentricity() const;

    Point center() const;

    std::pair<Line, Line> directrices() const;
    double perimeter() const override;

    double area() const override;

    bool operator==(const Shape& shape) const override;
    bool operator==(const Ellipse& ellipse) const;
    bool operator!=(const Shape& shape) const override;
    bool operator!=(const Ellipse& ellipse) const;

    bool isCongruentTo(const Shape& shape) const override;

    bool isSimilarTo(const Shape& other) const override;

    bool containsPoint(const Point& po) const override;

    void rotate(const Point& center, double angle) override;
    void reflect(const Point& center) override;

    void reflect(const Line& axis) override;

    void scale(const Point& center, double coefficient) override;

  protected:
    Point f1;
    Point f2;

    double r;
    double a() const;
    double b() const;
    double c() const;
};

// class Circle

class Circle : public Ellipse {
  public:
    Circle(const Point& x, double r);
    double radius() const;
    ~Circle() override;
};

// class Polygon

class Polygon : public Shape {
  protected:
    std::vector<Point> vertices;

  public:
    Polygon() = default;

    Polygon(const std::vector<Point>& vec);

    Polygon(const Point& p);

    Polygon(std::initializer_list<Point>& v);

    template <class... Args>
    Polygon(const Point& p, Args... args) : Polygon(args...) {
        vertices.push_back(p);
    }

    ~Polygon() override;
    int verticesCount() const;

    std::vector<Point> getVertices() const;

    bool isConvex() const;

    double perimeter() const override;

    double area() const override;

    bool operator==(const Shape& another) const override;

    bool operator!=(const Shape& other) const override;

    bool isSimilarTo(const Shape& other) const override;

    bool isCongruentTo(const Shape& other) const override;

    bool containsPoint(const Point& point) const override;

    void rotate(const Point& center, double angle) override;

    void reflect(const Point& center) override;

    void reflect(const Line& axis) override;

    void scale(const Point& center, double coefficient) override;

  private:
    static double helpArea(const Point& x, const Point& y, const Point& z);

    const Point& operator[](int id) const;
};

// class Rectangle

class Rectangle : public Polygon {
  public:
    Rectangle() = default;
    Rectangle(std::vector<Point>& vec);
    Rectangle(const Point& a, const Point& b, double c);
    ~Rectangle() override;
    Point center() const;
    std::pair<Line, Line> diagonals() const;
};

// class Square

class Square : public Rectangle {
  public:
    Square(std::vector<Point>& vec);
    Square(const Point& po1, const Point& po2);
    ~Square() override;
    Circle circumscribedCircle() const;
    Circle inscribedCircle() const;
};

// class Triangle

class Triangle : public Polygon {
  public:
    Triangle() = default;
    Triangle(const Point& a, const Point& b, const Point& c);
    ~Triangle() override;

    Circle circumscribedCircle() const;

    Circle inscribedCircle() const;

    Point centroid() const;

    Point orthocenter() const;

    Line EulerLine() const;

    Circle ninePointsCircle() const;

  private:
    Line normalToCenter(int i) const;

    Line line(int i) const;

    Line median(int i) const;

    Line normal(int i) const;

    Line bisector(int i) const;

    Point centerOfOpposite(int i) const;
};
