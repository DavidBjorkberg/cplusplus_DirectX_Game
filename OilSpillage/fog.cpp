#include "fog.h"
#include "States/PlayingGameState.h"
#include "PG/MapConfig.hpp"
#include <random>

using namespace DirectX::SimpleMath;

Fog::~Fog()
{
	while (quads.size() > 0)
	{
		delete quads[quads.size() - 1];
		quads.erase(quads.end() - 1);
	}

	while (fogTextures.size() > 0)
	{
		delete fogTextures[fogTextures.size() - 1];
		fogTextures.erase(fogTextures.end() - 1);
	}

}

void Fog::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slices, float spacing, FogMaterial material)
{
	Game::getGraphics().loadMesh("Quad");
	Game::getGraphics().loadShape(SHAPE_QUAD);
	Game::getGraphics().loadTexture("white1x1");
	this->spacing = spacing;
	for (UINT i = 1; i <= slices; ++i)
	{
		GameObject* q = new GameObject();
		q->mesh = Game::getGraphics().getMeshPointer("Quad");
		q->setPosition(Vector3(32*10, i * spacing - 1.5f, -32*10));
		q->setScale(Vector3(64.0 * 20, 64.0 * 20, 1.0));
		q->setColor(Vector4(0.0, 0.0, 0.0, (float(i) / slices)));
		q->setGlossMap(Game::getGraphics().getTexturePointer("white1x1"));
		q->setSpecularMap(Game::getGraphics().getTexturePointer("white1x1"));
		quads.push_back(q);
	}

	std::wstring shaderfolder = L"";
#pragma region DetermineShaderPath
#ifdef _DEBUG //Debug Mode
#ifdef _WIN64 //x64
	shaderfolder = L"..\\x64\\Debug\\";
#else  //x86 (Win32)
	shaderfolder = L"..\\Debug\\";
#endif
#else //Release Mode
#ifdef _WIN64 //x64
	shaderfolder = L"..\\x64\\Release\\";
#else  //x86 (Win32)
	shaderfolder = L"..\\Release\\";
#endif
#endif

	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{"SV_POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  }
	};
	UINT numElements = ARRAYSIZE(inputDesc);

	this->generateTextureShader.createVS(device.Get(), shaderfolder + L"SimpleVS.cso", inputDesc, numElements);
	this->generateTextureShader.createPS(device.Get(), shaderfolder + L"FogPS.cso");

	this->vp.Width = textureWidth;
	this->vp.Height = textureHeight;
	this->vp.MinDepth = 0.0f;
	this->vp.MaxDepth = 1.0f;
	this->vp.TopLeftX = 0;
	this->vp.TopLeftY = 0;

	
	generateTextures(device, deviceContext, material);

	this->drawShader.createVS(device.Get(), shaderfolder + L"FogDrawVS.cso", inputDesc, numElements);
	this->drawShader.createPS(device.Get(), shaderfolder + L"FogDrawPS.cso");
}

