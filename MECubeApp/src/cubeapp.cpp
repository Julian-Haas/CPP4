//-----------------------------------------------------------------------------
// Copyright (c) 2022 Sascha Worszeck. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "cubeapp.h"



#include "MicroEngine/d3d11util.h"
#include "MicroEngine/lightcomponent.h"
#include "MicroEngine/window.h"
#include "MicroEngine/d3d11device.h"
#include "MicroEngine/entity.h"
#include "MicroEngine/cameracomponent.h"
#include "MicroEngine/meshrenderercomponent.h"
#include "MicroEngine/primitivemeshes.h"

#include "MicroEngine/mesh.h"
#include "MicroEngine/input.h"
#include "MicroEngine/firstpersoncontrollercomponent.h"
#include "MicroEngine/directorywatcher.h"
#include "MicroEngine/transformcomponent.h"
#include "MicroEngine/vertex.h"

namespace capp
{

	CubeApp::CubeApp()
		: m_ControlledEntityID(me::INVALID_ENTITY_ID)
		, m_CameraID(me::INVALID_ENTITY_ID)
		, m_LightID(me::INVALID_ENTITY_ID)
	{
	}

	ExitCode::Enum CubeApp::Run(HINSTANCE hInst)
	{
		using namespace me;

		m_Window = std::make_unique<Window>("CubeApp", hInst);

		m_Renderer = std::make_shared<Renderer>();
		if (!m_Renderer->Init(m_Window->GetHWnd(), m_Window->GetWidth(), m_Window->GetHeight()))
			return ExitCode::GraphicInitError;

		m_SystemManager.AddSystem(m_Renderer);

		InitEntities();

        #ifndef TARGET_RETAIL
		    DirectoryWatcher watcher("../assets");
        #endif
		StopWatch stopWatch;

		while (m_Window->ProcessMessages())
		{
            const float deltaTime = stopWatch.Measure();
			#ifndef TARGET_RETAIL
			    watcher.CheckForChanges();
            #endif
			UpdateLogic(deltaTime);

			m_SystemManager.Update(deltaTime);

			Input::GetInstance()->OnFrameEnd();
		}

		return ExitCode::Success;
	}

	void CubeApp::InitEntities()
	{
		using namespace me;

		//Shaders
		{
		    D3D11Device* meDevice = D3D11Device::GetInstance();
			meDevice->AddVertexShader("assets://Mesh.hlsl", VertexLitTextured::s_Description);
			meDevice->AddPixelShader("assets://Mesh.hlsl", 64, 1);
			meDevice->AddVertexShader("assets://Terrain.hlsl", VertexTextured::s_Description, SHADER_NO_SETTINGS_BUFFER, 1);
			meDevice->AddVertexShader("assets://MeshUnlit.hlsl", VertexLitTextured::s_Description);
			meDevice->AddPixelShader("assets://MeshUnlit.hlsl", SHADER_NO_SETTINGS_BUFFER, 1);
		}

		//Camera
		{
            const auto camera = m_EntityManager.AddEntity();
			m_CameraID = camera->GetID();
			m_EntityManager.AddComponent<FirstPersonControllerComponent>(m_CameraID);
			auto cameraComp = m_EntityManager.AddComponent<CameraComponent>(m_CameraID);	
		}

	    //Camera2 (uncomment for split screen)
		/*{
			auto cameraComp = m_EntityManager.GetComponent<CameraComponent>(m_CameraID).lock();
			cameraComp->SetViewPortNormalized(0.5f, 1.0f, 0.0f, 0.0f);
            const auto camera = m_EntityManager.AddEntity();
			cameraComp = m_EntityManager.AddComponent<CameraComponent>(camera->GetID());
			cameraComp->SetViewPortNormalized(0.5f, 1.0f, 0.5f, 0.0f);
		}*/

		//Light
		{
            const auto light = m_EntityManager.AddEntity();
			m_LightID = light->GetID();
			auto lightComp = m_EntityManager.AddComponent<LightComponent>(m_LightID);
			lightComp->SetType(LightType::Directional);
			auto transform = light->GetComponent<TransformComponent>().lock();
			transform->Translate(0, 15, 30);
			transform->Rotate(45, 0, 0);

			auto meshRenderer = m_EntityManager.AddComponent<MeshRendererComponent>(m_LightID);
            const auto mesh = std::shared_ptr<Mesh>(CreateMeshFromFile("assets://pyramid.obj"));
			meshRenderer->SetMesh(mesh);
			Material& mat = mesh->GetSubMeshes()[0].m_Material;
		    mat.SetTexturePS(0, TextureInfo("me://textures/#ffff00ff"));
			mat.SetVertexShader("assets://MeshUnlit.hlsl");
			mat.SetPixelShader("assets://MeshUnlit.hlsl");
		}

		//Cube
		{
            const auto cube = m_EntityManager.AddEntity();
		    m_ControlledEntityID = cube->GetID();
			auto transform = cube->GetComponent<TransformComponent>().lock();
			transform->Translate(0.0f, 0.0f, 30);

			auto meshRenderer = m_EntityManager.AddComponent<MeshRendererComponent>(cube->GetID());

			Material cubeMat;
			cubeMat.AddShaderProperty(Color::s_White);
			cubeMat.AddShaderProperty(Color::s_Black);
			cubeMat.AddShaderProperty(Color::s_White);
			cubeMat.AddShaderProperty(30.0f);
			cubeMat.SetTexturePS(0, TextureInfo("assets://colormap.bmp"));
			cubeMat.SetVertexShader("assets://Mesh.hlsl");
			cubeMat.SetPixelShader("assets://Mesh.hlsl");
            const auto mesh = std::shared_ptr<Mesh>(CreateCube(10, 10, 10, cubeMat));
			meshRenderer->SetMesh(mesh);
		}

		//Terrain
		{
            const auto terrain = m_EntityManager.AddEntity();
			auto transform = terrain->GetComponent<TransformComponent>().lock();
			transform->Translate(0, -10.0f, 30.0f);

			Material terrainMat;
			terrainMat.AddShaderProperty(Color::s_White);
			terrainMat.AddShaderProperty(Color::s_Black);
			terrainMat.AddShaderProperty(Color::s_White);
			terrainMat.AddShaderProperty(10.0f);
			terrainMat.SetTextureVS(0, TextureInfo("assets://heightmap.bmp"));
			terrainMat.SetTexturePS(0, TextureInfo("assets://colormap.bmp"));
			terrainMat.SetVertexShader("assets://Terrain.hlsl");
			terrainMat.SetPixelShader("assets://Mesh.hlsl");
            const auto mesh = std::shared_ptr<Mesh>(CreatePlane(1, 1, 128, 128, terrainMat));
			auto meshRenderer = m_EntityManager.AddComponent<TerrainRendererComponent>(terrain->GetID());
			meshRenderer->SetMesh(mesh);
		}
	}

