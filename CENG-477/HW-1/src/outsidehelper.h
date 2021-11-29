
#ifndef _OUTSIDEHELPERS_H_
#define _OUTSIDEHELPERS_H_

#include <cmath>
#include "parser.h"

// Vector product helpers
// --------------------------------------------------------------------------------------
float dot_product(const parser::Vec3f & vec1, const parser::Vec3f & vec2) {
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}

parser::Vec3f cross_product(const parser::Vec3f & vec1, const parser::Vec3f & vec2) {
    parser::Vec3f result_vector;
    result_vector.x = vec1.y * vec2.z - vec1.z * vec2.y;
    result_vector.y = vec1.z * vec2.x - vec1.x * vec2.z;
    result_vector.z = vec1.x * vec2.y - vec1.y * vec2.x;
    return result_vector;
}
// --------------------------------------------------------------------------------------


// Specific vector calculations
// --------------------------------------------------------------------------------------
float vec_length(const parser::Vec3f & vector) {
    return std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z );
}

parser::Vec3f normalize(parser::Vec3f vector) {
    float vecLength = vec_length(vector);
    if (vecLength == 0.0f) {
        parser::Vec3f zero_vector;
        zero_vector.x = 0;
        zero_vector.y = 0;
        zero_vector.z = 0;
        return zero_vector;
    }
    vector = vector_division(vecLength, vector);
    return vector;
}
// --------------------------------------------------------------------------------------

float calc_determinant(float a, float b, float c, float m,float n,float k, float x,float y,float z) {
    return (a * (n * z - y * k)) - (m * (z * b - c * y)) + (x * (b * k - c * n));
}

#endif