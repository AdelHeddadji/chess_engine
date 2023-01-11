#include <iostream>
#include <array>
#include <SDL.h>
#include <array>
#include <SDL_image.h>
#include <string> 
#include <cmath>
#include <algorithm>
#include <vector>
#include <list>

using namespace std;

const int kWindowWidth = 800;
const int kWindowHeight = 800;
const int kGridSize = 8;

SDL_Color darkColor = {90, 128, 176, 255};  // black
SDL_Color lightColor = {204, 219, 237, 255};  // white
SDL_Color highlight = {54,181,158, 255};
SDL_Color select = {255,255,255,255};
SDL_Color outline = {0,0,0,255};

int pawn = 1;
int rook = 2;
int knight = 4;
int bishop = 8;
int queen = 16;
int king = 32;
int white = 64;
int black = 128;
bool turn = true; // White turn = true, Black turn = false
bool wQCastle = true;
bool wKCastle = true;
bool bQCastle = true;
bool bKCastle = true;
bool whitePromote = false;
int bKI = 0;
int bKJ = 4;
int wKI = 7;
int wKJ = 4;


int numPieces = 32;
SDL_Rect pieces[32];
SDL_Texture* textures[32];
SDL_Rect promotions[4];
int b_promote_vals[4] = {black | bishop, black | knight, black | rook, black | queen};
int w_promote_vals[4]= {white | bishop, white | knight, white | rook, white | queen};
SDL_Texture* textPromotions[4];


std::array<std::array<int, 8>, 8> board = {{
    {black | rook, black | knight, black | bishop, black | queen, black | king, black | bishop, black | knight, black | rook},
    {black | pawn, black | pawn, black | pawn, black | pawn, black | pawn, black | pawn, black | pawn, black | pawn},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {white | pawn, white | pawn, white | pawn, white | pawn, white | pawn, white | pawn, white | pawn, white | pawn},
    {white | rook, white | knight, white | bishop, white | queen, white | king, white | bishop, white | knight, white | rook},
}};

bool isWhite(int p) {
    return (p < 128 && p != 0);
}

bool isBlack(int p) {
    return (p >=128);
}

bool properTurn(int piece) {
    if (isWhite(piece)) {
        if (turn == true) {
            return true;
        }
    }
    if (isBlack(piece)) {
        if (turn == false) {
            return true;
        }
    }
    return false;
}

void drawSquare2(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;
  SDL_RenderFillRect(renderer, &rect);
}
SDL_Rect makePiece2(SDL_Renderer* renderer, int x, int y) {
    SDL_Rect currpiece;
    currpiece.x = y;
    currpiece.y = x;
    currpiece.w = 100;
    currpiece.h = 100;
    return currpiece;
}

void drawSelect(SDL_Renderer* renderer) {
    SDL_Surface* surface;
    int x = (kWindowWidth / 6);
    int y = (kWindowHeight / 6);
    int xf= kWindowWidth / 80;
    int yf = kWindowHeight / 80;
    drawSquare2(renderer, x - (xf/2), (2 * y) - (yf/2), (4 * x) + xf , (2 * y) + yf, outline);
    drawSquare2(renderer, x, 2 * y, 4 * x, 2 * y, select);
    const char* bPromote[4] = {"images/b_bishop.png", "images/b_knight.png", "images/b_rook.png", "images/b_queen.png"};
    const char* wPromote[4] = {"images/w_bishop.png", "images/w_knight.png", "images/w_rook.png", "images/w_queen.png"};
    for (int i = 0; i < 4; i++) {
        promotions[i] = makePiece2(renderer, (kWindowHeight/2) - 50, x + (i*x));
        if (whitePromote) {surface = IMG_Load(wPromote[i]);}
        if (!whitePromote) {surface = IMG_Load(bPromote[i]);}
        textPromotions[i] = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopy(renderer, textPromotions[i], NULL, &promotions[i]);
    }
}

void drawSquare(SDL_Renderer* renderer, int x, int y, SDL_Color color) {
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_Rect rect;
  rect.x = x * (kWindowWidth / kGridSize);
  rect.y = y * (kWindowHeight / kGridSize);
  rect.w = kWindowWidth / kGridSize;
  rect.h = kWindowHeight / kGridSize;
  SDL_RenderFillRect(renderer, &rect);
}

