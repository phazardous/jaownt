#pragma once

namespace qm {
	
typedef float default_precision;

template <typename T> struct t_vec3;
template <typename T> struct t_mat3;
template <typename T> struct t_vec4;
template <typename T> struct t_mat4;

template <typename T> inline T t_dot3 (float const * a, float const * b) {
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

template <typename T> inline T t_dot4 (float const * a, float const * b) {
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3];
}

template <typename T> inline t_vec3<T> t_mat3_mult_vec3 (t_mat3<T> const & m, t_vec3<T> const & v) {
	t_vec3<T> r {
		t_dot3<T>(m[0], v),
		t_dot3<T>(m[1], v),
		t_dot3<T>(m[2], v),
	};
	return r;
}

template <typename T> inline t_vec4<T> t_mat4_mult_vec4 (t_mat4<T> const & m, t_vec4<T> const & v) {
	t_vec4<T> r {
		t_dot4<T>(m[0], v),
		t_dot4<T>(m[1], v),
		t_dot4<T>(m[2], v),
		t_dot4<T>(m[3], v)
	};
	return r;
}

template <typename T>
struct t_vec3 {
	float values [3] = { 0, 0, 1 };
	t_vec3() = default;
	t_vec3(T x, T y, T z = 1) : values {x, y, z} {}
	
	operator float const * () const { return &values[0]; }
	float & operator [] (size_t i) { return values[i]; }
	float const & operator [] (size_t i) const { return values[i]; }
	t_vec3 operator * (t_mat3<T> const & m) const { return t_mat3_mult_vec3<T>(m, *this); }
};

template <typename T>
struct t_vec4 {
	float values [4] = { 0, 0, 0, 1 };
	t_vec4() = default;
	t_vec4(T x, T y, T z, T w = 1) : values {x, y, z, w} {}
	
	operator float const * () const { return &values[0]; }
	float & operator [] (size_t i) { return values[i]; }
	float const & operator [] (size_t i) const { return values[i]; }
	t_vec4 operator * (t_mat4<T> const & m) const { return t_mat4_mult_vec4<T>(m, *this); }
};

template <typename T>
struct t_mat3 {
	T values [3][3] = {
		{ 1, 0, 0 },
		{ 0, 1, 0 },
		{ 0, 0, 1 },
	};
	t_mat3() = default;
	
	static t_mat3<T> scale(T x, T y) {
		t_mat3<T> m;
		m[0][0] = x;
		m[1][1] = y;
		return m;
	}
	
	static t_mat3<T> translate(T x, T y) {
		t_mat3<T> m;
		m[2][0] = x;
		m[2][1] = y;
		return m;
	}
	
	static t_mat3<T> ortho(T top, T bottom, T left, T right) {
		t_mat3<T> m;
		m[0][0] = 2 / (right - left);
		m[1][1] = 2 / (top - bottom);
		m[2][0] = - (right + left) / (right - left);
		m[2][1] = - (top + bottom) / (top - bottom);
		return m;
	}
	
	static t_mat3<T> tmod(T bottom, T top, T left, T right) {
		t_mat3<T> m;
		m[0][0] = right - left;
		m[1][1] = top - bottom;
		m[2][0] = left;
		m[2][1] = bottom;
		return m;
	}
	
	operator float * () { return &values[0][0]; }
	operator float const * () const { return &values[0][0]; }
	
	t_mat3<T> & operator = (t_mat3<T> const & other) = default;
	
	float * operator [] (size_t i) { return values[i]; }
	float const * operator [] (size_t i) const { return values[i]; }
	
	t_vec3<T> operator * (t_vec3<T> const & v) const { return t_mat3_mult_vec3<T>(*this, v); }
	t_mat3 operator * (t_mat3 const & other) const {
		t_mat3 m;
		m[0][0] = other.values[0][0] * this->values[0][0] + other.values[0][1] * this->values[1][0] + other.values[0][2] * this->values[2][0];
		m[0][1] = other.values[0][0] * this->values[0][1] + other.values[0][1] * this->values[1][1] + other.values[0][2] * this->values[2][1];
		m[0][2] = other.values[0][0] * this->values[0][2] + other.values[0][1] * this->values[1][2] + other.values[0][2] * this->values[2][2];
		
		m[1][0] = other.values[1][0] * this->values[0][0] + other.values[1][1] * this->values[1][0] + other.values[1][2] * this->values[2][0];
		m[1][1] = other.values[1][0] * this->values[0][1] + other.values[1][1] * this->values[1][1] + other.values[1][2] * this->values[2][1];
		m[1][2] = other.values[1][0] * this->values[0][2] + other.values[1][1] * this->values[1][2] + other.values[1][2] * this->values[2][2];
		
		m[2][0] = other.values[2][0] * this->values[0][0] + other.values[2][1] * this->values[1][0] + other.values[2][2] * this->values[2][0];
		m[2][1] = other.values[2][0] * this->values[0][1] + other.values[2][1] * this->values[1][1] + other.values[2][2] * this->values[2][1];
		m[2][2] = other.values[2][0] * this->values[0][2] + other.values[2][1] * this->values[1][2] + other.values[2][2] * this->values[2][2];
		return m;
	}
	
