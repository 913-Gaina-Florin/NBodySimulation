#include <iostream>
#include <cmath>
#include <stdexcept>

class Vector2D {
public:
    // Components of the vector
    double x, y;

    // Constructors
    Vector2D() : x(0), y(0) {} // Default constructor
    Vector2D(double x, double y) : x(x), y(y) {} // Parameterized constructor

    // Vector addition
    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }

    // Vector subtraction
    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }

    // Scalar multiplication
    Vector2D operator*(double scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }

    // Scalar division
    Vector2D operator/(double scalar) const {
        if (scalar == 0) {
            throw std::runtime_error("Division by zero");
        }
        return Vector2D(x / scalar, y / scalar);
    }

    // Dot product
    double dot(const Vector2D& other) const {
        return x * other.x + y * other.y;
    }

    // Cross product (returns a scalar for 2D vectors)
    double cross(const Vector2D& other) const {
        return x * other.y - y * other.x;
    }

    // Magnitude of the vector
    double magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    // Normalize the vector (returns a unit vector)
    Vector2D normalize() const {
        double mag = magnitude();
        if (mag == 0) {
            throw std::runtime_error("Cannot normalize a zero vector");
        }
        return *this / mag;
    }

    // Equality comparison
    bool operator==(const Vector2D& other) const {
        return x == other.x && y == other.y;
    }

    // Inequality comparison
    bool operator!=(const Vector2D& other) const {
        return !(*this == other);
    }

    // Output stream overload for easy printing
    friend std::ostream& operator<<(std::ostream& os, const Vector2D& v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }
};