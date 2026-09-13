#include "Engine/Math/MathUtils.hpp"

#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/Capsule3.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Plane2.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/Polygon2.hpp"
#include "Engine/Math/Rotator.hpp"
#include "Engine/Math/Quaternion.hpp"
#include "Engine/Math/Sphere3.hpp"
#include "Engine/Profiling/ProfileLogScope.hpp"

#include <algorithm>
#include <array>
#include <cmath>

namespace MathUtils {

namespace {
static thread_local unsigned int MT_RANDOM_SEED = 0u;
}

const unsigned int GetRandomSeed() noexcept {
    return MT_RANDOM_SEED;
}

void SetRandomEngineSeed(unsigned int seed) noexcept {
    MT_RANDOM_SEED = seed;
    if(MT_RANDOM_SEED) {
        GetMT64RandomEngine(MT_RANDOM_SEED).seed(MT_RANDOM_SEED);
    } else {
        MT_RANDOM_SEED = GetRandomDevice()();
        GetMT64RandomEngine(MT_RANDOM_SEED).seed(MT_RANDOM_SEED);
    }
}

bool IsValid(Vector2 v) noexcept {
    return IsValid(v.x) && IsValid(v.y);
}

bool IsValid(Vector3 v) noexcept {
    return IsValid(v.x) && IsValid(v.y) && IsValid(v.z);
}

bool IsValid(Vector4 v) noexcept {
    return IsValid(v.x) && IsValid(v.y) && IsValid(v.z) && IsValid(v.w);
}

bool IsValid(Matrix4 v) noexcept {
    return IsValid(v.GetXComponents()) && IsValid(v.GetYComponents()) && IsValid(v.GetZComponents()) && IsValid(v.GetWComponents());
}

float ConvertDegreesToRadians(float degrees) noexcept {
    return degrees * (MathUtils::pi_v<float> / 180.0f);
}

float ConvertRadiansToDegrees(float radians) noexcept {
    return radians * (180.0f * MathUtils::inv_pi_v<float>);
}

std::random_device& GetRandomDevice() noexcept {
    static thread_local std::random_device rd;
    return rd;
}

std::mt19937_64& GetMT64RandomEngine(unsigned int seed /*= 0*/) noexcept {
    static thread_local std::mt19937_64 random_engine = std::mt19937_64(!seed ? GetRandomDevice()() : seed);
    return random_engine;
}

bool GetRandomBool() noexcept {
    return MathUtils::GetRandomLessThan(2) == 0;
}

double nCr(const int n, const int k) noexcept {
    return 1.0 / ((n + 1.0) * std::beta(static_cast<double>(n) - k + 1.0, k + 1.0));
}

double Combination(const int n, const int k) noexcept {
    return nCr(n, k);
}

double Combination_multiset(const int n, const int k) noexcept {
    return Combination(n + k - 1, k);
}

float SineWave(float t, float period /*= 1.0f*/, float phase /*= 0.0f*/) noexcept {
    return ((1.0f + std::sin(MathUtils::M_2PI * t * (1.0f / period) + phase)) * 0.5f);
}

float SineWaveDegrees(float t, float period /*= 1.0f*/, float phaseDegrees /*= 0.0f*/) noexcept {
    return SineWave(t, period, MathUtils::ConvertDegreesToRadians(phaseDegrees));
}

float SineWave(float t, TimeUtils::FPSeconds period /*= TimeUtils::FPSeconds{1.0f}*/, float phase /*= 0.0f*/) noexcept {
    return SineWave(t, period.count(), phase);
}

float SineWaveDegrees(float t, TimeUtils::FPSeconds period /*= TimeUtils::FPSeconds{1.0f}*/, float phaseDegrees /*= 0.0f*/) noexcept {
    return SineWaveDegrees(t, period.count(), phaseDegrees);
}

float SinCos(float sin, float cos) {
    return std::sin(sin) * std::cos(cos);
}

float CosSin(float cos, float sin) {
    return std::cos(cos) * std::sin(sin);
}

float SinCosDegrees(float sinDegrees, float cosDegrees) {
    return SinDegrees(sinDegrees) * CosDegrees(cosDegrees);
}

float CosSinDegrees(float cosDegrees, float sinDegrees) {
    return CosDegrees(cosDegrees) * SinDegrees(sinDegrees);
}

float CosDegrees(float degrees) noexcept {
    const auto radians = MathUtils::ConvertDegreesToRadians(degrees);
    return std::cos(radians);
}

float SinDegrees(float degrees) noexcept {
    const auto radians = MathUtils::ConvertDegreesToRadians(degrees);
    return std::sin(radians);
}

float Atan2Degrees(float y, float x) noexcept {
    const auto radians = std::atan2(y, x);
    return MathUtils::ConvertRadiansToDegrees(radians);
}

bool IsEquivalent(float a, float b, float epsilon /*= 0.00001f*/) noexcept {
    return std::abs(a - b) < epsilon;
}

bool IsEquivalent(double a, double b, double epsilon /*= 0.0001*/) noexcept {
    return std::abs(a - b) < epsilon;
}

bool IsEquivalent(long double a, long double b, long double epsilon /*= 0.0001L*/) noexcept {
    return std::abs(a - b) < epsilon;
}

bool IsEquivalent(const Vector2& a, const Vector2& b, float epsilon /*= 0.0001f*/) noexcept {
    return IsEquivalent(a.x, b.x, epsilon) && IsEquivalent(a.y, b.y, epsilon);
}

bool IsEquivalent(const Vector3& a, const Vector3& b, float epsilon /*= 0.0001f*/) noexcept {
    return IsEquivalent(a.x, b.x, epsilon) && IsEquivalent(a.y, b.y, epsilon) && IsEquivalent(a.z, b.z, epsilon);
}

bool IsEquivalent(const Vector4& a, const Vector4& b, float epsilon /*= 0.0001f*/) noexcept {
    return IsEquivalent(a.x, b.x, epsilon) && IsEquivalent(a.y, b.y, epsilon) && IsEquivalent(a.z, b.z, epsilon) && IsEquivalent(a.w, b.w, epsilon);
}

bool IsEquivalent(const Quaternion& a, const Quaternion& b, float epsilon /*= 0.0001f*/) noexcept {
    return IsEquivalent(a.w, b.w, epsilon) && IsEquivalent(a.axis, b.axis, epsilon);
}

bool IsEquivalent(const Rotator& a, const Rotator& b, float epsilon /*= 0.0001f*/) noexcept {
    return IsEquivalent(a.pitch, b.pitch, epsilon) && IsEquivalent(a.yaw, b.yaw, epsilon) && IsEquivalent(a.roll, b.roll, epsilon);
}

bool IsEquivalentOrLessThan(float a, float b, float epsilon /*= 0.00001f*/) noexcept {
    return a < b || IsEquivalent(a, b, epsilon);
}

bool IsEquivalentOrLessThan(double a, double b, double epsilon /*= 0.0001*/) noexcept {
    return a < b || IsEquivalent(a, b, epsilon);
}

bool IsEquivalentOrLessThan(long double a, long double b, long double epsilon /*= 0.0001L*/) noexcept {
    return a < b || IsEquivalent(a, b, epsilon);
}

bool IsEquivalentOrLessThanZero(float a, float epsilon /*= 0.00001f*/) noexcept {
    return IsEquivalentOrLessThan(a, 0.0f, epsilon);
}

bool IsEquivalentOrLessThanZero(double a, double epsilon /*= 0.0001*/) noexcept {
    return IsEquivalentOrLessThan(a, 0.0, epsilon);
}

bool IsEquivalentOrLessThanZero(long double a, long double epsilon /*= 0.0001L*/) noexcept {
    return IsEquivalentOrLessThan(a, 0.0L, epsilon);
}

bool IsEquivalentToZero(float a, float epsilon /*= 0.00001f*/) noexcept {
    return IsEquivalent(a, 0.0f, epsilon);
}

bool IsEquivalentToZero(double a, double epsilon /*= 0.0001*/) noexcept {
    return IsEquivalent(a, 0.0, epsilon);
}

bool IsEquivalentToZero(long double a, long double epsilon /*= 0.0001L*/) noexcept {
    return IsEquivalent(a, 0.0L, epsilon);
}

bool IsEquivalentToZero(const Vector2& a, float epsilon /*= 0.0001f*/) noexcept {
    return IsEquivalent(a, Vector2::Zero, epsilon);
}

bool IsEquivalentToZero(const Vector3& a, float epsilon /*= 0.0001f*/) noexcept {
    return IsEquivalent(a, Vector3::Zero, epsilon);
}

bool IsEquivalentToZero(const Vector4& a, float epsilon /*= 0.0001f*/) noexcept {
    return IsEquivalent(a, Vector4::Zero, epsilon);
}

bool IsEquivalentToZero(const Quaternion& a, float epsilon /*= 0.0001f*/) noexcept {
    return IsEquivalent(a, Quaternion::I, epsilon);
}

bool IsEquivalentToZero(const Rotator& a, float epsilon /*= 0.0001f*/) noexcept {
    return IsEquivalent(a, Rotator::Zero, epsilon);
}

float CalcDistance(const Vector2& a, const Vector2& b) noexcept {
    return (b - a).CalcLength();
}

float CalcDistance(const Vector3& a, const Vector3& b) noexcept {
    return (b - a).CalcLength();
}

float CalcDistance(const Vector4& a, const Vector4& b) noexcept {
    return (b - a).CalcLength4D();
}

float CalcDistance(const Vector2& p, const LineSegment2& line) noexcept {
    return std::sqrt(CalcDistanceSquared(p, line));
}

float CalcDistance(const Vector3& p, const LineSegment3& line) noexcept {
    return std::sqrt(CalcDistanceSquared(p, line));
}

float CalcDistance(const Vector2& p, const Polygon2& poly2) noexcept {
    return std::sqrt(CalcDistanceSquared(p, poly2));
}

float CalcDistance(const LineSegment2& line, const Polygon2& poly2) noexcept {
    return CalcDistance(poly2, line);
}

float CalcDistance(const Polygon2& poly2, const LineSegment2& line) noexcept {
    return std::sqrt(CalcDistanceSquared(poly2, line));
}

float CalcDistance(const LineSegment2& lineA, const LineSegment2& lineB) noexcept {
    return std::sqrt(CalcDistanceSquared(lineA, lineB));
}

float CalcDistance4D(const Vector4& a, const Vector4& b) noexcept {
    return (b - a).CalcLength4D();
}

float CalcDistance3D(const Vector4& a, const Vector4& b) noexcept {
    return (b - a).CalcLength3D();
}

float CalcDistanceSquared(const Vector2& a, const Vector2& b) noexcept {
    return (b - a).CalcLengthSquared();
}

float CalcDistanceSquared(const Vector3& a, const Vector3& b) noexcept {
    return (b - a).CalcLengthSquared();
}

float CalcDistanceSquared(const Vector4& a, const Vector4& b) noexcept {
    return (b - a).CalcLength4DSquared();
}

float CalcDistanceSquared(const Vector2& p, const LineSegment2& line) noexcept {
    return CalcDistanceSquared(p, CalcClosestPoint(p, line));
}

float CalcDistanceSquared(const Vector3& p, const LineSegment3& line) noexcept {
    return CalcDistanceSquared(p, CalcClosestPoint(p, line));
}

float CalcDistanceSquared(const Vector2& p, const Polygon2& poly2) noexcept {
    return CalcDistanceSquared(p, CalcClosestPoint(p, poly2));
}

float CalcDistanceSquared(const LineSegment2& line, const Polygon2& poly2) noexcept {
    return CalcDistanceSquared(poly2, line);
}

float CalcDistanceSquared(const Polygon2& poly2, const LineSegment2& line) noexcept {
    const auto& edges = poly2.GetEdges();

    auto smallest_distance_sq = std::numeric_limits<float>::infinity();
    for(const auto& edge : edges) {
        const auto new_dist = CalcDistanceSquared(edge, line);
        if(new_dist < smallest_distance_sq) {
            smallest_distance_sq = new_dist;
        }
    }
    return smallest_distance_sq;
}

float CalcDistanceSquared(const LineSegment2& lineA, const LineSegment2& lineB) noexcept {
    //https://stackoverflow.com/a/11427699/421178

    const auto a_len_sq = lineA.CalcLengthSquared();
    const auto b_len_sq = lineB.CalcLengthSquared();
    { //Early-out if either or both segments are too short to consider as segments.
        const auto is_a_len_sq_zero = IsEquivalentToZero(a_len_sq);
        const auto is_b_len_sq_zero = IsEquivalentToZero(b_len_sq);
        //Both "segments" are actually points...
        if(is_a_len_sq_zero && is_b_len_sq_zero) {
            return CalcDistanceSquared(lineA.start, lineB.start);
        }
        //Else either one is a point.
        if(is_a_len_sq_zero) {
            return CalcDistanceSquared(lineA.start, lineB);
        }
        if(is_b_len_sq_zero) {
            return CalcDistanceSquared(lineB.start, lineA);
        }
    }

    //Actual segment-segment distance...
    if(DoLineSegmentOverlap(lineA, lineB)) {
        return 0.0f;
    }

    const auto a1 = lineA.start;
    const auto a2 = lineA.end;
    const auto b1 = lineB.start;
    const auto b2 = lineB.end;
    const auto distances = std::array<float, 4>{CalcDistanceSquared(a1, lineB), CalcDistanceSquared(a2, lineB), CalcDistanceSquared(b1, lineA), CalcDistanceSquared(b2, lineA)};

    return *std::min_element(std::cbegin(distances), std::cend(distances));
}

float CalcDistanceSquared4D(const Vector4& a, const Vector4& b) noexcept {
    return CalcDistanceSquared(a, b);
}

float CalcDistanceSquared3D(const Vector4& a, const Vector4& b) noexcept {
    return (b - a).CalcLength3DSquared();
}

float DotProduct(const Vector2& a, const Vector2& b) noexcept {
    return a.x * b.x + a.y * b.y;
}

float DotProduct(const Vector3& a, const Vector3& b) noexcept {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float DotProduct(const Vector4& a, const Vector4& b) noexcept {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float DotProduct(const Quaternion& a, const Quaternion& b) noexcept {
    return (a.w * b.w) + DotProduct(a.axis, b.axis);
}

float CrossProduct(const Vector2& a, const Vector2& b) noexcept {
    const auto a1 = a.x;
    const auto a2 = a.y;

    const auto b1 = b.x;
    const auto b2 = b.y;

    return a1 * b2 - a2 * b1;
}

Vector3 CrossProduct(const Vector3& a, const Vector3& b) noexcept {
    const auto a1 = a.x;
    const auto a2 = a.y;
    const auto a3 = a.z;

    const auto b1 = b.x;
    const auto b2 = b.y;
    const auto b3 = b.z;

    return Vector3(a2 * b3 - a3 * b2, a3 * b1 - a1 * b3, a1 * b2 - a2 * b1);
}

float TripleProductScalar(const Vector3& a, const Vector3& b, const Vector3& c) noexcept {
    return DotProduct(a, CrossProduct(b, c));
}

Vector2 TripleProductVector(const Vector2& a, const Vector2& b, const Vector2& c) noexcept {
    //(A x B) x C = B * (A . C) - C * (A . B);
    return (b * DotProduct(a, c)) - (c * DotProduct(a, b));
}

Vector3 TripleProductVector(const Vector3& a, const Vector3& b, const Vector3& c) noexcept {
    return CrossProduct(CrossProduct(a, b), c);
}

Vector2 Project(const Vector2& a, const Vector2& b) noexcept {
    return (DotProduct(a, b) / DotProduct(b, b)) * b;
}

Vector3 Project(const Vector3& a, const Vector3& b) noexcept {
    return (DotProduct(a, b) / DotProduct(b, b)) * b;
}

Vector4 Project(const Vector4& a, const Vector4& b) noexcept {
    return (DotProduct(a, b) / DotProduct(b, b)) * b;
}

Vector2 Reject(const Vector2& a, const Vector2& b) noexcept {
    const auto proj_aB = Project(a, b);
    return a - proj_aB;
}

Vector3 Reject(const Vector3& a, const Vector3& b) noexcept {
    const auto proj_aB = Project(a, b);
    return a - proj_aB;
}

Vector4 Reject(const Vector4& a, const Vector4& b) noexcept {
    const auto proj_aB = Project(a, b);
    return a - proj_aB;
}

std::pair<Vector2, Vector2> DivideIntoProjectAndReject(const Vector2& a, const Vector2& b) noexcept {
    const auto proj = Project(a, b);
    return std::make_pair(proj, a - proj);
}
std::pair<Vector3, Vector3> DivideIntoProjectAndReject(const Vector3& a, const Vector3& b) noexcept {
    const auto proj = Project(a, b);
    return std::make_pair(proj, a - proj);
}
std::pair<Vector4, Vector4> DivideIntoProjectAndReject(const Vector4& a, const Vector4& b) noexcept {
    const auto proj = Project(a, b);
    return std::make_pair(proj, a - proj);
}

Vector2 Reflect(const Vector2& in, const Vector2& normal) noexcept {
    return in - ((2.0f * DotProduct(in, normal)) * normal);
}

Vector3 Reflect(const Vector3& in, const Vector3& normal) noexcept {
    return in - ((2.0f * DotProduct(in, normal)) * normal);
}

Vector4 Reflect(const Vector4& in, const Vector4& normal) noexcept {
    return in - ((2.0f * DotProduct(in, normal)) * normal);
}

Vector2 Rotate(const Vector2& v, const Quaternion& q) noexcept {
    return Vector2(Rotate(Vector3(v, 0.0f), q));
}

Vector3 Rotate(const Vector3& v, const Quaternion& q) noexcept {
    return (q * v * q.CalcInverse()).axis;
}

Vector2 ProjectAlongPlane(const Vector2& v, const Vector2& n) noexcept {
    return v - (DotProduct(v, n) * n);
}

Vector3 ProjectAlongPlane(const Vector3& v, const Vector3& n) noexcept {
    return v - (DotProduct(v, n) * n);
}

Vector4 ProjectAlongPlane(const Vector4& v, const Vector4& n) noexcept {
    return v - (DotProduct(v, n) * n);
}

unsigned int CalculateManhattanDistance(const IntVector2& start, const IntVector2& end) noexcept {
    return std::abs(end.x - start.x) + std::abs(end.y - start.y);
}

unsigned int CalculateManhattanDistance(const IntVector3& start, const IntVector3& end) noexcept {
    return std::abs(end.x - start.x) + std::abs(end.y - start.y) + std::abs(end.z - start.z);
}

unsigned int CalculateManhattanDistance(const IntVector4& start, const IntVector4& end) noexcept {
    return std::abs(end.x - start.x) + std::abs(end.y - start.y) + std::abs(end.z - start.z) + std::abs(end.w - start.w);
}

unsigned int CalculateChebyshevDistance(const IntVector2& start, const IntVector2& end) noexcept {
    return (std::max)({std::abs(end.x - start.x), std::abs(end.y - start.y)});
}

unsigned int CalculateChebyshevDistance(const IntVector3& start, const IntVector3& end) noexcept {
    return (std::max)({std::abs(end.x - start.x), std::abs(end.y - start.y), std::abs(end.z - start.z)});
}

unsigned int CalculateChebyshevDistance(const IntVector4& start, const IntVector4& end) noexcept {
    return (std::max)({std::abs(end.x - start.x), std::abs(end.y - start.y), std::abs(end.z - start.z), std::abs(end.w - start.w)});
}

unsigned int CalculateChessboardDistance(const IntVector2& start, const IntVector2& end) noexcept {
    return CalculateChebyshevDistance(start, end);
}
unsigned int CalculateChessboardDistance(const IntVector3& start, const IntVector3& end) noexcept {
    return CalculateChebyshevDistance(start, end);
}
unsigned int CalculateChessboardDistance(const IntVector4& start, const IntVector4& end) noexcept {
    return CalculateChebyshevDistance(start, end);
}

Vector2 GetRandomPointOn(const AABB2& aabb) noexcept {
    float result[2]{0.0f, 0.0f};
    const auto s = MathUtils::GetRandomLessThan(4);
    const auto c = s % 2;
    result[(c + 0) % 2] = s > 1 ? 1.0f : 0.0f;
    result[(c + 1) % 2] = s > 1 ? 1.0f : 0.0f;
    Vector2 point{result[0], result[1]};
    return aabb.CalcCenter() + (point * aabb.CalcDimensions());
}

Vector2 GetRandomPointOn(const Disc2& disc) noexcept {
    Vector2 point{};
    point.SetLengthAndHeadingDegrees(MathUtils::GetRandomZeroToOne<float>() * 360.0f, disc.radius);
    return disc.center + point;
}

Vector2 GetRandomPointOn(const LineSegment2& line) noexcept {
    const auto dir = line.CalcDirection();
    const auto len = line.CalcLength() * MathUtils::GetRandomZeroToOne<float>();
    return line.start + (dir * len);
}

Vector3 GetRandomPointOn(const AABB3& aabb) noexcept {
    float result[3]{0.0f, 0.0f, 0.0f};
    const auto s = MathUtils::GetRandomLessThan(6);
    const auto c = s % 3;
    result[(c + 0) % 3] = s > 2 ? 1.0f : 0.0f;
    result[(c + 1) % 3] = MathUtils::GetRandomZeroToOne<float>();
    result[(c + 2) % 3] = MathUtils::GetRandomZeroToOne<float>();
    Vector3 point{result[0], result[1], result[2]};
    return aabb.CalcCenter() + (point * aabb.CalcDimensions());
}

Vector3 GetRandomPointOn(const Sphere3& sphere) noexcept {
    //See: https://karthikkaranth.me/blog/generating-random-points-in-a-sphere/
    const auto u = MathUtils::GetRandomZeroToOne<float>();
    const auto v = MathUtils::GetRandomZeroToOne<float>();
    const auto theta_angle = MathUtils::M_2PI * u;
    const auto phi_angle = std::acos(2.0f * v - 1.0f);
    const auto r = sphere.radius;
    const auto sin_theta = std::sin(theta_angle);
    const auto cos_theta = std::cos(theta_angle);
    const auto sin_phi = std::sin(phi_angle);
    const auto cos_phi = std::cos(phi_angle);
    const auto x = r * sin_phi * cos_theta;
    const auto y = r * sin_phi * sin_theta;
    const auto z = r * cos_phi;
    return sphere.center + Vector3{x, y, z};
}

Vector3 GetRandomPointOn(const LineSegment3& line) noexcept {
    const auto dir = line.CalcDirection();
    const auto len = line.CalcLength() * MathUtils::GetRandomZeroToOne<float>();
    return line.start + (dir * len);
}

Vector2 GetRandomPointInside(const AABB2& aabb) noexcept {
    return Vector2{MathUtils::GetRandomInRange(aabb.mins.x, aabb.maxs.x), MathUtils::GetRandomInRange(aabb.mins.y, aabb.maxs.y)};
}

Vector2 GetRandomPointInside(const Disc2& disc) noexcept {
    Vector2 point{};
    point.SetLengthAndHeadingDegrees(MathUtils::GetRandomZeroToOne<float>() * 360.0f, std::sqrt(MathUtils::GetRandomZeroToOne<float>()) * disc.radius);
    return disc.center + point;
}

Vector3 GetRandomPointInside(const AABB3& aabb) noexcept {
    return Vector3{MathUtils::GetRandomInRange(aabb.mins.x, aabb.maxs.x), MathUtils::GetRandomInRange(aabb.mins.y, aabb.maxs.y), MathUtils::GetRandomInRange(aabb.mins.z, aabb.maxs.z)};
}

Vector3 GetRandomPointInside(const Sphere3& sphere) noexcept {
    //See: https://karthikkaranth.me/blog/generating-random-points-in-a-sphere/
    const auto u = MathUtils::GetRandomZeroToOne<float>();
    const auto v = MathUtils::GetRandomZeroToOne<float>();
    const auto theta_angle = MathUtils::M_2PI * u;
    const auto phi_angle = std::acos(2.0f * v - 1.0f);
    const auto r = sphere.radius * std::pow(MathUtils::GetRandomZeroToOne<float>(), 1.0f / 3.0f);
    const auto sin_theta = std::sin(theta_angle);
    const auto cos_theta = std::cos(theta_angle);
    const auto sin_phi = std::sin(phi_angle);
    const auto cos_phi = std::cos(phi_angle);
    const auto x = r * sin_phi * cos_theta;
    const auto y = r * sin_phi * sin_theta;
    const auto z = r * cos_phi;
    return sphere.center + Vector3{x, y, z};
}

Vector3 GetRandomPointInsideDisc(const Vector3& position, const Vector3& normal, float radius) noexcept {
    return GetRandomPointInsidePlane(Plane3(normal, position.CalcLength()), radius);
}

Vector2 GetRandomPointInsidePlane(const Plane2& p, float r) noexcept {
    return Vector2(GetRandomPointInsidePlane(Plane3(Vector3(p.normal, 0.0f), p.dist), r));
}

Vector3 GetRandomPointInsidePlane(const Plane3& p, float r) noexcept {
    Vector3 n = p.normal;
    Vector3 w;
    if(n.x == 0.0f) {
        w = CrossProduct(n, Vector3(1.0f, 0.0f, 0.0f));
    } else {
        w = CrossProduct(n, Vector3(0.0f, 0.0f, 1.0f));
    }

    // rotate the vector around n by a random angle
    // using Rodrigues' rotation formula
    // http://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
    float theta = MathUtils::GetRandomZeroToOne<float>() * MathUtils::pi_v<float>;
    Vector3 k = n.GetNormalize();
    w = w * std::cos(theta) + CrossProduct(k, w) * std::sin(theta);

    // Scale the vector fill our disk.
    // If the radius is zero, generate unit vectors
    if(r == 0.0f) {
        w *= (r / w.CalcLength());
    } else {
        r = MathUtils::GetRandomZeroToOne<float>() * r;
        w *= (r / w.CalcLength());
    }

    // now translate the vector from ax + by + cz = 0
    // to the plane ax + by + cz = d
    // http://en.wikipedia.org/wiki/Distance_from_a_point_to_a_plane
    float d = p.dist;
    if(d != 0.0f) {
        Vector3 t = n * (d / MathUtils::DotProduct(n, n));
        w += t;
    }

    return w;
}

Vector2 GetRandomPointInsidePlane(const Vector2& pos, const Vector2& normal, float r) noexcept {
    return GetRandomPointInsidePlane(Plane2(normal, pos.CalcLength()), r);
}

Vector3 GetRandomPointInsidePlane(const Vector3& pos, const Vector3& normal, float r) noexcept {
    return GetRandomPointInsidePlane(Plane3(normal, pos.CalcLength()), r);
}

Vector3 GetRandomPointInsideCube(float radius) noexcept {
    float x = GetRandomNegOneToOne<float>() * radius;
    float y = GetRandomNegOneToOne<float>() * radius;
    float z = GetRandomNegOneToOne<float>() * radius;
    return Vector3(x, y, z);
}

Vector3 GetRandomPointInsideSphere(float radius) noexcept {
    Vector3 p = GetRandomPointInsideCube(radius);
    while(p.CalcLength() > radius) {
        p = GetRandomPointInsideCube(radius);
    }
    return p;
}

bool Contains(const AABB2& aabb, const Vector2& point) noexcept {
    return IsPointInside(aabb, point);
}

bool Contains(const AABB2& a, const AABB2& b) noexcept {
    const auto tl = Vector2{b.mins.x, b.mins.y};
    const auto tr = Vector2{b.maxs.x, b.mins.y};
    const auto bl = Vector2{b.mins.x, b.maxs.y};
    const auto br = Vector2{b.maxs.x, b.maxs.y};
    return IsPointInside(a, tl) && IsPointInside(a, tr) && IsPointInside(a, bl) && IsPointInside(a, br);
}

bool Contains(const AABB2& a, const OBB2& b) noexcept {
    return Contains(a, AABB2(b));
}

bool Contains(const OBB2& a, const AABB2& b) noexcept {
    return Contains(AABB2(a), b);
}

bool Contains(const OBB2& a, const OBB2& b) noexcept {
    return Contains(AABB2(a), AABB2(b));
}

bool Contains(const Disc2& a, const AABB2& b) noexcept {
    const auto tl = Vector2{b.mins.x, b.mins.y};
    const auto tr = Vector2{b.maxs.x, b.mins.y};
    const auto bl = Vector2{b.mins.x, b.maxs.y};
    const auto br = Vector2{b.maxs.x, b.maxs.y};
    return IsPointInside(a, tl) && IsPointInside(a, tr) && IsPointInside(a, bl) && IsPointInside(a, br);
}

bool IsPointInside(const AABB2& aabb, const Vector2& point) noexcept {
    if(aabb.maxs.x < point.x)
        return false;
    if(point.x < aabb.mins.x)
        return false;
    if(aabb.maxs.y < point.y)
        return false;
    if(point.y < aabb.mins.y)
        return false;
    return true;
}

bool IsPointInside(const AABB3& aabb, const Vector3& point) noexcept {
    if(aabb.maxs.x < point.x)
        return false;
    if(point.x < aabb.mins.x)
        return false;
    if(aabb.maxs.y < point.y)
        return false;
    if(point.y < aabb.mins.y)
        return false;
    if(aabb.maxs.z < point.z)
        return false;
    if(point.z < aabb.mins.z)
        return false;
    return true;
}

bool IsPointInside(const OBB2& obb, const Vector2& point) noexcept {
    return DoOBBsOverlap(obb, {point, 0.0f});
}

bool IsPointInside(const Disc2& disc, const Vector2& point) noexcept {
    return CalcDistanceSquared(disc.center, point) < (disc.radius * disc.radius);
}

bool IsPointInside(const Capsule2& capsule, const Vector2& point) noexcept {
    return CalcDistanceSquared(point, capsule.line) < (capsule.radius * capsule.radius);
}

bool IsPointInside(const Polygon2& poly2, const Vector2& point) noexcept {
    if(!IsPointInside(poly2.GetBounds(), point)) {
        return false;
    }
    const auto& verts = poly2.GetVerts();
    const auto pointCount = verts.size();

    // Crossing Number Test and Winding Number Test
    // Copyright 2000 softSurfer, 2012 Dan Sunday
    // This code may be freely used and modified for any purpose
    // providing that this copyright notice is included with it.
    // SoftSurfer makes no warranty for this code, and cannot be held
    // liable for any real or imagined damage resulting from its use.
    // Users of this code must verify correctness for their application.

#if 0
    //Crossing Number Test
    const auto crossing_inside = [&]()
    {
        PROFILE_LOG_SCOPE("Crossing Inside");
        int cn = 0; // the  crossing number counter
        const auto n = pointCount;
        // loop through all edges of the polygon
        for(std::size_t i = 0u; i < n - 1u; i++) {                         // edge from V[i]  to V[i+1]
            if(((verts[i].y <= point.y) && (verts[i + 1u].y > point.y))       // an upward crossing
               || ((verts[i].y > point.y) && (verts[i + 1u].y <= point.y))) { // a downward crossing
                // compute the actual edge-ray intersect x-coordinate
                float vt = static_cast<float>(point.y - verts[i].y) / (verts[i + 1u].y - verts[i].y);
                if(point.x < verts[i].x + vt * (verts[i + 1u].x - verts[i].x)) // point.x < intersect
                    ++cn;                                     // a valid crossing of y=point.y right of point.x
            }
        }
        return (cn & 1) == 1; // 0 if even (out), and 1 if  odd (in)
    }();
#endif
    //Winding Number Test
    const auto winding_inside = [&]() {
        PROFILE_LOG_SCOPE("Winding Inside");
        int wn = 0; // the  winding number counter
        const auto isLeft = [&](const Vector2& P0, const Vector2& P1, const Vector2& P2) {
            return ((P1.x - P0.x) * (P2.y - P0.y)
                    - (P2.x - P0.x) * (P1.y - P0.y));
        };
        const auto n = pointCount;
        // loop through all edges of the polygon
        for(std::size_t i = 0u; i < n - 1u; i++) {                 // edge from V[i] to  V[i+1]
            if(verts[i].y <= point.y) {                            // start y <= point.y
                if(verts[i + 1u].y > point.y)                      // an upward crossing
                    if(isLeft(verts[i], verts[i + 1u], point) > 0) // P left of  edge
                        ++wn;                                      // have  a valid up intersect
            } else {                                               // start y > point.y (no test needed)
                if(verts[i + 1u].y <= point.y)                     // a downward crossing
                    if(isLeft(verts[i], verts[i + 1u], point) < 0) // P right of  edge
                        --wn;                                      // have  a valid down intersect
            }
        }
        return wn != 0;
    }();
    return winding_inside;
}

bool IsPointInside(const Sphere3& sphere, const Vector3& point) noexcept {
    return CalcDistanceSquared(sphere.center, point) < (sphere.radius * sphere.radius);
}

bool IsPointInside(const Capsule3& capsule, const Vector3& point) noexcept {
    return CalcDistanceSquared(point, capsule.line) < (capsule.radius * capsule.radius);
}

bool IsPointOn(const Disc2& disc, const Vector2& point) noexcept {
    const auto distanceSquared = CalcDistanceSquared(disc.center, point);
    const auto radiusSquared = disc.radius * disc.radius;
    return !(distanceSquared < radiusSquared || radiusSquared < distanceSquared);
}

bool IsPointOn(const LineSegment2& line, const Vector2& point) noexcept {
    return IsEquivalentToZero(CalcDistanceSquared(point, line));
}

bool IsPointOn(const Capsule2& capsule, const Vector2& point) noexcept {
    const auto distanceSquared = CalcDistanceSquared(point, capsule.line);
    const auto radiusSquared = capsule.radius * capsule.radius;
    return !(distanceSquared < radiusSquared || radiusSquared < distanceSquared);
}

bool IsPointOn(const LineSegment3& line, const Vector3& point) noexcept {
    return IsEquivalentToZero(CalcDistanceSquared(point, line));
}

bool IsPointOn(const Sphere3& sphere, const Vector3& point) noexcept {
    const auto distanceSquared = CalcDistanceSquared(sphere.center, point);
    const auto radiusSquared = sphere.radius * sphere.radius;
    return !(distanceSquared < radiusSquared || radiusSquared < distanceSquared);
}

bool IsPointOn(const Capsule3& capsule, const Vector3& point) noexcept {
    const auto distanceSquared = CalcDistanceSquared(point, capsule.line);
    const auto radiusSquared = capsule.radius * capsule.radius;
    return !(distanceSquared < radiusSquared || radiusSquared < distanceSquared);
}

bool IsPointOn(const Polygon2& poly2, const Vector2& point) noexcept {
    const auto edges = poly2.GetEdges();
    for(const auto& edge : edges) {
        if(IsPointOn(edge, point)) {
            return true;
        }
    }
    return false;
}

Vector2 CalcClosestPoint(const Vector2& p, const AABB2& aabb) noexcept {
    if(IsPointInside(aabb, p)) {
        return p;
    }
    if(p.x < aabb.mins.x && aabb.maxs.y < p.y) {
        return Vector2(aabb.mins.x, aabb.maxs.y);
    }
    if(p.x < aabb.mins.x && p.y < aabb.mins.y) {
        return Vector2(aabb.mins.x, aabb.mins.y);
    }
    if(aabb.maxs.x < p.x && p.y < aabb.mins.y) {
        return Vector2(aabb.maxs.x, aabb.mins.y);
    }
    if(aabb.maxs.x < p.x && aabb.maxs.y < p.y) {
        return Vector2(aabb.maxs.x, aabb.maxs.y);
    }
    if(p.x < aabb.mins.x) {
        return Vector2(aabb.mins.x, p.y);
    }
    if(aabb.maxs.x < p.x) {
        return Vector2(aabb.maxs.x, p.y);
    }
    if(p.y < aabb.mins.y) {
        return Vector2(p.x, aabb.mins.y);
    }
    if(aabb.maxs.y < p.y) {
        return Vector2(p.x, aabb.maxs.y);
    }
    return Vector2::Zero;
}

Vector2 CalcClosestPoint(const Vector2& p, const OBB2& obb) noexcept {
    if(IsPointInside(obb, p)) {
        return p;
    }
    const auto displacement = p - obb.position;
    const auto T = Matrix4::CreateTranslationMatrix(obb.position);
    const auto R = Matrix4::Create2DRotationDegreesMatrix(obb.orientationDegrees);
    const auto M = Matrix4::MakeRT(R, T);
    const auto u0 = M.TransformDirection(Vector2(obb.half_extents.x, 0.0f).GetNormalize());
    const auto u1 = M.TransformDirection(Vector2(0.0f, obb.half_extents.y).GetNormalize());

    auto x_distance = MathUtils::DotProduct(u0, displacement);
    if(x_distance > obb.half_extents.x) {
        x_distance = obb.half_extents.x;
    }
    if(x_distance < -obb.half_extents.x) {
        x_distance = -obb.half_extents.x;
    }
    auto y_distance = MathUtils::DotProduct(u1, displacement);
    if(y_distance > obb.half_extents.y) {
        y_distance = obb.half_extents.y;
    }
    if(y_distance < -obb.half_extents.y) {
        y_distance = -obb.half_extents.y;
    }
    return M.TransformPosition(Vector2{x_distance, y_distance});
}

Vector3 CalcClosestPoint(const Vector3& p, const AABB3& aabb) noexcept {
    const auto nearestX = std::clamp(p.x, aabb.mins.x, aabb.maxs.x);
    const auto nearestY = std::clamp(p.y, aabb.mins.y, aabb.maxs.y);
    const auto nearestZ = std::clamp(p.z, aabb.mins.z, aabb.maxs.z);

    return Vector3(nearestX, nearestY, nearestZ);
}

Vector2 CalcClosestPoint(const Vector2& p, const Polygon2& poly2) noexcept {
    if(MathUtils::IsPointInside(poly2, p)) {
        return p;
    }
    const auto& edges = poly2.GetEdges();
    const auto iter = std::min_element(std::cbegin(edges), std::cend(edges), [&](const LineSegment2& a, const LineSegment2& b) {
        return MathUtils::CalcDistanceSquared(p, a) < MathUtils::CalcDistanceSquared(p, b);
    });
    return MathUtils::CalcClosestPoint(p, *iter);
}

Vector2 CalcClosestPoint(const Vector2& p, const Disc2& disc) noexcept {
    const auto dir = (p - disc.center).GetNormalize();
    return disc.center + dir * disc.radius;
}

Vector2 CalcClosestPoint(const Vector2& p, const LineSegment2& line) noexcept {
    const auto lineDisplacement = line.end - line.start;
    const auto lineDirection = lineDisplacement.GetNormalize();

    const auto lineSP = p - line.start;
    const auto regionI = MathUtils::DotProduct(lineDirection, lineSP);
    if(regionI < 0.0f) {
        return line.start;
    }

    const auto lineEP = p - line.end;
    const auto regionII = MathUtils::DotProduct(lineDirection, lineEP);
    if(regionII > 0.0f) {
        return line.end;
    }

    const auto lengthToClosestPoint = MathUtils::DotProduct(lineDirection, lineSP);
    const auto C = lineDirection * lengthToClosestPoint;
    const auto ConL = line.start + C;
    return ConL;
}

Vector2 CalcClosestPoint(const Vector2& p, const Capsule2& capsule) noexcept {
    const auto closestP = CalcClosestPoint(p, capsule.line);
    const auto dir_to_p = (p - closestP).GetNormalize();
    return closestP + (dir_to_p * capsule.radius);
}

Vector3 CalcClosestPoint(const Vector3& p, const LineSegment3& line) noexcept {
    const auto D = line.end - line.start;
    const auto T = D.GetNormalize();
    const auto SP = p - line.start;
    float regionI = MathUtils::DotProduct(T, SP);
    if(regionI < 0.0f) {
        return line.start;
    }

    const auto EP = p - line.end;
    const auto regionII = MathUtils::DotProduct(T, EP);
    if(regionII > 0.0f) {
        return line.end;
    }

    const auto directionSE = D.GetNormalize();
    const auto lengthToClosestPoint = MathUtils::DotProduct(directionSE, SP);
    const auto C = directionSE * lengthToClosestPoint;
    const auto ConL = line.start + C;
    return ConL;
}

Vector3 CalcClosestPoint(const Vector3& p, const Sphere3& sphere) noexcept {
    const auto dir = (p - sphere.center).GetNormalize();
    return sphere.center + dir * sphere.radius;
}

Vector3 CalcClosestPoint(const Vector3& p, const Capsule3& capsule) noexcept {
    const auto closestP = CalcClosestPoint(p, capsule.line);
    const auto dir_to_p = (p - closestP).GetNormalize();
    return closestP + (dir_to_p * capsule.radius);
}

Vector2 CalcNormalizedPointFromPoint(const Vector2& pos, const AABB2& bounds) noexcept {
    const auto x_norm = RangeMap(pos.x, bounds.mins.x, bounds.maxs.x, 0.0f, 1.0f);
    const auto y_norm = RangeMap(pos.y, bounds.mins.y, bounds.maxs.y, 0.0f, 1.0f);
    return Vector2(x_norm, y_norm);
}

Vector2 CalcPointFromNormalizedPoint(const Vector2& uv, const AABB2& bounds) noexcept {
    const auto x = RangeMap(uv.x, 0.0f, 1.0f, bounds.mins.x, bounds.maxs.x);
    const auto y = RangeMap(uv.y, 0.0f, 1.0f, bounds.mins.y, bounds.maxs.y);
    return Vector2(x, y);
}

Vector2 CalcNormalizedHalfExtentsFromPoint(const Vector2& pos, const AABB2& bounds) noexcept {
    const auto x_norm = RangeMap(pos.x, bounds.mins.x, bounds.maxs.x, -0.5f, 0.5f);
    const auto y_norm = RangeMap(pos.y, bounds.mins.y, bounds.maxs.y, -0.5f, 0.5f);
    return Vector2(x_norm, y_norm);
}

Vector2 CalcPointFromNormalizedHalfExtents(const Vector2& uv, const AABB2& bounds) noexcept {
    const auto x = RangeMap(uv.x, -0.5f, 0.5f, bounds.mins.x, bounds.maxs.x);
    const auto y = RangeMap(uv.y, -0.5f, 0.5f, bounds.mins.y, bounds.maxs.y);
    return Vector2(x, y);
}

AABB2 ScaleToFit(const AABB2& a, const AABB2& b) noexcept {
    const auto [aw, ah] = a.CalcDimensions();
    const auto [bw, bh] = b.CalcDimensions();
    auto scaled_dims = Vector2{aw, ah};
    if(aw > bw || ah > bh) {
        if(aw > bw) {
            scaled_dims.x = bw;
            scaled_dims.y = (scaled_dims.x * ah) / aw;
        }
        if(scaled_dims.y > bh) {
            scaled_dims.y = bh;
            scaled_dims.x = (scaled_dims.y * aw) / ah;
        }
    }
    return AABB2{Vector2::Zero, scaled_dims};
}

AABB2 StretchToFit(const AABB2& a, const AABB2& b) noexcept {
    const auto [aw, ah] = a.CalcDimensions();
    const auto [bw, bh] = b.CalcDimensions();
    auto stretched_dims = Vector2{aw, ah};
    if(bw > aw || bh > ah) {
        if(bw > aw) {
            stretched_dims.x = bw;
            stretched_dims.y = (stretched_dims.x * ah) / aw;
        }
        if(stretched_dims.y > bh) {
            stretched_dims.y = bh;
            stretched_dims.x = (stretched_dims.y * aw) / ah;
        }
    }
    return AABB2{Vector2::Zero, stretched_dims};
}

bool DoDiscsOverlap(const Disc2& a, const Disc2& b) noexcept {
    return DoDiscsOverlap(a.center, a.radius, b.center, b.radius);
}

bool DoDiscsOverlap(const Vector2& centerA, float radiusA, const Vector2& centerB, float radiusB) noexcept {
    return CalcDistanceSquared(centerA, centerB) < (radiusA + radiusB) * (radiusA + radiusB);
}

bool DoDiscsOverlap(const Disc2& a, const Capsule2& b) noexcept {
    return CalcDistanceSquared(a.center, b.line) < (a.radius + b.radius) * (a.radius + b.radius);
}

bool DoDiscsOverlap(const Disc2& a, const AABB2& b) noexcept {
    const auto closest_point = CalcClosestPoint(a.center, b);
    return IsPointInside(a, closest_point);
}

bool DoSpheresOverlap(const Sphere3& a, const Sphere3& b) noexcept {
    return DoSpheresOverlap(a.center, a.radius, b.center, b.radius);
}

bool DoSpheresOverlap(const Vector3& centerA, float radiusA, const Vector3& centerB, float radiusB) noexcept {
    return CalcDistanceSquared(centerA, centerB) < (radiusA + radiusB) * (radiusA + radiusB);
}

bool DoSpheresOverlap(const Sphere3& a, const Capsule3& b) noexcept {
    return CalcDistanceSquared(a.center, b.line) < (a.radius + b.radius) * (a.radius + b.radius);
}

bool DoSpheresOverlap(const Sphere3& a, const AABB3& b) noexcept {
    const auto closest_point = CalcClosestPoint(a.center, b);
    return IsPointInside(a, closest_point);
}

bool DoAABBsOverlap(const AABB2& a, const AABB2& b) noexcept {
    if(a.maxs.x < b.mins.x)
        return false;
    if(b.maxs.x < a.mins.x)
        return false;
    if(a.maxs.y < b.mins.y)
        return false;
    if(b.maxs.y < a.mins.y)
        return false;
    return true;
}

bool DoAABBsOverlap(const AABB3& a, const AABB3& b) noexcept {
    if(a.maxs.x < b.mins.x)
        return false;
    if(b.maxs.x < a.mins.x)
        return false;
    if(a.maxs.y < b.mins.y)
        return false;
    if(b.maxs.y < a.mins.y)
        return false;
    if(a.maxs.z < b.mins.z)
        return false;
    if(b.maxs.z < a.mins.z)
        return false;
    return true;
}

bool DoAABBsOverlap(const AABB2& a, const Disc2& b) noexcept {
    return DoDiscsOverlap(b, a);
}

bool DoAABBsOverlap(const AABB3& a, const Sphere3& b) noexcept {
    return DoSpheresOverlap(b, a);
}

bool DoOBBsOverlap(const OBB2& a, const OBB2& b) noexcept {
    const auto polyA = Polygon2(a);
    const auto polyB = Polygon2(b);
    return DoPolygonsOverlap(polyA, polyB);
}

bool DoPolygonsOverlap(const Polygon2& a, const Polygon2& b) noexcept {
    //Separating Axis Theorem
    for(const auto& an : a.GetNormals()) {
        auto min_a = std::numeric_limits<float>::infinity();
        auto max_a = std::numeric_limits<float>::lowest();
        for(const auto& ac : a.GetVerts()) {
            const auto proj_dp = DotProduct(ac, an);
            min_a = (std::min)(min_a, proj_dp);
            max_a = (std::max)(max_a, proj_dp);
        }

        auto min_b = std::numeric_limits<float>::infinity();
        auto max_b = std::numeric_limits<float>::lowest();
        for(const auto& bc : b.GetVerts()) {
            const auto proj_dp = DotProduct(bc, an);
            min_b = (std::min)(min_b, proj_dp);
            max_b = (std::max)(max_b, proj_dp);
        }

        if(max_a < min_b)
            return false;
        if(max_b < min_a)
            return false;
    }
    for(const auto& bn : b.GetNormals()) {
        auto min_b = std::numeric_limits<float>::infinity();
        auto max_b = std::numeric_limits<float>::lowest();
        for(const auto& bc : b.GetVerts()) {
            const auto proj_dp = DotProduct(bc, bn);
            min_b = (std::min)(min_b, proj_dp);
            max_b = (std::max)(max_b, proj_dp);
        }

        auto min_a = std::numeric_limits<float>::infinity();
        auto max_a = std::numeric_limits<float>::lowest();
        for(const auto& ac : a.GetVerts()) {
            const auto proj_dp = DotProduct(ac, bn);
            min_a = (std::min)(min_a, proj_dp);
            max_a = (std::max)(max_a, proj_dp);
        }

        if(max_b < min_a)
            return false;
        if(max_a < min_b)
            return false;
    }
    return true;
}

std::optional<Vector2> DoLineSegmentOverlap(const LineSegment2& a, const LineSegment2& b) noexcept {
    //https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection

    const auto x1 = a.start.x;
    const auto y1 = a.start.y;
    const auto x2 = a.end.x;
    const auto y2 = a.end.y;
    const auto x3 = b.start.x;
    const auto y3 = b.start.y;
    const auto x4 = b.end.x;
    const auto y4 = b.end.y;
    const auto denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if(MathUtils::IsEquivalentToZero(denominator)) {
        return {};
    }

    const auto tNumerator = (x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4);
    const auto t = tNumerator / denominator;

    if(0.0f <= t && t <= 1.0f) {
        const auto vT = Vector2{x1 + t * (x2 - x1), y1 + t * (y2 - y1)};
        if(MathUtils::IsPointOn(a, vT) && MathUtils::IsPointOn(b, vT)) {
            return std::make_optional(vT);
        }
    }
    return {};
}

bool DoLineSegmentOverlap(const Disc2& a, const LineSegment2& b) noexcept {
    return CalcDistanceSquared(a.center, b) < a.radius * a.radius;
}

bool DoLineSegmentOverlap(const Sphere3& a, const LineSegment3& b) noexcept {
    return CalcDistanceSquared(a.center, b) < a.radius * a.radius;
}

bool DoCapsuleOverlap(const Disc2& a, const Capsule2& b) noexcept {
    return CalcDistanceSquared(a.center, b.line) < (a.radius + b.radius) * (a.radius + b.radius);
}

bool DoCapsuleOverlap(const Sphere3& a, const Capsule3& b) noexcept {
    return CalcDistanceSquared(a.center, b.line) < (a.radius + b.radius) * (a.radius + b.radius);
}

bool DoPlaneOverlap(const Disc2& a, const Plane2& b) noexcept {
    return std::fabs(DotProduct(a.center, b.normal) - b.dist) < a.radius;
}

bool DoPlaneOverlap(const Sphere3& a, const Plane3& b) noexcept {
    return std::fabs(DotProduct(a.center, b.normal) - b.dist) < a.radius;
}

bool DoPlaneOverlap(const Capsule2& a, const Plane2& b) noexcept {
    const auto both_capsule_points_in_front = IsPointInFrontOfPlane(a.line.start, b) && IsPointInFrontOfPlane(a.line.end, b);
    const auto both_capsule_points_in_back = IsPointBehindOfPlane(a.line.start, b) && IsPointBehindOfPlane(a.line.end, b);

    if(both_capsule_points_in_front || both_capsule_points_in_back) {
        return CalcDistanceSquared(Vector2::Zero, a.line) < (a.radius + b.dist) * (a.radius + b.dist);
    }
    return true;
}

bool DoPlaneOverlap(const Capsule3& a, const Plane3& b) noexcept {
    const auto both_capsule_points_in_front = IsPointInFrontOfPlane(a.line.start, b) && IsPointInFrontOfPlane(a.line.end, b);
    const auto both_capsule_points_in_back = IsPointBehindOfPlane(a.line.start, b) && IsPointBehindOfPlane(a.line.end, b);

    if(both_capsule_points_in_front || both_capsule_points_in_back) {
        return CalcDistanceSquared(Vector3::Zero, a.line) < (a.radius + b.dist) * (a.radius + b.dist);
    }
    return true;
}

bool IsPointInFrontOfPlane(const Vector3& point, const Plane3& plane) noexcept {
    return (DotProduct(point, plane.normal) > plane.dist);
}

bool IsPointBehindOfPlane(const Vector3& point, const Plane3& plane) noexcept {
    return (DotProduct(point, plane.normal) < plane.dist);
}

bool IsPointOnPlane(const Vector3& point, const Plane3& plane) noexcept {
    return !IsPointInFrontOfPlane(point, plane) && !IsPointBehindOfPlane(point, plane);
}

bool IsPointInFrontOfPlane(const Vector2& point, const Plane2& plane) noexcept {
    return (DotProduct(point, plane.normal) > plane.dist);
}

bool IsPointBehindOfPlane(const Vector2& point, const Plane2& plane) noexcept {
    return (DotProduct(point, plane.normal) < plane.dist);
}

bool IsPointOnPlane(const Vector2& point, const Plane2& plane) noexcept {
    return !IsPointInFrontOfPlane(point, plane) && !IsPointBehindOfPlane(point, plane);
}

float CalculateMatrix3Determinant(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) noexcept {
    const auto a = m00;
    const auto b = m01;
    const auto c = m02;
    const auto det_not_a = CalculateMatrix2Determinant(m11, m12, m21, m22);
    const auto det_not_b = CalculateMatrix2Determinant(m10, m12, m20, m22);
    const auto det_not_c = CalculateMatrix2Determinant(m10, m11, m20, m21);

    return a * det_not_a - b * det_not_b + c * det_not_c;
}

float CalculateMatrix2Determinant(float m00, float m01, float m10, float m11) noexcept {
    return m00 * m11 - m01 * m10;
}

/************************************************************************/
/* https://en.wikipedia.org/wiki/Slerp#Source_Code                      */
/************************************************************************/
Quaternion SLERP(const Quaternion& a, const Quaternion& b, float t) noexcept {
    Quaternion start = a;
    Quaternion end = b;

    start.Normalize();
    end.Normalize();

    auto dp = MathUtils::DotProduct(start, end);

    if(dp < 0.0f) {
        end = -end;
        dp = -dp;
    }

    //Really close together
    if(dp > 0.99995f) {
        auto result = MathUtils::Interpolate(start, end, t);
        result.Normalize();
        return result;
    }

    dp = std::clamp(dp, -1.0f, 1.0f);

    const auto theta_0 = std::acos(dp);
    const auto theta = theta_0 * t;

    const auto scale0 = std::cos(theta) - dp * std::sin(theta) / std::sin(theta_0);
    const auto scale1 = std::sin(theta) / std::sin(theta_0);

    return (scale0 * start) + (scale1 * end);
}

Vector2 Interpolate(const Vector2& a, const Vector2& b, float t) {
    const auto x = Interpolate(a.x, b.x, t);
    const auto y = Interpolate(a.y, b.y, t);
    return Vector2(x, y);
}

Vector2 InterpolateBezier(const Vector2& a, const Vector2& b, const Vector2& c, float t) {
    const auto x = InterpolateBezier(a.x, b.x, c.x, t);
    const auto y = InterpolateBezier(a.y, b.y, c.y, t);
    return Vector2{x, y};
}

Vector3 Interpolate(const Vector3& a, const Vector3& b, float t) {
    const auto x = Interpolate(a.x, b.x, t);
    const auto y = Interpolate(a.y, b.y, t);
    const auto z = Interpolate(a.z, b.z, t);
    return Vector3(x, y, z);
}

Vector4 Interpolate(const Vector4& a, const Vector4& b, float t) {
    const auto x = Interpolate(a.x, b.x, t);
    const auto y = Interpolate(a.y, b.y, t);
    const auto z = Interpolate(a.z, b.z, t);
    const auto w = Interpolate(a.w, b.w, t);
    return Vector4(x, y, z, w);
}

IntVector2 Interpolate(const IntVector2& a, const IntVector2& b, float t) {
    const auto x = Interpolate(static_cast<float>(a.x), static_cast<float>(b.x), t);
    const auto y = Interpolate(static_cast<float>(a.y), static_cast<float>(b.y), t);
    return IntVector2(Vector2(x, y));
}

IntVector3 Interpolate(const IntVector3& a, const IntVector3& b, float t) {
    const auto x = Interpolate(static_cast<float>(a.x), static_cast<float>(b.x), t);
    const auto y = Interpolate(static_cast<float>(a.y), static_cast<float>(b.y), t);
    const auto z = Interpolate(static_cast<float>(a.z), static_cast<float>(b.z), t);
    return IntVector3(Vector3(x, y, z));
}

IntVector4 Interpolate(const IntVector4& a, const IntVector4& b, float t) {
    const auto x = Interpolate(static_cast<float>(a.x), static_cast<float>(b.x), t);
    const auto y = Interpolate(static_cast<float>(a.y), static_cast<float>(b.y), t);
    const auto z = Interpolate(static_cast<float>(a.z), static_cast<float>(b.z), t);
    const auto w = Interpolate(static_cast<float>(a.w), static_cast<float>(b.w), t);
    return IntVector4(Vector4(x, y, z, w));
}

AABB2 Interpolate(const AABB2& a, const AABB2& b, float t) {
    const auto mins(Interpolate(a.mins, b.mins, t));
    const auto maxs(Interpolate(a.maxs, b.maxs, t));
    return AABB2(mins, maxs);
}

AABB3 Interpolate(const AABB3& a, const AABB3& b, float t) {
    const auto mins(Interpolate(a.mins, b.mins, t));
    const auto maxs(Interpolate(a.maxs, b.maxs, t));
    return AABB3(mins, maxs);
}

OBB2 Interpolate(const OBB2& a, const OBB2& b, float t) {
    const auto orientation(Interpolate(a.orientationDegrees, b.orientationDegrees, t));
    const auto he(Interpolate(a.half_extents, b.half_extents, t));
    const auto p(Interpolate(a.position, b.position, t));
    return OBB2(p, he, orientation);
}

Polygon2 Interpolate(const Polygon2& a, const Polygon2& b, float t) {
    const auto sides = static_cast<int>(Interpolate(static_cast<float>(a.GetSides()), static_cast<float>(b.GetSides()), t));
    const auto orientation(Interpolate(a.GetOrientationDegrees(), b.GetOrientationDegrees(), t));
    const auto he(Interpolate(a.GetHalfExtents(), b.GetHalfExtents(), t));
    const auto p(Interpolate(a.GetPosition(), b.GetPosition(), t));
    return Polygon2(sides, p, he, orientation);
}

Disc2 Interpolate(const Disc2& a, const Disc2& b, float t) {
    const auto c(Interpolate(a.center, b.center, t));
    const auto r(Interpolate(a.radius, b.radius, t));
    return Disc2(c, r);
}

LineSegment2 Interpolate(const LineSegment2& a, const LineSegment2& b, float t) {
    const auto start(Interpolate(a.start, b.start, t));
    const auto end(Interpolate(a.end, b.end, t));
    return LineSegment2(start, end);
}

Capsule2 Interpolate(const Capsule2& a, const Capsule2& b, float t) {
    const auto line(Interpolate(a.line, b.line, t));
    const auto r(Interpolate(a.radius, b.radius, t));
    return Capsule2(line, r);
}

LineSegment3 Interpolate(const LineSegment3& a, const LineSegment3& b, float t) {
    const auto start(Interpolate(a.start, b.start, t));
    const auto end(Interpolate(a.end, b.end, t));
    return LineSegment3(start, end);
}

Sphere3 Interpolate(const Sphere3& a, const Sphere3& b, float t) {
    const auto c(Interpolate(a.center, b.center, t));
    const auto r(Interpolate(a.radius, b.radius, t));
    return Sphere3(c, r);
}

Capsule3 Interpolate(const Capsule3& a, const Capsule3& b, float t) {
    const auto line(Interpolate(a.line, b.line, t));
    const auto r(Interpolate(a.radius, b.radius, t));
    return Capsule3(line, r);
}

Plane2 Interpolate(const Plane2& a, const Plane2& b, float t) {
    const auto d = Interpolate(a.dist, b.dist, t);
    const auto n = Interpolate(a.normal, b.normal, t);
    return Plane2(n, d);
}

Plane3 Interpolate(const Plane3& a, const Plane3& b, float t) {
    const auto d = Interpolate(a.dist, b.dist, t);
    const auto n = Interpolate(a.normal, b.normal, t);
    return Plane3(n, d);
}

Quaternion Interpolate(const Quaternion& a, const Quaternion& b, float t) {
    const auto w = Interpolate(a.w, b.w, t);
    const auto axis = Interpolate(a.axis, b.axis, t);
    return Quaternion(w, axis);
}

Rgba Interpolate(const Rgba& a, const Rgba& b, float t) {
    auto [ar, ag, ab, aa] = a.GetAsFloats();
    auto [br, bg, bb, ba] = b.GetAsFloats();

    const auto red = Interpolate(ar, br, t);
    const auto green = Interpolate(ag, bg, t);
    const auto blue = Interpolate(ab, bb, t);
    const auto alpha = Interpolate(aa, aa, t);

    Rgba result{};
    result.SetFromFloats({red, green, blue, alpha});
    return result;
}

Vector2 RangeMap(const Vector2& valueToMap, const Vector2& minmaxInputRange, const Vector2& minmaxOutputRange) {
    const auto x = RangeMap(valueToMap.x, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    const auto y = RangeMap(valueToMap.y, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    return Vector2{x, y};
}

Vector3 RangeMap(const Vector3& valueToMap, const Vector2& minmaxInputRange, const Vector2& minmaxOutputRange) {
    const auto x = RangeMap(valueToMap.x, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    const auto y = RangeMap(valueToMap.y, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    const auto z = RangeMap(valueToMap.z, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    return Vector3{x, y, z};
}

Vector4 RangeMap(const Vector4& valueToMap, const Vector2& minmaxInputRange, const Vector2& minmaxOutputRange) {
    const auto x = RangeMap(valueToMap.x, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    const auto y = RangeMap(valueToMap.y, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    const auto z = RangeMap(valueToMap.z, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    const auto w = RangeMap(valueToMap.w, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    return Vector4{x, y, z, w};
}

IntVector2 RangeMap(const IntVector2& valueToMap, const IntVector2& minmaxInputRange, const IntVector2& minmaxOutputRange) {
    const auto x = RangeMap(valueToMap.x, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    const auto y = RangeMap(valueToMap.y, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    return IntVector2{x, y};
}

IntVector3 RangeMap(const IntVector3& valueToMap, const IntVector2& minmaxInputRange, const IntVector2& minmaxOutputRange) {
    const auto x = RangeMap(valueToMap.x, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    const auto y = RangeMap(valueToMap.y, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    const auto z = RangeMap(valueToMap.z, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    return IntVector3{x, y, z};
}

IntVector4 RangeMap(const IntVector4& valueToMap, const IntVector2& minmaxInputRange, const IntVector2& minmaxOutputRange) {
    const auto x = RangeMap(valueToMap.x, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    const auto y = RangeMap(valueToMap.y, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    const auto z = RangeMap(valueToMap.z, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    const auto w = RangeMap(valueToMap.w, minmaxInputRange.x, minmaxInputRange.y, minmaxOutputRange.x, minmaxOutputRange.y);
    return IntVector4{x, y, z, w};
}

Vector4 Wrap(const Vector4& valuesToWrap, const Vector4& minValues, const Vector4& maxValues) {
    const auto x = Wrap(valuesToWrap.x, minValues.x, maxValues.x);
    const auto y = Wrap(valuesToWrap.y, minValues.y, maxValues.y);
    const auto z = Wrap(valuesToWrap.z, minValues.z, maxValues.z);
    const auto w = Wrap(valuesToWrap.w, minValues.w, maxValues.w);
    return Vector4(x, y, z, w);
}

Vector3 Wrap(const Vector3& valuesToWrap, const Vector3& minValues, const Vector3& maxValues) {
    const auto x = Wrap(valuesToWrap.x, minValues.x, maxValues.x);
    const auto y = Wrap(valuesToWrap.y, minValues.y, maxValues.y);
    const auto z = Wrap(valuesToWrap.z, minValues.z, maxValues.z);
    return Vector3(x, y, z);
}

Vector2 Wrap(const Vector2& valuesToWrap, const Vector2& minValues, const Vector2& maxValues) {
    const auto x = Wrap(valuesToWrap.x, minValues.x, maxValues.x);
    const auto y = Wrap(valuesToWrap.y, minValues.y, maxValues.y);
    return Vector2(x, y);
}

IntVector4 Wrap(const IntVector4& valuesToWrap, const IntVector4& minValues, const IntVector4& maxValues) {
    const auto x = Wrap(valuesToWrap.x, minValues.x, maxValues.x);
    const auto y = Wrap(valuesToWrap.y, minValues.y, maxValues.y);
    const auto z = Wrap(valuesToWrap.z, minValues.z, maxValues.z);
    const auto w = Wrap(valuesToWrap.w, minValues.w, maxValues.w);
    return IntVector4(x, y, z, w);
}

IntVector3 Wrap(const IntVector3& valuesToWrap, const IntVector3& minValues, const IntVector3& maxValues) {
    const auto x = Wrap(valuesToWrap.x, minValues.x, maxValues.x);
    const auto y = Wrap(valuesToWrap.y, minValues.y, maxValues.y);
    const auto z = Wrap(valuesToWrap.z, minValues.z, maxValues.z);
    return IntVector3(x, y, z);
}

IntVector2 Wrap(const IntVector2& valuesToWrap, const IntVector2& minValues, const IntVector2& maxValues) {
    const auto x = Wrap(valuesToWrap.x, minValues.x, maxValues.x);
    const auto y = Wrap(valuesToWrap.y, minValues.y, maxValues.y);
    return IntVector2(x, y);
}

std::size_t WrapCount(const Vector4& valuesToWrap, const Vector4& minValues, const Vector4& maxValues, Vector4& outValues) {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
    float w{0.0f};
    const auto x_count = WrapCount(valuesToWrap.x, minValues.x, maxValues.x, x);
    const auto y_count = WrapCount(valuesToWrap.y, minValues.y, maxValues.y, y);
    const auto z_count = WrapCount(valuesToWrap.z, minValues.z, maxValues.z, z);
    const auto w_count = WrapCount(valuesToWrap.w, minValues.w, maxValues.w, w);
    outValues = Vector4(x, y, z, w);
    return x_count + y_count + z_count + w_count;
}

std::size_t WrapCount(const Vector3& valuesToWrap, const Vector3& minValues, const Vector3& maxValues, Vector3& outValues) {
    float x{0.0f};
    float y{0.0f};
    float z{0.0f};
    const auto x_count = WrapCount(valuesToWrap.x, minValues.x, maxValues.x, x);
    const auto y_count = WrapCount(valuesToWrap.y, minValues.y, maxValues.y, y);
    const auto z_count = WrapCount(valuesToWrap.z, minValues.z, maxValues.z, z);
    outValues = Vector3(x, y, z);
    return x_count + y_count + z_count;
}

std::size_t WrapCount(const Vector2& valuesToWrap, const Vector2& minValues, const Vector2& maxValues, Vector2& outValues) {
    float x{0.0f};
    float y{0.0f};
    const auto x_count = WrapCount(valuesToWrap.x, minValues.x, maxValues.x, x);
    const auto y_count = WrapCount(valuesToWrap.y, minValues.y, maxValues.y, y);
    outValues = Vector2(x, y);
    return x_count + y_count;
}

std::size_t WrapCount(const IntVector4& valuesToWrap, const IntVector4& minValues, const IntVector4& maxValues, IntVector4& outValues) {
    int x{0};
    int y{0};
    int z{0};
    int w{0};
    const auto x_count = WrapCount(valuesToWrap.x, minValues.x, maxValues.x, x);
    const auto y_count = WrapCount(valuesToWrap.y, minValues.y, maxValues.y, y);
    const auto z_count = WrapCount(valuesToWrap.z, minValues.z, maxValues.z, z);
    const auto w_count = WrapCount(valuesToWrap.w, minValues.w, maxValues.w, w);
    outValues = IntVector4(x, y, z, w);
    return x_count + y_count + z_count + w_count;
}

std::size_t WrapCount(const IntVector3& valuesToWrap, const IntVector3& minValues, const IntVector3& maxValues, IntVector3& outValues) {
    int x{0};
    int y{0};
    int z{0};
    const auto x_count = WrapCount(valuesToWrap.x, minValues.x, maxValues.x, x);
    const auto y_count = WrapCount(valuesToWrap.y, minValues.y, maxValues.y, y);
    const auto z_count = WrapCount(valuesToWrap.z, minValues.z, maxValues.z, z);
    outValues = IntVector3(x, y, z);
    return x_count + y_count + z_count;
}

std::size_t WrapCount(const IntVector2& valuesToWrap, const IntVector2& minValues, const IntVector2& maxValues, IntVector2& outValues) {
    int x{0};
    int y{0};
    const auto x_count = WrapCount(valuesToWrap.x, minValues.x, maxValues.x, x);
    const auto y_count = WrapCount(valuesToWrap.y, minValues.y, maxValues.y, y);
    outValues = IntVector2(x, y);
    return x_count + y_count;
}













void TurnToward(float& currentDegrees, float goalDegrees, float maxTurnDegrees) {
    float signedAngularDistance = CalcShortestAngularDistance(currentDegrees, goalDegrees);
    if(std::fabs(signedAngularDistance) <= maxTurnDegrees) {
        currentDegrees = goalDegrees;
    } else if(signedAngularDistance > 0.0f) {
        currentDegrees += maxTurnDegrees;
    } else {
        currentDegrees -= maxTurnDegrees;
    }
}

float CalcShortestAngularDistance(float startDegrees, float endDegrees) {
    float signedAngularDistance = endDegrees - startDegrees;
    while(signedAngularDistance > 180.0f) {
        signedAngularDistance -= 360.0f;
    }
    while(signedAngularDistance < -180.0f) {
        signedAngularDistance += 360.0f;
    }
    return signedAngularDistance;
}

} // namespace MathUtils