void drawBoard(SDL_Renderer* renderer) {
    for (int i = 0; i < kGridSize; i++) {
        for (int j = 0; j < kGridSize; j++) {
            if ((i + j) % 2 == 0) {
            drawSquare(renderer, i, j, lightColor);
            } else {
            drawSquare(renderer, i, j, darkColor);
            }
        }
    }
}

SDL_Surface* getSurface(int piece) {
    SDL_Surface* surface;
    if (piece == (black | pawn)) {surface = IMG_Load("images/b_pawn.png");}
    else if (piece == (black | rook)) {surface = IMG_Load("images/b_rook.png");}
    else if (piece == (black | knight)) {surface = IMG_Load("images/b_knight.png");}
    else if (piece == (black | bishop)) {surface = IMG_Load("images/b_bishop.png");}
    else if (piece == (black | king)) {surface = IMG_Load("images/b_king.png");}
    else if (piece == (black | queen)) {surface = IMG_Load("images/b_queen.png");}

    else if (piece == (white | pawn)) {surface = IMG_Load("images/w_pawn.png");}
    else if (piece == (white | rook)) {surface = IMG_Load("images/w_rook.png");}
    else if (piece == (white | knight)) {surface = IMG_Load("images/w_knight.png");}
    else if (piece == (white | bishop)) {surface = IMG_Load("images/w_bishop.png");}
    else if (piece == (white | king)) {surface = IMG_Load("images/w_king.png");}
    else if (piece == (white | queen)) {surface = IMG_Load("images/w_queen.png");}
    return surface;
}

void makeTextures(SDL_Renderer* renderer) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 8; j++) {
            textures[i*8 + j] = SDL_CreateTextureFromSurface(renderer, getSurface(board[i][j]));
        }
    }
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 8; j++) {
            textures[16 + (i*8 + j)] = SDL_CreateTextureFromSurface(renderer, getSurface(board[i+6][j]));
        }
    }
}

SDL_Rect makePiece(SDL_Renderer* renderer, int x, int y) {
    SDL_Rect currpiece;
    currpiece.x = y * (kWindowHeight / kGridSize);
    currpiece.y = x * (kWindowWidth / kGridSize);
    currpiece.w = 100;
    currpiece.h = 100;
    return currpiece;
}

void makePieces(SDL_Renderer* renderer) {
    for (int i = 0; i < 2; i ++){
        for (int j = 0; j < 8; j++) {
            pieces[i*8 + j] = makePiece(renderer, i, j);
        }
    }
    for (int i = 0; i < 2; i ++){
        for (int j = 0; j < 8; j++) {
            pieces[16 + (i*8 + j)] = makePiece(renderer, i+6, j);
        }
    }
}

void drawPieces(SDL_Renderer* renderer) {
    for (int i = 0; i < numPieces; i++) {
        SDL_RenderCopy(renderer, textures[i], NULL, &pieces[i]);
    }
}

void drawOnePiece(SDL_Rect p, SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_RenderCopy(renderer, texture, NULL, &p);
}

void swap(int i) {
    SDL_Rect temp;
    SDL_Texture* t;
    temp = pieces[i];
    t = textures[i];
    pieces[i] = pieces[numPieces - 1];
    textures[i] = textures[numPieces - 1];
    pieces[numPieces - 1] = temp;
    textures[numPieces - 1] = t;
}

void removePiece(int i) {
    swap(i);
    numPieces--;
}

int getPiece(int x, int y) {
    for (int i  = 0; i < numPieces; i++) {
        if ((pieces[i].x / 100) == x && (pieces[i].y / 100) == y) {
            return i;
        }
    }
    return -1;
}

// void fakeMove(int i0, int j0, int i1, int j1, std::array<std::array<int, 8>, 8> board) {
//     board[i1][j1] = board[i0][j0];
//     board[i0][j0] = 0;
// }

void pieceMove(int i0, int j0, int i1, int j1) {
    int piece = board[i0][j0];
    if (board[i1][j1] != 0 && (i0 != i1 || j0 != j1)) {
        int p = getPiece(j1, i1);
        removePiece(p);
    }
    if (piece == (black | king)) {
        bKI = i1;
        bKJ = j1;
    }
    if (piece == (white | king)) {
        wKI = i1;
        wKJ = j1;
    }
    board[i1][j1] = board[i0][j0];
    board[i0][j0] = 0;
}

void undoMove(int i0, int j0, int i1, int j1, int piece) {
    board[i0][j0] = board[i1][j1];
    board[i1][j1] = piece;
    numPieces++;
}

