#pragma once
#include "d3dApp.h"
#include "UploadBuffer.h"


class BoxApp : public D3DApp
{
public:
	typedef D3DApp Super;
	BoxApp(HINSTANCE hInstance);
	virtual void Init() override;

protected:
	virtual void OnResize() override;
	virtual void Draw() override;
	virtual void Update() override;

	virtual void OnMouseDown(WPARAM key, int x, int y) override;
	virtual void OnMouseUp(WPARAM key, int x, int y) override;
	virtual void OnMouseMove(WPARAM key, int x, int y) override;

	void BuildDesciptorHeaps();
	void BuildConstantBuffers();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildBoxGeometry();
	void BuildPipelineStateObject();

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mRootSignature;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mCBVHeap;
	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;
	Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode;
	Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO;
	std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

	DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	POINT mLastMousePos;
	float mTheta = 1.5f * DirectX::XM_PI;
	float mPhi = DirectX::XM_PIDIV4;
	float mRadius = 5.0f;
};
