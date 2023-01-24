#pragma once

#include <Labyrinth/Renderer/SubTexture.h>

namespace Laby {

	class EditorResources
	{
	public:
		// Viewport
		inline static Ref<Texture2D> PlayIcon = nullptr;
		inline static Ref<Texture2D> StopIcon = nullptr;
		inline static Ref<Texture2D> SimulateIcon = nullptr;

		// Content Browser Panel
		inline static Ref<Texture2D> FileIcon = nullptr;
		inline static Ref<Texture2D> DirectoryIcon = nullptr;
		inline static Ref<Texture2D> GroupIcon = nullptr;

		// Textures
		inline static Ref<Texture2D> NoTexture = nullptr;
		inline static Ref<SubTexture2D> NoSubTexture = nullptr;

		// ImGui Colours
		static constexpr glm::vec4 HoveredColour = { 0.25f, 1.0f, 1.0f, 0.5f };
		static constexpr glm::vec4 HighlightedColour = { 1.0f, 0.5f, 0.0f, 0.6f };
		static constexpr glm::vec4 ClearColour = { 0.0f, 0.0f, 0.0f, 0.0f };

		static void Init()
		{
			PlayIcon = LoadTexture("viewport/playbutton.png");
			StopIcon = LoadTexture("viewport/stopbutton.png");
			SimulateIcon = LoadTexture("viewport/simbutton.png");

			FileIcon = LoadTexture("content-browser/fileIcon.png");
			DirectoryIcon = LoadTexture("content-browser/directoryIcon.png");
			GroupIcon = LoadTexture("content-browser/groupIcon.png");

			NoTexture = LoadTexture("checkerboard.png");

			static constexpr glm::vec2 COORDS[4] = { { 0.f, 0.f }, { 1.f, 0.f }, { 1.f, 1.f }, { 0.f, 1.f } };
			Ref<Texture2DSheet> sheet = Ref<Texture2DSheet>::Create("NoSheetName", NoTexture, glm::vec2{ 1.f });
			NoSubTexture = Ref<SubTexture2D>::Create(NoTexture, COORDS);
		}

		static void Shutdown()
		{
			PlayIcon.reset();
			StopIcon.reset();
			SimulateIcon.reset();

			FileIcon.reset();
			DirectoryIcon.reset();
			GroupIcon.reset();

			NoTexture.reset();
			NoSubTexture.reset();
		}

	private:
		static Ref<Texture2D> LoadTexture(const std::filesystem::path& relativePath)
		{
			std::filesystem::path path = std::filesystem::path("resources") / "editor" / relativePath;

			if (!std::filesystem::exists(path))
			{
				LAB_CORE_CRITICAL("Failed to load icon! The file doesn't exist.", path.string());
				LAB_CORE_ASSERT(false);
			}

			return Ref<Texture2D>::Create(path.string());
		}
	};

}