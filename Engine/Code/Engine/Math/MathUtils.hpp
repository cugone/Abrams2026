#pragma once

#include "Engine/Core/TimeUtils.hpp"

#include "Engine/Math/IntVector2.hpp"
#include "Engine/Math/IntVector3.hpp"
#include "Engine/Math/IntVector4.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Math/Matrix4.hpp"


#include <algorithm>
#include <cmath>
#include <numbers>
#include <optional>
#include <random>
#include <ratio>
#include <utility>

class AABB2;
class AABB3;
class Capsule2;
class Disc2;
class LineSegment2;
class LineSegment3;
class OBB2;
class Sphere3;
class Capsule3;
class Plane2;
class Plane3;
class Polygon2;
class Quaternion;
class Rgba;
class Rotator;

namespace MathUtils {

//The values for:
//e, log2e, log10e, pi, 1/pi (inv_pi),
//1/sqrt(pi) (inv_sqrtpi), ln2, ln10, sqrt2, sqrt3,
//1/sqrt3 (inv_sqrt3), egamma, and phi
//are available in the <numbers> header.

using namespace std::numbers;

constexpr const float M_1PI_6 = 0.52359877559829887307f;         // 1pi/6
constexpr const float M_1PI_4 = 0.78539816339744830962f;         // 1pi/4
constexpr const float M_1PI_3 = 1.04719755119659774615f;         // 1pi/3
constexpr const float M_1PI_2 = 1.57079632679489661923f;         // 1pi/2
constexpr const float M_2PI_3 = 2.09439510239319549230f;         // 2pi/3
constexpr const float M_3PI_4 = 2.35619449019234492884f;         // 3pi/4
constexpr const float M_5PI_6 = 2.61799387799149436538f;         // 5pi/6
constexpr const float M_7PI_6 = 3.66519142918809211153f;         // 7pi/6
constexpr const float M_5PI_4 = 3.92699081698724154807f;         // 5pi/4
constexpr const float M_4PI_3 = 4.18879020478639098461f;         // 4pi/3
constexpr const float M_3PI_2 = 4.71238898038468985769f;         // 3pi/2
constexpr const float M_5PI_3 = 5.23598775598298873077f;         // 5pi/3
constexpr const float M_7PI_4 = 5.49778714378213816730f;         // 7pi/4
constexpr const float M_11PI_6 = 5.75958653158128760384f;        // 11pi/6
constexpr const float M_2PI = 6.28318530717958647692f;           // 2pi
constexpr const float M_PI_2 = 1.57079632679489661923f;          // pi/2
constexpr const float M_PI_4 = 0.78539816339744830962f;          // pi/4
constexpr const float M_2_PI = 0.63661977236758134308f;          // 2/pi
constexpr const float M_2_SQRTPI = 1.12837916709551257390f;      // 2/sqrt(pi)
constexpr const float M_1_SQRT2 = 0.70710678118654752440f;       // 1/sqrt(2)
constexpr const float M_SQRT3_3 = 0.57735026918962576451f;       // sqrt(3)/3
constexpr const float M_TAU = 1.61803398874989484821f;           // tau (golden ratio)
constexpr const std::ratio<16, 9> WIDESCREEN_RATIO;              // 16:9
constexpr const std::ratio<4, 3> FULLSCREEN_RATIO;               // 4:3
constexpr const float M_16_BY_9_RATIO = 1.77777777777777777778f; // 16/9
constexpr const float M_4_BY_3_RATIO = 1.33333333333333333333f;  // 4/3
constexpr const std::ratio<1, 1024> KIB_BYTES_RATIO;             // Kilobyte/Bytes
constexpr const std::ratio<1, 1048576> MIB_BYTES_RATIO;          // Megabyte/Bytes
constexpr const std::ratio<1, 1073741824> GIB_BYTES_RATIO;       // Gigabyte/Bytes
constexpr const std::ratio<1024, 1> BYTES_KIB_RATIO;             // Bytes/Kilobytes
constexpr const std::ratio<1048576, 1> BYTES_MIB_RATIO;          // Bytes/Megabytes
constexpr const std::ratio<1073741824, 1> BYTES_GIB_RATIO;       // Bytes/Gigabytes
constexpr const float M_SPEED_OF_LIGHT = 299311200.0f;           // m/s Speed of light in a vacuum

template<std::integral auto num>
constexpr int DigitCount = num >= -9 && num <= 9 ? 1 : 1 + DigitCount<num / 10>;

template<typename T>
requires(std::integral<T>)
const std::size_t DigitLength(T num) noexcept {
    std::size_t digits = num < 0 ? 1 : 0;
    do {
        num /= 10;
        ++digits;
    } while(num);
    return digits;
}

const unsigned int GetRandomSeed() noexcept;
void SetRandomEngineSeed(unsigned int seed) noexcept;
[[nodiscard]] std::random_device& GetRandomDevice() noexcept;
[[nodiscard]] std::mt19937_64& GetMT64RandomEngine(unsigned int seed = 0) noexcept;

template<typename T>
requires(std::floating_point<T>)
[[nodiscard]] std::pair<T, T> SplitFloatingPointValue(T value) noexcept {
    auto int_part = T{};
    const auto frac = std::modf(value, &int_part);
    return std::make_pair(int_part, frac);
}

template<typename T>
[[nodiscard]] bool IsValid(T value) noexcept {
    return std::isfinite(value);
}
[[nodiscard]] bool IsValid(Vector2 v) noexcept;
[[nodiscard]] bool IsValid(Vector3 v) noexcept;
[[nodiscard]] bool IsValid(Vector4 v) noexcept;
[[nodiscard]] bool IsValid(Matrix4 v) noexcept;

[[nodiscard]] float ConvertDegreesToRadians(float degrees) noexcept;
[[nodiscard]] float ConvertRadiansToDegrees(float radians) noexcept;

[[nodiscard]] bool GetRandomBool() noexcept;

template<typename T>
requires(!std::same_as<T, bool>)
[[nodiscard]] T GetRandomInRange(const T& minInclusive, const T& maxInclusive) noexcept {
    auto d = [&]() {
        if constexpr(std::is_floating_point_v<T>) {
            return std::uniform_real_distribution<T>{minInclusive, std::nextafter(maxInclusive, maxInclusive + T{1.0})};
        } else if constexpr(std::is_integral_v<T>) {
            return std::uniform_int_distribution<T>{minInclusive, maxInclusive};
        }
    }();
    return d(GetMT64RandomEngine(GetRandomSeed()));
}

template<typename T>
requires(!std::same_as<T, bool>)
[[nodiscard]] T GetRandomLessThan(const T& maxValueNotInclusive) noexcept {
    auto d = [&]() {
        if constexpr(std::is_floating_point_v<T>) {
            return std::uniform_real_distribution<T>{T{0}, maxValueNotInclusive};
        } else if constexpr(std::is_integral_v<T>) {
            return std::uniform_int_distribution<T>{T{0}, maxValueNotInclusive - T{1}};
        }
    }();
    return d(GetMT64RandomEngine(GetRandomSeed()));
}

template<typename T>
requires(std::floating_point<T>)
[[nodiscard]] bool IsPercentChance(const T& probability) noexcept {
    auto d = std::bernoulli_distribution(std::clamp(probability, T{0}, T{1}));
    return d(GetMT64RandomEngine(GetRandomSeed()));
}

template<typename T>
requires(std::floating_point<T>)
[[nodiscard]] T GetRandomZeroToOne() noexcept {
    return GetRandomInRange(T{0}, std::nextafter(T{1}, T{2}));
}

template<typename T>
requires(std::floating_point<T>)
[[nodiscard]] T GetRandomZeroUpToOne() noexcept {
    return GetRandomLessThan(T{1});
}

template<typename T>
requires(std::floating_point<T>)
[[nodiscard]] T GetRandomNegOneToOne() noexcept {
    return GetRandomInRange(T{-1}, T{1});
}

template<typename T>
requires(std::integral<T>)
[[nodiscard]] T NextMultipleAfter(T value, T multiple) noexcept {
    return value + (multiple - (value % multiple));
}

template<typename T>
requires(std::integral<T>)
[[nodiscard]] T NextMultipleBefore(T value, T multiple) noexcept {
    return value - (multiple - (value % multiple));
}

[[nodiscard]] double nCr(const int n, const int k) noexcept;
[[nodiscard]] double Combination(const int n, const int k) noexcept;
[[nodiscard]] double Combination_multiset(const int n, const int k) noexcept;

template<size_t N>
requires(N <= 20)
[[nodiscard]] constexpr unsigned long long Permutation() noexcept {
    static constexpr unsigned long long factorials[] = {1ull, 1ull, 2ull, 6ull, 24ull, 120ull, 720ull, 5'040ull, 40'320ull, 362'880ull, 3'628'800ull, 39'916'800ull, 479'001'600ull, 6'227'020'800ull, 87'178'291'200ull, 1'307'674'368'000ull, 20'922'789'888'000ull, 355'687'428'096'000ull, 6'402'373'705'728'000ull, 121'645'100'408'832'000ull, 2'432'902'008'176'640'000ull};
    return factorials[N];
}

template<size_t N, size_t K>
requires(N <= 20 && K <= 20 && (K <= N) && (N - K) <= 20)
[[nodiscard]] constexpr unsigned long long nPr() noexcept {
    return Permutation<N>() / Permutation<(N - K)>();
}

[[nodiscard]] float SineWave(float t, float period = 1.0f, float phase = 0.0f) noexcept;
[[nodiscard]] float SineWave(float t, TimeUtils::FPSeconds period = TimeUtils::FPSeconds{1.0f}, float phase = 0.0f) noexcept;
[[nodiscard]] float SineWaveDegrees(float t, float period = 1.0f, float phaseDegrees = 0.0f) noexcept;
[[nodiscard]] float SineWaveDegrees(float t, TimeUtils::FPSeconds period = TimeUtils::FPSeconds{1.0f}, float phase = 0.0f) noexcept;
[[nodiscard]] float SinCos(float sin, float cos);
[[nodiscard]] float CosSin(float cos, float sin);
[[nodiscard]] float SinCosDegrees(float sinDegrees, float cosDegrees);
[[nodiscard]] float CosSinDegrees(float cosDegrees, float sinDegrees);
[[nodiscard]] float CosDegrees(float degrees) noexcept;
[[nodiscard]] float SinDegrees(float degrees) noexcept;
[[nodiscard]] float Atan2Degrees(float y, float x) noexcept;

[[nodiscard]] bool IsEquivalent(float a, float b, float epsilon = 0.00001f) noexcept;
[[nodiscard]] bool IsEquivalent(double a, double b, double epsilon = 0.0001) noexcept;
[[nodiscard]] bool IsEquivalent(long double a, long double b, long double epsilon = 0.0001L) noexcept;
[[nodiscard]] bool IsEquivalent(const Vector2& a, const Vector2& b, float epsilon = 0.0001f) noexcept;
[[nodiscard]] bool IsEquivalent(const Vector3& a, const Vector3& b, float epsilon = 0.0001f) noexcept;
[[nodiscard]] bool IsEquivalent(const Vector4& a, const Vector4& b, float epsilon = 0.0001f) noexcept;
[[nodiscard]] bool IsEquivalent(const Quaternion& a, const Quaternion& b, float epsilon = 0.0001f) noexcept;
[[nodiscard]] bool IsEquivalent(const Rotator& a, const Rotator& b, float epsilon = 0.0001f) noexcept;

[[nodiscard]] bool IsEquivalentOrLessThan(float a, float b, float epsilon = 0.00001f) noexcept;
[[nodiscard]] bool IsEquivalentOrLessThan(double a, double b, double epsilon = 0.0001) noexcept;
[[nodiscard]] bool IsEquivalentOrLessThan(long double a, long double b, long double epsilon = 0.0001L) noexcept;

[[nodiscard]] bool IsEquivalentOrLessThanZero(float a, float epsilon = 0.00001f) noexcept;
[[nodiscard]] bool IsEquivalentOrLessThanZero(double a, double epsilon = 0.0001) noexcept;
[[nodiscard]] bool IsEquivalentOrLessThanZero(long double a, long double epsilon = 0.0001L) noexcept;

[[nodiscard]] bool IsEquivalentToZero(float a, float epsilon = 0.00001f) noexcept;
[[nodiscard]] bool IsEquivalentToZero(double a, double epsilon = 0.0001) noexcept;
[[nodiscard]] bool IsEquivalentToZero(long double a, long double epsilon = 0.0001L) noexcept;
[[nodiscard]] bool IsEquivalentToZero(const Vector2& a, float epsilon = 0.0001f) noexcept;
[[nodiscard]] bool IsEquivalentToZero(const Vector3& a, float epsilon = 0.0001f) noexcept;
[[nodiscard]] bool IsEquivalentToZero(const Vector4& a, float epsilon = 0.0001f) noexcept;
[[nodiscard]] bool IsEquivalentToZero(const Quaternion& a, float epsilon = 0.0001f) noexcept;
[[nodiscard]] bool IsEquivalentToZero(const Rotator& a, float epsilon = 0.0001f) noexcept;

[[nodiscard]] float CalcDistance(const Vector2& a, const Vector2& b) noexcept;
[[nodiscard]] float CalcDistance(const Vector3& a, const Vector3& b) noexcept;
[[nodiscard]] float CalcDistance(const Vector4& a, const Vector4& b) noexcept;
[[nodiscard]] float CalcDistance(const Vector2& p, const LineSegment2& line) noexcept;
[[nodiscard]] float CalcDistance(const Vector3& p, const LineSegment3& line) noexcept;
[[nodiscard]] float CalcDistance(const Vector2& p, const Polygon2& poly2) noexcept;
[[nodiscard]] float CalcDistance(const LineSegment2& line, const Polygon2& poly2) noexcept;
[[nodiscard]] float CalcDistance(const Polygon2& poly2, const LineSegment2& line) noexcept;
[[nodiscard]] float CalcDistance(const LineSegment2& lineA, const LineSegment2& lineB) noexcept;

[[nodiscard]] float CalcDistanceSquared(const Vector2& a, const Vector2& b) noexcept;
[[nodiscard]] float CalcDistanceSquared(const Vector3& a, const Vector3& b) noexcept;
[[nodiscard]] float CalcDistanceSquared(const Vector4& a, const Vector4& b) noexcept;
[[nodiscard]] float CalcDistanceSquared(const Vector2& p, const LineSegment2& line) noexcept;
[[nodiscard]] float CalcDistanceSquared(const Vector3& p, const LineSegment3& line) noexcept;
[[nodiscard]] float CalcDistanceSquared(const Vector2& p, const Polygon2& poly2) noexcept;
[[nodiscard]] float CalcDistanceSquared(const LineSegment2& line, const Polygon2& poly2) noexcept;
[[nodiscard]] float CalcDistanceSquared(const Polygon2& poly2, const LineSegment2& line) noexcept;
[[nodiscard]] float CalcDistanceSquared(const LineSegment2& lineA, const LineSegment2& lineB) noexcept;

[[nodiscard]] float CrossProduct(const Vector2& a, const Vector2& b) noexcept;
[[nodiscard]] Vector3 CrossProduct(const Vector3& a, const Vector3& b) noexcept;

[[nodiscard]] float DotProduct(const Vector2& a, const Vector2& b) noexcept;
[[nodiscard]] float DotProduct(const Vector3& a, const Vector3& b) noexcept;
[[nodiscard]] float DotProduct(const Vector4& a, const Vector4& b) noexcept;
[[nodiscard]] float DotProduct(const Quaternion& a, const Quaternion& b) noexcept;

[[nodiscard]] float TripleProductScalar(const Vector3& a, const Vector3& b, const Vector3& c) noexcept;
[[nodiscard]] Vector2 TripleProductVector(const Vector2& a, const Vector2& b, const Vector2& c) noexcept;
[[nodiscard]] Vector3 TripleProductVector(const Vector3& a, const Vector3& b, const Vector3& c) noexcept;

[[nodiscard]] Vector2 Project(const Vector2& a, const Vector2& b) noexcept;
[[nodiscard]] Vector3 Project(const Vector3& a, const Vector3& b) noexcept;
[[nodiscard]] Vector4 Project(const Vector4& a, const Vector4& b) noexcept;

[[nodiscard]] Vector2 Reject(const Vector2& a, const Vector2& b) noexcept;
[[nodiscard]] Vector3 Reject(const Vector3& a, const Vector3& b) noexcept;
[[nodiscard]] Vector4 Reject(const Vector4& a, const Vector4& b) noexcept;

[[nodiscard]] std::pair<Vector2, Vector2> DivideIntoProjectAndReject(const Vector2& a, const Vector2& b) noexcept;
[[nodiscard]] std::pair<Vector3, Vector3> DivideIntoProjectAndReject(const Vector3& a, const Vector3& b) noexcept;
[[nodiscard]] std::pair<Vector4, Vector4> DivideIntoProjectAndReject(const Vector4& a, const Vector4& b) noexcept;

[[nodiscard]] Vector2 Reflect(const Vector2& in, const Vector2& normal) noexcept;
[[nodiscard]] Vector3 Reflect(const Vector3& in, const Vector3& normal) noexcept;
[[nodiscard]] Vector4 Reflect(const Vector4& in, const Vector4& normal) noexcept;

[[nodiscard]] Vector2 Rotate(const Vector2& v, const Quaternion& q) noexcept;
[[nodiscard]] Vector3 Rotate(const Vector3& v, const Quaternion& q) noexcept;

[[nodiscard]] Vector2 ProjectAlongPlane(const Vector2& v, const Vector2& n) noexcept;
[[nodiscard]] Vector3 ProjectAlongPlane(const Vector3& v, const Vector3& n) noexcept;
[[nodiscard]] Vector4 ProjectAlongPlane(const Vector4& v, const Vector4& n) noexcept;

[[nodiscard]] unsigned int CalculateManhattanDistance(const IntVector2& start, const IntVector2& end) noexcept;
[[nodiscard]] unsigned int CalculateManhattanDistance(const IntVector3& start, const IntVector3& end) noexcept;
[[nodiscard]] unsigned int CalculateManhattanDistance(const IntVector4& start, const IntVector4& end) noexcept;

[[nodiscard]] unsigned int CalculateChebyshevDistance(const IntVector2& start, const IntVector2& end) noexcept;
[[nodiscard]] unsigned int CalculateChebyshevDistance(const IntVector3& start, const IntVector3& end) noexcept;
[[nodiscard]] unsigned int CalculateChebyshevDistance(const IntVector4& start, const IntVector4& end) noexcept;

[[nodiscard]] unsigned int CalculateChessboardDistance(const IntVector2& start, const IntVector2& end) noexcept;
[[nodiscard]] unsigned int CalculateChessboardDistance(const IntVector3& start, const IntVector3& end) noexcept;
[[nodiscard]] unsigned int CalculateChessboardDistance(const IntVector4& start, const IntVector4& end) noexcept;

[[nodiscard]] Vector2 GetRandomPointOn(const AABB2& aabb) noexcept;
[[nodiscard]] Vector2 GetRandomPointOn(const Disc2& disc) noexcept;
[[nodiscard]] Vector2 GetRandomPointOn(const LineSegment2& line) noexcept;

[[nodiscard]] Vector3 GetRandomPointOn(const AABB3& aabb) noexcept;
[[nodiscard]] Vector3 GetRandomPointOn(const Sphere3& sphere) noexcept;
[[nodiscard]] Vector3 GetRandomPointOn(const LineSegment3& line) noexcept;

[[nodiscard]] Vector2 GetRandomPointInside(const AABB2& aabb) noexcept;
[[nodiscard]] Vector2 GetRandomPointInside(const Disc2& disc) noexcept;

[[nodiscard]] Vector3 GetRandomPointInsideDisc(const Vector3& position, const Vector3& normal, float radius) noexcept;
[[nodiscard]] Vector3 GetRandomPointInsideCube(float radius) noexcept;
[[nodiscard]] Vector3 GetRandomPointInsideSphere(float radius) noexcept;

[[nodiscard]] Vector2 GetRandomPointInsidePlane(const Plane2& p, float r) noexcept;
[[nodiscard]] Vector3 GetRandomPointInsidePlane(const Plane3& p, float r) noexcept;
[[nodiscard]] Vector2 GetRandomPointInsidePlane(const Vector2& pos, const Vector2& normal, float r) noexcept;
[[nodiscard]] Vector3 GetRandomPointInsidePlane(const Vector3& pos, const Vector3& normal, float r) noexcept;

[[nodiscard]] Vector3 GetRandomPointInside(const AABB3& aabb) noexcept;
[[nodiscard]] Vector3 GetRandomPointInside(const Sphere3& sphere) noexcept;

[[nodiscard]] bool Contains(const AABB2& aabb, const Vector2& point) noexcept;
[[nodiscard]] bool Contains(const AABB2& a, const AABB2& b) noexcept;
[[nodiscard]] bool Contains(const AABB2& a, const OBB2& b) noexcept;
[[nodiscard]] bool Contains(const OBB2& a, const AABB2& b) noexcept;
[[nodiscard]] bool Contains(const OBB2& a, const OBB2& b) noexcept;
[[nodiscard]] bool Contains(const Disc2& a, const AABB2& b) noexcept;

[[nodiscard]] bool IsPointInside(const AABB2& aabb, const Vector2& point) noexcept;
[[nodiscard]] bool IsPointInside(const AABB3& aabb, const Vector3& point) noexcept;
[[nodiscard]] bool IsPointInside(const OBB2& obb, const Vector2& point) noexcept;
[[nodiscard]] bool IsPointInside(const Disc2& disc, const Vector2& point) noexcept;
[[nodiscard]] bool IsPointInside(const Capsule2& capsule, const Vector2& point) noexcept;
[[nodiscard]] bool IsPointInside(const Polygon2& poly2, const Vector2& point) noexcept;
[[nodiscard]] bool IsPointInside(const Sphere3& sphere, const Vector3& point) noexcept;
[[nodiscard]] bool IsPointInside(const Capsule3& capsule, const Vector3& point) noexcept;

[[nodiscard]] bool IsPointOn(const Disc2& disc, const Vector2& point) noexcept;
[[nodiscard]] bool IsPointOn(const LineSegment2& line, const Vector2& point) noexcept;
[[nodiscard]] bool IsPointOn(const Capsule2& capsule, const Vector2& point) noexcept;
[[nodiscard]] bool IsPointOn(const LineSegment3& line, const Vector3& point) noexcept;
[[nodiscard]] bool IsPointOn(const Sphere3& sphere, const Vector3& point) noexcept;
[[nodiscard]] bool IsPointOn(const Capsule3& capsule, const Vector3& point) noexcept;
[[nodiscard]] bool IsPointOn(const Polygon2& poly2, const Vector2& point) noexcept;

[[nodiscard]] Vector2 CalcClosestPoint(const Vector2& p, const AABB2& aabb) noexcept;
[[nodiscard]] Vector2 CalcClosestPoint(const Vector2& p, const OBB2& obb) noexcept;
[[nodiscard]] Vector3 CalcClosestPoint(const Vector3& p, const AABB3& aabb) noexcept;
[[nodiscard]] Vector2 CalcClosestPoint(const Vector2& p, const Polygon2& poly2) noexcept;
[[nodiscard]] Vector2 CalcClosestPoint(const Vector2& p, const Disc2& disc) noexcept;
[[nodiscard]] Vector2 CalcClosestPoint(const Vector2& p, const LineSegment2& line) noexcept;
[[nodiscard]] Vector2 CalcClosestPoint(const Vector2& p, const Capsule2& capsule) noexcept;

[[nodiscard]] Vector3 CalcClosestPoint(const Vector3& p, const LineSegment3& line) noexcept;
[[nodiscard]] Vector3 CalcClosestPoint(const Vector3& p, const Sphere3& sphere) noexcept;
[[nodiscard]] Vector3 CalcClosestPoint(const Vector3& p, const Capsule3& capsule) noexcept;

[[nodiscard]] Vector2 CalcNormalizedPointFromPoint(const Vector2& pos, const AABB2& bounds) noexcept;
[[nodiscard]] Vector2 CalcPointFromNormalizedPoint(const Vector2& uv, const AABB2& bounds) noexcept;
[[nodiscard]] Vector2 CalcNormalizedHalfExtentsFromPoint(const Vector2& pos, const AABB2& bounds) noexcept;
[[nodiscard]] Vector2 CalcPointFromNormalizedHalfExtents(const Vector2& uv, const AABB2& bounds) noexcept;
[[nodiscard]] AABB2 ScaleToFit(const AABB2& a, const AABB2& b) noexcept;
[[nodiscard]] AABB2 StretchToFit(const AABB2& a, const AABB2& b) noexcept;

[[nodiscard]] bool DoDiscsOverlap(const Disc2& a, const Disc2& b) noexcept;
[[nodiscard]] bool DoDiscsOverlap(const Vector2& centerA, float radiusA, const Vector2& centerB, float radiusB) noexcept;
[[nodiscard]] bool DoDiscsOverlap(const Disc2& a, const Capsule2& b) noexcept;
[[nodiscard]] bool DoDiscsOverlap(const Disc2& a, const AABB2& b) noexcept;

[[nodiscard]] bool DoSpheresOverlap(const Sphere3& a, const Sphere3& b) noexcept;
[[nodiscard]] bool DoSpheresOverlap(const Vector3& centerA, float radiusA, const Vector3& centerB, float radiusB) noexcept;
[[nodiscard]] bool DoSpheresOverlap(const Sphere3& a, const Capsule3& b) noexcept;
[[nodiscard]] bool DoSpheresOverlap(const Sphere3& a, const AABB3& b) noexcept;

[[nodiscard]] bool DoAABBsOverlap(const AABB2& a, const AABB2& b) noexcept;
[[nodiscard]] bool DoAABBsOverlap(const AABB3& a, const AABB3& b) noexcept;
[[nodiscard]] bool DoAABBsOverlap(const AABB2& a, const Disc2& b) noexcept;
[[nodiscard]] bool DoAABBsOverlap(const AABB3& a, const Sphere3& b) noexcept;
[[nodiscard]] bool DoOBBsOverlap(const OBB2& a, const OBB2& b) noexcept;
[[nodiscard]] bool DoPolygonsOverlap(const Polygon2& a, const Polygon2& b) noexcept;

[[nodiscard]] std::optional<Vector2> DoLineSegmentOverlap(const LineSegment2& a, const LineSegment2& b) noexcept;
[[nodiscard]] bool DoLineSegmentOverlap(const Disc2& a, const LineSegment2& b) noexcept;
[[nodiscard]] bool DoLineSegmentOverlap(const Sphere3& a, const LineSegment3& b) noexcept;

[[nodiscard]] bool DoCapsuleOverlap(const Disc2& a, const Capsule2& b) noexcept;
[[nodiscard]] bool DoCapsuleOverlap(const Sphere3& a, const Capsule3& b) noexcept;

[[nodiscard]] bool DoPlaneOverlap(const Disc2& a, const Plane2& b) noexcept;
[[nodiscard]] bool DoPlaneOverlap(const Sphere3& a, const Plane3& b) noexcept;
[[nodiscard]] bool DoPlaneOverlap(const Capsule2& a, const Plane2& b) noexcept;
[[nodiscard]] bool DoPlaneOverlap(const Capsule3& a, const Plane3& b) noexcept;

[[nodiscard]] bool IsPointInFrontOfPlane(const Vector3& point, const Plane3& plane) noexcept;
[[nodiscard]] bool IsPointBehindOfPlane(const Vector3& point, const Plane3& plane) noexcept;
[[nodiscard]] bool IsPointOnPlane(const Vector3& point, const Plane3& plane) noexcept;

[[nodiscard]] bool IsPointInFrontOfPlane(const Vector2& point, const Plane2& plane) noexcept;
[[nodiscard]] bool IsPointBehindOfPlane(const Vector2& point, const Plane2& plane) noexcept;
[[nodiscard]] bool IsPointOnPlane(const Vector2& point, const Plane2& plane) noexcept;

//Column major
[[nodiscard]] float CalculateMatrix3Determinant(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) noexcept;

//Column major
[[nodiscard]] float CalculateMatrix2Determinant(float m00, float m01, float m10, float m11) noexcept;

[[nodiscard]] Quaternion SLERP(const Quaternion& a, const Quaternion& b, float t) noexcept;

template<typename T>
[[nodiscard]] T Interpolate(const T& a, const T& b, float t) {
    return ((T{1} - t) * a) + (t * b);
}

template<typename T>
[[nodiscard]] T InterpolateBezier(const T& a, const T& b, const T& c, float t) {
    return Interpolate(Interpolate(a, b, t), Interpolate(b, c, t), t);
}

[[nodiscard]] Vector2 Interpolate(const Vector2& a, const Vector2& b, float t);
[[nodiscard]] Vector2 InterpolateBezier(const Vector2& a, const Vector2& b, const Vector2& c, float t);
[[nodiscard]] Vector3 Interpolate(const Vector3& a, const Vector3& b, float t);
[[nodiscard]] Vector4 Interpolate(const Vector4& a, const Vector4& b, float t);
[[nodiscard]] IntVector2 Interpolate(const IntVector2& a, const IntVector2& b, float t);
[[nodiscard]] IntVector3 Interpolate(const IntVector3& a, const IntVector3& b, float t);
[[nodiscard]] IntVector4 Interpolate(const IntVector4& a, const IntVector4& b, float t);
[[nodiscard]] AABB2 Interpolate(const AABB2& a, const AABB2& b, float t);
[[nodiscard]] AABB3 Interpolate(const AABB3& a, const AABB3& b, float t);
[[nodiscard]] OBB2 Interpolate(const OBB2& a, const OBB2& b, float t);
[[nodiscard]] Polygon2 Interpolate(const Polygon2& a, const Polygon2& b, float t);
[[nodiscard]] Disc2 Interpolate(const Disc2& a, const Disc2& b, float t);
[[nodiscard]] LineSegment2 Interpolate(const LineSegment2& a, const LineSegment2& b, float t);
[[nodiscard]] Capsule2 Interpolate(const Capsule2& a, const Capsule2& b, float t);
[[nodiscard]] LineSegment3 Interpolate(const LineSegment3& a, const LineSegment3& b, float t);
[[nodiscard]] Sphere3 Interpolate(const Sphere3& a, const Sphere3& b, float t);
[[nodiscard]] Capsule3 Interpolate(const Capsule3& a, const Capsule3& b, float t);
[[nodiscard]] Plane2 Interpolate(const Plane2& a, const Plane2& b, float t);
[[nodiscard]] Plane3 Interpolate(const Plane3& a, const Plane3& b, float t);
[[nodiscard]] Quaternion Interpolate(const Quaternion& a, const Quaternion& b, float t);
[[nodiscard]] Rgba Interpolate(const Rgba& a, const Rgba& b, float t);

template<typename T>
[[nodiscard]] T RangeMap(const T& valueToMap, const T& minInputRange, const T& maxInputRange, const T& minOutputRange, const T& maxOutputRange) {
    return (valueToMap - minInputRange) * (maxOutputRange - minOutputRange) / (maxInputRange - minInputRange) + minOutputRange;
}

[[nodiscard]] Vector2 RangeMap(const Vector2& valueToMap, const Vector2& minmaxInputRange, const Vector2& minmaxOutputRange);
[[nodiscard]] Vector3 RangeMap(const Vector3& valueToMap, const Vector2& minmaxInputRange, const Vector2& minmaxOutputRange);
[[nodiscard]] Vector4 RangeMap(const Vector4& valueToMap, const Vector2& minmaxInputRange, const Vector2& minmaxOutputRange);

[[nodiscard]] IntVector2 RangeMap(const IntVector2& valueToMap, const IntVector2& minmaxInputRange, const IntVector2& minmaxOutputRange);
[[nodiscard]] IntVector3 RangeMap(const IntVector3& valueToMap, const IntVector2& minmaxInputRange, const IntVector2& minmaxOutputRange);
[[nodiscard]] IntVector4 RangeMap(const IntVector4& valueToMap, const IntVector2& minmaxInputRange, const IntVector2& minmaxOutputRange);

[[nodiscard]] auto Wrap(const auto& valueToWrap, const auto& minValue, const auto& maxValue) {
    auto result = decltype(maxValue + minValue){valueToWrap};
    const auto range = decltype(maxValue - minValue){maxValue - minValue};
    while(result < minValue) {
        result += range;
    }
    while(result > maxValue) {
        result -= range;
    }
    return result;
}

[[nodiscard]] std::size_t WrapCount(const auto& valueToWrap, const auto& minValue, const auto& maxValue, auto& outValue) {
    std::size_t count{0u};
    auto result = decltype(maxValue + minValue){valueToWrap};
    const auto range = decltype(maxValue - minValue){maxValue - minValue};
    while(result < minValue) {
        result += range;
        ++count;
    }
    while(result > maxValue) {
        result -= range;
        ++count;
    }
    outValue = result;
    return count;
}

[[nodiscard]] constexpr auto CalcShortestPeriodicDistance(const auto& startValue, const auto& endValue, const auto& minValue = 0.0f, const auto& maxValue = 360.0f) {
    const auto a = Wrap(endValue - startValue, minValue, maxValue);
    const auto b = Wrap(startValue - endValue, minValue, maxValue);
    return (std::min)(a, b);
}

[[nodiscard]] constexpr auto CalcShortestPeriodicDistanceSigned(const auto& startValue, const auto& endValue, const auto& minValue = 0.0f, const auto& maxValue = 360.0f) {
    const auto a = Wrap(endValue - startValue, minValue, maxValue);
    const auto b = Wrap(startValue - endValue, minValue, maxValue);
    if(b > a) {
        return a;
    } else {
        return -b;
    }
}

[[nodiscard]] Vector4 Wrap(const Vector4& valuesToWrap, const Vector4& minValues, const Vector4& maxValues);
[[nodiscard]] Vector3 Wrap(const Vector3& valuesToWrap, const Vector3& minValues, const Vector3& maxValues);
[[nodiscard]] Vector2 Wrap(const Vector2& valuesToWrap, const Vector2& minValues, const Vector2& maxValues);
[[nodiscard]] IntVector4 Wrap(const IntVector4& valuesToWrap, const IntVector4& minValues, const IntVector4& maxValues);
[[nodiscard]] IntVector3 Wrap(const IntVector3& valuesToWrap, const IntVector3& minValues, const IntVector3& maxValues);
[[nodiscard]] IntVector2 Wrap(const IntVector2& valuesToWrap, const IntVector2& minValues, const IntVector2& maxValues);

[[nodiscard]] std::size_t WrapCount(const Vector4& valuesToWrap, const Vector4& minValues, const Vector4& maxValues, Vector4& outValues);
[[nodiscard]] std::size_t WrapCount(const Vector3& valuesToWrap, const Vector3& minValues, const Vector3& maxValues, Vector3& outValues);
[[nodiscard]] std::size_t WrapCount(const Vector2& valuesToWrap, const Vector2& minValues, const Vector2& maxValues, Vector2& outValues);
[[nodiscard]] std::size_t WrapCount(const IntVector4& valuesToWrap, const IntVector4& minValues, const IntVector4& maxValues, IntVector4& outValues);
[[nodiscard]] std::size_t WrapCount(const IntVector3& valuesToWrap, const IntVector3& minValues, const IntVector3& maxValues, IntVector3& outValues);
[[nodiscard]] std::size_t WrapCount(const IntVector2& valuesToWrap, const IntVector2& minValues, const IntVector2& maxValues, IntVector2& outValues);



void TurnToward(float& currentDegrees, float goalDegrees, float maxTurnDegrees);
[[nodiscard]] float CalcShortestAngularDistance(float startDegrees, float endDegrees);

namespace EasingFunctions {

namespace detail {

template<typename T, std::size_t... Is>
[[nodiscard]] T SmoothStart_helper(const T& t, std::index_sequence<Is...>) {
    return (((void)Is, t) * ...);
}

template<typename T, std::size_t... Is>
[[nodiscard]] T SmoothStop_helper(const T& t, std::index_sequence<Is...>) {
    return (((void)Is, (1.0f - t)) * ...);
}

} // namespace detail

template<std::size_t N, typename T>
requires(N > 0 && std::floating_point<T>)
[[nodiscard]] T SmoothStart(const T& t) {
    return detail::SmoothStart_helper(t, std::make_index_sequence<N>{});
}

template<std::size_t N, typename T>
requires(N > 0 && std::floating_point<T>)
[[nodiscard]] T SmoothStop(const T& t) {
    return detail::SmoothStop_helper(t, std::make_index_sequence<N>{});
}

template<std::size_t N, typename T>
requires(N > 0 && std::floating_point<T>)
[[nodiscard]] T SmoothStep(const T& t) {
    return Interpolate(SmoothStart<N>(t), SmoothStop<N>(t), T{0.5});
}

template<std::size_t N, typename T>
requires(N > 0 && std::floating_point<T>)
[[nodiscard]] T Arc(const T& t) {
    return SmoothStart<N>(t) + SmoothStop<N>(t);
}

template<std::size_t N, typename T>
requires(N > 0 && std::floating_point<T>)
[[nodiscard]] T SmoothStartElastic(const T& t) {
    return t < T{0.0} ? T{0.0} : (t > T{1.0} ? T{1.0} : -std::pow(T{2.0}, T{10.0} * t - T{10.0}) * std::sin((t * T{10.0} - T{10.75}) * MathUtils::M_2PI_3));
}

template<std::size_t N, typename T>
requires(N > 0 && std::floating_point<T>)
[[nodiscard]] T SmoothStopElastic(const T& t) {
    return t < T{0.0} ? T{0.0} : (t > T{1.0} ? T{1.0} : -std::pow(T{2.0}, T{-10.0} * t) * std::sin((t * T{10.0} - T{0.75}) * MathUtils::M_2PI_3) + T{1.0});
}

} // namespace EasingFunctions

} // namespace MathUtils