bool isLegalPawn(int i0, int j0, int i1, int j1, std::array<std::array<int, 8>, 8> b) {
    if (isWhite(board[i0][j0])) {
        //Standard move
        if (j0 == j1 && (i0 - i1) == 1 && board[i1][j1] == 0) {
            return true;
        }
        //Capture
        else if (abs(j0-j1) == 1 && (i0 - i1) == 1 && isBlack(board[i1][j1])) {
            return true;
        }
        //En Passant
        else if (j0 == j1 && (i0 - i1) == 2 && board[i1][j1] == 0 && (i0 == 6)) {
            return true;
        }
        else {
            return false;
        }
    }
    if (isBlack(board[i0][j0])) {
        //Standard move
        if (j0 == j1 && (i1 - i0) == 1 && board[i1][j1] == 0) {
            return true;
        }
        //Capture
        else if (abs(j0-j1) == 1 && (i1 - i0) == 1 && isWhite(board[i1][j1])) {
            return true;
        }
        // En Passant
        else if (j0 == j1 && (i1 - i0) == 2 && board[i1][j1] == 0 && (i0 == 1)) {
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}

bool canTake(int piece, int fin) {
    if (isWhite(piece)){
                if (fin == 0 || isBlack(fin)){
                    return true;
                }
            }
    else if (isBlack(piece)) {
        if (fin == 0 || isWhite(fin)){
            return true;
        }
    }
    return false;
}

bool iterateDiag(int x, int y, int i0, int j0, int i1, int j1, std::array<std::array<int, 8>, 8> board) {
    for (int z = 1; z < (abs(i0-i1)); z++) {
        if (board[i0 + (z * x)][j0 + (z * y)] != 0) {
            return false;
        }
    }
    return true;
}

bool checkDiag(int x0, int y0, int x1, int y1, std::array<std::array<int, 8>, 8> board) {
    // Check if the coordinates are the same
    if (x0 == x1 && y0 == y1) {
    return true;
    }

    // Check if the coordinates are not on the same diagonal
    if (abs(x0 - x1) != abs(y0 - y1)) {
    return false;
    }
    // Find the minimum and maximum x and y values
    int min_x = std::min(x0, x1);
    int max_x = std::max(x0, x1);
    int min_y = std::min(y0, y1);
    int max_y = std::max(y0, y1);
    // UPRIGHT/DOWNLEFT
    if ((x0 > x1 && y0 < y1) || (x1 > x0 && y1 < y0)) {
        for (int x = 1; x < abs(x1 - x0); x++) {
            if (board[max_x - x][min_y + x] != 0) {
                return false;
            }
        }
    }
    // UPLEFT/DOWNRIGHT
    if ((x0 < x1 && y0 < y1) || (x1 < x0 && y1 < y0)) {
        for (int x = 1; x < abs(x1 - x0); x++) {
            if (board[min_x + x][min_y + x] != 0){
                return false;
            }
        }
    }

    return true;
}

bool isLegalBishop(int i0, int j0, int i1, int j1, std::array<std::array<int, 8>, 8> board) {
    int piece = board[i0][j0];
    int fin = board[i1][j1];
    if (abs(i1-i0) == abs(j1-j0)) {
        if (checkDiag(i0, j0, i1, j1, board)) {
            if (canTake(piece, fin)) return true;
        }
    }
    return false;
}

bool checkLane(int x0, int y0, int x1, int y1, std::array<std::array<int, 8>, 8> board) {
    int min_x = std::min(x0, x1);
    int max_x = std::max(x0, x1);
    int min_y = std::min(y0, y1);
    int max_y = std::max(y0, y1);
    if (x0 == x1 && (y0 < y1 || y1 < y0)) {
        for (int x = 1; x < abs(y1 - y0); x++) {
            if (board[x0][min_y + x] != 0) {
                return false;
            }
        }
    }
    if (y0 == y1 && (x0 < x1 || x1 < x0)) {
        for (int x = 1; x < abs(x1 - x0); x++) {
            if (board[min_x + x][y0] != 0) {
                return false;
            }
        }
    }
    return true;
}

void updateCastle(int i0, int j0, std::array<std::array<int, 8>, 8> board) {
    int piece = board[i0][j0];
    if (piece == (white | rook) || piece == (black | rook)) {
        if (i0 == 7 && j0 == 0) {wQCastle = false;}
        if (i0 == 7 && j0 == 7) {wKCastle = false;}
        if (i0 == 0 && j0 == 0) {bQCastle = false;}
        if (i0 == 0 && j0 == 7) {bKCastle = false;}
    }
    if (piece == (white | king)) {
        wQCastle = false;
        wKCastle = false;
    }
    if ( piece == (black | king)) {
        bQCastle = false;
        bKCastle = false;
    }
}

bool isLegalRook(int i0, int j0, int i1, int j1, std::array<std::array<int, 8>, 8> board) {
    int piece = board[i0][j0];
    int fin = board[i1][j1];
    if (i0 == i1 || j0 == j1) {
        if (checkLane(i0, j0, i1, j1, board)) {
            if (canTake(piece, fin))  {
                updateCastle(i0, j0, board);
                return true;
            }
        }
    }
    return false;
}

bool isLegalKnight(int i0, int j0, int i1, int j1, std::array<std::array<int, 8>, 8> board) {
    int piece = board[i0][j0];
    int fin = board[i1][j1];
    int rowDiff = abs(j1 - j0);
    int colDiff = abs(i1 - i0);
    if ((rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2)) {
        if (canTake(piece, fin)) return true;
    }
    return false;
}

void moveRook(int i0, int j0, int i1, int j1, std::array<std::array<int, 8>, 8> board) {
    int r = getPiece(j0, i0);
    pieces[r].x = j1 * 100;
    pieces[r].y = i1 * 100;
}

bool canMakeCastle(int i0, int j0, int i1, int j1, std::array<std::array<int, 8>, 8> board) {
    int piece = board[i0][j0];
    int fin = board[i1][j1];
    if (piece == (white | king)) {
        if (j1 < j0 && wQCastle && properTurn(piece)) {
        // if (j1 < j0 && wQCastle) {
            pieceMove(i0, 0, i0, j1+1);
            moveRook(i0, 0, i0, j1+1, board);
            wQCastle = false;
            wKCastle = false;
            return true;
        }
        else if (j1 > j0 && wKCastle && properTurn(piece)) {
        // else if (j1 > j0 && wKCastle) {
            pieceMove(i0, 7, i0, j1-1);
            moveRook(i0, 7, i0, j1-1, board);
            wQCastle = false;
            wKCastle = false;
            return true;
        }
    }
    if (piece == (black | king)) {
        if (j1 < j0 && bQCastle && properTurn(piece)) {
            pieceMove(i0, 0, i0, j1+1);
            moveRook(i0, 0, i0, j1+1, board);
            bQCastle = false;
            bKCastle = false;
            return true;
        }
        else if (j1 > j0 && bKCastle && properTurn(piece)) {
        // else if (j1 > j0 && bKCastle) {x
            pieceMove(i0, 7, i0, j1-1);
            moveRook(i0, 7, i0, j1-1, board);
            bQCastle = false;
            bKCastle = false;
            return true;
        }
    }
    return false;
}

bool canCastle(int i0, int j0, int i1, int j1, std::array<std::array<int, 8>, 8> board) {
    int piece = board[i0][j0];
    int fin = board[i1][j1];
    int rowDiff = abs(j1 - j0);
    int colDiff = abs(i1 - i0);
    if (colDiff == 0 && checkLane(i0, j0, i1, j1, board) && rowDiff == 2 && canMakeCastle(i0, j0, i1, j1, board)) {
        return true;
    }
    return false;
}

bool isLegalKing(int i0, int j0, int i1, int j1, std::array<std::array<int, 8>, 8> board) {
    int piece = board[i0][j0];
    int fin = board[i1][j1];
    int rowDiff = abs(j1 - j0);
    int colDiff = abs(i1 - i0);
    if (rowDiff <= 1 && colDiff <= 1) {
        if (canTake(piece, fin)) {
            updateCastle(i0, j0, board);
            return true;
        }
    }
    if (canCastle(i0, j0, i1, j1, board)) {
        updateCastle(i0, j0, board);
        return true;
        }
    return false;
}

bool isLegal(int i0, int j0, int i1, int j1, std::array<std::array<int, 8>, 8> board) {
    int piece = board[i0][j0];
    bool ans;
    if (i1 < 0 || i1 > 7 || j1 < 0 || j1 > 7) {
        return false;
    }
    if ((piece == (black | pawn)) || piece == (white | pawn)) {
        ans = isLegalPawn(i0, j0, i1, j1, board);
    }
    if ((piece == (black | bishop)) || piece == (white | bishop)) {
        ans = isLegalBishop(i0, j0, i1, j1, board);
    }
    if ((piece == (black | rook)) || piece == (white | rook)) {
        ans = isLegalRook(i0, j0, i1, j1, board);
    }
    if ((piece == (black | knight)) || piece == (white | knight)) {
        ans = isLegalKnight(i0, j0, i1, j1, board);
    }
    if ((piece == (black | queen)) || piece == (white | queen)) {
        ans = isLegalRook(i0, j0, i1, j1, board) || isLegalBishop(i0, j0, i1, j1, board);
    }
    if ((piece == (black | king)) || piece == (white | king)) {
        ans = isLegalKing(i0, j0, i1, j1, board);
    }
    if (properTurn(piece) && ans) {
        turn = !turn;
        return true;
    }
    return false; 
}

vector<pair<int, int> > getKnightMoves(int x, int y, std::array<std::array<int, 8>, 8> board) {
    int dx[] = {-2, -2, -1, -1, 1, 1, 2, 2};
    int dy[] = {-1, 1, -2, 2, -2, 2, -1, 1};

    vector<pair<int, int> > moves;

    // Check all the possible knight moves
    for (int i = 0; i < 8; i++) {
        int newX = x + dx[i];
        int newY = y + dy[i];

        // If the new position is within the bounds of the board, add it to the list of moves
        if (isLegalKnight(y, x, newY, newX, board)) {
            moves.push_back(make_pair(newX, newY));
        }
    }
    return moves;
}

vector<pair<int, int> > getBishopMoves(int x, int y, std::array<std::array<int, 8>, 8> board) {
    vector<pair<int, int> > moves;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (isLegalBishop(y, x, i, j, board)) {
                moves.push_back(make_pair(j, i));
            }
        }
    }
    return moves;
}

