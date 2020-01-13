#include <iostream>
#include <windows.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace std;
using namespace DirectX;

ostream& XM_CALLCONV operator <<(ostream& os, FXMVECTOR v)
{
	XMFLOAT3 dest;
	XMStoreFloat3(&dest, v);
	os << "(" << dest.x << ", " << dest.y << ", " << dest.z << ")";
	return os;
}

int main()
{
	cout.setf(ios_base::boolalpha);
	if (!XMVerifyCPUSupport())
	{
		cout << "directx math not supported!" << endl;
		return 0;
	}

	// ---------- XMVECTOR Setter 函数 ------------------
	XMVECTOR p = XMVectorZero();
	XMVECTOR q = XMVectorSplatOne();
	XMVECTOR u = XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
	XMVECTOR v = XMVectorReplicate(-2.0f);
	XMVECTOR w = XMVectorSplatZ(u);

	cout << "p = " << p << endl;
	cout << "q = " << q << endl;
	cout << "u = " << u << endl;
	cout << "v = " << v << endl;
	cout << "w = " << w << endl;

	// ----------- 向量函数 ------------------------------

	cout << "----------------------------------------------" << endl;
	XMVECTOR n = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	u = XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
	v = XMVectorSet(-2.0f, 1.0f, -3.0f, 0.0f);
	w = XMVectorSet(0.707f, 0.707f, 0.0f, 0.0f);

	XMVECTOR a = u + v;
	XMVECTOR b = u - v;
	XMVECTOR c = 10.0f * u;
	XMVECTOR L = XMVector3Length(u);
	XMVECTOR d = XMVector3Normalize(u);
	XMVECTOR s = XMVector3Dot(u, v);
	XMVECTOR e = XMVector3Cross(u, v);

	XMVECTOR projW;
	XMVECTOR perpW;
	XMVector3ComponentsFromNormal(&projW, &perpW, w, n);

	bool equal = XMVector3Equal(projW + perpW, w) != 0;
	bool notEqual = XMVector3NotEqual(projW + perpW, w) != 0;

	XMVECTOR vecAngle = XMVector3AngleBetweenVectors(projW, perpW);
	float angleRadians = XMVectorGetX(vecAngle);
	float angleDegree = XMConvertToDegrees(angleRadians);

	cout << "u = " << u << endl;
	cout << "v = " << v << endl;
	cout << "w = " << w << endl;
	cout << "n = " << n << endl;

	cout << "a = u + v = " << a << endl;
	cout << "b = u - v = " << b << endl;
	cout << "c = 10 * u = " << c << endl;
	cout << "L = Length(u) = " << L << endl;
	cout << "d = Normalize(u) = " << d << endl;
	cout << "s = dot(u, v) = " << s << endl;
	cout << "e = cross(u, v) = " << e << endl;

	cout << "Proj_n(w) = " << projW << endl;
	cout << "Perp_n(w) = " << perpW << endl;
	cout << "Proj_n(w) + Perp_n(w) = w : " << equal << endl;
	cout << "Proj_n(w) + Perp_n(w) != w : " << notEqual << endl;
	cout << "Angle between Proj_n(w) and Perp_n(w) in radians: " << angleRadians << endl;
	cout << "Angle between Proj_n(w) and Perp_n(w) in degrees: " << angleDegree << endl;

	cout << "------------------------------------------" << endl;

	p = XMVectorSet(2.0f, 2.0f, 1.0f, 0.0f);
	q = XMVectorSet(2.0f, -0.5f, 0.5f, 0.0f);
	u = XMVectorSet(1.0f, 2.0f, 4.0f, 8.0f);
	v = XMVectorSet(-2.0f, -1.0f, -3.0f, 2.5f);
	w = XMVectorSet(0.0f, XM_PIDIV4, XM_PIDIV2, XM_PI);

	cout << "XMVectorAbs(v) = " << XMVectorAbs(v) << endl;
	cout << "XMVectorCos(w) = " << XMVectorCos(w) << endl;
	cout << "XMVectorLog(u) = " << XMVectorLog(u) << endl;
	cout << "XMVectorExp(p) = " << XMVectorExp(p) << endl;
	cout << "XMVectorPow(u, p) = " << XMVectorPow(u, p) << endl;
	cout << "XMVectorSqrt(u) = " << XMVectorSqrt(u) << endl;
	cout << "XMVectorSwizzle(u, 2, 2, 1, 3) = " << XMVectorSwizzle<2, 2, 1, 3>(u) << endl;
	cout << "XMVectorSwizzle(u, 2, 1, 0, 3) = " << XMVectorSwizzle(u, 2, 1, 0, 3) << endl;
	cout << "XMVectorMultiply(u, v) = " << XMVectorMultiply(u, v) << endl;
	cout << "XMVectorSaturate(q) = " << XMVectorSaturate(q) << endl;
	cout << "XMvectorMin(p, v) = " << XMVectorMin(p, v) << endl;
	cout << "XMVectorMax(p, v) = " << XMVectorMax(p, v) << endl;

	return 0;
}