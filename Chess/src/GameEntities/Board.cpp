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

		//auto& rows = mBoard.getChildren();
		//for (size_t i = 0; i < mBoard.getChildCount(); i++)
		//{
		//	auto squares = rows[i].getChildren();
		//	BuildRow(i, squares);
		//}
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
			LAB_ERROR("Could not load board from file"); return false;
		if (!mWhite)
			LAB_ERROR("Could not load white pieces from file"); return false;
		if (!mBlack)
			LAB_ERROR("Could not load black pieces from file"); return false;

		return true;

	}

	void Board::BuildRow(int row, std::vector<Entity>& squares)
	{
		for (int j = 0; j < squares.size(); j++)
		{
			Entity* pieceInSquare = nullptr;
			auto& square = squares[j].addComponent<SquareComponent>();
			square.colour = ((row + j) % 2 == 0) ? Colour::White : Colour::Black;
			square.position = { row, j };

			Entity side;
			if ((row == 0) || (row == 1))
				side = mWhite;
			if ((row == 6) || (row == 7))
				side = mBlack;

			if (!side) return;

			if ((row == 1) || (row == 6))
			{
				auto& pieces = side.getChildren()[0].getChildren();
				square.currentPiece = &pieces[j];
			}
			else if ((row == 0) || (row == 7))
			{
				if (j < 3)
				{
					Entity& piece = side.getChildren()[j + 1].getChildren()[0];
					square.currentPiece = &piece;
				}
				else if ((4 < j) && (j < 8))
				{
					Entity& piece = side.getChildren()[8 - j].getChildren()[1];
					square.currentPiece = &piece;
				}
				else
				{
					Entity& piece = side.getChildren()[j + 1];
					square.currentPiece = &piece;
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