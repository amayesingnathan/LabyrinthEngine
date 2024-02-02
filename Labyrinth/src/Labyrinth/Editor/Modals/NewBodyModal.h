#pragma once

#include <Labyrinth/Editor/IEditorModal.h>

#include <Labyrinth/Scene/Scene.h>
#include <Labyrinth/Scene/Entity.h>
#include <Labyrinth/Scene/Components.h>

namespace Laby {

	class NewBodyModal : public IEditorModal
	{
	public: // Class Types
		enum class ShapeType { Box, Circle };

		struct NoCollider {};
		using Collider = std::variant<NoCollider, BoxColliderComponent, CircleColliderComponent>;

		struct BodyDef
		{
			TransformComponent trans;
			RigidBodyComponent body;

			bool hasShape = false;
			ShapeType shape = ShapeType::Box;
			Collider shapeDef = NoCollider();

			bool addRender = false;
		};

	public:
		NewBodyModal(const Ref<Scene>& scene);

		void OnRender() override;
		void OnComplete() override;

	private:
		void ShapeOptions();
		void ConvertShapeOptions();

	private:
		Ref<Scene> mContext;
		Entity mNewEntity;
		BodyDef mNewBodyDef;
	};

}
