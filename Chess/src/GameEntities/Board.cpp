#include "Board.h"

#include "../ChessEngine.h"

namespace Labyrinth {

	constexpr char* BoardPath = "assets/scenes/Chess.laby";


	void Board::create(Ref<Scene> scene, const glm::vec2& viewportSize)
	{
		mContext = scene;
		mViewportSize = viewportSize;

		FramebufferSpec fbSpec;
		fbSpec.width = Cast<uint32_t>(viewportSize.x);
		fbSpec.height = Cast<uint32_t>(viewportSize.y);
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mBoardFramebuffer = Framebuffer::Create(fbSpec);
		mPiecesFramebuffer = Framebuffer::Create(fbSpec);

		ResetPieces();
	}

	void Board::onUpdate(Timestep ts)
	{
		mViewportFocused = Input::IsWindowFocused();
		mViewportHovered = Input::IsWindowHovered();

		auto mousePos = Input::GetMousePosition();
		mousePos.y = mViewportSize.y - mousePos.y;
		int mouseX = (int)mousePos.x;
		int mouseY = (int)mousePos.y;

		DrawFramebuffers();
		if (mousePos.x >= 0.0f && mousePos.y >= 0.0f && mousePos.x < mViewportSize.x && mousePos.y < mViewportSize.y)
		{
			// Check for hovered board square
			mBoardFramebuffer->bind();
			int boardPixelData = mBoardFramebuffer->readPixel(1, mouseX, mouseY);
			mHoveredSquare = (boardPixelData == -1) ? Entity() : Entity((entt::entity)boardPixelData, mContext);

			// Check for hovered piece
			mPiecesFramebuffer->bind();
			int piecePixelData = mPiecesFramebuffer->readPixel(1, mouseX, mouseY);
			mHoveredPiece = (piecePixelData == -1) ? Entity() : Entity((entt::entity)piecePixelData, mContext);
		}
	}

