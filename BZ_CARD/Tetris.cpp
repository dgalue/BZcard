#include <stdint.h>
#include "Tetris.h"
#include <Arduino.h>

Tetris::Tetris(DisplayManager& display, InputManager& input, StorageManager& storage)
    : displayManager(display), inputManager(input), storageManager(storage) {}

namespace {
// Piece definitions (x and y offsets for each rotation)
const int8_t PIECES_L[4][2][4] = {
    {{0,0,0,1},{0,1,2,2}},
    {{0,1,2,2},{1,1,1,0}},
    {{0,1,1,1},{0,0,1,2}},
    {{0,0,1,2},{1,0,0,0}}
};

const int8_t PIECES_S_L[2][2][4] = {
    {{0,0,1,1},{0,1,1,2}},
    {{0,1,1,2},{1,1,0,0}}
};

const int8_t PIECES_S_R[2][2][4] = {
    {{1,1,0,0},{0,1,1,2}},
    {{0,1,1,2},{0,0,1,1}}
};

const int8_t PIECES_SQ[1][2][4] = {
    {{0,1,0,1},{0,0,1,1}}
};

const int8_t PIECES_T[4][2][4] = {
    {{0,0,1,0},{0,1,1,2}},
    {{0,1,1,2},{1,0,1,1}},
    {{1,0,1,1},{0,1,1,2}},
    {{0,1,1,2},{0,0,1,0}}
};

const int8_t PIECES_I[2][2][4] = {
    {{0,1,2,3},{0,0,0,0}},
    {{0,0,0,0},{0,1,2,3}}
};
} // namespace

