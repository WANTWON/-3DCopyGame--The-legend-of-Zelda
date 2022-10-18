#include "stdafx.h"
#include "..\Public\Imgui_Manager.h"
#include "imgui.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CImgui_Manager)

CImgui_Manager::CImgui_Manager()
	: m_pTerrain_Manager(CTerrain_Manager::Get_Instance())
{
	Safe_AddRef(m_pTerrain_Manager);
}


HRESULT CImgui_Manager::Initialize(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{

	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;


	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	return S_OK;
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CTerrain_Manager::TERRAINDESC TerrainDesc = m_pTerrain_Manager->Get_TerrainDesc();

	if (pGameInstance->Key_Up(DIK_LEFT))
		TerrainDesc.m_iPositionX -= m_pTerrain_Manager->Get_MoveOffset();

	if (pGameInstance->Key_Up(DIK_RIGHT))
		TerrainDesc.m_iPositionX += m_pTerrain_Manager->Get_MoveOffset();

	if (pGameInstance->Key_Up(DIK_UP))
		TerrainDesc.m_iPositionZ += m_pTerrain_Manager->Get_MoveOffset();

	if (pGameInstance->Key_Up(DIK_DOWN))
		TerrainDesc.m_iPositionZ -= m_pTerrain_Manager->Get_MoveOffset();

	if (pGameInstance->Key_Up(DIK_SPACE))
	{
		LEVEL pLevelIndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

		if (FAILED(m_pTerrain_Manager->Create_Terrain(pLevelIndex, TEXT("Layer_Terrain"))))
			return;
	}

	m_pTerrain_Manager->Set_TerrainDesc(&TerrainDesc);
	RELEASE_INSTANCE(CGameInstance);

	ShowGui();

	


	ImGui::EndFrame();

}

void CImgui_Manager::Render()
{
	// Rendering


	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());



}

