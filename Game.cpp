#include <iostream>
#include <windows.h>
#include <ctime>
#include <conio.h>

using namespace std;

// Screen size
const int width = 40;
const int height = 25;

// Player
int playerX, playerY;

// Enemies
int enemy1X, enemy1Y;
int enemy2X, enemy2Y;
int enemy3X, enemy3Y;
int enemyDir1 = 1;
int enemyDir3 = -1;

// Bullets
int bulletX[10], bulletY[10];
bool bulletActive[10];

// Game state
int score, health;
int highScore = 0;
int level;
int enemyDelay, enemyCounter;
bool paused;

// ================= COLOR UTILITIES =================
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void gotoxy(int x, int y) {
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void hideCursor() {
    CONSOLE_CURSOR_INFO ci;
    ci.dwSize = 1;
    ci.bVisible = false;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
}

// ================= MENU =================
void displayMenu() {
    system("cls");
    
    // Big ASCII Title
    setColor(14); // Yellow
    gotoxy(4, 2);
    cout << " _____ _____ _____ _____ _____ "<<endl;
    gotoxy(4, 3);
    cout << "|   __|  _  |  _  |     |   __|"<<endl;
    gotoxy(4, 4);
    cout << "|__   |   __|     |   --|   __|"<<endl;
    gotoxy(4, 5);
    cout <<"|_____|__|  |__|__|_____|_____|"<<endl;
    
    setColor(11); // Cyan
    gotoxy(6, 7);
    cout << " _____ _____ _____ _____ _____ "<<endl;
    gotoxy(6, 8);
    cout << "|   __|  |  |     |     |_   _|"<<endl;
    gotoxy(6, 9);
    cout << "|__   |     |  |  |  |  | | |"<<endl;
    gotoxy(6, 10);
    cout <<"|_____|__|__|_____|_____| |_|"<<endl;
    
    // Menu border
    setColor(11); // Cyan
    gotoxy(8, 12);
    cout << "============================"<<endl;
    gotoxy(8, 13);
    setColor(14); // Yellow
    cout << "        MAIN MENU"<<endl;
    gotoxy(8, 14);
    setColor(11);
    cout << "============================"<<endl;
    
    // Display High Score
    setColor(13); // Magenta
    gotoxy(12, 16);
    cout << "HIGH SCORE: " << highScore <<endl;
    
    setColor(10); // Green
    gotoxy(15, 18);
    cout << "1. Play Game"<<endl;
    
    gotoxy(15, 19);
    cout << "2. Instructions"<<endl;
    
    gotoxy(15, 20);
    cout << "3. Exit"<<endl;
    
    setColor(7); // White
    gotoxy(12, 23);
    cout << "Select option (1-3): "<<endl;
}

void displayInstructions() {
    system("cls");
    setColor(14); // Yellow
    gotoxy(12, 2);
    cout << "HOW TO PLAY";
    
    setColor(11); // Cyan
    gotoxy(5, 5);
    cout << "CONTROLS:";
    
    setColor(7); // White
    gotoxy(5, 7);
    cout << "Arrow Keys  : Move your spaceship";
    gotoxy(5, 8);
    cout << "SPACE       : Shoot bullets";
    gotoxy(5, 9);
    cout << "ESC         : Pause/Resume game";
    
    setColor(11);
    gotoxy(5, 12);
    cout << "OBJECTIVE:";
    
    setColor(7);
    gotoxy(5, 14);
    cout << "Destroy enemies and earn points!";
    gotoxy(5, 15);
    cout << "Avoid collisions with enemies";
    gotoxy(5, 16);
    cout << "Don't let enemies pass the bottom";
    
    setColor(11);
    gotoxy(5, 18);
    cout << "SCORING:";
    
    setColor(7);
    gotoxy(5, 20);
    cout << "(<OO>) = 10 points";
    gotoxy(5, 21);
    cout << "(<##>) = 15 points";
    gotoxy(5, 22);
    cout << "(<@@>) = 20 points";
    
    setColor(10);
    gotoxy(8, 25);
    cout << "Enemy speed increases every 200 pts!";
    gotoxy(10, 26);
    cout << "Press any key to return...";
    
    setColor(7);
    while (kbhit()) getch(); // Clear buffer
    getch();
}

// ================= DRAW =================
void drawPlayer(bool erase) {
    gotoxy(playerX, playerY);
    if (!erase) setColor(10); // Green
    cout << (erase ? "       " : "  /\\  ");
    gotoxy(playerX, playerY + 1);
    cout << (erase ? "       " : " |__| ");
    setColor(7);
}

void drawEnemy1(bool erase) {
    gotoxy(enemy1X, enemy1Y);
    if (!erase) setColor(12); // Red
    cout << (erase ? "      " : "(|**|)");
    setColor(7);
}

void drawEnemy2(bool erase) {
    gotoxy(enemy2X, enemy2Y);
    if (!erase) setColor(13); // Magenta
    cout << (erase ? "      " : "([##])");
    setColor(7);
}

void drawEnemy3(bool erase) {
    gotoxy(enemy3X, enemy3Y);
    if (!erase) setColor(14); // Yellow
    cout << (erase ? "      " : "(<@@>)");
    setColor(7);
}

void drawBorder() {
    setColor(9); // Blue
    for (int i = 0; i <= width; i++) {
        gotoxy(i, 0); cout << "#";
        gotoxy(i, height); cout << "#";
    }
    for (int i = 0; i <= height; i++) {
        gotoxy(0, i); cout << "#";
        gotoxy(width, i); cout << "#";
    }
    setColor(7);
}

// ================= BULLETS =================
void moveBullets() {
    for (int i = 0; i < 10; i++) {
        if (bulletActive[i]) {
            gotoxy(bulletX[i], bulletY[i]);
            cout << " ";
            bulletY[i]--;

            if (bulletY[i] <= 1)
                bulletActive[i] = false;
            else {
                gotoxy(bulletX[i], bulletY[i]);
                setColor(11); // Cyan
                cout << "|";
                setColor(7);
            }
        }
    }
}

// ================= COLLISION =================
void checkCollision() {
    for (int i = 0; i < 10; i++) {
        if (!bulletActive[i]) continue;

        // Enemy 1
        if (bulletX[i] >= enemy1X && bulletX[i] <= enemy1X + 5 && bulletY[i] == enemy1Y) {
            score += 10; bulletActive[i] = false;
            drawEnemy1(true);
            enemy1Y = 1; enemy1X = rand() % (width - 7) + 1;
        }

        // Enemy 2
        if (bulletX[i] >= enemy2X && bulletX[i] <= enemy2X + 5 && bulletY[i] == enemy2Y) {
            score += 15; bulletActive[i] = false;
            drawEnemy2(true);
            enemy2Y = 1; enemy2X = rand() % (width - 7) + 1;
        }

        // Enemy 3
        if (bulletX[i] >= enemy3X && bulletX[i] <= enemy3X + 5 && bulletY[i] == enemy3Y) {
            score += 20; bulletActive[i] = false;
            drawEnemy3(true);
            enemy3Y = 1; enemy3X = rand() % (width - 7) + 1;
        }
    }
}

// ================= PLAYER COLLISION =================
void checkPlayerCollision() {
    if ((playerX <= enemy1X + 5 && playerX + 6 >= enemy1X && playerY <= enemy1Y + 1 && playerY + 1 >= enemy1Y) ||
        (playerX <= enemy2X + 5 && playerX + 6 >= enemy2X && playerY <= enemy2Y + 1 && playerY + 1 >= enemy2Y) ||
        (playerX <= enemy3X + 5 && playerX + 6 >= enemy3X && playerY <= enemy3Y + 1 && playerY + 1 >= enemy3Y)) {
        health--;
        if (playerY <= enemy1Y + 1 && playerY + 1 >= enemy1Y) { enemy1Y = 1; enemy1X = rand() % (width - 7) + 1; }
        if (playerY <= enemy2Y + 1 && playerY + 1 >= enemy2Y) { enemy2Y = 1; enemy2X = rand() % (width - 7) + 1; }
        if (playerY <= enemy3Y + 1 && playerY + 1 >= enemy3Y) { enemy3Y = 1; enemy3X = rand() % (width - 7) + 1; }
    }
}

// ================= LEVEL SYSTEM =================
void updateLevel() {
    // Calculate level based on score (every 200 points = 1 level)
    level = (score / 200) + 1;
    
    // Enemy delay decreases as level increases (gets faster)
    // Start at 10, minimum at 3
    enemyDelay = 10 - level;
    if (enemyDelay < 3) enemyDelay = 3;
    
    // No visual notification - just silently update
}

// ================= RESET GAME =================
void resetGame() {
    playerX = width / 2;
    playerY = height - 5;

    enemy1X = 2; enemy1Y = 2;
    enemy2X = width / 2; enemy2Y = 1;
    enemy3X = width - 10; enemy3Y = 1;

    enemyDir1 = 1;
    enemyDir3 = -1;

    score = 0;
    health = 3;
    level = 1;
    enemyDelay = 10;
    enemyCounter = 0;
    paused = false;

    for (int i = 0; i < 10; i++)
        bulletActive[i] = false;

    system("cls");
    drawBorder();
}

// ================= MAIN =================
int main() {
    srand(time(0));
    hideCursor();

    while (true) {
        // Clear keyboard buffer
        while (kbhit()) getch();
        
        // Display Menu
        displayMenu();
        char choice = getch();

        if (choice == '1') {
            // Start Game
            bool restartGame = true;
            
            while (restartGame) {
                resetGame();

                while (health > 0) {
                    if (GetAsyncKeyState(VK_ESCAPE) & 1) {
                        paused = !paused;
                        Sleep(200); // Debounce
                    }

                    if (paused) {
                        gotoxy(14, height / 2);
                        setColor(14);
                        cout << "== PAUSED ==";
                        setColor(7);
                        Sleep(100);
                        continue;
                    } else {
                        gotoxy(14, height / 2);
                        cout << "            ";
                    }

                    // Player movement - NO SLEEP for fast response
                    if (GetAsyncKeyState(VK_LEFT) & 0x8000 && playerX > 1) { drawPlayer(true); playerX--; }
                    if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && playerX < width - 7) { drawPlayer(true); playerX++; }
                    if (GetAsyncKeyState(VK_UP) & 0x8000 && playerY > 1) { drawPlayer(true); playerY--; }
                    if (GetAsyncKeyState(VK_DOWN) & 0x8000 && playerY < height - 2) { drawPlayer(true); playerY++; }

                    // Shooting - fast shooting
                    static int shootDelay = 0;
                    if (GetAsyncKeyState(VK_SPACE) & 0x8000 && shootDelay <= 0) {
                        for (int i = 0; i < 10; i++) {
                            if (!bulletActive[i]) {
                                bulletActive[i] = true;
                                bulletX[i] = playerX + 3;
                                bulletY[i] = playerY - 1;
                                shootDelay = 5; // Small delay between shots
                                break;
                            }
                        }
                    }
                    if (shootDelay > 0) shootDelay--;

                    // Enemy movement
                    enemyCounter++;
                    if (enemyCounter >= enemyDelay) {
                        drawEnemy1(true);
                        drawEnemy2(true);
                        drawEnemy3(true);

                        enemy1Y++; enemy1X += enemyDir1;
                        if (enemy1X <= 1 || enemy1X >= width - 7) enemyDir1 *= -1;

                        enemy2Y++;

                        enemy3Y++; enemy3X += enemyDir3;
                        if (enemy3X <= 1 || enemy3X >= width - 7) enemyDir3 *= -1;

                        if (enemy1Y >= height || enemy2Y >= height || enemy3Y >= height) health--;

                        if (enemy1Y >= height) { enemy1Y = 1; enemy1X = rand() % (width - 7) + 1; }
                        if (enemy2Y >= height) { enemy2Y = 1; enemy2X = rand() % (width - 7) + 1; }
                        if (enemy3Y >= height) { enemy3Y = 1; enemy3X = rand() % (width - 7) + 1; }

                        enemyCounter = 0;
                    }

                    moveBullets();
                    checkCollision();
                    checkPlayerCollision();
                    updateLevel();

                    drawPlayer(false);
                    drawEnemy1(false);
                    drawEnemy2(false);
                    drawEnemy3(false);

                    gotoxy(2, height + 1);
                    setColor(11);
                    cout << "Score: ";
                    setColor(14);
                    cout << score;
                    setColor(11);
                    cout << "  Health: ";
                    setColor(12);
                    cout << health;
                    setColor(11);
                    cout << "  Level: ";
                    setColor(10);
                    cout << level << "   ";
                    setColor(7);

                    Sleep(20); // Game loop speed
                }

                // Update high score
                if (score > highScore) {
                    highScore = score;
                }

                // GAME OVER SCREEN
                system("cls");
                setColor(12); // Red
                gotoxy(15, 8);
                cout << "GAME OVER!";
                
                setColor(14);
                gotoxy(13, 11);
                cout << "Final Score: " << score;
                gotoxy(13, 12);
                cout << "Final Level: " << level;
                
                setColor(13);
                gotoxy(13, 14);
                cout << "High Score: " << highScore;
                
                setColor(10);
                gotoxy(11, 17);
                cout << "Press R to Restart";
                gotoxy(11, 18);
                cout << "Press M for Menu";
                setColor(7);

                // Clear buffer before waiting
                while (kbhit()) getch();
                
                // Wait for user choice
                restartGame = false;
                while (true) {
                    if (GetAsyncKeyState('R') & 0x8000) {
                        restartGame = true;
                        Sleep(200);
                        break;
                    }
                    if (GetAsyncKeyState('M') & 0x8000) {
                        restartGame = false;
                        Sleep(200);
                        break;
                    }
                    Sleep(50);
                }
            }
        }
        else if (choice == '2') {
            // Instructions
            displayInstructions();
        }
        else if (choice == '3') {
            // Exit
            system("cls");
            setColor(11);
            gotoxy(12, 12);
            cout << "Thanks for playing!";
            setColor(7);
            Sleep(1000);
            return 0;
        }
    }
}