#include "Board.h"

#include "GameComponents.h"

namespace Labyrinth {

	constexpr char* BoardPath = "Chess/assets/scenes/Chess.laby";

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
			for (size_t j = 0; j < rows[i].getChildCount(); j++)
			{
				Entity* pieceInSquare = nullptr;
				auto& square = squares[i].addComponent<SquareComponent>();
				square.colour = ((i + j) % 2 == 0) ? Colour::White : Colour::Black;
				square.position = { i, j };
			}
		}
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

		//std::ifstream freshBoard(filepath);

		//if (!freshBoard) return;
		//std::string line;
		//std::string piece;
		//int rColour;
		//int rType;

		//int row = 0;
		//int column = 0;

		//while (std::getline(freshBoard, line))
		//{
		//	if (column >= 8) return;

		//	std::istringstream boardLine(line);

		//	while (std::getline(boardLine, piece, ','))
		//	{
		//		if (row >= 8) return;

		//		rColour = std::stoi(piece.substr(0, 1));
		//		rType = std::stoi(piece.substr(1, 1));

		//		if (!rColour || !rType) continue;

		//		//TODO: Create piece

		//		row++;
		//	}

		//	column++;
		//	row = 0;
		//}
	}
}