#pragma once
#include "d3dApp.h"
#include "UploadBuffer.h"


class BoxApp : public D3DApp
{
public:
	typedef D3DApp Super;
	BoxApp(HINSTANCE hInstance);

protected:
	virtual void Draw();
	virtual void Update();
	virtual void OnMouseMove(WPARAM key, int x, int y);
	void BuildConstantBuffers();

private:
	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCBVHeap;

	DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();
	POINT mLastMousePos;
	float mTheta = 1.5f * DirectX::XM_PI;
	float mPhi = DirectX::XM_PIDIV4;
	float mRadius = 5.0f;
};
