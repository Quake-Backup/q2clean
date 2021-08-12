#pragma once

#include "config.h"
#include "lib/types.h"
#include "lib/math/vector.h"

// randomness!

// return a random unsigned integer between [0, UINT_MAX]
uint32_t Q_rand();

// return a random unsigned integer between [0, max).
// if max is zero, always returns 0.
uint32_t Q_rand_uniform(const uint32_t &max);

template<typename T>
auto random_of(const std::initializer_list<T> &args)
{
	return *(args.begin() + Q_rand_uniform(args.size()));
}

template<typename T, size_t N>
auto random_of(const T (&args)[N])
{
	return args[Q_rand_uniform(lengthof(args))];
}

// return a random float between [0, 1)
float random();

// return a random float between [0, max)
float random(const float &max);

// return a random float between [min, max)
float random(const float &min, const float &max);

// return a random vector between [0, 1)
vector randomv();

// return a random vector between [0, max)
vector randomv(const vector &max);

// return a random vector between [min, max)
vector randomv(const vector &min, const vector &max);

inline float crandom()
{
	return random(-1.f, 1.f);
}

inline vector crandomv()
{
	return randomv({ -1, -1, -1 }, { 1, 1, 1 });
}
