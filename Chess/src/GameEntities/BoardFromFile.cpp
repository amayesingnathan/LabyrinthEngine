
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