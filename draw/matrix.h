#pragma once

typedef float Matrix4[16];

void Matrix4Perspective(Matrix4 M, float fovy, float aspect, float znear, float zfar);
void Matrix4Translation(Matrix4 M, float x, float y, float z);
void Matrix4Rotation(Matrix4 M, float x, float y, float z);
void Matrix4Mul(Matrix4 M, Matrix4 A, Matrix4 B);