	t_mat3<T> & operator *= (t_mat3<T> const & other) {
		*this = other * *this;
		return *this;
	}
};

template <typename T>
struct t_mat4 {
	T values [4][4] = {
		{ 1, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 0, 0, 1 }
	};
	t_mat4() = default;
	
	static t_mat4<T> scale(T x, T y, T z) {
		t_mat4<T> m;
		m[0][0] = x;
		m[1][1] = y;
		m[2][2] = z;
		return m;
	}
	
	static t_mat4<T> translate(T x, T y, T z) {
		t_mat4<T> m;
		m[3][0] = x;
		m[3][1] = y;
		m[3][2] = z;
		return m;
	}
	
	static t_mat4<T> ortho(T top, T bottom, T left, T right, T near, T far) {
		t_mat4<T> m;
		m[0][0] = 2 / (right - left);
		m[1][1] = 2 / (top - bottom);
		m[2][2] = - 2 / (far - near);
		m[3][0] = - (right + left) / (right - left);
		m[3][1] = - (top + bottom) / (top - bottom);
		m[3][2] = - (far + near) / (far - near);
		return m;
	}
	
	operator float * () { return &values[0][0]; }
	operator float const * () const { return &values[0][0]; }
	
	t_mat4<T> & operator = (t_mat4<T> const & other) = default;
	
	float * operator [] (size_t i) { return values[i]; }
	float const * operator [] (size_t i) const { return values[i]; }
	
	t_vec4<T> operator * (t_vec4<T> const & v) const { return t_mat4_mult_vec4<T>(*this, v); }
	t_mat4 operator * (t_mat4 const & other) const {
		t_mat4 m;
		m[0][0] = other.values[0][0] * this->values[0][0] + other.values[0][1] * this->values[1][0] + other.values[0][2] * this->values[2][0] + other.values[0][3] * this->values[3][0];
		m[0][1] = other.values[0][0] * this->values[0][1] + other.values[0][1] * this->values[1][1] + other.values[0][2] * this->values[2][1] + other.values[0][3] * this->values[3][1];
		m[0][2] = other.values[0][0] * this->values[0][2] + other.values[0][1] * this->values[1][2] + other.values[0][2] * this->values[2][2] + other.values[0][3] * this->values[3][2];
		m[0][3] = other.values[0][0] * this->values[0][3] + other.values[0][1] * this->values[1][3] + other.values[0][2] * this->values[2][3] + other.values[0][3] * this->values[3][3];
		
		m[1][0] = other.values[1][0] * this->values[0][0] + other.values[1][1] * this->values[1][0] + other.values[1][2] * this->values[2][0] + other.values[1][3] * this->values[3][0];
		m[1][1] = other.values[1][0] * this->values[0][1] + other.values[1][1] * this->values[1][1] + other.values[1][2] * this->values[2][1] + other.values[1][3] * this->values[3][1];
		m[1][2] = other.values[1][0] * this->values[0][2] + other.values[1][1] * this->values[1][2] + other.values[1][2] * this->values[2][2] + other.values[1][3] * this->values[3][2];
		m[1][3] = other.values[1][0] * this->values[0][3] + other.values[1][1] * this->values[1][3] + other.values[1][2] * this->values[2][3] + other.values[1][3] * this->values[3][3];
		
		m[2][0] = other.values[2][0] * this->values[0][0] + other.values[2][1] * this->values[1][0] + other.values[2][2] * this->values[2][0] + other.values[2][3] * this->values[3][0];
		m[2][1] = other.values[2][0] * this->values[0][1] + other.values[2][1] * this->values[1][1] + other.values[2][2] * this->values[2][1] + other.values[2][3] * this->values[3][1];
		m[2][2] = other.values[2][0] * this->values[0][2] + other.values[2][1] * this->values[1][2] + other.values[2][2] * this->values[2][2] + other.values[2][3] * this->values[3][2];
		m[2][3] = other.values[2][0] * this->values[0][3] + other.values[2][1] * this->values[1][3] + other.values[2][2] * this->values[2][3] + other.values[2][3] * this->values[3][3];
		
		m[3][0] = other.values[3][0] * this->values[0][0] + other.values[3][1] * this->values[1][0] + other.values[3][2] * this->values[2][0] + other.values[3][3] * this->values[3][0];
		m[3][1] = other.values[3][0] * this->values[0][1] + other.values[3][1] * this->values[1][1] + other.values[3][2] * this->values[2][1] + other.values[3][3] * this->values[3][1];
		m[3][2] = other.values[3][0] * this->values[0][2] + other.values[3][1] * this->values[1][2] + other.values[3][2] * this->values[2][2] + other.values[3][3] * this->values[3][2];
		m[3][3] = other.values[3][0] * this->values[0][3] + other.values[3][1] * this->values[1][3] + other.values[3][2] * this->values[2][3] + other.values[3][3] * this->values[3][3];
		return m;
	}
	
	t_mat4<T> & operator *= (t_mat4<T> const & other) {
		*this = other * *this;
		return *this;
	}
};

typedef t_vec3<default_precision> vec3;
typedef t_mat3<default_precision> mat3;
typedef t_vec4<default_precision> vec4;
typedef t_mat4<default_precision> mat4;

}
