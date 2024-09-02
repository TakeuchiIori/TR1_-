#include "Easing.h" 

float Lerp(const float& a, const float& b, float t) {
	// tが0ならa、1ならbを返す
	return a + t * (b - a);
}
Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
	// 各次元ごとにLerpを適用
	float newX = a.x + t * (b.x - a.x);
	float newY = a.y + t * (b.y - a.y);
	float newZ = a.z + t * (b.z - a.z);
	return Vector3(newX, newY, newZ);
}
float EaseInSine(float x){return 1 - cosf((x * float(M_PI)) / 2); };

float EaseInBack(float x) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1;

	return c3 * x * x * x - c1 * x * x;
}

float EaseOutSine(float x) { return sinf((x * float(M_PI)) / 2); }

float EaseOutBack(float x) {
	const float c1 = 1.70158f;
	const float c3 = c1 + 1;

	return 1 + c3 * std::pow(x - 1, 3.0f) + c1 * std::pow(x - 1, 2.0f);
}

float EaseInOutExpo(float x) { return x = 0.0f ? 0.0f : x = 1.0f ? 1.0f : x < 0.5f ? (float)std::pow(2, 20 * x - 10.0f) / 2 : (2 - (float)std::pow(2, -20 * x + 10.0f)) / 2; }

float EaseOutBounce(float x) {
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	if (x < 1 / d1) {
		return n1 * x * x;
	} else if (x < 2 / d1) {
		return n1 * (x -= 1.5f / d1) * x + 0.75f;
	} else if (x < 2.5 / d1) {
		return n1 * (x -= 2.25f / d1) * x + 0.9375f;
	} else {
		return n1 * (x -= 2.625f / d1) * x + 0.984375f;
	}
}

float EaseInBounce(float x) { return 1 - EaseOutBounce(1 - x); }


// 三次イージング関数
float easeInCubic(float t) { return t * t * t; }

float easeOutCubic(float t) { return (--t) * t * t + 1; }

float easeInOutCubic(float t) { return t < 0.5 ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1; }

