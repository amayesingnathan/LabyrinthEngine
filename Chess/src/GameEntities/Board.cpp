#include "Board.h"

#include "GameComponents.h"

namespace Labyrinth {

	constexpr char* BoardPath = "assets/scenes/Chess.laby";

	void Board::create(Ref<Scene> scene)
	{
		mContext = scene;

		ResetPieces();
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
			piece.position = { 1, pieceCount };
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& rook : whiteRooks)
		{
			auto& piece = rook.addComponent<PieceComponent>();
			piece.colour = Colour::White;
			piece.type = PieceType::Rook;
			piece.position = { 0, (pieceCount == 0) ? 0 : 7};
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& knight : whiteKnights)
		{
			auto& piece = knight.addComponent<PieceComponent>();
			piece.colour = Colour::White;
			piece.type = PieceType::Knight;
			piece.position = { 0, (pieceCount == 0) ? 1 : 6 };
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& bishop : whiteBishops)
		{
			auto& piece = bishop.addComponent<PieceComponent>();
			piece.colour = Colour::White;
			piece.type = PieceType::Bishop;
			piece.position = { 0, (pieceCount == 0) ? 2 : 5 };
			pieceCount++;
		}

		{
			auto& piece = whiteQueen.addComponent<PieceComponent>();
			piece.colour = Colour::White;
			piece.type = PieceType::Queen;
			piece.position = { 0, 3 };
		}

		{
			auto& piece = whiteKing.addComponent<PieceComponent>();
			piece.colour = Colour::White;
			piece.type = PieceType::King;
			piece.position = { 0, 4 };
		}

	}

	void Board::BuildBlackPieces()
	{
		auto& blackPieces = mBlack.getChildren();
		auto& blackPawns = blackPieces[0].getChildren();
		auto& blackRooks = blackPieces[1].getChildren();
		auto& blackKnights = blackPieces[2].getChildren();
		auto& blackBishops = blackPieces[3].getChildren();
		auto& blackQueen = blackPieces[4];
		auto& blackKing = blackPieces[5];

		int pieceCount = 0;
		for (auto& pawn : blackPawns)
		{
			auto& piece = pawn.addComponent<PieceComponent>();
			piece.colour = Colour::Black;
			piece.type = PieceType::Pawn;
			piece.position = { 6, pieceCount };
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& rook : blackRooks)
		{
			auto& piece = rook.addComponent<PieceComponent>();
			piece.colour = Colour::Black;
			piece.type = PieceType::Rook;
			piece.position = { 7, (pieceCount == 0) ? 0 : 7 };
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& knight : blackKnights)
		{
			auto& piece = knight.addComponent<PieceComponent>();
			piece.colour = Colour::Black;
			piece.type = PieceType::Knight;
			piece.position = { 7, (pieceCount == 0) ? 1 : 6 };
			pieceCount++;
		}

		pieceCount = 0;
		for (auto& bishop : blackBishops)
		{
			auto& piece = bishop.addComponent<PieceComponent>();
			piece.colour = Colour::Black;
			piece.type = PieceType::Bishop;
			piece.position = { 7, (pieceCount == 0) ? 2 : 5 };
			pieceCount++;
		}

		{
			auto& piece = blackQueen.addComponent<PieceComponent>();
			piece.colour = Colour::Black;
			piece.type = PieceType::Queen;
			piece.position = { 7, 3 };
		}

		{
			auto& piece = blackKing.addComponent<PieceComponent>();
			piece.colour = Colour::Black;
			piece.type = PieceType::King;
			piece.position = { 7, 4 };
		}
	}

	void Board::BuildRow(int row, std::vector<Entity>& squares)
	{
		for (int j = 0; j < squares.size(); j++)
		{
			Entity* pieceInSquare = nullptr;
			auto& square = squares[j];
			auto& squareComp = square.addComponent<SquareComponent>();
			if (square.hasComponent<SquareComponent>())
				int test = 0;
			squareComp.colour = ((row + j) % 2 == 0) ? Colour::White : Colour::Black;
			squareComp.position = { row, j };

			Entity side;
			if ((row == 0) || (row == 1))
				side = mWhite;
			if ((row == 6) || (row == 7))
				side = mBlack;

			if (!side) continue;

			if ((row == 1) || (row == 6))
			{
				auto& pieces = side.getChildren()[0].getChildren();
				squareComp.currentPiece = &pieces[j];
			}
			else if ((row == 0) || (row == 7))
			{
				if (j < 3)
				{
					Entity& piece = side.getChildren()[j + 1].getChildren()[0];
					squareComp.currentPiece = &piece;
				}
				else if ((4 < j) && (j < 8))
				{
					Entity& piece = side.getChildren()[8 - j].getChildren()[1];
					squareComp.currentPiece = &piece;
				}
				else
				{
					Entity& piece = side.getChildren()[j + 1];
					squareComp.currentPiece = &piece;
				}
			}
		}
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