#pragma once

#include "basic.h"
#include "Templates/concepts.h"

template <numeric number_type>
struct vector2;
template <numeric number_type>
struct vector3;
template <numeric number_type>
struct vector4;

using vec2f = vector2<float>;
using vec3f = vector3<float>;
using vec4f = vector4<float>;
using vec2i = vector2<s32>;
using vec3i = vector3<s32>;
using vec4i = vector4<s32>;

template <numeric number_type>
struct vector2 {
public:
	using value_type = number_type;

	value_type X;
	value_type Y;

	constexpr static vector2 Zeroes{0, 0};
	constexpr static vector2 Ones{1, 1};
	constexpr static vector2 UnitX{1, 0};
	constexpr static vector2 UnitY{0, 1};

	template <numeric other_number_type>
	FORCEINLINE explicit operator vector2<other_number_type>() const;
	template <numeric other_number_type>
	FORCEINLINE explicit operator vector3<other_number_type>() const;
	template <numeric other_number_type>
	FORCEINLINE explicit operator vector4<other_number_type>() const;
	
	FORCEINLINE value_type GetComponent(index Index) const;
	FORCEINLINE value_type& GetComponent(index Index);

	template <iso_precise<value_type> compatible_type>
	FORCEINLINE bool operator==(const vector2<compatible_type>& Other) const;

	FORCEINLINE vector2 operator-() const;

	// Component-wise addition
	template <iso_precise<value_type> compatible_type>
	FORCEINLINE vector2 operator+(const vector2<compatible_type>& Other) const;
	// Component-wise subtraction
	template <iso_precise<value_type> compatible_type>
	FORCEINLINE vector2 operator-(const vector2<compatible_type>& Other) const;
	// Component-wise multiplication
	template <iso_precise<value_type> compatible_type>
	FORCEINLINE vector2 operator*(const vector2<compatible_type>& Other) const;
	// Component-wise division
	template <iso_precise<value_type> compatible_type>
	FORCEINLINE vector2 operator/(const vector2<compatible_type>& Other) const;

	// Component-wise addition
	template <iso_precise<value_type> compatible_type>
	FORCEINLINE vector2 operator+=(const vector2<compatible_type>& Other);
	// Component-wise subtraction
	template <iso_precise<value_type> compatible_type>
	FORCEINLINE vector2 operator-=(const vector2<compatible_type>& Other);
	// Component-wise multiplication
	template <iso_precise<value_type> compatible_type>
	FORCEINLINE vector2 operator*=(const vector2<compatible_type>& Other);
	// Component-wise division
	template <iso_precise<value_type> compatible_type>
	FORCEINLINE vector2 operator/=(const vector2<compatible_type>& Other);

	// Component-wise addition
	template <iso_precise<value_type> compatible_type>
	FORCEINLINE vector2 operator+(compatible_type Other) const;
	// Component-wise subtraction
	template <iso_precise<value_type> compatible_type>
	FORCEINLINE vector2 operator-(compatible_type Other) const;
	// Component-wise multiplication
	template <iso_precise<value_type> compatible_type>
	FORCEINLINE vector2 operator*(compatible_type Other) const;
	// Component-wise division
	template <iso_precise<value_type> compatible_type>
	FORCEINLINE vector2 operator/(compatible_type Other) const;
	
	// Component-wise addition
	template <iso_precise<value_type> compatible_type>
	FORCEINLINE vector2 operator+=(compatible_type Other);
	// Component-wise subtraction
	template <iso_precise<value_type> compatible_type>
	FORCEINLINE vector2 operator-=(compatible_type Other);
	// Component-wise multiplication
	template <iso_precise<value_type> compatible_type>
	FORCEINLINE vector2 operator*=(compatible_type Other);
	// Component-wise division
	template <iso_precise<value_type> compatible_type>
	FORCEINLINE vector2 operator/=(compatible_type Other);
};

template <numeric number_type>
struct vector3 {
public:
	using value_type = number_type;

	value_type X;
	value_type Y;
	value_type Z;

	constexpr static vector3 Zeroes{0, 0, 0};
	constexpr static vector3 Ones{1, 1, 1};
	constexpr static vector3 UnitX{1, 0, 0};
	constexpr static vector3 UnitY{0, 1, 0};
	constexpr static vector3 UnitZ{0, 0, 1};
	constexpr static vector3 Forward{1, 0, 0};
	constexpr static vector3 Right{0, 1, 0};
	constexpr static vector3 Up{0, 0, 1};
	constexpr static vector3 Backward{-1, 0, 0};
	constexpr static vector3 Left{0, -1, 0};
	constexpr static vector3 Down{0, 0, -1};
};

template <numeric number_type>
struct vector4 {
public:
	using value_type = number_type;

	value_type X;
	value_type Y;
	value_type Z;
	value_type W;

	constexpr static vector4 ZeroVector{0, 0, 0, 0};
	constexpr static vector4 OneVector{1, 1, 1, 1};
	constexpr static vector4 UnitX{1, 0, 0, 0};
	constexpr static vector4 UnitY{0, 1, 0, 0};
	constexpr static vector4 UnitZ{0, 0, 1, 0};
	constexpr static vector4 UnitW{0, 0, 0, 1};
};
