#include "Board.h"

#include "../ChessEngine.h"

#include <imgui/imgui.h>

namespace Labyrinth {

	constexpr char* BoardPath = "assets/scenes/Chess.laby";

	Board::Board(Ref<Scene> scene, const glm::vec2& viewportSize)
		: mContext(scene), mViewportSize(viewportSize)
	{
		FramebufferSpec fbSpec;
		fbSpec.width = Cast<uint32_t>(viewportSize.x);
		fbSpec.height = Cast<uint32_t>(viewportSize.y);
		fbSpec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		fbSpec.samples = 1;

		mBoardFramebuffer = Framebuffer::Create(fbSpec);

		ResetPieces();
	}

	void Board::onUpdate()
	{
		mViewportFocused = Input::IsWindowFocused();
		mViewportHovered = Input::IsWindowHovered();

		auto mousePos = Input::GetMousePosition();
		mousePos.y = mViewportSize.y - mousePos.y;
		int mouseX = (int)mousePos.x;
		int mouseY = (int)mousePos.y;

		DrawBoardFramebuffer();
		if (mousePos.x >= 0.0f && mousePos.y >= 0.0f && mousePos.x < mViewportSize.x && mousePos.y < mViewportSize.y)
		{
			// Check for hovered board square
			mBoardFramebuffer->bind();
			int boardPixelData = mBoardFramebuffer->readPixel(1, mouseX, mouseY);

			mHoveredSquare = (boardPixelData == -1) ? Entity() : Entity((entt::entity)boardPixelData, mContext);		
		}
	}

