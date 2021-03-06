#pragma once
#include<d3d11.h>
#include<wrl/client.h>
#include<d3dcompiler.h>
#include"..///DynamicCamera.h"
#include<mutex>
using namespace DirectX::SimpleMath;
struct Particle
{
	Vector4 position;
	Vector4 direction;
	Vector2 time;//time and totalTime
};
struct CameraInfo
{
	Matrix viewProj;
	Vector4 camPos;
	Vector4 upp;
};
struct ParticleParams
{
	Vector4 emitterLocation;
	Vector4 randomVector;
	Vector4 initialDirection;
};
struct ParticlesAdd
{
	ParticleParams arr[32];
};
struct ParticleRenderParams
{
	Vector4 colors[4];
	Vector4 config;//x = nrOfColors, y = startSize, z = endSize
};
struct SimulationParams
{
	Vector4 timeFactors;
	Vector4 vectorField;
	Vector4 physicsConfig; //.x = mass, .y = gravity
};
struct IndirDraw
{
	UINT vertexCountPerInstance;//particle count
	UINT instanceCount;//1
	UINT vertexStartLoc;
	UINT instanceStartLoc;
};
struct ParticleShaders
{
	char csUpdate[128];
	char csCreate[128];
	char gsPrimitive[128];
	char pixelShader[128];
	char vertexShader[128];
};
struct ParticleSData
{
	char name[128];
	ParticleShaders shaders;
	ParticleRenderParams renderParams;
	float vectorFieldPower;
	float vectorFieldSize;
};
class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();
	void setCapacity(int cap);
	void setOnlyAdd(bool arg);
	void setNameofSystem(std::string name);
	std::string getName();
	void initiateParticles(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	bool addParticle(int nrOf, float lifeTime, Vector3 position, Vector3 initialDirection);
	bool addParticle(int nrOf, float lifeTime, Vector3 position, Vector4 initialDirection);
	void addParticleToList(float lifeTime, Vector3 position, Vector4 initialDirection);
	void updateParticles(float delta, Matrix viewProj);
	void changeColornSize(Vector4 colors[4], int nrOfColors, float startSize, float endSize);
	void setSize(float startSize, float endSize);
	void setColor(Vector4 colors[4], int nrOfColors);
	void setMass(float mass);
	void setTexture(Texture* texture);
	//gravity = -1 means -1 * 9.82
	void setGravity(float gravity);
	void changeVectorField(float vectorFieldPower, float vectorFieldSize);
	void setParticleShaders(std::string csUpdate, std::string csCreate, std::string vertexShader = "ParticleVS.cso", std::string pixelShader = "ParticlePS.cso");
	void setVertexShader(std::string vertexShader);
	void setUpdateShader(std::string csUpdate);
	void setCreateShader(std::string csCreate);
	void setPixelShader(std::string pixelShader);
	void setBufferType(D3D11_BUFFER_UAV_FLAG flag);

	void clearSystem();
	void drawAll(DynamicCamera* camera);
	bool loadSystem();
	bool saveSystem();

	void setQuad();

	void setShaders();

	float getStartSize()const;
	float getEndSize()const;
	float getVectorFieldPower()const;
	float getVectorFieldSize()const;
	Vector4 getColor(int index)const;
private:

	std::wstring StringToWString(const std::string& aString)
	{
		std::wstring temp(aString.length(), L' ');
		std::copy(aString.begin(), aString.end(), temp.begin());
		return temp;
	};

	std::string WStringToString(const std::wstring& aWstring)
	{
		std::string temp(aWstring.length(), ' ');
		std::copy(aWstring.begin(), aWstring.end(), temp.begin());
		return temp;
	};
	int capParticle;//100*512
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	int nrOfParticles;
	float otherFrame;
	int firstAdd;
	float deltaTime;
	float sinMovement;
	IndirDraw indDraw;
	ParticleParams pParams;
	ParticlesAdd addArr;

	SimulationParams sP;
	ParticleSData systemData;
	bool quad;
	bool onlyAdd;
	D3D11_BUFFER_UAV_FLAG bufferType;

	std::vector<ParticleParams> addList;
	std::mutex mx;
	int indexForTrail;
	Texture* texture;
	Microsoft::WRL::ComPtr<ID3D11Buffer> particleParamCB;//For compshader add
	Microsoft::WRL::ComPtr<ID3D11Buffer> particleParamArr;//For adding a batch of 32

	Microsoft::WRL::ComPtr<ID3D11Buffer> particleParamRenderCB;//For the draw
	Microsoft::WRL::ComPtr<ID3D11Buffer> viewProjBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> nrOfParticlesCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> simParams;//for update
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;//for update

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> pixelShaderBlob;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> computeShaderBlob;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> createComputeShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> createComputeShaderBlob;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> clearComputeShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> clearComputeShaderBlob;

	Microsoft::WRL::ComPtr<ID3D11Buffer> particlesBuffer;//one will be the updated
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particlesUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particlesSRV;

	Microsoft::WRL::ComPtr<ID3D11Buffer> particlesBuffer2;//one will be the current
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particlesUAV2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particlesSRV2;

	Microsoft::WRL::ComPtr<ID3D11Buffer> indArgsBuffer;//one will be the current

};