	void Board::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseMovedEvent>(LAB_BIND_EVENT_FUNC(Board::OnMouseMoved));
		dispatcher.dispatch<MouseButtonPressedEvent>(LAB_BIND_EVENT_FUNC(Board::OnMouseButtonPressed));
		dispatcher.dispatch<MouseButtonReleasedEvent>(LAB_BIND_EVENT_FUNC(Board::OnMouseButtonReleased));
	}

	void Board::onViewportResize(const glm::vec2& newSize)
	{
		mViewportSize = newSize;
		mBoardFramebuffer->resize((uint32_t)newSize.x, (uint32_t)newSize.y);
		mPiecesFramebuffer->resize((uint32_t)newSize.x, (uint32_t)newSize.y);
	}

	void Board::ResetPieces()
	{
		if (!LoadBoard(BoardPath)) return;

		auto& rows = mBoard.getChildren();
		for (size_t i = 0; i < mBoard.getChildCount(); i++)
		{
			auto squares = rows[i].getChildren();
			std::string tag = rows[i].getComponent<TagComponent>();
			BuildRow(i, squares);
		}

		BuildWhitePieces();
		BuildBlackPieces();
	}

	bool Board::LoadBoard(const std::string& filepath)
	{
		Serialiser::Deserialise<Scene>(filepath, mContext);

		mContext->view<RootComponent>().each([&](auto entity, auto& root)
			{
				Entity currEnt = { entity, mContext };
				std::string& tag = currEnt.getComponent<TagComponent>();

				if (tag == "Pieces")
				{
					const auto& children = currEnt.getChildren();
					if (children.size() == 2)
					{
						mWhite = children[0];
						mBlack = children[1];
					}
				}
				else if (tag == "Chessboard")
					mBoard = currEnt;
			});

		if (!mBoard)
		{
			LAB_ERROR("Could not load board from file"); 
			return false;
		}
		if (!mWhite)
		{
			LAB_ERROR("Could not load white pieces from file"); 
			return false;
		}
		if (!mBlack)
		{
			LAB_ERROR("Could not load black pieces from file"); 
			return false;
		}

		return true;

	}

	void Board::BuildWhitePieces()
	{
		auto& whitePieces = mWhite.getChildren();
		auto& whitePawns = whitePieces[0].getChildren();
		auto& whiteRooks = whitePieces[1].getChildren();
		auto& whiteKnights = whitePieces[2].getChildren();
		auto& whiteBishops = whitePieces[3].getChildren();
		auto& whiteQueen = whitePieces[4];
		auto& whiteKing = whitePieces[5];

		int pieceCount = 0;
		for (auto& pawn : whitePawns)
		{
			auto& piece = pawn.addComponent<PieceComponent>();
			piece.colour = Colour::White;
			piece.type = PieceType::Pawn;
			piece.position = { pieceCount, 1 };
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& rook : whiteRooks)
		{
			auto& piece = rook.addComponent<PieceComponent>();
			piece.colour = Colour::White;
			piece.type = PieceType::Rook;
			piece.position = { (pieceCount == 0) ? 0 : 7, 0};
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& knight : whiteKnights)
		{
			auto& piece = knight.addComponent<PieceComponent>();
			piece.colour = Colour::White;
			piece.type = PieceType::Knight;
			piece.position = { (pieceCount == 0) ? 1 : 6, 0 };
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& bishop : whiteBishops)
		{
			auto& piece = bishop.addComponent<PieceComponent>();
			piece.colour = Colour::White;
			piece.type = PieceType::Bishop;
			piece.position = { (pieceCount == 0) ? 2 : 5, 0 };
			pieceCount++;
		}

		{
			auto& piece = whiteQueen.addComponent<PieceComponent>();
			piece.colour = Colour::White;
			piece.type = PieceType::Queen;
			piece.position = { 3, 0 };
		}

		{
			auto& piece = whiteKing.addComponent<PieceComponent>();
			piece.colour = Colour::White;
			piece.type = PieceType::King;
			piece.position = { 4, 0 };
		}

	}

	void Board::BuildBlackPieces()
	{
		auto& blackPieces = mBlack.getChildren();
		auto& blackPawns = blackPieces[0].getChildren();
		auto& blackRooks = blackPieces[1].getChildren();
		auto& blackKnights = blackPieces[2].getChildren();
		auto& blackBishops = blackPieces[3].getChildren();
		auto& blackQueen = blackPieces[4].getChildren();
		auto& blackKing = blackPieces[5].getChildren();

		int pieceCount = 0;
		for (auto& pawn : blackPawns)
		{
			auto& piece = pawn.addComponent<PieceComponent>();
			piece.colour = Colour::Black;
			piece.type = PieceType::Pawn;
			piece.position = { pieceCount, 6 };
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& rook : blackRooks)
		{
			auto& piece = rook.addComponent<PieceComponent>();
			piece.colour = Colour::Black;
			piece.type = PieceType::Rook;
			piece.position = { (pieceCount == 0) ? 0 : 7, 7 };
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& knight : blackKnights)
		{
			auto& piece = knight.addComponent<PieceComponent>();
			piece.colour = Colour::Black;
			piece.type = PieceType::Knight;
			piece.position = { (pieceCount == 0) ? 1 : 6, 7 };
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& bishop : blackBishops)
		{
			auto& piece = bishop.addComponent<PieceComponent>();
			piece.colour = Colour::Black;
			piece.type = PieceType::Bishop;
			piece.position = { (pieceCount == 0) ? 2 : 5, 7 };
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& queen : blackQueen)
		{
			auto& piece = queen.addComponent<PieceComponent>();
			piece.colour = Colour::Black;
			piece.type = PieceType::Queen;
			piece.position = { 3, 7 };
		}

		pieceCount = 0;
		for (auto& king : blackKing)
		{
			auto& piece = king.addComponent<PieceComponent>();
			piece.colour = Colour::Black;
			piece.type = PieceType::King;
			piece.position = { 4, 7 };
		}
	}

	void Board::BuildRow(int row, std::vector<Entity>& squares)
	{
		for (int j = 0; j < squares.size(); j++)
		{
			auto& square = squares[j];
			auto& squareComp = square.addComponent<SquareComponent>();
			if (square.hasComponent<SquareComponent>())
				int test = 0;
			squareComp.colour = ((row + j) % 2 == 0) ? Colour::White : Colour::Black;
			squareComp.position = { j, row };

			Entity side;
			if ((row == 0) || (row == 1))
				side = mWhite;
			if ((row == 6) || (row == 7))
				side = mBlack;

			if (!side) continue;

			if ((row == 1) || (row == 6))
			{
				auto& pieces = side.getChildren()[0].getChildren();
				squareComp.currentPiece = pieces[j];
			}
			else if ((row == 0) || (row == 7))
			{
				if (j < 3)
				{
					Entity& piece = side.getChildren()[j + 1].getChildren()[0];
					squareComp.currentPiece = piece;
				}
				else if ((4 < j) && (j < 8))
				{
					Entity& piece = side.getChildren()[8 - j].getChildren()[1];
					squareComp.currentPiece = piece;
				}
				else
				{
					Entity& piece = side.getChildren()[j + 1];
					squareComp.currentPiece = piece;
				}
			}
		}
	}

	void Board::ResolveMove()
	{
		if (!mHoveredSquare) return;

		auto& piece = mSelectedPiece.getComponent<PieceComponent>();
		auto& square = mHoveredSquare.getComponent<SquareComponent>();

		auto& trans = mSelectedPiece.getComponent<TransformComponent>().translation;

		if (IsValidTarget(piece, square))
		{
			const auto& squareTrans = mHoveredSquare.getComponent<TransformComponent>().translation;
			trans = { squareTrans.x, squareTrans.y, trans.z };
			piece.position = square.position;

			auto& prevSquare = mLastPieceSquare.getComponent<SquareComponent>();
			prevSquare.currentPiece = {};
			square.currentPiece = mSelectedPiece;

			if (piece.unmoved) piece.unmoved = false;
		}
		else 
		{ 
			const auto& squareTrans = mLastPieceSquare.getComponent<TransformComponent>().translation;
			trans = { squareTrans.x, squareTrans.y, trans.z };
		}
	}

	bool Board::IsValidTarget(const PieceComponent& piece, SquareComponent& square)
	{
		std::vector<BoardPosition> validMoves;
		Chess::GetValidMoves(piece, validMoves);

		if (validMoves.empty()) 
			return false;

		if (std::find(validMoves.begin(), validMoves.end(), square.position) == validMoves.end())
			return false;

		if (square.currentPiece)
			mContext->DestroyEntity(square.currentPiece);
		return true;
	}

	void Board::DrawFramebuffers()
	{
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = mContext->view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto& [trans, cam] = view.get<TransformComponent, CameraComponent>(entity);

				if (cam.primary)
				{
					mainCamera = &cam.camera;
					cameraTransform = trans;
				}
			}
		}

		if (mainCamera)
		{
			mBoardFramebuffer->bind();
			Renderer2D::BeginState(*mainCamera, cameraTransform);

			RenderCommand::SetClearColor({ 0.125f, 0.0625f, 0.25f, 1.0f });
			RenderCommand::Clear();

			// Clear our entity ID attachment to -1
			mBoardFramebuffer->clearAttachment(1, -1);

			auto boardView = mContext->view<TransformComponent, SpriteRendererComponent, SquareComponent>();
			for (auto entity : boardView)
			{
				auto& [transform, sprite] = boardView.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawSprite(transform, sprite, (int)entity);
			}

			Renderer2D::EndState();

			mPiecesFramebuffer->bind();
			Renderer2D::BeginState(*mainCamera, cameraTransform);

			RenderCommand::SetClearColor({ 0.125f, 0.0625f, 0.25f, 1.0f });
			RenderCommand::Clear();

			// Clear our entity ID attachment to -1
			mPiecesFramebuffer->clearAttachment(1, -1);

			auto piecesView = mContext->view<TransformComponent, SpriteRendererComponent, PieceComponent>();
			for (auto entity : piecesView)
			{
				auto& [transform, sprite] = piecesView.get<TransformComponent, SpriteRendererComponent>(entity);

				Renderer2D::DrawSprite(transform, sprite, (int)entity);
			}

			Renderer2D::EndState();
		}
	}

	bool Board::OnMouseMoved(MouseMovedEvent& e)
	{
		if (Input::IsMouseButtonPressed(LAB_MOUSE_BUTTON_LEFT) && mSelectedPiece)
		{
			glm::vec2 delta = e.getPos() - mLastMousePos;
			auto& trans = mSelectedPiece.getComponent<TransformComponent>();
			trans.translation += (0.01f * glm::vec3(delta.x, -delta.y, 0));
		}

		mLastMousePos = e.getPos();
		return false;
	}

	bool Board::OnMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		switch (e.getMouseButton())
		{
		case LAB_MOUSE_BUTTON_LEFT:
		{
			if (mViewportHovered && !Input::IsKeyPressed(LAB_KEY_LALT))
				if (mHoveredPiece)
				{
					mSelectedPiece = mHoveredPiece;
					mLastPieceSquare = mHoveredSquare;
				}
		}
		break;
		}

		return false;
	}

	bool Board::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		switch (e.getMouseButton())
		{
		case LAB_MOUSE_BUTTON_LEFT:
		{
			if (mSelectedPiece)
			{
				ResolveMove();
				mSelectedPiece = {};
			}
		}
		}

		return false;
	}

	template<>
	void Scene::onComponentAdded<SquareComponent>(Entity entity, SquareComponent& component)
	{
	}

	template<>
	void Scene::onComponentAdded<PieceComponent>(Entity entity, PieceComponent& component)
	{
	}
}