	void Board::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseMovedEvent>(LAB_BIND_EVENT_FUNC(Board::OnMouseMoved));
		dispatcher.dispatch<MouseButtonPressedEvent>(LAB_BIND_EVENT_FUNC(Board::OnMouseButtonPressed));
		dispatcher.dispatch<MouseButtonReleasedEvent>(LAB_BIND_EVENT_FUNC(Board::OnMouseButtonReleased));
	}

	void Board::onImGuiRender()
	{
		ImGui::Text("Held Piece: %d", mSelectedPiece.getEntID());
		ImGui::Text("Hovered Square: %d", mHoveredSquare.getEntID());

		ImGui::NewLine();

		ImGui::Text(mWhiteChecked ? "White Checked: Yes" : "White Checked: No");
		ImGui::Text(mBlackChecked ? "Black Checked: Yes" : "Black Checked: No");

	}

	void Board::onViewportResize(const glm::vec2& newSize)
	{
		mViewportSize = newSize;
		mBoardFramebuffer->resize((uint32_t)newSize.x, (uint32_t)newSize.y);
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
						mWhitePiecesRoot = children[0];
						mBlackPiecesRoot = children[1];
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
		if (!mWhitePiecesRoot)
		{
			LAB_ERROR("Could not load white pieces from file"); 
			return false;
		}
		if (!mBlackPiecesRoot)
		{
			LAB_ERROR("Could not load black pieces from file"); 
			return false;
		}

		if (!mBoard.hasComponent<BoardComponent>())
			mBoardState = &mBoard.addComponent<BoardComponent>().boardState;
		else
			mBoardState = &mBoard.getComponent<BoardComponent>().boardState;

		return true;
	}

	void Board::BuildWhitePieces()
	{
		mWhitePieces.reserve(16);
		auto& whitePieces = mWhitePiecesRoot.getChildren();
		auto& whitePawns = whitePieces[0].getChildren();
		auto& whiteRooks = whitePieces[1].getChildren();
		auto& whiteKnights = whitePieces[2].getChildren();
		auto& whiteBishops = whitePieces[3].getChildren();
		auto& whiteQueen = whitePieces[4].getChildren();
		auto& whiteKing = whitePieces[5].getChildren();

		int pieceCount = 0;
		for (auto& pawn : whitePawns)
		{
			auto& piece = pawn.addComponent<PieceComponent>();
			piece.colour = Colour::White;
			piece.type = PieceType::Pawn;
			piece.position = { pieceCount, 1 };
			mWhitePieces.push_back(pawn);
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& rook : whiteRooks)
		{
			auto& piece = rook.addComponent<PieceComponent>();
			piece.colour = Colour::White;
			piece.type = PieceType::Rook;
			int row = (pieceCount == 0) ? 0 : 7;
			piece.position = { row , 0 };
			mWhitePieces.push_back(rook);
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& knight : whiteKnights)
		{
			auto& piece = knight.addComponent<PieceComponent>();
			piece.colour = Colour::White;
			piece.type = PieceType::Knight;
			int row = (pieceCount == 0) ? 1 : 6;
			piece.position = { row, 0 };
			mWhitePieces.push_back(knight);
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& bishop : whiteBishops)
		{
			auto& piece = bishop.addComponent<PieceComponent>();
			piece.colour = Colour::White;
			piece.type = PieceType::Bishop;
			int row = (pieceCount == 0) ? 2 : 5;
			piece.position = { row, 0 };
			mWhitePieces.push_back(bishop);
			pieceCount++;
		}

		for (auto& queen : whiteQueen)
		{
			auto& piece = queen.addComponent<PieceComponent>();
			piece.colour = Colour::White;
			piece.type = PieceType::Queen;
			piece.position = { 3, 0 };
			mWhitePieces.push_back(queen);
		}

		for (auto& king : whiteKing)
		{
			mWhiteKing = king;
			auto& piece = king.addComponent<PieceComponent>();
			piece.colour = Colour::White;
			piece.type = PieceType::King;
			piece.position = { 4, 0 };
			mWhitePieces.push_back(king);
		}

	}

	void Board::BuildBlackPieces()
	{
		mBlackPieces.reserve(16);
		auto& blackPieces = mBlackPiecesRoot.getChildren();
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
			mWhitePieces.push_back(pawn);
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& rook : blackRooks)
		{
			auto& piece = rook.addComponent<PieceComponent>();
			piece.colour = Colour::Black;
			piece.type = PieceType::Rook;
			int row = (pieceCount == 0) ? 0 : 7;
			piece.position = { row, 7 };
			mWhitePieces.push_back(rook);
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& knight : blackKnights)
		{
			auto& piece = knight.addComponent<PieceComponent>();
			piece.colour = Colour::Black;
			piece.type = PieceType::Knight;
			int row = (pieceCount == 0) ? 1 : 6;
			piece.position = { row, 0 };
			mWhitePieces.push_back(knight);
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& bishop : blackBishops)
		{
			auto& piece = bishop.addComponent<PieceComponent>();
			piece.colour = Colour::Black;
			piece.type = PieceType::Bishop;
			int row = (pieceCount == 0) ? 2 : 5;
			piece.position = { row, 0 };
			mWhitePieces.push_back(bishop);
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& queen : blackQueen)
		{
			auto& piece = queen.addComponent<PieceComponent>();
			piece.colour = Colour::Black;
			piece.type = PieceType::Queen;
			piece.position = { 3, 7 };
			mWhitePieces.push_back(queen);
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& king : blackKing)
		{
			mBlackKing = king;
			auto& piece = king.addComponent<PieceComponent>();
			piece.colour = Colour::Black;
			piece.type = PieceType::King;
			piece.position = { 4, 7 };
			mWhitePieces.push_back(king);
			pieceCount++;
		}
	}

	void Board::BuildRow(int row, std::vector<Entity>& squares)
	{
		for (int j = 0; j < squares.size(); j++)
		{
			auto& square = squares[j];
			(*mBoardState)(j, row) = square;

			auto& squareComp = square.addComponent<SquareComponent>();
			squareComp.colour = ((row + j) % 2 == 0) ? Colour::White : Colour::Black;
			squareComp.position = { j, row };

			Entity side;
			if ((row == 0) || (row == 1))
				side = mWhitePiecesRoot;
			if ((row == 6) || (row == 7))
				side = mBlackPiecesRoot;

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
					Entity& piece = side.getChildren()[j + 1].getChildren()[0];
					squareComp.currentPiece = piece;
				}
			}
		}
	}

	void Board::ResolveMove()
	{
		auto& moveAttackingPieces = (mCurrPlayer == Colour::White) ? mBlackPieces : mWhitePieces;
		bool& moveInCheck = (mCurrPlayer == Colour::White) ? mWhiteChecked : mBlackChecked;
		mNextMove = Move(*mBoardState, mSelectedPiece, mLastSquare, mHoveredSquare, moveInCheck, moveAttackingPieces);
		mNextMove.resolve(mCurrPlayer);

		std::vector<BoardPosition> checkCheck;
		Chess::GetValidMoves(*mBoardState, mSelectedPiece, checkCheck);

		if (!checkCheck.empty())
		{
			bool inCheck = false;
			for (const auto& move : checkCheck)
			{
				const Entity& pieceInSquare = (*mBoardState)(move.x, move.y).getComponent<SquareComponent>().currentPiece;
				const Entity& king = (mCurrPlayer == Colour::White) ? mWhiteKing : mBlackKing;
				if (pieceInSquare == king)
					inCheck = true;
			}

			if (mCurrPlayer == Colour::White)
				mWhiteChecked = inCheck;
			else
				mBlackChecked = inCheck;
		}

		for (const auto& move : mPieceMoves)
			(*mBoardState)(move.x, move.y).getComponent<SpriteRendererComponent>().colour = ((move.x + move.y) % 2 == 0) ? mWhiteColour : mBlackColour;
		mPieceMoves.clear();

		mSelectedPiece.getComponent<SpriteRendererComponent>().layer--;
		mSelectedPiece = {};
	}

	void Board::DrawBoardFramebuffer()
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
			if (!mViewportHovered || Input::IsKeyPressed(LAB_KEY_LALT)) break;
			
			if (!mHoveredSquare) break;
			
			Entity pieceInSquare = mHoveredSquare.getComponent<SquareComponent>().currentPiece;

			if (!pieceInSquare) break;
			if (mCurrPlayer != pieceInSquare.getComponent<PieceComponent>().colour) break;

			mSelectedPiece = pieceInSquare;
			mSelectedPiece.getComponent<SpriteRendererComponent>().layer++;
			mLastSquare = mHoveredSquare;

			mPieceMoves.clear();
			Chess::GetValidMoves(*mBoardState, mSelectedPiece.getComponent<PieceComponent>(), mPieceMoves, (mCurrPlayer == Colour::White) ? mWhiteChecked : mBlackChecked);

			for (const auto& move : mPieceMoves)
				(*mBoardState)(move.x, move.y).getComponent<SpriteRendererComponent>().colour = mValidMoveColour;

			break;
		}
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
				ResolveMove();
			break;
		}
		}

		return false;
	}

	template<>
	void Scene::onComponentAdded<BoardComponent>(Entity entity, BoardComponent& component)
	{
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