#pragma once
#include "d3dUtil.h"

class GeometryGenerator
{
public:
	struct Vertex
	{
		Vertex() { }

		Vertex(
			const DirectX::XMFLOAT3& p,
			const DirectX::XMFLOAT3& n,
			const DirectX::XMFLOAT3& t,
			const DirectX::XMFLOAT2& uv
		)
			: Position(p)
			, Normal(n)
			, TengentU(t)
			, TexC(uv)
		{ }

		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v
		)
			: Position(px, py, pz)
			, Normal(nx, ny, nz)
			, TengentU(tx, ty, tz)
			, TexC(u, v)
		{ }

		DirectX::XMFLOAT3 Position;
		DirectX::XMFLOAT3 Normal;
		DirectX::XMFLOAT3 TengentU;
		DirectX::XMFLOAT2 TexC;
	};

	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<std::uint32_t> Indices32;
		std::vector<std::uint16_t>& GetIndices16()
		{
			if (mIndices16.empty())
			{
				mIndices16.resize(Indices32.size());
				for (std::size_t i = 0; i < Indices32.size(); ++i)
				{
					mIndices16[i] = static_cast<std::uint16_t>(Indices32[i]);
				}
			}
			return mIndices16;
		}

	private:
		std::vector<std::uint16_t> mIndices16;
	};

	MeshData CreateCylinder(float bottomRadius, float topRadius,
		float height, int sliceCount, int stackCount)
	{
		MeshData meshData;

		float stackHeight = height / stackCount;
		float radiusStep = (topRadius - bottomRadius) / stackCount;
		int ringCount = stackCount + 1;

		for (int i = 0; i < ringCount; ++i)
		{
			float y = -0.5f * height + i * stackHeight;
			float r = bottomRadius + i * radiusStep;
			float dTheta = 2.0f * DirectX::XM_PI / sliceCount;
			for (int j = 0; j <= sliceCount; ++j)
			{
				Vertex vertex;
				float c = cosf(j * dTheta);
				float s = sinf(j * dTheta);
				vertex.Position = DirectX::XMFLOAT3(r * c, y, r * s);
				vertex.TexC.x = (float)j / sliceCount;
				vertex.TexC.y = 1.0f - (float)i / stackCount;
				
				vertex.TengentU = DirectX::XMFLOAT3(-s, 0.0f, c);
				float dr = bottomRadius - topRadius;
				DirectX::XMFLOAT3 bitangent(dr * c, -height, dr * s);

				DirectX::XMVECTOR T = DirectX::XMLoadFloat3(&vertex.TengentU);
				DirectX::XMVECTOR B = DirectX::XMLoadFloat3(&bitangent);
				DirectX::XMVECTOR N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(T, B));
				DirectX::XMStoreFloat3(&vertex.Normal, N);

				meshData.Vertices.push_back(vertex);
			}
		}

		int ringVertexCount = sliceCount;
		for (int i = 0; i < stackCount; ++i)
		{
			for (int j = 0; j < sliceCount; ++j)
			{
				meshData.Indices32.push_back(i * ringVertexCount + j);
				meshData.Indices32.push_back((i + 1) * ringVertexCount + j);
				meshData.Indices32.push_back((i + 1) * ringVertexCount + j + 1);

				meshData.Indices32.push_back(i * ringVertexCount + j);
				meshData.Indices32.push_back((i + 1) * ringVertexCount + j + 1);
				meshData.Indices32.push_back(i * ringVertexCount + j + 1);
			}
		}

		BuildCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
		BuildCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, meshData);
	}

	MeshData CreateSphere();
	MeshData CreateGeoSphere(float radius, int numSubdivisions)
	{
		MeshData meshData;

		numSubdivisions = std::min<int>(numSubdivisions, 6);
	}

	MeshData CreateBox();
	MeshData CreateGrid();

private:
	void BuildCylinderTopCap(float bottomRadius, float topRadius, float height,
		int sliceCount, int stackCount, MeshData& meshData);
	void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height,
		int sliceCount, int stackCount, MeshData& meshData);
};
