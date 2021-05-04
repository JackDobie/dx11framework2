#include "Application.h"

Application* application;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
		return true;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_SIZE: // Handle window resizing
	{
		int width = LOWORD(lParam);
		int height = HIWORD(lParam);

		if (application != nullptr)
			application->ResizeWindow(height, width);

		break;
	}

	case WM_SETFOCUS:
		if (application != nullptr)
			application->focused = true;
		break;

	case WM_KILLFOCUS:
		if (application != nullptr)
			application->focused = false;
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pTerrainPlaneVertexBuffer = nullptr;
	_pTerrainPlaneIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
	CCWcullMode=nullptr;
	CWcullMode= nullptr;
	DSLessEqual = nullptr;
	RSCullNone = nullptr;
	_WindowHeight = 0;
	_WindowWidth = 0;
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	CreateDDSTextureFromFile(_pd3dDevice, L"Resources\\stone.dds", nullptr, &_pTextureRV);
	CreateDDSTextureFromFile(_pd3dDevice, L"Resources\\floor.dds", nullptr, &_pGroundTextureRV);
	//CreateDDSTextureFromFile(_pd3dDevice, L"Resources\\Hercules_COLOR.dds", nullptr, &_pHerculesTextureRV);
	
    // Setup Camera
	XMFLOAT3 eye = XMFLOAT3(0.0f, 1.0f, -4.0f);
	XMFLOAT3 at = XMFLOAT3(0.0f, 0.0f, 1.0f);
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);

	_camera = new Camera(eye, at, up, (float)_renderWidth, (float)_renderHeight, 0.01f, 1000.0f);

	//imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.WantCaptureMouse = true;
	//io.FontGlobalScale = 1.0f;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(_hWnd);
	ImGui_ImplDX11_Init(_pd3dDevice, _pImmediateContext);
	ImFont* font = io.Fonts->AddFontFromFileTTF("Fonts/ProggyVector Regular.ttf", 25.0f);
	IM_ASSERT(font != NULL);

	application = this;
	focused = true;

	// Setup the scene's light
	basicLight.AmbientLight = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	basicLight.DiffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	basicLight.SpecularLight = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	basicLight.SpecularPower = 20.0f;
	basicLight.LightVecW = XMFLOAT3(0.0f, 1.0f, -1.0f);

	//set up objects
	Geometry donutGeometry;
	objMeshData = OBJLoader::Load("Models/donut.obj", _pd3dDevice);
	donutGeometry.modelDimensions = objMeshData.ModelDimensions;
	donutGeometry.centreOfMass = objMeshData.CentreOfMass;
	donutGeometry.indexBuffer = objMeshData.IndexBuffer;
	donutGeometry.numberOfIndices = objMeshData.IndexCount;
	donutGeometry.vertexBuffer = objMeshData.VertexBuffer;
	donutGeometry.vertexBufferOffset = objMeshData.VBOffset;
	donutGeometry.vertexBufferStride = objMeshData.VBStride;
	
	Geometry cubeGeometry;
	objMeshData = OBJLoader::Load("Models/cube.obj", _pd3dDevice);
	cubeGeometry.modelDimensions = objMeshData.ModelDimensions;
	cubeGeometry.centreOfMass = objMeshData.CentreOfMass;
	cubeGeometry.indexBuffer = objMeshData.IndexBuffer;
	cubeGeometry.vertexBuffer = objMeshData.VertexBuffer;
	cubeGeometry.numberOfIndices = objMeshData.IndexCount;
	cubeGeometry.vertexBufferOffset = objMeshData.VBOffset;
	cubeGeometry.vertexBufferStride = objMeshData.VBStride;

	/*Geometry planeGeometry;
	planeGeometry.modelDimensions = XMFLOAT3(2.0f, 2.0f, 0.0f);
	planeGeometry.centreOfMass = XMFLOAT3(0.0f, 0.0f, 0.0f);
	planeGeometry.indexBuffer = _pPlaneIndexBuffer;
	planeGeometry.vertexBuffer = _pPlaneVertexBuffer;
	planeGeometry.numberOfIndices = 6;
	planeGeometry.vertexBufferOffset = 0;
	planeGeometry.vertexBufferStride = sizeof(SimpleVertex);*/

	Geometry terrainPlaneGeometry;
	terrainPlaneGeometry.modelDimensions = XMFLOAT3(10.0f, 0.0f, 10.0f);
	terrainPlaneGeometry.centreOfMass = XMFLOAT3(0.0f, 0.0f, 0.0f);
	terrainPlaneGeometry.vertexBuffer = _pTerrainPlaneVertexBuffer;
	terrainPlaneGeometry.indexBuffer = _pTerrainPlaneIndexBuffer;
	terrainPlaneGeometry.vertexBufferOffset = 0;
	terrainPlaneGeometry.vertexBufferStride = sizeof(SimpleVertex);
	Terrain* terrain = new Terrain(_pd3dDevice, &terrainPlaneGeometry);
	terrain->MakePlane(10, 10, 100, 100, "Resources/Heightmap.raw");

	Material shinyMaterial;
	shinyMaterial.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	shinyMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	shinyMaterial.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	shinyMaterial.specularPower = 10.0f;

	Material noSpecMaterial;
	noSpecMaterial.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	noSpecMaterial.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	noSpecMaterial.specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	noSpecMaterial.specularPower = 0.0f;

	//GameObject* gameObject = new GameObject("Floor", planeGeometry, noSpecMaterial, new Transform(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(XMConvertToRadians(90.0f), 0.0f, 0.0f), XMFLOAT3(15.0f, 15.0f, 15.0f)), true, 1.0f, false);
	///*gameObject->SetPosition(0.0f, 0.0f, 0.0f);
	//gameObject->SetScale(15.0f, 15.0f, 15.0f);
	//gameObject->SetRotation(XMConvertToRadians(90.0f), 0.0f, 0.0f);*/
	//gameObject->SetTextureRV(_pGroundTextureRV);

	GameObject* gameObject = new GameObject("Terrain", terrainPlaneGeometry, noSpecMaterial, new Transform(XMFLOAT3(0.0f, 0.0f, -10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)), true, 1.0f, false, 0.0f);
	gameObject->SetTextureRV(_pGroundTextureRV);

	_gameObjects.push_back(gameObject);

	for (int i = 0; i < NUMBER_OF_CUBES; i++)
	{
		gameObject = new GameObject("Cube " + to_string(i), cubeGeometry, shinyMaterial, new Transform(XMFLOAT3(-2.0f + (i * 4.0f), 0.0f, 10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f)), true, 2.5f, true, true);
		gameObject->SetTextureRV(_pTextureRV);
		_gameObjects.push_back(gameObject);
	}
	//gameObject = new GameObject("donut", donutGeometry, shinyMaterial, new Transform(XMFLOAT3(-4.0f, 0.5f, 10.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.5f, 0.5f, 0.5f)), true, 5.0f, false);
	///*gameObject->SetScale(0.5f, 0.5f, 0.5f);
	//gameObject->SetPosition(-4.0f, 0.5f, 10.0f);*/
	//gameObject->SetTextureRV(_pTextureRV);

	//_gameObjects.push_back(gameObject);

	//CreateGrid(5, 5, 10, 10);

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;
	
    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = _pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;
	
	// Create vertex buffer
	SimpleVertex planeVertices[] =
	{
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 5.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(5.0f, 5.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(5.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = planeVertices;

	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPlaneVertexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

	// Create plane index buffer
	WORD planeIndices[] =
	{
		0, 3, 1,
		3, 2, 1,
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * 6;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = planeIndices;
	hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pPlaneIndexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 1280, 720};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"FGGC Semester 2 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	UINT sampleCount = 4;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _renderWidth;
    sd.BufferDesc.Height = _renderHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
	sd.SampleDesc.Count = sampleCount;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_renderWidth;
    vp.Height = (FLOAT)_renderHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	InitVertexBuffer();
	InitIndexBuffer();

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

    if (FAILED(hr))
        return hr;

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _renderWidth;
	depthStencilDesc.Height = _renderHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = sampleCount;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

	// Rasterizer
	D3D11_RASTERIZER_DESC cmdesc;

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &RSCullNone);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	_pd3dDevice->CreateDepthStencilState(&dssDesc, &DSLessEqual);

	ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

	cmdesc.FillMode = D3D11_FILL_SOLID;
	cmdesc.CullMode = D3D11_CULL_BACK;

	cmdesc.FrontCounterClockwise = true;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &CCWcullMode);

	cmdesc.FrontCounterClockwise = false;
	hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &CWcullMode);

    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();
	if (_pSamplerLinear) _pSamplerLinear->Release();

	if (_pTextureRV) _pTextureRV->Release();

	if (_pGroundTextureRV) _pGroundTextureRV->Release();

    if (_pConstantBuffer) _pConstantBuffer->Release();

    if (_pTerrainPlaneVertexBuffer) _pTerrainPlaneVertexBuffer->Release();
    if (_pTerrainPlaneIndexBuffer) _pTerrainPlaneIndexBuffer->Release();
	if (_pPlaneVertexBuffer) _pPlaneVertexBuffer->Release();
	if (_pPlaneIndexBuffer) _pPlaneIndexBuffer->Release();

    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();

	if (DSLessEqual) DSLessEqual->Release();
	if (RSCullNone) RSCullNone->Release();

	if (CCWcullMode) CCWcullMode->Release();
	if (CWcullMode) CWcullMode->Release();

	if (_camera)
	{
		delete _camera;
		_camera = nullptr;
	}

	for (auto gameObject : _gameObjects)
	{
		if (gameObject)
		{
			delete gameObject;
			gameObject = nullptr;
		}
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Application::MoveLeft(int objectNumber)
{
	if (!_gameObjects[objectNumber]->GetRigidbody()->colliding)
		_gameObjects[objectNumber]->GetRigidbody()->AddVelOrAcc(XMFLOAT3(-10.0f, 0.0f, 0.0f));
	else if (_gameObjects[objectNumber]->GetRigidbody()->colliding)
		_gameObjects[objectNumber]->GetRigidbody()->AddVelOrAcc(XMFLOAT3(10.0f, 0.0f, 0.0f));
}
void Application::MoveRight(int objectNumber)
{
	if (!_gameObjects[objectNumber]->GetRigidbody()->colliding)
		_gameObjects[objectNumber]->GetRigidbody()->AddVelOrAcc(XMFLOAT3(10.0f, 0.0f, 0.0f));
	else if (_gameObjects[objectNumber]->GetRigidbody()->colliding)
		_gameObjects[objectNumber]->GetRigidbody()->AddVelOrAcc(XMFLOAT3(-10.0f, 0.0f, 0.0f));
}
void Application::MoveForward(int objectNumber)
{
	if(!_gameObjects[objectNumber]->GetRigidbody()->colliding)
		_gameObjects[objectNumber]->GetRigidbody()->AddVelOrAcc(XMFLOAT3(0.0f, 0.0f, 10.0f));
	else if(_gameObjects[objectNumber]->GetRigidbody()->colliding)
		_gameObjects[objectNumber]->GetRigidbody()->AddVelOrAcc(XMFLOAT3(0.0f, 0.0f, -10.0f));
}
void Application::MoveBackward(int objectNumber)
{
	if (!_gameObjects[objectNumber]->GetRigidbody()->colliding)
		_gameObjects[objectNumber]->GetRigidbody()->AddVelOrAcc(XMFLOAT3(0.0f, 0.0f, -10.0f));
	else if (_gameObjects[objectNumber]->GetRigidbody()->colliding)
		_gameObjects[objectNumber]->GetRigidbody()->AddVelOrAcc(XMFLOAT3(0.0f, 0.0f, 10.0f));
}

void Application::Update()
{
    // Update our time
    static float deltaTime = 0.0f;
    static DWORD dwTimeStart = 0;

    DWORD dwTimeCur = GetTickCount64();

    if (dwTimeStart == 0)
        dwTimeStart = dwTimeCur;

	deltaTime = (dwTimeCur - dwTimeStart) / 1000.0f;

	if (deltaTime < FPS_60)
	{
		return;
	}
	if (deltaTime >= FPS_60)
	{
		dwTimeStart = dwTimeCur;
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	DrawUI();

	if (focused)
	{
		Keyboard(deltaTime);
	}

	_camera->Update();

	// Update objects
	for (auto gameObject : _gameObjects)
	{
		gameObject->Update(deltaTime);
	}

	for (int i = 0; i < _gameObjects.size(); i++)
	{
		if (_gameObjects[i]->GetRigidbody()->GetCollisionEnabled())
		{
			for (int j = 0; j < _gameObjects.size(); j++)
			{
				if (j != i)
				{
					if (_gameObjects[j]->GetRigidbody()->GetCollisionEnabled())
					{
						if (_gameObjects[i]->GetRigidbody()->CheckCollision(_gameObjects[j]->GetTransform()->position, _gameObjects[j]->GetRigidbody()->GetAABBFaces()))
						{
							static int k = 0;
							Debug::Print(to_string(k++) + _gameObjects[i]->GetType() + " has collided with " + _gameObjects[j]->GetType() + "\n");
							_gameObjects[i]->GetRigidbody()->Collide(_gameObjects[j]->GetTransform()->position, _gameObjects[j]->GetRigidbody()->GetAABBFaces());
							//_gameObjects[j]->GetRigidbody()->Collide(_gameObjects[i]->GetTransform()->position, _gameObjects[i]->GetRigidbody()->GetBoundingSphereRadius());
							
							//_gameObjects[i] respond to collision
							// get velocity and reflect or something? or just reset vel/accel
						}
					}
				}
			}
		}
	}

	deltaTime -= FPS_60;

	Draw();
}

void Application::Draw()
{
    //
    // Clear buffers
    //

	float ClearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    //
    // Setup buffers and render scene
    //

	_pImmediateContext->IASetInputLayout(_pVertexLayout);

	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);

	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

    ConstantBuffer cb;

	XMFLOAT4X4 viewAsFloats = _camera->GetView();
	XMFLOAT4X4 projectionAsFloats = _camera->GetProjection();

	XMMATRIX view = XMLoadFloat4x4(&viewAsFloats);
	XMMATRIX projection = XMLoadFloat4x4(&projectionAsFloats);

	cb.View = XMMatrixTranspose(view);
	cb.Projection = XMMatrixTranspose(projection);
	
	cb.light = basicLight;
	cb.EyePosW = _camera->GetPos();

	// Render all scene objects
	for (auto gameObject : _gameObjects)
	{
		// Get render material
		Material material = gameObject->GetMaterial();

		// Copy material to shader
		cb.surface.AmbientMtrl = material.ambient;
		cb.surface.DiffuseMtrl = material.diffuse;
		cb.surface.SpecularMtrl = material.specular;

		// Set world matrix
		cb.World = XMMatrixTranspose(gameObject->GetWorldMatrix());

		// Set texture
		if (gameObject->HasTexture())
		{
			ID3D11ShaderResourceView * textureRV = gameObject->GetTextureRV();
			_pImmediateContext->PSSetShaderResources(0, 1, &textureRV);
			cb.HasTexture = 1.0f;
		}
		else
		{
			cb.HasTexture = 0.0f;
		}

		// Update constant buffer
		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

		// Draw object
		gameObject->Draw(_pImmediateContext);
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);
}

void Application::DrawUI()
{
	ImGui::Begin("Control Window");
	if (ImGui::CollapsingHeader("Objects"))//, ImGuiTreeNodeFlags_None)
	{
		static vector<string> headers;
		
		//auto collapse = [&](const char* label) {
		//	using namespace ImGui;
		//	ImGuiContext& g = *ImGui::GetCurrentContext();
		//	ImGuiWindow* window = g.CurrentWindow;
		//	ImGuiStorage* storage = window->DC.StateStorage;
		//	storage->SetInt(window->GetID(label), 0/*is_open*/);
		//};

		for each (GameObject * obj in _gameObjects)
		{
			string type = obj->GetType();
			if (!(type.find("Terrain") != string::npos))
			{
				if (!(find(headers.begin(), headers.end(), type) != headers.end()))
				{
					headers.push_back(type);
				}

				if (ImGui::CollapsingHeader(type.c_str()))
				{
					// display information on position and speed
					XMFLOAT3 pos = obj->GetRigidbody()->GetPosition();
					XMFLOAT3 vel = obj->GetRigidbody()->GetVelocity();
					XMFLOAT3 accel = obj->GetRigidbody()->GetAcceleration();
					bool* dragEnabled = obj->GetRigidbody()->GetDragEnabled();
					XMFLOAT3 dragForce = obj->GetRigidbody()->GetDragForce();
					bool* rotating = obj->GetRigidbody()->GetRotating();
					ImGui::Text(("Position:\nX:" + to_string(pos.x) + ", Y: " + to_string(pos.y) + ", Z: " + to_string(pos.z)).c_str());
					ImGui::Text(("Velocity:\nX:" + to_string(vel.x) + ", Y: " + to_string(vel.y) + ", Z: " + to_string(vel.z)).c_str());
					ImGui::Text(("Acceleration:\nX:" + to_string(accel.x) + ", Y: " + to_string(accel.y) + ", Z: " + to_string(accel.z)).c_str());
					if (*dragEnabled)
					{
						ImGui::Text(("Drag Force:\nX:" + to_string(dragForce.x) + ", Y: " + to_string(dragForce.y) + ", Z: " + to_string(dragForce.z)).c_str());
					}
					// buttons to reset position and speed
					if (ImGui::Button("Reset position"))
					{
						obj->ResetPosition();
					}
					if (ImGui::Button("Reset speed"))
					{
						obj->GetRigidbody()->SetVelocity(XMFLOAT3(0.0f, 0.0f, 0.0f));
						obj->GetRigidbody()->SetAcceleration(XMFLOAT3(0.0f, 0.0f, 0.0f));
					}
					ImGui::Checkbox("Drag", dragEnabled);
					if (*dragEnabled)
					{
						int e =	obj->GetRigidbody()->GetUseLaminarDrag();
						if (ImGui::RadioButton("Laminar Drag", &e, 1))
						{
							obj->GetRigidbody()->SetUseLaminarDrag(true);
						}
						ImGui::SameLine();
						if (ImGui::RadioButton("Turbulent Drag", &e, 0))
						{
							obj->GetRigidbody()->SetUseLaminarDrag(false);
						}

						ImGui::SliderFloat("Drag Factor", obj->GetRigidbody()->GetDragFactor(), 0.0f, 1.0f);
					}
					if (obj->GetRigidbody()->colliding)
						ImGui::Text("Colliding: true");
					else
						ImGui::Text("Colliding: false");

					ImGui::Checkbox("Rotate", obj->GetRigidbody()->GetRotating());
					if (*obj->GetRigidbody()->GetRotating())
					{
						ImGui::SliderFloat("Angular Damp", obj->GetRigidbody()->GetAngularDamp(), 0.0f, 1.0f);

						float torquePoint[3] = { obj->GetRigidbody()->GetTorquePoint().x, obj->GetRigidbody()->GetTorquePoint().y, obj->GetRigidbody()->GetTorquePoint().z };
						ImGui::InputFloat3("Torque Position", torquePoint);
						obj->GetRigidbody()->SetTorquePoint(XMFLOAT3(torquePoint[0], torquePoint[1], torquePoint[2]));

						float torqueForce[3] = { obj->GetRigidbody()->GetTorqueForce().x, obj->GetRigidbody()->GetTorqueForce().y, obj->GetRigidbody()->GetTorqueForce().z };
						ImGui::InputFloat3("Torque Force", torqueForce);
						obj->GetRigidbody()->SetTorqueForce(XMFLOAT3(torqueForce[0], torqueForce[1], torqueForce[2]));
					}
					else
					{
						obj->GetRigidbody()->SetAngularDamp(0.0f);
					}

					for each (string head in headers)
					{
						if (head != type) // for every header other than this
						{
							auto it = find(headers.begin(), headers.end(), head);
							int index = distance(headers.begin(), it);
							const char* c = headers[index].c_str();
							if (ImGui::GetStateStorage()->GetInt(ImGui::GetID(c)) != 0) // if other header is open
							{
								ImGui::GetStateStorage()->SetInt(ImGui::GetID(c), 0); // close header
							}
						}
					}
				}
			}
			else
			{
				ImGui::Text(obj->GetType().c_str());
			}
		}
	}

	if (ImGui::CollapsingHeader("Lighting"))//, ImGuiTreeNodeFlags_None)
	{
		float* temp[4] = { &basicLight.AmbientLight.x, &basicLight.AmbientLight.y, &basicLight.AmbientLight.z, &basicLight.AmbientLight.w };
		ImGui::SliderFloat4("Ambient Light", *temp, 0.0f, 1.0f);

		float* temp2[4] = { &basicLight.DiffuseLight.x, &basicLight.DiffuseLight.y, &basicLight.DiffuseLight.z, &basicLight.DiffuseLight.w };
		ImGui::SliderFloat4("Diffuse Light", *temp2, 0.0f, 1.0f);

		float* temp3[4] = { &basicLight.SpecularLight.x, &basicLight.SpecularLight.y, &basicLight.SpecularLight.z, &basicLight.SpecularLight.w };
		ImGui::SliderFloat4("Specular Light", *temp3, 0.0f, 1.0f);

		float* tempfloat = &basicLight.SpecularPower;
		ImGui::SliderFloat("SpecularPower", tempfloat, 0.1f, 50.0f);

		float* temp4[3] = { &basicLight.LightVecW.x, &basicLight.LightVecW.y, &basicLight.LightVecW.z };
		ImGui::InputFloat3("Light Vector", *temp4);
	}
	ImGui::End();
}

void Application::ResizeWindow(int height, int width)
{
	_WindowHeight = height;
	_WindowWidth = width;
	_camera->Reshape(_WindowWidth, _WindowHeight);
}

void Application::Keyboard(float deltaTime)
{
	//https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	// Move gameobjects
	if (GetAsyncKeyState(0x49) < 0) // I key
	{
		MoveForward(1);
	}
	if (GetAsyncKeyState(0x4B) < 0) // K key
	{
		MoveBackward(1);
	}
	if (GetAsyncKeyState(0x4A) < 0) // J key
	{
		MoveLeft(1);
	}
	if (GetAsyncKeyState(0x4C) < 0) // L key
	{
		MoveRight(1);
	}

	if (GetAsyncKeyState(VK_NUMPAD8) < 0)
	{
		MoveForward(2);
	}
	if (GetAsyncKeyState(VK_NUMPAD2) < 0)
	{
		MoveBackward(2);
	}
	if (GetAsyncKeyState(VK_NUMPAD4) < 0)
	{
		MoveLeft(2);
	}
	if (GetAsyncKeyState(VK_NUMPAD6) < 0)
	{
		MoveRight(2);
	}

	if (GetAsyncKeyState(0x51) < 0) // Q key
	{
		_gameObjects[1]->GetRigidbody()->SetThrustEnabled(true);
	}
	else
	{
		if (_gameObjects[1]->GetRigidbody()->GetThrustEnabled())
			_gameObjects[1]->GetRigidbody()->SetThrustEnabled(false);
	}

	if (GetAsyncKeyState(0x45) < 0) // E key
	{
		_gameObjects[2]->GetRigidbody()->SetThrustEnabled(true);
	}
	else
	{
		if (_gameObjects[2]->GetRigidbody()->GetThrustEnabled())
			_gameObjects[2]->GetRigidbody()->SetThrustEnabled(false);
	}

	//if (GetAsyncKeyState(0x45) & 0x0001) // E key
	//{
	//	bool rot = *_gameObjects[1]->GetRigidbody()->GetRotating();
	//	_gameObjects[1]->GetRigidbody()->SetRotating(!rot);
	//	//_gameObjects[1]->GetRigidbody()->Rotate(deltaTime);
	//}
	//if (GetAsyncKeyState(0x52) & 0x0001) // R key
	//{
	//	bool rot = *_gameObjects[2]->GetRigidbody()->GetRotating();
	//	_gameObjects[2]->GetRigidbody()->SetRotating(!rot);
	//	//_gameObjects[2]->GetRigidbody()->Rotate(deltaTime);
	//}

	if (GetAsyncKeyState(0x57) < 0)//if W is pressed down
	{
		//move cam forwards
		_camera->Move(deltaTime);
	}
	else if (GetAsyncKeyState(0x53) < 0)//if S is pressed down
	{
		//move cam backwards
		_camera->Move(-deltaTime);
	}
	if (GetAsyncKeyState(0x41) < 0)//if A is pressed down
	{
		//move cam left
		_camera->Strafe(-deltaTime);
	}
	else if (GetAsyncKeyState(0x44) < 0)//if D is pressed down
	{
		//move cam right
		_camera->Strafe(deltaTime);
	}

	if (GetAsyncKeyState(VK_UP) < 0) //if up arrow is pressed
	{
		//point cam up
		_camera->AddAt(XMFLOAT3(0.0f, -150.0f * deltaTime, 0.0f));
	}

	else if (GetAsyncKeyState(VK_DOWN) < 0) //if down arrow is pressed
	{
		//point cam down
		_camera->AddAt(XMFLOAT3(0.0f, 150.0f * deltaTime, 0.0f));
	}

	else if (GetAsyncKeyState(VK_LEFT) < 0) //if left arrow is pressed
	{
		//point cam left
		_camera->AddAt(XMFLOAT3(0.0f, 0.0f, -150.0f * deltaTime));
	}

	else if (GetAsyncKeyState(VK_RIGHT) < 0) //if right arrow is pressed
	{
		//point cam right
		_camera->AddAt(XMFLOAT3(0.0f, 0.0f, 150.0f * deltaTime));
	}
}