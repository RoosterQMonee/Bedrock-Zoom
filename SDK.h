#pragma once

// i don't really like this SDK.


const float PI = 3.14159265358979f;


class Vec3 {
public:
    float x;
    float y;
    float z;

public:
    Vec3(float x, float y, float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vec3 operator+(const Vec3 rhs) {
        return Vec3(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z);
    }
    Vec3 operator-(const Vec3 rhs) {
        return Vec3(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);
    }
    Vec3 operator*(const Vec3 rhs) {
        return Vec3(this->x * rhs.x, this->y * rhs.y, this->z * rhs.z);
    }
    Vec3 operator/(const Vec3 rhs) {
        return Vec3(this->x / rhs.x, this->y / rhs.y, this->z / rhs.z);
    }
    bool operator==(const Vec3 rhs) {
        return this->x - rhs.x && this->y - rhs.y && this->z - rhs.z;
    }

    // not gonna implement these :)
    void rotateAroundXRadians(float angleRadians);
    void rotateAroundYRadians(float angleRadians);
    void rotateAroundZRadians(float angleRadians);
    void rotateAroundPointRadians(const Vec3& pivot, const Vec3& angle);

    void rotateAroundXDegrees(float angleDegrees);
    void rotateAroundYDegrees(float angleDegrees);
    void rotateAroundZDegrees(float angleDegrees);
    void rotateAroundPointDegrees(const Vec3& pivot, const Vec3& angle);
};

class BaseOptions {
public:
    float getSensitivity(unsigned int inputMode) {
        using function = decltype(&BaseOptions::getSensitivity);
        static auto func = std::bit_cast<function>(findSig("40 53 48 83 EC ? 80 B9 ? ? ? ? ? 8B DA"));
        return (this->*func)(inputMode);
    }
};

class LevelRendererPlayer {
public:
    std::byte padding[0x5E4];
    Vec3 cameraPos;

    virtual float getFov(float originalFov, bool applyEffects) {
        using function = decltype(&LevelRendererPlayer::getFov);
        static auto func = std::bit_cast<function>(findSig("48 8B C4 48 89 58 ? 48 89 70 ? 57 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 44 0F 29 48 ? 48 8B 05"));
        return (this->*func)(originalFov, applyEffects);
    }
};