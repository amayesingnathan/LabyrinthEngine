#include "FluidLayer.h"

#include "FluidComponents.h"

#include "Labyrinth/Tools/PlatformUtils.h"

namespace Labyrinth {

	FluidLayer::FluidLayer()
		: Layer("FluidLayer"), mScene(Ref<Scene>::Create()), mViewportSize(Application::Get().getWindow().getSize())
	{
		mScene->onViewportResize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
	}

	void FluidLayer::onAttach()
	{
		LAB_PROFILE_FUNCTION();

		FramebufferSpec fbSpec;
		fbSpec.width = Cast<uint32_t>(mViewportSize.x);
		fbSpec.height = Cast<uint32_t>(mViewportSize.y);
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mFramebuffer = Framebuffer::Create(fbSpec);

		mViewportBounds[0] = { 0, 0 };
		mViewportBounds[1] = { fbSpec.width, fbSpec.height };

		FluidSpec spec;
		spec.diff = 0.0001f; spec.visc = 0.0001f; spec.size = 100;
		mFluid = Ref<Fluid>::Create(mScene, spec);

		for (uint i = 0; i < spec.size; i++) {
			for (uint j = 0; j < spec.size; j++) {
				Entity cellEnt = mScene->CreateEntity("");
				auto& trans = cellEnt.getComponent<TransformComponent>().translation;
				trans.x = Cast<float>(i);
				trans.y = Cast<float>(j);

				cellEnt.addComponent<CellComponent>(i, j);
				cellEnt.addComponent<SpriteRendererComponent>(glm::vec4{ 0.f, 0.f, 0.f, 1.f });
			}
		}

		Entity camera = mScene->CreateEntity("Camera");
		auto& trans = camera.getComponent<TransformComponent>().translation;
		trans.x = Cast<float>(spec.size) / 2.f;
		trans.y = Cast<float>(spec.size) / 2.f;
		trans.z = 1.f;

		auto& cc = camera.addComponent<CameraComponent>();
		cc.camera.setProjectionType(SceneCamera::ProjectionType::Orthographic);

		cc.camera.setOrthographicSize(100.f);
		cc.camera.setOrthographicNearClip(-1.f);
		cc.camera.setOrthographicFarClip(2.f);

		cc.primary = true;
		cc.fixedAspectRatio = false;
	}

	void FluidLayer::onDetach()
	{
		LAB_PROFILE_FUNCTION();
	}

	void FluidLayer::onUpdate(Timestep ts)
	{
		LAB_PROFILE_FUNCTION();

		if (Input::IsMouseButtonPressed(Mouse::ButtonLeft))
		{
			glm::vec2 mousePos = Input::GetMousePosition();
			glm::vec2 screenSize = Application::Get().getWindow().getSize();
			uint meshSize = mFluid->getSize();
			float padding = Cast<float>(screenSize.x - screenSize.y);
			if ((mousePos.x > (padding * 0.5f)) && (mousePos.x < (screenSize.y + padding * 0.5f)))
			{
				uint x = Cast<uint>(floorf(((mousePos.x - 0.5f * padding) / screenSize.y) * meshSize));
				uint y = Cast<uint>(floorf(((screenSize.y - mousePos.y) / screenSize.y) * meshSize));
				mFluid->addDensity(x, y, 10.f);

				glm::vec2 amount = { 20.f, 20.f };
				float tick = Stopwatch::GetTime() * 0.5f;
				amount.x *= cosf(tick);
				amount.y *= sinf(tick);
				mFluid->addVelocity(x, y, amount.x, amount.y);
			}
		}

		mPrevMousePos = Input::GetMousePosition();

		mFluid->onUpdate(ts);

		mViewportHovered = Input::IsWindowHovered();

		if (FramebufferSpec spec = mFramebuffer->getSpecification();
			mViewportSize.x > 0.0f && mViewportSize.y > 0.0f && // zero sized framebuffer is invalid
			(spec.width != mViewportSize.x || spec.height != mViewportSize.y))
		{
			mFramebuffer->resize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
			mScene->onViewportResize((uint32_t)mViewportSize.x, (uint32_t)mViewportSize.y);
		}

		Renderer2D::ResetStats();

		mFramebuffer->bind();
		RenderCommand::SetClearColor({ 0.125f, 0.0625f, 0.25f, 1.0f });
		RenderCommand::Clear();

		mScene->onUpdateRuntime(ts);

		Renderer2D::DrawFramebuffer(mFramebuffer);

	}

	void FluidLayer::onImGuiRender()
	{
		LAB_PROFILE_FUNCTION();
	}

	void FluidLayer::onEvent(Event& e)
	{
		LAB_PROFILE_FUNCTION();
	}

	template<>
	void Scene::onComponentAdded<CellComponent>(Entity entity, CellComponent& component)
	{
	}
	template<>
	void Scene::onComponentAdded<FluidComponent>(Entity entity, FluidComponent& component)
	{
	}
}