vector<pair<int, int> > getRookMoves(int x, int y, std::array<std::array<int, 8>, 8> board) {
    vector<pair<int, int> > moves;
    // Check top-left diagonal
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (isLegalRook(y, x, i, j, board)) {
                moves.push_back(make_pair(j, i));
            }
        }
    }
    return moves;
}

vector<pair<int, int> > getQueenMoves(int x, int y, std::array<std::array<int, 8>, 8> board) {
    vector<pair<int, int> > rMoves = getRookMoves(x, y, board);
    vector<pair<int, int> > bMoves = getBishopMoves(x, y, board);
    rMoves.insert(rMoves.begin(), bMoves.begin(), bMoves.end());
    return rMoves;
}

vector<pair<int, int> > getPawnMoves(int x, int y, std::array<std::array<int, 8>, 8> board) {
    vector<pair<int, int> > moves;
    // Check top-left diagonal
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (isLegalPawn(y, x, i, j, board)) {
                moves.push_back(make_pair(j, i));
            }
        }
    }
    return moves;
}
// Doesn't get Castle Moves
vector<pair<int, int> > getKingMoves(int x, int y, std::array<std::array<int, 8>, 8> b) {

    int piece = b[y][x];
    int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    vector<pair<int, int> > moves;

    for (int i = 0; i < 8; i++) {
        int newX = x + dx[i];
        int newY = y + dy[i];

        if (b[newY][newX] == 0) {
            moves.push_back(make_pair(newX, newY));
        }
    }
    int left = x - 2;
    int right = x + 2;

    if (isWhite(piece)) {
        if (wQCastle && checkLane(y, x, 7, 0, board)) {
            moves.push_back(make_pair(left, 7));
        }
        if (wKCastle && checkLane(y, x, 7, 7, board)) {
            moves.push_back(make_pair(right, 7));
        }
    }

    else if(isBlack(piece)) {
        if (bQCastle && checkLane(y, x, 0, 0, board)) {
            moves.push_back(make_pair(left, 0));
        }
        if (bKCastle && checkLane(y, x, 0, 7, board)) {
            moves.push_back(make_pair(right, 0));
        }
    }
    return moves;
}

