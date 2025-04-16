#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unordered_map>
using namespace std;

class Game;
class Player;

class Player {
protected:
    char marker;
    string name;
public:
    Player(char marker) : marker(marker), name("Unnamed") {}
    virtual void makeMove(Game& game) = 0;
    char getMarker() const { return marker; }
    string getName() const { return name; }
    void setName(const string& playerName) { name = playerName; }
};

class HumanPlayer : public Player {
public:
    HumanPlayer(char marker) : Player(marker) {}
    void makeMove(Game& game) override;
};

class AIPlayer : public Player {
public:
    AIPlayer(char marker) : Player(marker) {}

    void makeMove(Game& game) override;
};

class Game {
private:
    char board[3][3];
    vector<Player*> players;
    int currentPlayerIndex;
    int movesCount;
    unordered_map<string, string> credentials;

    int checkWinner();
    void swapPlayer();
    void drawBoard();

public:
    Game(Player* player1, Player* player2);
    void start();
    void loginPlayers();
    bool placeMarker(int slot, char marker);
    void registerUser();
    string loginUser();
};

Game::Game(Player* player1, Player* player2) : currentPlayerIndex(0), movesCount(0) {
    players.push_back(player1);
    players.push_back(player2);
    int count = 1;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            board[i][j] = '0' + count++;
        }
    }
}

bool Game::placeMarker(int slot, char marker) {
    int row = (slot - 1) / 3;
    int col = (slot - 1) % 3;

    if (board[row][col] != 'X' && board[row][col] != 'O') {
        board[row][col] = marker;
        return true;
    }
    return false;
}

int Game::checkWinner() {
    for (int i = 0; i < 3; ++i) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2]) return currentPlayerIndex;
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i]) return currentPlayerIndex;
    }
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) return currentPlayerIndex;
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0]) return currentPlayerIndex;
    return -1;
}

void Game::swapPlayer() {
    currentPlayerIndex = (currentPlayerIndex + 1) % 2;
}

void Game::drawBoard() {
    cout << "-------------\n";
    for (int i = 0; i < 3; ++i) {
        cout << "| ";
        for (int j = 0; j < 3; ++j) {
            cout << board[i][j] << " | ";
        }
        cout << "\n-------------\n";
    }
}

void Game::registerUser() {
    string username, password;
    cout << "Register\n";
    cout << "Enter a username: ";
    cin >> username;

    // Check if the username already exists
    while (credentials.find(username) != credentials.end()) {
        cout << "Username already exists! Please choose a different username: ";
        cin >> username;
    }

    cout << "Enter a password: ";
    cin >> password;

    // Store the username and password
    credentials[username] = password;
    cout << "Registration successful!\n";
}

string Game::loginUser() {
    string username, password;
    cout << "Login\n";
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    // Check if the username exists and the password matches
    if (credentials.find(username) != credentials.end() && credentials[username] == password) {
        cout << "Login successful!\n";
        return username;
    } else {
        cout << "Invalid username or password. Please try again.\n";
        return "";
    }
}

void Game::loginPlayers() {
    for (int i = 0; i < players.size(); ++i) {
        if (dynamic_cast<AIPlayer*>(players[i])) {
            // If the player is an AI, set the name and skip login
            players[i]->setName("AIPlayer");
            continue;
        }

        string username;
        cout << "Player " << (i + 1) << " login.\n";

        // Allow players to either register or login
        char choice;
        cout << "Do you want to (R)egister or (L)ogin? ";
        cin >> choice;

        if (choice == 'R' || choice == 'r') {
            registerUser();
        }

        // Login and set the username as the display name
        while ((username = loginUser()).empty()) {
            // Keep prompting until the login is successful
        }

        players[i]->setName(username); // Set the username as the player's display name
    }
}

void Game::start() {
    loginPlayers();

    drawBoard();
    int playerWon = -1;

    for (movesCount = 0; movesCount < 9; ++movesCount) {
        cout << "It's " << players[currentPlayerIndex]->getName() << "'s turn. Enter your slot: ";
        players[currentPlayerIndex]->makeMove(*this);

        drawBoard();

        playerWon = checkWinner();
        if (playerWon != -1) {
            cout << "Player " << players[playerWon]->getName() << " wins!\n";
            return;
        }
        swapPlayer();
    }

    cout << "It's a tie!\n";
}

void HumanPlayer::makeMove(Game& game) {
    int slot;
    cin >> slot;
    while (slot < 1 || slot > 9 || !game.placeMarker(slot, marker)) {
        cout << "Invalid slot! Try again: ";
        cin >> slot;
    }
}

void AIPlayer::makeMove(Game& game) {
    int slot;
    do {
        slot = rand() % 9 + 1;
    } while (!game.placeMarker(slot, marker));
    cout << "AI (" << marker << ") chose slot " << slot << "\n";
}

int main() {
    srand(time(0)); // Initialize the random seed
    char choice;
    cout << "Choose player 2 (H for human, A for AI): ";
    cin >> choice;

    HumanPlayer player1('X');
    Player* player2;

    if (choice == 'A' || choice == 'a') {
        player2 = new AIPlayer('O');
    } else {
        player2 = new HumanPlayer('O');
    }

    Game game(&player1, player2);
    game.start();

    delete player2; // Clean up memory if AI player was used
    return 0;
}
