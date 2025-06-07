#include "Snake.h"
#include <Arduino.h>

Snake::Snake(DisplayManager& display, InputManager& input, StorageManager& storage)
    : displayManager(display), inputManager(input), storageManager(storage) {}

void Snake::play() {
    displayManager.u8g2.setPowerSave(0);
    bool gameRunning = true;
    unsigned long lastMoveTime = 0;
    const unsigned long initialMoveDelay = 150;
    unsigned long moveDelay = initialMoveDelay;
    const int maxSnakeLength = 100;
    int snakeX[maxSnakeLength];
    int snakeY[maxSnakeLength];
    int snakeLength = 3;
    int direction = 0; // 0: right, 1: down, 2: left, 3: up
    int foodX;
    int foodY;
    const int gameAreaX = 0;
    const int gameAreaY = 10;
    const int gameAreaWidth = 128;
    const int gameAreaHeight = 54;
    const int gridSize = 4;
    int score = 0;
    int highScore = storageManager.getHighScore();
    bool turnButtonPressed = false;
    bool drawFood = true;  // Flag to control food rendering
    bool needNewFood = false;  // Flag to place new food on next frame
    for (int i = 0; i < snakeLength; i++) {
        snakeX[i] = 64 - (i * gridSize);
        snakeY[i] = 32;
    }
placeFood:
    foodX = random(gameAreaX + gridSize, gameAreaWidth - gridSize);
    foodY = random(gameAreaY + gridSize, gameAreaHeight - gridSize);
    foodX = foodX - (foodX % gridSize);
    foodY = foodY - (foodY % gridSize);
    for (int i = 0; i < snakeLength; i++) {
        if (abs(foodX - snakeX[i]) < gridSize && abs(foodY - snakeY[i]) < gridSize) {
            goto placeFood;
        }
    }
    displayManager.u8g2.firstPage();
    do {
        displayManager.u8g2.setFont(u8g2_font_profont17_tr);
        int titleWidth = displayManager.u8g2.getStrWidth("SNAKE");
        displayManager.u8g2.drawStr((128 - titleWidth) / 2, 20, "SNAKE");
        displayManager.u8g2.setFont(u8g2_font_6x12_tr);  // Smaller font for instructions
        displayManager.u8g2.drawStr(15, 35, "Press UP to start");
        char highText[20];
        sprintf(highText, "High: %d", highScore);
        int highWidth = displayManager.u8g2.getStrWidth(highText);
        displayManager.u8g2.drawStr((128 - highWidth) / 2, 50, highText);
    } while (displayManager.u8g2.nextPage());
    while (true) {
        inputManager.update();
        if (inputManager.isUpPressed()) break;
        delay(10);
    }
    while (inputManager.isUpPressed()) {
        inputManager.update();
        delay(10);
    }
    displayManager.u8g2.firstPage();
    do {
        displayManager.u8g2.setFont(u8g2_font_6x12_tr);  // Smaller font to fit display
        displayManager.u8g2.drawStr(5, 15, "Controls:");
        displayManager.u8g2.drawStr(5, 28, "UP: Turn left");
        displayManager.u8g2.drawStr(5, 41, "DOWN: Turn right");
        displayManager.u8g2.drawStr(5, 54, "PWR: Exit");
    } while (displayManager.u8g2.nextPage());
    delay(2000);
    while (gameRunning) {
        inputManager.update();
        if (inputManager.isUpPressed() && !turnButtonPressed) {
            direction = (direction + 3) % 4;
            turnButtonPressed = true;
        } else if (inputManager.isDownPressed() && !turnButtonPressed) {
            direction = (direction + 1) % 4;
            turnButtonPressed = true;
        } else if (!inputManager.isUpPressed() && !inputManager.isDownPressed()) {
            turnButtonPressed = false;
        }
        if (inputManager.isPowerPressed()) {
            gameRunning = false;
        }
        if (millis() - lastMoveTime > moveDelay) {
            lastMoveTime = millis();
            for (int i = snakeLength - 1; i > 0; i--) {
                snakeX[i] = snakeX[i - 1];
                snakeY[i] = snakeY[i - 1];
            }
            switch (direction) {
                case 0: snakeX[0] += gridSize; break;
                case 1: snakeY[0] += gridSize; break;
                case 2: snakeX[0] -= gridSize; break;
                case 3: snakeY[0] -= gridSize; break;
            }
            if (snakeX[0] >= gameAreaWidth) snakeX[0] = gameAreaX;
            else if (snakeX[0] < gameAreaX) snakeX[0] = gameAreaWidth - gridSize;
            if (snakeY[0] >= gameAreaHeight) snakeY[0] = gameAreaY;
            else if (snakeY[0] < gameAreaY) snakeY[0] = gameAreaHeight - gridSize;
            for (int i = 1; i < snakeLength; i++) {
                if (abs(snakeX[0] - snakeX[i]) < gridSize/2 && abs(snakeY[0] - snakeY[i]) < gridSize/2) {
                    if (score > highScore) {
                        highScore = score;
                        storageManager.saveHighScore(highScore);
                        displayManager.u8g2.firstPage();
                        do {
                            displayManager.u8g2.setFont(u8g2_font_profont17_tr);
                            int newHighWidth = displayManager.u8g2.getStrWidth("NEW HIGH!");
                            displayManager.u8g2.drawStr((128 - newHighWidth) / 2, 25, "NEW HIGH!");
                            char scoreText[20];
                            sprintf(scoreText, "Score: %d", score);
                            int scoreWidth = displayManager.u8g2.getStrWidth(scoreText);
                            displayManager.u8g2.drawStr((128 - scoreWidth) / 2, 40, scoreText);
                        } while (displayManager.u8g2.nextPage());
                        delay(2000);
                    }
                    displayManager.u8g2.firstPage();
                    do {
                        displayManager.u8g2.setFont(u8g2_font_profont17_tr);
                        int titleWidth = displayManager.u8g2.getStrWidth("GAME OVER");
                        displayManager.u8g2.drawStr((128 - titleWidth) / 2, 20, "GAME OVER");
                        char scoreText[20];
                        sprintf(scoreText, "Score: %d", score);
                        int scoreWidth = displayManager.u8g2.getStrWidth(scoreText);
                        displayManager.u8g2.drawStr((128 - scoreWidth) / 2, 35, scoreText);
                        char highText[20];
                        sprintf(highText, "High: %d", highScore);
                        int highWidth = displayManager.u8g2.getStrWidth(highText);
                        displayManager.u8g2.drawStr((128 - highWidth) / 2, 50, highText);
                    } while (displayManager.u8g2.nextPage());
                    delay(2000);
                    gameRunning = false;
                    goto endGame;
                }
            }
            if (abs(snakeX[0] - foodX) < gridSize && abs(snakeY[0] - foodY) < gridSize) {
                if (snakeLength < maxSnakeLength) snakeLength++;
                score += 10;
                if (moveDelay > 50) moveDelay -= 5;
                // Mark that we need new food
                needNewFood = true;
                drawFood = false;
            }
        }
        
        // Handle food placement outside movement timing
        if (needNewFood) {
placeNewFood:
            foodX = random(gameAreaX + gridSize, gameAreaWidth - gridSize);
            foodY = random(gameAreaY + gridSize, gameAreaHeight - gridSize);
            foodX = foodX - (foodX % gridSize);
            foodY = foodY - (foodY % gridSize);
            for (int i = 0; i < snakeLength; i++) {
                if (abs(foodX - snakeX[i]) < gridSize && abs(foodY - snakeY[i]) < gridSize) {
                    goto placeNewFood;
                }
            }
            drawFood = true;
            needNewFood = false;
        }
        
        displayManager.u8g2.firstPage();
        do {
            displayManager.u8g2.drawFrame(gameAreaX, gameAreaY, gameAreaWidth, gameAreaHeight);
            for (int i = 0; i < snakeLength; i++) {
                if (i == 0) displayManager.u8g2.drawBox(snakeX[i], snakeY[i], gridSize, gridSize);
                else displayManager.u8g2.drawFrame(snakeX[i], snakeY[i], gridSize, gridSize);
            }
            // Only draw food if it hasn't just been eaten
            if (drawFood) displayManager.u8g2.drawBox(foodX, foodY, gridSize, gridSize);
            displayManager.u8g2.setFont(u8g2_font_6x10_tr);  // Smaller font for in-game score
            displayManager.u8g2.drawStr(2, 8, "Score:");
            char scoreStr[10];
            sprintf(scoreStr, "%d", score);
            displayManager.u8g2.drawStr(40, 8, scoreStr);  // Adjusted x position for smaller font
        } while (displayManager.u8g2.nextPage());
        delay(10);
    }
endGame:
    ;
} 