vector<pair<int, int> > getMoves(int x, int y, std::array<std::array<int, 8>, 8> b) {
    int piece = b[y][x];
    vector<pair<int, int> > moves;

    if ((piece == (black | bishop)) || piece == (white | bishop)) {
        moves = getBishopMoves(x, y, b);
    }
    if ((piece == (black | knight)) || piece == (white | knight)) {
        moves = getKnightMoves(x, y, b);
    }
    if ((piece == (black | rook)) || piece == (white | rook)) {
        moves = getRookMoves(x, y, b);
    }
    if ((piece == (black | queen)) || piece == (white | queen)) {
        moves = getQueenMoves(x, y, b);
    }
    if ((piece == (black | pawn)) || piece == (white | pawn)) {
        moves = getPawnMoves(x, y, b);
    }
    if ((piece == (black | king)) || piece == (white | king)) {
        moves = getKingMoves(x, y, b);
    }
    return moves;
}

void printAllMoves(vector<pair<int, int> > moves) {
    for (int i = 0; i < moves.size(); i++) {
        printf("(%d, %d)", moves[i].second, moves[i].first);
    }
}

vector<pair<int, int> > getAllMoves(bool color, std::array<std::array<int, 8>, 8> b) {
    vector<pair<int, int> > moves;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (color && isWhite(b[i][j])) {
                vector<pair<int, int> > others = getMoves(j, i, b);
                moves.insert(moves.begin(), others.begin(), others.end());
            }
            else if (!color && isBlack(b[i][j])) {
                vector<pair<int, int> > others = getMoves(j, i, b);
                moves.insert(moves.begin(), others.begin(), others.end());
            }
        }
    }
    return moves;
}

