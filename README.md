# chessEngine

Chess engine written in C++. It uses the Monte Carlo Tree Search method (https://www.chessprogramming.org/Monte-Carlo_Tree_Search).

The engine respects the rules of chess regarding movement with consideration for castling, piece promotion, capturing pieces, forcing the player to move out of check and inability to move into check. En passant is also implemented.

In chess programming there are two types of move generation: pseudo-legal and legal. In Pseudo-legal move generation pieces obey their normal rules of movement, but they're not checked beforehand to see if they'll leave the king in check. In Legal move generation, as the name implies, only legal moves are generated, which means extra time must be spent to make sure the king isn't going to be left or placed in check after each move. Pins are the main difficulty, particularly when en passant is involved. This engine uses legal move generation.

More information about the engine and its performance can be found in my bachelor thesis here: [Vukasin Jokic Diplomski (AutoRecovered).pdf](https://github.com/vukasinjokic/chessEngine/files/7712403/Vukasin.Jokic.Diplomski.AutoRecovered.pdf)
