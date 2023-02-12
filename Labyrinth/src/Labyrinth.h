#pragma once

//For use by Labyrinth applications

#include <Labyrinth/Core/Application.h>
#include <Labyrinth/Core/UUID.h>

#include <Labyrinth/Assets/AssetManager.h>

#include <Labyrinth/Containers/Buffer.h>
#include <Labyrinth/Containers/Grid.h>
#include <Labyrinth/Containers/StaticString.h>
#include <Labyrinth/Containers/TSQueue.h>

#include <Labyrinth/ImGui/ImGuiWidgets.h>

#include <Labyrinth/IO/Input.h>
#include <Labyrinth/IO/Filesystem.h>
#include <Labyrinth/IO/KeyCodes.h>
#include <Labyrinth/IO/MouseCodes.h>
#include <Labyrinth/IO/JSON.h>
#include <Labyrinth/IO/YAML.h>

#include <Labyrinth/Networking/ClientLayer.h>
#include <Labyrinth/Networking/ServerLayer.h>

#include <Labyrinth/Project/Project.h>

#include "Labyrinth/Renderer/Renderer.h"
#include "Labyrinth/Renderer/Renderer2D.h"
#include "Labyrinth/Renderer/Buffer.h"
#include "Labyrinth/Renderer/Shader.h"
#include "Labyrinth/Renderer/Framebuffer.h"
#include "Labyrinth/Renderer/Texture.h"
#include "Labyrinth/Renderer/SubTexture.h"
#include "Labyrinth/Renderer/Camera.h"

#include "Labyrinth/Scene/Scene.h"
#include "Labyrinth/Scene/Entity.h"
#include "Labyrinth/Scene/Components.h"
#include "Labyrinth/Scene/SceneSerialiser.h"

#include "Labyrinth/Scripting/MarshalUtils.h"
#include "Labyrinth/Scripting/ScriptEngine.h"
#include "Labyrinth/Scripting/ScriptCache.h"

#include <Labyrinth/Tilemap/Tilemap.h>

#include "Labyrinth/Tools/EnumUtils.h"
#include "Labyrinth/Tools/MathUtils.h"
#include "Labyrinth/Tools/StringUtils.h"
#include "Labyrinth/Tools/Timing.h"