void CImgui_Manager::ShowGui()
{
	ImGui::Begin(u8"Editor", NULL, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_HorizontalScrollbar
		| ImGuiWindowFlags_AlwaysVerticalScrollbar);


	//SettingMenu();
	//메뉴바
	if (ImGui::BeginMenuBar())
	{
		// 메뉴
		if (ImGui::BeginMenu("File"))
		{

			if (ImGui::MenuItem("save"))
			{
				int a = 0;
			}

			ImGui::Separator();
			if (ImGui::MenuItem("open"))
			{
				int a = 0;
			}

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Debug"))
		{
			ImGui::MenuItem("Show Mouse Pos", NULL, &m_bShowSimpleMousePos);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}


	const char* Level[] = { "Level_GamePlay", "Level_Boss" };
	static int iCurrentLevel = 0;
	ImGui::Combo("##0", &iCurrentLevel, Level, IM_ARRAYSIZE(Level));

	ImGui::NewLine();


	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Terrain Tool"))
		{
			Terrain_Map();
			Object_Map();
			View_Selected_Object_Info();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Camera Tool"))
		{
			ImGui::Text("This is the Camera Tool tab!\nblah blah blah blah blah");
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Tool,,Tool"))
		{
			ImGui::Text("This is the Tool,,Tool Tool tab!\nblah blah blah blah blah");
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	if (m_bShowSimpleMousePos)      ShowSimpleMousePos(&m_bShowSimpleMousePos);

	ImGui::End();

}


void CImgui_Manager::Terrain_Map()
{

	_bool bCreateTerrain;

	ImGui::GetIO().NavActive = false;
	ImGui::GetIO().WantCaptureMouse = true;

	if (!ImGui::CollapsingHeader("Terrain_Map"))
		return;

	CTerrain_Manager::TERRAINDESC TerrainDesc = m_pTerrain_Manager->Get_TerrainDesc();

	static _int iNumVertice[2] = { TerrainDesc.m_iVerticeNumX, TerrainDesc.m_iVerticeNumZ };
	ImGui::Text("NumVerticeX / NumVerticeZ");
	ImGui::SameLine(0, 10);
	ImGui::InputInt2("##1", iNumVertice, 10);
	TerrainDesc.m_iVerticeNumX = iNumVertice[0];
	TerrainDesc.m_iVerticeNumZ = iNumVertice[1];



	static _int iTerrianX = TerrainDesc.m_iPositionX;
	iTerrianX = TerrainDesc.m_iPositionX;
	ImGui::Text("Position X");
	ImGui::SameLine();
	ImGui::Text("%d", iTerrianX);


	static _int iTerrianZ = TerrainDesc.m_iPositionZ;
	iTerrianZ = TerrainDesc.m_iPositionZ;
	ImGui::Text("Position Z");
	ImGui::SameLine();
	ImGui::Text("%d", iTerrianZ);


	static _float fTerrianHeight = TerrainDesc.m_fHeight;
	ImGui::Text("Position Y");
	ImGui::SameLine();
	ImGui::SliderFloat("##PositionY", &fTerrianHeight, -10, 10);
	TerrainDesc.m_fHeight = fTerrianHeight;

	static _int iOffset = m_pTerrain_Manager->Get_MoveOffset();
	ImGui::Text("Move Offset");
	ImGui::SameLine();
	ImGui::InputInt("##MoveOffset", &iOffset);
	m_pTerrain_Manager->Set_MoveOffset(iOffset);


	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(0.f, 0.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::ImColor(0.f, 1.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::ImColor(1.f, 0.f, 0.f));
	bCreateTerrain = ImGui::Button("Create Terrain");
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::Checkbox("WireFrame", &TerrainDesc.m_bShowWireFrame);
	m_pTerrain_Manager->Set_bWireFrame(TerrainDesc.m_bShowWireFrame);
	ImGui::SameLine();
	ImGui::Checkbox("Show", &m_bTerrain_Show);
	m_pTerrain_Manager->Set_TerrainShow(m_bTerrain_Show);
	ImGui::NewLine();

	m_pTerrain_Manager->Set_TerrainDesc(&TerrainDesc);

	if (bCreateTerrain)
	{
		LEVEL pLevelIndex = (LEVEL)CLevel_Manager::Get_Instance()->Get_CurrentLevelIndex();

		if (FAILED(m_pTerrain_Manager->Create_Terrain(pLevelIndex, TEXT("Layer_Terrain"))))
			return;

		bCreateTerrain = false;
	}




}


void CImgui_Manager::Object_Map()
{
	_bool ret;

	ImGui::GetIO().NavActive = false;
	ImGui::GetIO().WantCaptureMouse = true;

	if (!ImGui::CollapsingHeader("Object_Map"))
		return;

	static _float Pos[3] = { TempPos.x , TempPos.y, TempPos.z };
	ImGui::Text("set Scale");
	ImGui::SameLine();
	ImGui::InputFloat3("##1", Pos);

	static _float Pos2[3] = { TempPos.x , TempPos.y, TempPos.z };
	ImGui::Text("set Rotation AXIS");
	ImGui::SameLine();
	ImGui::InputFloat3("##2", Pos2);
	//Pos2.x = m_TerrainInfo.vPos.x;
	//Pos2.y = m_TerrainInfo.vPos.y;
	//Pos2.z = m_TerrainInfo.vPos.z;

	static _float Pos3[3] = { TempPos.x , TempPos.y };
	ImGui::Text("Rotaion Angle / dist");
	ImGui::SameLine();
	ImGui::InputFloat2("##3", Pos3);

	//	Pos3.y = 1.f;

	ImGui::Text("vPos X : %f", TempPos.x);
	ImGui::SameLine();
	ImGui::Text("vPos Y : %f", TempPos.x);
	ImGui::SameLine();
	ImGui::Text("vPos Z : %f", TempPos.x);
	ImGui::Text("fAngle : %f", TempPos.x);
	ImGui::SameLine();
	ImGui::Text("Dist : %f", Pos3[1]);
	ImGui::NewLine();

	const char* ObjectID[] = { "OBJ_BACKGROUND", "OBJ_MONSTER", "OBJ_BLOCK", "OBJ_INTERATIVE", "OBJ_UNINTERATIVE", "OBJ_END" };
	static int iObjectID = 5;
	ImGui::Combo("Object_ID", &iObjectID, ObjectID, IM_ARRAYSIZE(ObjectID));
	m_eObjID = (OBJID)iObjectID;


	switch (m_eObjID)
	{

	case Client::OBJ_BACKGROUND:
	{
		const char* ObjectList[] = { "Terrain1", "Terrain2", "Terrain3", "Terrain4" };
		static int ObjectCurrentList = 0; // Here we store our selection data as an index.
		ImGui::Combo("ObjectList", &ObjectCurrentList, ObjectList, IM_ARRAYSIZE(ObjectList));
		break;
	}
	case Client::OBJ_MONSTER:
	{
		const char* ObjectList[] = { "Pig", "Mpbline", "Spider", "Bearger", "Boarwarrior", "Boss" };
		static int ObjectCurrentList = 0; // Here we store our selection data as an index.
		ImGui::Combo("ObjectList", &ObjectCurrentList, ObjectList, IM_ARRAYSIZE(ObjectList));
		break;
	}
	case Client::OBJ_BLOCK:
	{
		const char* ObjectList[] = { "Block1", "Block2", "Block3" };
		static int ObjectCurrentList = 0; // Here we store our selection data as an index.
		ImGui::Combo("ObjectList", &ObjectCurrentList, ObjectList, IM_ARRAYSIZE(ObjectList));
		break;
	}
	case Client::OBJ_INTERATIVE:
	{
		const char* ObjectList[] = { "Grass", "Tree", "Pot", "Tent", "Food", "Flower" };
		static int ObjectCurrentList = 0; // Here we store our selection data as an index.
		ImGui::Combo("ObjectList", &ObjectCurrentList, ObjectList, IM_ARRAYSIZE(ObjectList));
		break;
	}
	case Client::OBJ_UNINTERATIVE:
	{
		const char* ObjectList[] = { "Wall", "Rock", "Deco1", "Deco2", "Deco3" };
		static int ObjectCurrentList = 0; // Here we store our selection data as an index.
		ImGui::Combo("ObjectList", &ObjectCurrentList, ObjectList, IM_ARRAYSIZE(ObjectList));
		break;
	}
	default:
		break;
	}




	//ImGui::Text("OK");
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::ImColor(0.f, 0.f, 1.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::ImColor(0.f, 1.f, 0.f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::ImColor(1.f, 0.f, 0.f));
	ret = ImGui::Button("Create_Object");
	ImGui::PopStyleColor(3);
	ImGui::NewLine();

}

void CImgui_Manager::View_Selected_Object_Info()
{
	_bool ret;

	ImGui::GetIO().NavActive = false;
	ImGui::GetIO().WantCaptureMouse = true;

	if (!ImGui::CollapsingHeader("View Selected Object Info"))
		return;

	const char* ObjectID[] = { "OBJ_BACKGROUND", "OBJ_MONSTER", "OBJ_BLOCK", "OBJ_INTERATIVE", "OBJ_UNINTERATIVE", "OBJ_END" };
	static int iObjectID = m_eObjID;
	ImGui::Text("OBJECT_ID : ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "%s", ObjectID[iObjectID]);


	ImGui::Text("OBJECT_TYPE : ");
	ImGui::SameLine();
	switch (m_eObjID)
	{
	case Client::OBJ_BACKGROUND:
	{
		const char* ObjectList[] = { "Terrain1", "Terrain2", "Terrain3", "Terrain4" };
		static int ObjectCurrentList = m_iObjectList; // Here we store our selection data as an index.
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", ObjectList[ObjectCurrentList]);
		break;
	}
	case Client::OBJ_MONSTER:
	{
		const char* ObjectList[] = { "Pig", "Mpbline", "Spider", "Bearger", "Boarwarrior", "Boss" };
		static int ObjectCurrentList = m_iObjectList; // Here we store our selection data as an index.
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", ObjectList[ObjectCurrentList]);
		break;
	}
	case Client::OBJ_BLOCK:
	{
		const char* ObjectList[] = { "Block1", "Block2", "Block3" };
		static int ObjectCurrentList = m_iObjectList; // Here we store our selection data as an index.
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", ObjectList[ObjectCurrentList]);
		break;
	}
	case Client::OBJ_INTERATIVE:
	{
		const char* ObjectList[] = { "Grass", "Tree", "Pot", "Tent", "Food", "Flower" };
		static int ObjectCurrentList = m_iObjectList; // Here we store our selection data as an index.
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", ObjectList[ObjectCurrentList]);
		break;
	}
	case Client::OBJ_UNINTERATIVE:
	{
		const char* ObjectList[] = { "Wall", "Rock", "Deco1", "Deco2", "Deco3" };
		static int ObjectCurrentList = m_iObjectList; // Here we store our selection data as an index.
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "%s", ObjectList[ObjectCurrentList]);
		break;
	}
	case Client::OBJ_END:
	{
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "NONE");
		break;
	}
	default:
		break;
	}


	static _float fPosition[3] = { TempPos.x , TempPos.y, TempPos.z };
	ImGui::Text("Position (");
	ImGui::SameLine();
	ImGui::Text("%f, %f , %f )", fPosition);

	static _float fScale[3] = { TempPos.x , TempPos.y, TempPos.z };
	ImGui::Text("Scale (");
	ImGui::SameLine();
	ImGui::Text("%f, %f , %f )", fScale);

}

void CImgui_Manager::ShowSimpleMousePos(bool* p_open)
{
	static int location = 0;
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if (location >= 0)
	{
		const float PAD = 10.0f;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
		ImVec2 work_size = viewport->WorkSize;
		ImVec2 window_pos, window_pos_pivot;
		window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
		window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
		window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
		window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		window_flags |= ImGuiWindowFlags_NoMove;
	}
	else if (location == -2)
	{
		// Center window
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		window_flags |= ImGuiWindowFlags_NoMove;
	}
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	if (ImGui::Begin("Mouse Debug tab", p_open, window_flags))
	{
		ImGui::Text("Mouse Debug");
		ImGui::Separator();
		if (ImGui::IsMousePosValid())
		{
			_float3 vPickingPos = m_pTerrain_Manager->Get_PickingPos();
			ImGui::Text("Mouse UI: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
			ImGui::Text("Mouse World: (%.1f,%.1f,%.1f)", vPickingPos.x, vPickingPos.y, vPickingPos.z);
		}
		else
			ImGui::Text("Mouse Position: <invalid>");
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Custom", NULL, location == -1)) location = -1;
			if (ImGui::MenuItem("Center", NULL, location == -2)) location = -2;
			if (ImGui::MenuItem("Top-left", NULL, location == 0)) location = 0;
			if (ImGui::MenuItem("Top-right", NULL, location == 1)) location = 1;
			if (ImGui::MenuItem("Bottom-left", NULL, location == 2)) location = 2;
			if (ImGui::MenuItem("Bottom-right", NULL, location == 3)) location = 3;
			if (p_open && ImGui::MenuItem("Close")) *p_open = false;
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void CImgui_Manager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//CleanupDeviceD3D();
	//::DestroyWindow(hwnd);
	//::UnregisterClass(wc.lpszClassName, wc.hInstance);

	Safe_Release(m_pTerrain_Manager);
	CTerrain_Manager::Get_Instance()->Destroy_Instance();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