void printBoard(std::array<std::array<int, 8>, 8> b) {
    for (int i = 0; i < 8; i++) {
        printf("{");
        for (int j = 0; j < 8; j++) {
            printf("%d, ", b[i][j]);
        }
        printf("}\n");
    }
}

bool inCheck(bool turn, int y, int x, std::array<std::array<int, 8>, 8> b) {
    vector<pair<int, int> > moves;
    int piece = b[y][x];
    bool gotKing = false;
    int tempbi = bKI;
    int tempbj = bKJ;
    int tempwi = wKI;
    int tempwj = wKJ;

    if (piece == (black | king)) {
        bKI = y;
        bKJ = x;
        gotKing = true;
    }
    if (piece == (white | king)) {
        wKI = y;
        wKJ = x;
        gotKing = true;
    }

    if (!turn) {
        x = wKJ;
        y = wKI;
        moves = getAllMoves(false, b);
    }
    else if (turn) {
        x = bKJ;
        y = bKI;
        moves = getAllMoves(true, b);
    }
    for (int i = 0; i < moves.size(); i++) {
        if (moves[i].second == y && moves[i].first == x) {
            bKI = tempbi;
            bKJ = tempbj;
            wKI = tempwi;
            wKJ = tempwj;
            return true;
        }
    }

    bKI = tempbi;
    bKJ = tempbj;
    wKI = tempwi;
    wKJ = tempwj;
    return false;
}

bool inCheck2(bool turn, int piece, std::array<std::array<int, 8>, 8> b) {
    vector<pair<int, int> > moves;
    int x;
    int y;
    if (!turn) {
        x = wKJ;
        y = wKI;
        moves = getAllMoves(false, b);
    }
    else if (turn) {
        x = bKJ;
        y = bKI;
        moves = getAllMoves(true, b);
    }
    for (int i = 0; i < moves.size(); i++) {
        if (moves[i].second == y && moves[i].first == x) {
            return true;
        }
    }
    return false;
}

void highlightSquares(SDL_Renderer* renderer, vector<pair<int, int> > moves) {
    int length = moves.size();
    for (int i = 0; i < length; i++) {
        drawSquare(renderer, moves[i].first, moves[i].second, highlight);
    }
}

void promotePawn(SDL_Renderer* renderer ,int val, int y, int x, bool white) {
    int piece;
    if (white) {piece = w_promote_vals[val];}
    if (!white) {piece = b_promote_vals[val];}
    board[x][y] = piece;
    int i = getPiece(y, x);
    SDL_Surface *surface = getSurface(piece);
    textures[i] = SDL_CreateTextureFromSurface(renderer, surface);
}

std::array<std::array<int, 8>, 8> copy(std::array<std::array<int, 8>, 8> b) {
    std::array<std::array<int, 8>, 8> newb;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            newb[i][j] = b[i][j];
        }
    }
    return newb;
}

