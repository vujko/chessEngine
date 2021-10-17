#pragma once
class UCI
{
public:

	void parseGo(char* line, class ComputeOptions* info, class Board* pos);
	void parsePosition(char* lineIn, Board* pos);
	class Move parseMove(char* ptrChar, Board* pos);
	void UCILoop();
};

