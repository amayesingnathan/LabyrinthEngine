#pragma once

#include <string>

#include "Common/Application.h"

#include <Labyrinth/Core/System/Base.h>

#include "Labyrinth/Events/Event.h"

#include <Labyrinth/IO/Filesystem.h>

#include <Labyrinth/Networking/ServerLayer.h>
#include <Labyrinth/Networking/ClientLayer.h>

#include <Labyrinth/Scripting/ScriptEngine.h>

int main(int argc, char* argv[]);

namespace Laby {

	using Resolution = slc::Resolution;

	struct ApplicationSpec : public slc::ApplicationSpecification
	{
		fs::path startupProject;
		ScriptEngineConfig scriptConfig;

		~ApplicationSpec() override {}
	};

	class Application : public slc::Application
	{
	public:
		Application(ApplicationSpec* spec);

	protected:
		template<IsClient T, typename... Args>
		void SetClient(Args&&... args)
		{
			mNetworkLayer = new T(std::forward<Args>(args)...);
			PushLayer(mNetworkLayer);
		}
		void SetClient()
		{
			mNetworkLayer = new ClientLayer();
			PushLayer(mNetworkLayer);
		}
		template<IsServer T, typename... Args>
		void SetServer(Args&&... args)
		{
			mNetworkLayer = new T(std::forward<Args>(args)...);
			PushLayer(mNetworkLayer);
		}

	public:
		static void SendNetMessage(const Message& msg);

		static void ReadSettings(const std::filesystem::path& settingsPath, ApplicationSpec& outSpec);
		static void WriteSettings(const std::filesystem::path& settingsPath);

	private:
		NetworkLayer* mNetworkLayer = nullptr;

	private:
		inline static Application* sInstance = nullptr;
		friend int ::main(int argc, char** argv);
	};
}