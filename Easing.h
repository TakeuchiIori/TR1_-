#pragma once
#include <iostream>
#define _USE_MATH_DEFINES
#include "Vector3.h"
#include <math.h>
#include <iostream>
#include <functional>
float Lerp(const float& a, const float& b, float t);
Vector3 Lerp(const Vector3& a, const Vector3& b, float t);
float EaseInSine(float x);
float EaseInBack(float x);
float EaseOutSine(float x);
float EaseOutBack(float x);
float EaseInOutExpo(float x);
float EaseOutBounce(float x);
float EaseInBounce(float x);
// 三次イージング関数
float easeInCubic(float t);

float easeOutCubic(float t);

float easeInOutCubic(float t);
