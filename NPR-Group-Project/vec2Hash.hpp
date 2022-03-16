#ifndef VEC2HASH_H
#define VEC2HASH_H

#include <glm/glm.hpp>
using namespace std;
using namespace glm;

namespace std
{
	template<>
	struct hash<dvec2>
	{
		size_t operator ()(const dvec2& value) const
		{
			uint64_t key = ((uint64_t)value.x) ^ (uint64_t)value.y;
			key ^= (key >> 33);
			key *= 0xff51afd7ed558ccd;
			key ^= (key >> 33);
			key *= 0xc4ceb9fe1a85ec53;
			key ^= (key >> 33);
			return (size_t)key;
		}
	};
	template<>
	struct hash<vec2>
	{
		size_t operator ()(const vec2& value) const
		{
			uint64_t key = ((uint64_t)value.x) << 32 | (uint64_t)value.y;
			key ^= (key >> 33);
			key *= 0xff51afd7ed558ccd;
			key ^= (key >> 33);
			key *= 0xc4ceb9fe1a85ec53;
			key ^= (key >> 33);
			return (size_t)key;
		}
	};
	template<>
	struct hash<ivec2>
	{
		size_t operator ()(const ivec2& value) const
		{
			uint64_t key = ((uint64_t)value.x) << 32 | (uint64_t)value.y;
			key ^= (key >> 33);
			key *= 0xff51afd7ed558ccd;
			key ^= (key >> 33);
			key *= 0xc4ceb9fe1a85ec53;
			key ^= (key >> 33);
			return (size_t)key;
		}
	};


}

#endif