void Fog::generateTextures(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, FogMaterial& material)
{
	float clearColor[4] = {
		0,0,0,0
	};

	deviceContext->RSSetViewports(1, &this->vp);

	deviceContext->PSSetShader(this->generateTextureShader.ps.getShader(), nullptr, 0);
	deviceContext->VSSetShader(this->generateTextureShader.vs.getShader(), nullptr, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(this->generateTextureShader.vs.getInputLayout());

	Microsoft::WRL::ComPtr<ID3D11Buffer> viewProjBuffer;
	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(sizeof(Matrix) + (16 - (sizeof(Matrix) % 16)));
	desc.StructureByteStride = 0;
	device->CreateBuffer(&desc, 0, &viewProjBuffer);
	deviceContext->VSSetConstantBuffers(0, 1, viewProjBuffer.GetAddressOf());


	Microsoft::WRL::ComPtr<ID3D11Buffer> worldBuffer;
	desc.ByteWidth = static_cast<UINT>(sizeof(Matrix) + (16 - (sizeof(Matrix) % 16)));
	device->CreateBuffer(&desc, 0, &worldBuffer);
	deviceContext->VSSetConstantBuffers(1, 1, worldBuffer.GetAddressOf());


	Microsoft::WRL::ComPtr<ID3D11Buffer> materialBuffer;
	desc.ByteWidth = static_cast<UINT>(sizeof(FogMaterial));
	device->CreateBuffer(&desc, 0, &materialBuffer);
	deviceContext->VSSetConstantBuffers(0, 1, materialBuffer.GetAddressOf());

	Microsoft::WRL::ComPtr <ID3D11SamplerState> sampler;
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&samplerDesc, &sampler);
	deviceContext->PSSetSamplers(0, 1, sampler.GetAddressOf());

	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-1.0, 1.0);
	float* randomNumbers = new float[textureDesc.Height * textureDesc.Width * 2];
	for (int i = 0; i < textureDesc.Width * textureDesc.Height * 2; ++i) randomNumbers[i] = dis(gen);

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = randomNumbers;
	initData.SysMemPitch = textureDesc.Width * sizeof(float) * 2;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> randomLookUpTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> randomLookUpTextureView;

	device->CreateTexture2D(&textureDesc, &initData, &randomLookUpTexture);
	delete[] randomNumbers;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(randomLookUpTexture.Get(), &srvDesc, &randomLookUpTextureView);

	SimpleMath::Matrix viewProj = Matrix(
		Vector4(1.0, 0.0, 0.0, 0.0),
		Vector4(0.0, 1.0, 0.0, 0.0),
		Vector4(0.0, 0.0, 0.0, 1.0),
		Vector4(0.0, 0.0, 0.0, 0.0));
	SimpleMath::Matrix viewProjTr = DirectX::XMMatrixTranspose(viewProj);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(viewProjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &viewProjTr, sizeof(SimpleMath::Matrix));
	deviceContext->Unmap(viewProjBuffer.Get(), 0);

	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;

	device->CreateBlendState(&blendStateDescription, &blendState);

	float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	//deviceContext->OMSetBlendState(blendState.Get(), blendFactor, 0xffffffff);

	device->CreateSamplerState(&samplerDesc, sampler.GetAddressOf());
	deviceContext->OMSetDepthStencilState(NULL, 0);

	float scale = material.scale;
	float density = material.density;

	for (UINT i = 0; i < quads.size(); ++i)
	{
		GameObject* object = quads[i];
		UINT vertexCount = object->mesh->getVertexCount();
		UINT stride = sizeof(Vertex3D);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, object->mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		
		SimpleMath::Matrix world = Matrix::CreateTranslation(Vector3(0.0, 0.0, 1.0));
		SimpleMath::Matrix worldTr = DirectX::XMMatrixTranspose(world);
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceContext->Map(worldBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &worldTr, sizeof(SimpleMath::Matrix));
		deviceContext->Unmap(worldBuffer.Get(), 0);

		material.scale = scale * (1.0f + spacing * i * 1.0f);
		material.density = density / (1.0f + spacing * i * 1.0f);
		hr = deviceContext->Map(materialBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &material, sizeof(FogMaterial));
		deviceContext->Unmap(materialBuffer.Get(), 0);
		object->setColor(Vector4(material.color.x, material.color.z, material.color.z, 0.0));


		Texture* diffuseTexture = new Texture();
		diffuseTexture->Initialize(device.Get(), deviceContext.Get(), textureWidth, textureHeight);
		device.Get()->CreateRenderTargetView(diffuseTexture->getTexture2D(), NULL, textureRTV.ReleaseAndGetAddressOf());
		deviceContext.Get()->ClearRenderTargetView(textureRTV.Get(), clearColor);
		deviceContext.Get()->OMSetRenderTargets(1, textureRTV.GetAddressOf(), NULL);

		deviceContext->VSSetConstantBuffers(0, 1, viewProjBuffer.GetAddressOf());
		deviceContext->VSSetConstantBuffers(1, 1, worldBuffer.GetAddressOf());
		deviceContext->PSSetConstantBuffers(0, 1, materialBuffer.GetAddressOf());
		deviceContext->PSSetShaderResources(0, 1, randomLookUpTextureView.GetAddressOf());
		deviceContext->Draw(vertexCount, 0);

		fogTextures.push_back(diffuseTexture);
		object->setRotation(Vector3(XM_PIDIV2, 0.0, 0.0));
	}

	for (int i = 0; i < quads.size(); ++i)
	{
		quads[i]->setTexture(fogTextures[i]);
	}
}

std::vector<GameObject*>& Fog::getQuads()
{
	return quads;
}

const Vector2& Fog::getWindSpeed() const
{
	return wind;
}

void Fog::setWindSpeed(Vector2&& speed)
{
	this->wind = speed;
}
