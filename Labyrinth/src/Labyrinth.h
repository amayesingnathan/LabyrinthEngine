#pragma once

//For use by Labyrinth applications

#include "Labyrinth/Common/Core/Base.h"
#include "Labyrinth/Common/Core/Log.h"
#include "Labyrinth/Common/Core/Assert.h"
#include "Labyrinth/Common/Core/Cast.h"

#include "Labyrinth/Common/Application.h"
#include "Labyrinth/Common/Layer.h"

#include "Labyrinth/Common/Timestep.h"
#include "Labyrinth/Common/CameraController.h"

#include "Labyrinth/IO/Input.h"
#include "Labyrinth/IO/KeyCodes.h"
#include "Labyrinth/IO/MouseButtonCodes.h"

#include "Labyrinth/Maths/Constant.h"
#include "Labyrinth/Maths/Quad.h"

#include "Labyrinth/ImGui/ImGuiLayer.h"

#include "Labyrinth/Scene/Scene.h"
#include "Labyrinth/Scene/Entity.h"
#include "Labyrinth/Scene/Components.h"
#include "Labyrinth/Scene/Serialiser.h"

#include "Labyrinth/Scripting/ScriptEngine.h"
#include "Labyrinth/Scripting/ScriptableEntity.h"

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

// Commented to avoid constant recompiles
//#include "Labyrinth/Tangle/inc/Tangle.h"