int isCheckMate(std::array<std::array<int, 8>, 8> board, bool turn) {
    vector<pair<int, int> > allmoves = getAllMoves(turn, board);
    vector<pair<int, int> > notChecks;
    for (int i = 0; i < allmoves.size(); i++) {
        if (!inCheck(turn, allmoves[i].second, allmoves[i].first, board)) {
            // moves.push_back(make_pair(right, 7));
            notChecks.push_back(make_pair(allmoves[i].second, allmoves[i].first));
        }
    }
    return (notChecks.size());
    // return (notChecks.size() == 0);
}

int main(int argc, char* argv[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    // Initialize SDL.
    SDL_Init(SDL_INIT_VIDEO);

    // Create the window and renderer.
    window = SDL_CreateWindow("Grid", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            kWindowWidth, kWindowHeight, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    bool appIsRunning = true;

    bool mouse_down = false;
    makePieces(renderer);
    makeTextures(renderer);
    bool select_screen = false;
    int init_x;
    int init_y;
    bool dragging = false;
    bool highlight = false;
    vector<pair<int, int> > moves;
    int pawn_x;
    int pawn_y;
    bool w;
    //main game/app loop
    while (appIsRunning)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                appIsRunning = false;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                mouse_down = true;
                highlight = false;
                int x_val = event.button.x;
                int y_val = event.button.y;
                int val;
                if (select_screen) {
                    for (int i = 0; i < 4; i++) {
                        if (x_val <= promotions[i].x + 100 && x_val >= promotions[i].x && y_val <= promotions[i].y + 100 && y_val >= promotions[i].y) {
                            val = i;
                        }
                    }
                    promotePawn(renderer, val, pawn_x, pawn_y, whitePromote);
                    select_screen = false;
                }
            }
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {

                highlight = !highlight;
                int x_val = event.button.x;
                int y_val = event.button.y;
                for (int i = 0; i < numPieces; i++) {
                    if (x_val <= pieces[i].x + 100 && x_val >= pieces[i].x && y_val <= pieces[i].y + 100 && y_val >= pieces[i].y && dragging == false) {
                            init_x = x_val / 100;
                            init_y = y_val / 100;
                    }
                }
                moves = getMoves(init_x, init_y, board);
            }

            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
                mouse_down = false;
                int x_val;
                int y_val;
                if (dragging) {
                    int x_val = event.button.x / 100;
                    int y_val = event.button.y / 100;
                    pieces[numPieces-1].x = x_val * 100;
                    pieces[numPieces-1].y = y_val * 100;
                    std::array<std::array<int, 8>, 8> fakeBoard = copy(board);
                    fakeBoard[y_val][x_val] = fakeBoard[init_y][init_x];
                    fakeBoard[init_y][init_x] = 0;
                    if (!inCheck(!turn, y_val, x_val, fakeBoard) && isLegal(init_y, init_x, y_val, x_val, board)) {
                        pieceMove(init_y, init_x, y_val, x_val);
                        if ((board[y_val][x_val] == (black | pawn) && y_val == 7)) {
                            pawn_x = x_val;
                            pawn_y = y_val;
                            whitePromote = false;
                            select_screen = true;
                        }
                        if (board[y_val][x_val] == (white | pawn) && y_val == 0) {
                                pawn_x = x_val;
                                pawn_y = y_val;
                                whitePromote = true;
                                select_screen = true;
                        }
                        printf("Is Checkmate: %d \n", isCheckMate(board, !turn));
                    }
                    else {
                        pieces[numPieces-1].x = init_x * 100;
                        pieces[numPieces-1].y = init_y * 100;
                    }
                }
                dragging = false;
            }

            if (mouse_down) {
                int curr_i;
                if (dragging == false) {
                    int x_val = event.button.x;
                    int y_val = event.button.y;
                    for (int i = 0; i < numPieces; i++) {
                        if (x_val <= pieces[i].x + 100 && x_val >= pieces[i].x && y_val <= pieces[i].y + 100 && y_val >= pieces[i].y && dragging == false) {
                                init_x = x_val / 100;
                                init_y = y_val / 100;
                                curr_i = i;
                                swap(curr_i);
                                dragging = true;
                            }
                    }
                }

                if (dragging) {
                    pieces[numPieces - 1].x = event.button.x - 50;
                    pieces[numPieces - 1].y = event.button.y - 50;
                }

            }
        }
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        drawBoard(renderer);
        if (highlight) {
            highlightSquares(renderer, moves);
        }
        drawPieces(renderer);
        if (select_screen) {
            drawSelect(renderer);
        }

        SDL_RenderPresent(renderer);
    }

    IMG_Quit();
    SDL_Quit();
}