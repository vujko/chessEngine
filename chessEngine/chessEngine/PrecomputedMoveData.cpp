#include "PrecomputedMoveData.h"

const int* PrecomputedMoveData::directionOffsets = new int[8]{ 8, -8, -1, 1, 7, -7, 9, -9 };

 const std::array<std::array<std::byte, 2>, 2> PrecomputedMoveData::pawnAttackDirections = std::array<std::array<std::byte, 2>, 2>{
	 std::array{
		 std::byte{4}, std::byte{6}
	 }, 
	 std::array{
		 std::byte{7},std::byte{5}
	 } 
 };