	void CubeApp::UpdateLogic(float deltaTime)
	{
		using namespace me;

		//Allow capturing mouse when the left button is held and it moves outside the window
		if(Input::GetInstance()->IsKeyDown(VK_LBUTTON))
			SetCapture(m_Window->GetHWnd());
		if (Input::GetInstance()->IsKeyUp(VK_LBUTTON))
			ReleaseCapture();

		FinalPostEffect* finalPE = m_Renderer->GetPostEffect<FinalPostEffect>();
		if (Input::GetInstance()->IsKeyDown(VK_ADD))
			finalPE->SetBrightness(finalPE->GetBrightness() + deltaTime);
		if (Input::GetInstance()->IsKeyDown(VK_SUBTRACT))
			finalPE->SetBrightness(finalPE->GetBrightness() - deltaTime);

		//Control selected entity
        const auto entity = m_EntityManager.GetEntity(m_ControlledEntityID).lock();
		auto controlledEntity = entity ? entity->GetComponent<TransformComponent>().lock() : std::shared_ptr<TransformComponent>();
		if (controlledEntity)
		{
			if (Input::GetInstance()->IsKeyDown('Z'))
			{
				controlledEntity->RotateLocal(0, 0, 50 * deltaTime);
			}

			if (Input::GetInstance()->IsKeyDown('X'))
			{
				controlledEntity->RotateLocal(50 * deltaTime, 0, 0);
			}

			if (Input::GetInstance()->IsKeyDown('Y'))
			{
				controlledEntity->RotateLocal(0, 50 * deltaTime, 0);
			}

			if (Input::GetInstance()->IsKeyDown(VK_NUMPAD8))
			{
				controlledEntity->TranslateLocal(0, 0, 10 * deltaTime);
			}

			if (Input::GetInstance()->IsKeyDown(VK_NUMPAD6))
			{
				controlledEntity->TranslateLocal(10 * deltaTime, 0, 0);
			}

			if (Input::GetInstance()->IsKeyDown(VK_NUMPAD2))
			{
				controlledEntity->TranslateLocal(0, 0, -10 * deltaTime);
			}

			if (Input::GetInstance()->IsKeyDown(VK_NUMPAD4))
			{
				controlledEntity->TranslateLocal(-10 * deltaTime, 0, 0);
			}
		}

		//Update all entities
		m_EntityManager.UpdateEntities(deltaTime);
	}
};