#pragma once

//For use by Labyrinth applications

#include "Labyrinth/Core/System/Base.h"
#include "Labyrinth/Core/System/Log.h"
#include "Labyrinth/Core/System/Assert.h"
#include "Labyrinth/Core/System/Ref.h"
#include "Labyrinth/Core/System/Cast.h"
#include "Labyrinth/Core/System/Types.h"

#include "Labyrinth/Core/Application.h"
#include "Labyrinth/Core/Layer.h"
#include "Labyrinth/Core/Timestep.h"
#include "Labyrinth/Core/CameraController.h"

#include "Labyrinth/IO/Input.h"
#include "Labyrinth/IO/KeyCodes.h"
#include "Labyrinth/IO/MouseButtonCodes.h"
#include "Labyrinth/IO/JSON.h"

#include "Labyrinth/Maths/Constant.h"

#include "Labyrinth/ImGui/ImGuiLayer.h"

#include "Labyrinth/Scene/Scene.h"
#include "Labyrinth/Scene/Entity.h"
#include "Labyrinth/Scene/Components.h"
#include "Labyrinth/Scene/SceneSerialiser.h"
#include "Labyrinth/Scene/NativeScript.h"

#include "Labyrinth/Assets/AssetManager.h"
#include "Labyrinth/Assets/AssetGroup.h"

// ---Renderer------------------------
#include "Labyrinth/Renderer/Renderer.h"
#include "Labyrinth/Renderer/Renderer2D.h"
#include "Labyrinth/Renderer/RenderCommand.h"

#include "Labyrinth/Renderer/Buffer.h"
#include "Labyrinth/Renderer/Shader.h"
#include "Labyrinth/Renderer/Framebuffer.h"
#include "Labyrinth/Renderer/Texture.h"
#include "Labyrinth/Renderer/SubTexture.h"
#include "Labyrinth/Renderer/VertexArray.h"

#include "Labyrinth/Renderer/Camera.h"
// -----------------------------------

#include "Labyrinth/Tools/Timer.h"