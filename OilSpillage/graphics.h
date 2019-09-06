#pragma once
#include "window.h"
#include "GameObject.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <unordered_map>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

class Graphics {
	Window* window;
	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	D3D11_VIEWPORT vp;

	ID3D11RenderTargetView* renderTargetView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RasterizerState* rasterState;
	ID3D11BlendState* alphaEnableBlendingState;
	ID3D11Buffer* viewProjBuffer;
	ID3D11Buffer* worldBuffer;

	ID3D11PixelShader* pxShader;
	ID3D11VertexShader* vxShader;
	ID3D11InputLayout* vertexLayout;
	ID3D11SamplerState* sampler;
	std::unordered_map<const char*, Mesh> meshes;
	std::vector<GameObject*> drawableObjects;

	float fieldOfView;
	float screenNear;
	float screenDepth;
	glm::mat4 projection;
	glm::mat4 view;
	ID3D11Debug* debug;
public:
	Graphics();
	~Graphics();
	bool init(Window* window, float fov);
	void loadMesh(const char* fileName);
	const Mesh* getMeshPointer(const char* fileName);
	void addToDraw(GameObject* o);
	void removeFromDraw(GameObject* o);
	void render();
	bool createShaders();
};