void Tetris::play() {
    displayManager.u8g2.setPowerSave(0);

    bool grid[10][18]{};
    int8_t activePiece[2][4];
    int currentType = 0;
    int nextType = random(6);
    int rotation = 0;
    int pieceX = 3;
    int pieceY = 0;
    unsigned long lastFall = millis();
    const unsigned long fallDelay = 400;
    bool running = true;

    auto copyPiece = [&](int type, int rot){
        const int8_t (*src)[4];
        switch(type) {
            case 0: src = PIECES_L[rot]; break;
            case 1: src = PIECES_S_L[rot]; break;
            case 2: src = PIECES_S_R[rot]; break;
            case 3: src = PIECES_SQ[0]; rot = 0; break;
            case 4: src = PIECES_T[rot]; break;
            default: src = PIECES_I[rot]; break;
        }
        for(int i=0;i<4;i++){ activePiece[0][i]=src[0][i]; activePiece[1][i]=src[1][i]; }
    };

    auto maxRot = [&](int type){
        if(type==0||type==4) return 4; // L or T
        if(type==1||type==2||type==5) return 2; // S pieces and I
        return 1; // square
    };

    auto collision = [&](int dx, int dy){
        for(int i=0;i<4;i++){
            int x = pieceX + activePiece[0][i] + dx;
            int y = pieceY + activePiece[1][i] + dy;
            if(x<0 || x>9 || y>17) return true;
            if(y>=0 && grid[x][y]) return true;
        }
        return false;
    };

    auto placePiece = [&](){
        for(int i=0;i<4;i++){
            int x = pieceX + activePiece[0][i];
            int y = pieceY + activePiece[1][i];
            if(x>=0 && x<10 && y>=0 && y<18)
                grid[x][y] = true;
        }
    };

    auto clearLines = [&](){
        for(int y=17;y>=0;y--){
            bool full=true;
            for(int x=0;x<10;x++) if(!grid[x][y]) { full=false; break; }
            if(full){
                for(int yy=y;yy>0;yy--)
                    for(int x=0;x<10;x++) grid[x][yy]=grid[x][yy-1];
                for(int x=0;x<10;x++) grid[x][0]=false;
                y++; // recheck same line
            }
        }
    };

    auto drawBoard = [&](){
        displayManager.u8g2.clearBuffer();
        // draw placed blocks
        for(int x=0;x<10;x++){
            for(int y=0;y<18;y++) if(grid[x][y])
                displayManager.u8g2.drawBox(3 + x*6, 19 + y*6, 5, 5);
        }
        // draw active piece
        for(int i=0;i<4;i++){
            int x = pieceX + activePiece[0][i];
            int y = pieceY + activePiece[1][i];
            displayManager.u8g2.drawBox(3 + x*6, 19 + y*6, 5, 5);
        }
        displayManager.u8g2.sendBuffer();
    };

    // generate first piece
    currentType = nextType;
    nextType = random(6);
    rotation = 0;
    pieceX = 3;
    pieceY = 0;
    copyPiece(currentType, rotation);

    while(running){
        inputManager.update();

        if(inputManager.isPowerPressed()) {
            running=false; break;
        }

        if(inputManager.isUpPressed() && !collision(-1,0)) {
            pieceX--; drawBoard(); delay(120);
        }

        if(inputManager.isDownPressed() && !collision(1,0)) {
            pieceX++; drawBoard(); delay(120);
        }

        if(inputManager.isEnterPressed()) {
            int newRot = (rotation + 1) % maxRot(currentType);
            int8_t temp[2][4];
            const int8_t (*src)[4];
            switch(currentType){
                case 0: src = PIECES_L[newRot]; break;
                case 1: src = PIECES_S_L[newRot]; break;
                case 2: src = PIECES_S_R[newRot]; break;
                case 3: src = PIECES_SQ[0]; newRot=0; break;
                case 4: src = PIECES_T[newRot]; break;
                default: src = PIECES_I[newRot]; break;
            }
            for(int i=0;i<4;i++){ temp[0][i]=src[0][i]; temp[1][i]=src[1][i]; }
            bool col=false;
            for(int i=0;i<4;i++){
                int x=pieceX+temp[0][i];
                int y=pieceY+temp[1][i];
                if(x<0||x>9||y>17|| (y>=0 && grid[x][y])) { col=true; break; }
            }
            if(!col){ rotation=newRot; for(int i=0;i<4;i++){ activePiece[0][i]=temp[0][i]; activePiece[1][i]=temp[1][i]; } drawBoard(); delay(150);}    
        }

        if(millis()-lastFall > fallDelay){
            lastFall = millis();
            if(!collision(0,1)) {
                pieceY++;
            } else {
                placePiece();
                clearLines();
                currentType = nextType;
                nextType = random(6);
                rotation = 0;
                pieceX = 3;
                pieceY = 0;
                copyPiece(currentType, rotation);
                if(collision(0,0)) { running=false; }
            }
            drawBoard();
        }
    }

    // show game over
    displayManager.u8g2.clearBuffer();
    displayManager.u8g2.setFont(u8g2_font_profont17_tr);
    int w = displayManager.u8g2.getStrWidth("GAME OVER");
    displayManager.u8g2.drawStr((128-w)/2,30,"GAME OVER");
    displayManager.u8g2.sendBuffer();

    delay(1000);

    while(inputManager.anyButtonPressed()){
        inputManager.update();
        delay(10);
    }
    while(!inputManager.anyButtonPressed()){
        inputManager.update();
        delay(10);
    }
    while(inputManager.anyButtonPressed()) {
        inputManager.update();
        delay(10);
    }

    displayManager.u8g2.firstPage();
    do {
        displayManager.u8g2.setFont(u8g2_font_profont17_tr);
        int w = displayManager.u8g2.getStrWidth("TETRIS");
        displayManager.u8g2.drawStr((128 - w) / 2, 30, "TETRIS");
        displayManager.u8g2.setFont(u8g2_font_6x10_tr);
        displayManager.u8g2.drawStr(22, 50, "Not implemented");
    } while (displayManager.u8g2.nextPage());

    // Wait for the game-launching button to be released
    while (inputManager.anyButtonPressed()) {
        inputManager.update();
        delay(10);
    }

    // Wait for a new button press to exit the placeholder screen
    while (!inputManager.anyButtonPressed()) {
        inputManager.update();
        delay(10);
    }

    // Wait for release before returning to the menu
    while (inputManager.anyButtonPressed()) {
        inputManager.update();
        delay(10);
    }
}
