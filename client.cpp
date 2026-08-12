

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>

////////////////////////////////////////////////////////////////////////////
// common communication protocol
// /////////////////////////////////////////////////////////////////////////

enum PacketType {
    PKT_PLAYER_STATE = 0,
    PKT_PLAYER_HIT = 1
};



////// data
struct NetPlayerState {
    int id;
    float x;
    float y;
    int state;        // Idle, Move, Jump, Attack
    bool facingRight;
};


///////////////////////////////////////////////////////////////////////////////
// Enemy class (previously struct Enemy)
///////////////////////////////////////////////////////////////////////////////

class Enemy {
public:
    sf::Sprite sprite;
    bool facingRight = true;
    int frame = 0;
    float animationTimer = 0.f;

    Enemy() = default;


    void setTexture(const sf::Texture& tex) {
        sprite.setTexture(tex);
    }

    sf::FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }

    sf::Vector2f getPosition() const {
        return sprite.getPosition();
    }

    void move(float x, float y) {
        sprite.move(0.f, 50.f);
    }

    void setPosition(float x, float y) {
        sprite.setPosition(x, y);
    }

    void setScale(float sx, float sy) {
        sprite.setScale(sx, sy);
    }
};

///////////////////////////////////////////////////////////////////////////////
// LevelData - holds textures used by levels
///////////////////////////////////////////////////////////////////////////////
struct LevelData {
    sf::Texture platformTexture;
};

///////////////////////////////////////////////////////////////////////////////
// Level class - setups level
///////////////////////////////////////////////////////////////////////////////
class Level {
public:
    Level(LevelData* ld, sf::Texture* keyTex, sf::Texture* enemyTex)
        : data(ld), key_texture(keyTex), enemyTexture(enemyTex)
    {
    }


    void setupLevel3(std::vector<sf::RectangleShape>& platforms, sf::Sprite* player, std::vector<Enemy>& enemies) {
        platforms.clear();
        player->setPosition(0.f, 50.f);

        sf::RectangleShape ground(sf::Vector2f(1800.f, 100.f));
        ground.setTexture(&data->platformTexture);
        ground.setPosition(0.f, 740.f);
        platforms.push_back(ground);

        sf::RectangleShape mid1(sf::Vector2f(200.f, 30.f));
        mid1.setTexture(&data->platformTexture);
        mid1.setPosition(400.f, 450.f);
        platforms.push_back(mid1);
        sf::RectangleShape mid2(sf::Vector2f(200.f, 30.f));
        mid2.setTexture(&data->platformTexture);
        mid2.setPosition(850.f, 400.f);
        platforms.push_back(mid2);
        sf::RectangleShape mid3(sf::Vector2f(200.f, 30.f));
        mid3.setTexture(&data->platformTexture);
        mid3.setPosition(625.f, 530.f);
        platforms.push_back(mid3);
        sf::RectangleShape mid4(sf::Vector2f(200.f, 30.f));
        mid4.setTexture(&data->platformTexture);
        mid4.setPosition(350.f, 650.f);
        platforms.push_back(mid4);
        sf::RectangleShape mid5(sf::Vector2f(200.f, 30.f));
        mid5.setTexture(&data->platformTexture);
        mid5.setPosition(800.f, 650.f);
        platforms.push_back(mid5);

        std::vector<sf::Vector2f> enemyPositions = {
            {1400.f, 620.f}, {-300.f, 620.f}, {1800.f, 620.f}, {-100.f, 620.f}, {2000.f, 620.f}
        };

        for (auto& pos : enemyPositions) {
            Enemy enemy;
            enemy.setTexture(*enemyTexture);
            enemy.setPosition(pos.x, pos.y);
            enemy.setScale(2.f, 2.f);
            enemy.facingRight = true;
            enemies.push_back(enemy);
        }

    }
private:
    LevelData* data;
    sf::Texture* key_texture;
    sf::Texture* enemyTexture;
};

///////////////////////////////////////////////////////////////////////////////
// Menu class - handles main menu display
///////////////////////////////////////////////////////////////////////////////
class Menu {
public:
    Menu() = default;
};
// returns true to start game, false to exit

///////////////////////////////////////////////////////////////////////////////
// Player class - encapsulates player data and behavior
///////////////////////////////////////////////////////////////////////////////
class Player {
public:
    Player() {
        state = State::Idle;
    }

    enum class State { Idle, Move, Jump, Attack };

    bool loadTextures() {
        if (!idlerightTexture.loadFromFile("idleright.png")) return false;
        if (!idleleftTexture.loadFromFile("idleleft.png")) return false;
        if (!moverightTexture.loadFromFile("runright.png")) return false;
        if (!moveleftTexture.loadFromFile("runleft.png")) return false;
        if (!jumprightTexture.loadFromFile("jumpright.png")) return false;
        if (!jumpleftTexture.loadFromFile("jumpleft.png")) return false;
        if (!attackRightTexture.loadFromFile("attackright.png")) return false;
        if (!attackLeftTexture.loadFromFile("attackleft.png")) return false;
        if (!tiredTexture.loadFromFile("tired.png")) return false;
        if (!deathTexture.loadFromFile("Death.png")) return false;
        return true;
    }

    void initializeSprite() {
        sprite.setTexture(idlerightTexture);
        sprite.setPosition(0.f, 500.f);
        sprite.setScale(2.f, 2.5f);
    }

    void handleInput(sf::RenderWindow* window, int* level, float& velocityY, bool& canJump, bool hasKey, bool& facingRight, bool& inputlocked) {
        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window->close();
        }

        if (!inputlocked) {
            float moveSpeed = 5.f;
            sf::FloatRect bounds = sprite.getGlobalBounds();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                if (bounds.left > 0)
                    sprite.move(-moveSpeed, 0.f);
                facingRight = false;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                if (bounds.left + bounds.width < window->getSize().x + 200)
                    sprite.move(moveSpeed, 0.f);
                facingRight = true;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && canJump) {
                velocityY = -13.f;
                canJump = false;
            }
            if (sprite.getPosition().x > window->getSize().x && *level < 3 && hasKey) {
                (*level)++;
            }
        }
    }

    sf::FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }

    sf::Vector2f getPosition() const {
        return sprite.getPosition();
    }

    void setPosition(float x, float y) {
        sprite.setPosition(x, y);
    }

    void setTextureRect(const sf::IntRect& rect) {
        sprite.setTextureRect(rect);
    }

    void setTexture(const sf::Texture& t) {
        sprite.setTexture(t);
    }

    void move(float x, float y) {
        sprite.move(x, y);
    }

    void setTiredTexture() {
        sprite.setTexture(tiredTexture);
    }

    void setDeathTexture() {
        sprite.setTexture(deathTexture);
    }

    sf::Sprite sprite;
    int health = 20;
    bool isDead = false;
    sf::Clock hitCooldown;

    // animation and state
    State state;
    bool isAttacking = false;
    bool facingRight = true;
    int frame = 0;
    float animationTimer = 0.f;
    const float animationSpeed = 0.05f;
    sf::Vector2i frameSize{ 64, 64 };

    // textures
    sf::Texture idlerightTexture, idleleftTexture, moverightTexture, moveleftTexture, jumprightTexture, jumpleftTexture, attackRightTexture, attackLeftTexture, tiredTexture, deathTexture;
};

// MULTIPLAYER DATA QUEUE
class NetStateQueue {
private:
    static const int MAX = 128;
    NetPlayerState data[MAX];
    int front = 0;
    int rear = 0;
    int count = 0;

public:
    bool push(const NetPlayerState& s) {
        if (count >= MAX)
            return false;

        data[rear] = s;
        rear = (rear + 1) % MAX;
        count++;
        return true;
    }

    bool pop(NetPlayerState& s) {
        if (count <= 0)
            return false;

        s = data[front];
        front = (front + 1) % MAX;
        count--;
        return true;
    }

    bool empty() const {
        return count == 0;
    }
};


///////////////////////////////////////////////////////////////////////////////
// Game class - orchestrates the game (previously main + helper functions)
///////////////////////////////////////////////////////////////////////////////
class Game {
public:
    Game() {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }
    void spawnEnemies() {
        enemies.clear();

        int enemyCount = 3 + enemyWave * 2;

        for (int i = 0; i < enemyCount; i++) {
            Enemy e;
            e.setTexture(enemyMoveRight);
            e.setPosition(800.f + i * 120.f, 620.f);
            e.setScale(2.f, 2.f);
            e.facingRight = true;
            enemies.push_back(e);
        }

        enemyWave++;
        spawning = false;
    }


    // 5 MULTIPLAYER FUNCTIONS 
    void receiveStates() {
        sf::Packet packet;

        while (socket.receive(packet) == sf::Socket::Done) {
            int type;
            packet >> type;

            if (type == PKT_PLAYER_STATE) {
                NetPlayerState s;
                packet >> s.id >> s.x >> s.y >> s.state >> s.facingRight;
                if (s.id != clientID)
                    incomingStates.push(s);
            }

            else if (type == PKT_PLAYER_HIT) {
                int attackerID, targetID;
                packet >> attackerID >> targetID;

                // If YOU are the target → take damage
                if (targetID == clientID) {
                    player.health -= 2;
                    std::cout << "You were hit by " << attackerID
                        << " HP: " << player.health << "\n";
                }
            }
        }
    }


    void processIncomingStates() {
        NetPlayerState s;

        while (!incomingStates.empty()) {
            if (!incomingStates.pop(s))
                break;

            if (remotePlayers.find(s.id) == remotePlayers.end()) {
                Player* p = new Player();
                p->loadTextures();
                p->initializeSprite();
                remotePlayers[s.id] = p;
            }

            Player* rp = remotePlayers[s.id];


            rp->setPosition(s.x, s.y);
            rp->state = static_cast<Player::State>(s.state);
            rp->facingRight = s.facingRight;

            switch (rp->state) {
            case Player::State::Idle:
                rp->sprite.setTexture(rp->facingRight ? rp->idlerightTexture : rp->idleleftTexture);
                break;
            case Player::State::Move:
                rp->sprite.setTexture(rp->facingRight ? rp->moverightTexture : rp->moveleftTexture);
                break;
            case Player::State::Jump:
                rp->sprite.setTexture(rp->facingRight ? rp->jumprightTexture : rp->jumpleftTexture);
                break;
            case Player::State::Attack:
                rp->sprite.setTexture(rp->facingRight ? rp->attackRightTexture : rp->attackLeftTexture);
                break;
            }
            if (rp->health <= 0) {
                rp->setPosition(0.f, 300.f);
            }

        }
    }
    void sendPlayerState() {
        sf::Packet packet;

        packet << PKT_PLAYER_STATE
            << clientID
            << player.getPosition().x
            << player.getPosition().y
            << static_cast<int>(player.state)
            << player.facingRight;

        socket.send(packet);
    }

    void handlePlayerVsPlayerCombat() {
        // Only if local player is attacking
        if (player.state != Player::State::Attack)
            return;

        for (auto& pair : remotePlayers) {
            Player* rp = pair.second;

            // Bounding box collision
            if (player.getGlobalBounds().intersects(rp->getGlobalBounds())) {

                // Cooldown so damage is not applied every frame
                if (rp->hitCooldown.getElapsedTime().asSeconds() > 0.5f) {
                    rp->health -= 2;   // damage
                    rp->hitCooldown.restart();

                    std::cout << "Hit remote player " << pair.first
                        << " HP: " << rp->health << "\n";
                    if (rp->hitCooldown.getElapsedTime().asSeconds() > 0.5f) {
                        sendHitPacket(pair.first);
                        rp->hitCooldown.restart();
                    }

                    if (rp->health <= 0) {
                        rp->isDead = true;
                    }
                }
            }
        }
    }
    void sendHitPacket(int targetID) {
        sf::Packet packet;
        packet << PKT_PLAYER_HIT << clientID << targetID;
        socket.send(packet);
    }




    int run() {

        // --- Load key texture and enemy textures and other assets ---
        if (!key_texture.loadFromFile("key.png")) return 0;

        if (!enemyMoveRight.loadFromFile("enemymoveright.png")) return 0;
        if (!enemyMoveLeft.loadFromFile("enemymoveleft.png")) return 0;
        if (!enemyAttackRight.loadFromFile("enemyattackright.png")) return 0;
        if (!enemyAttackLeft.loadFromFile("enemyattackleft.png")) return 0;

        if (!levelData.platformTexture.loadFromFile("ground.jpg")) return 0;

        if (!player.loadTextures()) return 0;
        player.initializeSprite();

        // fonts
        if (!font.loadFromFile("gothic.TTF")) return 0;
        if (!healthfont.loadFromFile("ARIAL.TTF")) return 0;

        // UI texts
        quote.setFont(font);
        quote.setString("ONLY IN DEATH,\nDOES DUTY END");
        quote.setCharacterSize(50);
        quote.setScale(3.f, 3.f);
        quote.setFillColor(sf::Color::Red);
        quote.setPosition(500.f, 300.f);

        healthtext.setFont(healthfont);
        healthtext.setCharacterSize(20);
        healthtext.setFillColor(sf::Color::Red);

        tiredtext.setFont(healthfont);
        tiredtext.setCharacterSize(30);
        tiredtext.setFillColor(sf::Color::Red);
        tiredtext.setString("YOU ARE TIRED");

        // Initialize variables (same as original)
        level = 1;
        currentLevel = 1;
        hasKey = false;
        health = 20;

        isOnGround = false;
        canJump = true;
        inputlocked = false;
        death = false;
        deathcounter = 1;
        timer = 0.f;
        attacktimer = 0.f;
        velocityY = 0.f;

        // Initialize window and background
        initializeGameWindow(level);

        // Setup initial level (same as original)
        setupLevel3();

        //CONNECT TO SERVER
        socket.setBlocking(false);  // IMPORTANT for games

        if (socket.connect(serverIP, serverPort) != sf::Socket::Done) {
            std::cout << " Connected to server\n";
        }
        else {
            std::cout << " Connection to server failed\n";
        }

        // RECEIVE CLIENT ID
        sf::Packet idPacket;
        if (socket.receive(idPacket) == sf::Socket::Done) {
            idPacket >> clientID;
            std::cout << "Assigned client ID: " << clientID << "\n";
        }


        // main loop
        while (window.isOpen()) {
            // calling networking functions
            receiveStates();
            processIncomingStates();
            // player input & movement (this method polls events)
            player.handleInput(&window, &level, velocityY, canJump, hasKey, player.facingRight, inputlocked);

            // update health text
            healthtext.setString("HP:" + std::to_string(health));
            healthtext.setPosition(player.getPosition().x + 40.f, player.getPosition().y - 20.f);

            // attack handling: original logic uses A/D keys as attack triggers (A for left, D for right)
            if (!inputlocked) {
                if (!player.isAttacking) {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                        player.state = Player::State::Attack;
                        player.isAttacking = true;
                        player.facingRight = false;
                        hit = true;
                        player.animationTimer = 0.f;
                    }
                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                        player.state = Player::State::Attack;
                        hit = true;
                        player.isAttacking = true;
                        player.facingRight = true;
                        player.animationTimer = 0.f;
                    }
                    else
                        hit = false;
                }

                if (!player.isAttacking) {
                    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) && isOnGround) {
                        player.state = Player::State::Move;
                    }
                    else if (!isOnGround) {
                        player.state = Player::State::Jump;
                    }
                    else {
                        player.state = Player::State::Idle;
                    }
                }

                // apply textures based on state (preserving original mapping)
                switch (player.state) {
                case Player::State::Idle:
                    player.sprite.setTexture(player.facingRight ? player.idlerightTexture : player.idleleftTexture);
                    break;
                case Player::State::Move:
                    player.sprite.setTexture(player.facingRight ? player.moverightTexture : player.moveleftTexture);
                    break;
                case Player::State::Jump:
                    player.sprite.setTexture(player.facingRight ? player.jumprightTexture : player.jumpleftTexture);
                    break;
                case Player::State::Attack:
                    player.sprite.setTexture(player.facingRight ? player.attackRightTexture : player.attackLeftTexture);
                    break;
                }

                // animation frame update
                player.animationTimer += player.animationSpeed;
                if (player.animationTimer >= player.animationSpeed) {
                    player.frame++;
                    if (player.frame >= static_cast<int>(player.sprite.getTexture()->getSize().x / player.frameSize.x)) {
                        player.frame = 0;
                        if (player.state == Player::State::Attack) {
                            player.isAttacking = false;
                        }
                    }
                    player.animationTimer = 0.f;
                }
                player.setTextureRect(sf::IntRect(player.frame * player.frameSize.x, 0, player.frameSize.x, player.frameSize.y));

                // physics & gravity & movement
                velocityY += gravity;
                player.move(0.f, velocityY);

                // collision with platforms (identical to original)
                isOnGround = false;
                for (auto& platform : platforms) {
                    if (player.getGlobalBounds().intersects(platform.getGlobalBounds())) {
                        sf::FloatRect playerBounds = player.getGlobalBounds();
                        sf::FloatRect platformBounds = platform.getGlobalBounds();
                        if (playerBounds.top + playerBounds.height - velocityY <= platformBounds.top) {
                            player.setPosition(player.getPosition().x, platformBounds.top - playerBounds.height);
                            velocityY = 0.f;
                            isOnGround = true;
                            canJump = true;
                        }
                    }
                }
            }

            // key collection (levels 1 and 2)
            for (auto& key : keys) {
                if ((level == 1 || level == 2) && player.getGlobalBounds().intersects(key.getGlobalBounds())) {
                    hasKey = true;
                }
            }

            if (inputlocked) {
                tiredtext.setPosition(player.getPosition().x + 20.f, player.getPosition().y - 60.f);
                player.setTiredTexture();
            }

            // enemy behaviour & collisions - preserve exactly the same logic
            for (auto it = enemies.begin(); it != enemies.end(); ) {
                if (player.getGlobalBounds().intersects(it->sprite.getGlobalBounds()) && hit) {
                    it = enemies.erase(it);
                }
                else {
                    if (player.getPosition().x + 5 > it->sprite.getPosition().x) {
                        it->sprite.move(1.f, 0.f);
                        it->facingRight = true;
                        it->sprite.setTexture(enemyMoveRight);
                    }
                    else if (player.getPosition().x + 5 < it->sprite.getPosition().x) {
                        it->sprite.move(-1.f, 0.f);
                        it->facingRight = false;
                        it->sprite.setTexture(enemyMoveLeft);
                    }
                    else if (inputlocked && player.getPosition().x + 5 == it->sprite.getPosition().x) {
                        it->sprite.setTexture(enemyAttackLeft);
                        player.setDeathTexture();
                        death = true;
                    }

                    if (player.getGlobalBounds().intersects(it->sprite.getGlobalBounds())) {
                        attacktimer += 1.f;
                        if (attacktimer == 30.f) {
                            if (inputlocked)
                                health -= 10;
                            else
                                health--;
                            attacktimer = 0.f;

                            if (it->facingRight)
                                it->sprite.setTexture(enemyAttackRight);
                            else
                                it->sprite.setTexture(enemyAttackLeft);
                        }
                    }
                    ++it;
                }
            }
            // plyer combat function call
            handlePlayerVsPlayerCombat();


            // sending your player data
            sendPlayerState();


            // when all enemies are dead, start spawn timer
            if (enemies.empty() && !spawning) {
                spawning = true;
                spawnClock.restart();
            }

            // delay before spawning next wave
            if (spawning && spawnClock.getElapsedTime().asSeconds() > 2.f) {
                spawnEnemies();
            }




            // death & respawn logic (NO game over screen)
            if (health <= 0) {
                health = 20;
                // Respawn position
                player.setPosition(0.f, 300.f);
            }


            // level change
            if (level != currentLevel) {
                currentLevel = level;
                initializeGameWindow(level);
                player.setPosition(0.f, 700.f);
                hasKey = false;

                setupLevel3();
                deathcounter++;

            }


            timer++;
            drawGame();
        }

        return 0;
    }

private:
    // Window & background
    sf::RenderWindow window;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    //enemies spawning
    int enemyWave = 1;
    bool spawning = false;
    sf::Clock spawnClock;


    // Textures & assets
    sf::Texture key_texture;
    sf::Texture enemyMoveRight, enemyMoveLeft, enemyAttackRight, enemyAttackLeft;

    LevelData levelData;

    // core game objects
    Player player;
    Level levelObj{ &levelData, &key_texture, &enemyMoveRight };

    //MULTIPLAYER QUEUE CODE
    // networking
    sf::TcpSocket socket;
    sf::IpAddress serverIP = "10.7.244.48";
    unsigned short serverPort = 54000;
    int clientID = -1;

    // manual queue
    NetStateQueue incomingStates;

    // remote players
    std::map<int, Player*> remotePlayers;



    // world
    std::vector<sf::RectangleShape> platforms;
    std::vector<sf::RectangleShape> keys;
    std::vector<Enemy> enemies;

    // UI
    sf::Font font, healthfont;
    sf::Text quote, healthtext, tiredtext;

    // game state variables (kept as in original)
    int level = 1;
    int currentLevel = 1;
    bool hasKey = false;
    int health = 20;
    bool isOnGround = false;
    bool canJump = true;
    bool hit = false;
    bool inputlocked = false;
    bool death = false;
    bool enemyfacingright = true;
    int deathcounter = 1;

    float timer = 0.f;
    float attacktimer = 0.f;
    float attackholdtimer = 0.f;
    float velocityY = 0.f;
    const float gravity = 0.5f;

    // helper wrappers that call Level's functions
    void setupLevel3() { levelObj.setupLevel3(platforms, &player.sprite, enemies); }

    void initializeGameWindow(int lvl) {
        window.create(sf::VideoMode(1600, 1000), "THE KNIGHT GAME");
        window.setFramerateLimit(60);

        switch (lvl) {
        case 1:
        case 2:
            if (!backgroundTexture.loadFromFile("boss lvl.jpg")) {
                exit(1);
            }
            break;
        case 3:
            if (!backgroundTexture.loadFromFile("boss lvl.jpg")) {
                exit(1);
            }
            break;
        }
        backgroundSprite.setTexture(backgroundTexture);
        backgroundSprite.setScale(
            float(window.getSize().x) / backgroundTexture.getSize().x,
            float(window.getSize().y) / backgroundTexture.getSize().y);
    }


    // Draw the game frame (kept identical to original drawGame)
    void drawGame() {
        window.clear(sf::Color::White);
        window.draw(backgroundSprite);
        window.draw(player.sprite);

        // DRAW REMOTE PLAYERS
        for (auto& pair : remotePlayers) {
            window.draw(pair.second->sprite);
        }

        for (const auto& platform : platforms)
            window.draw(platform);
        if ((level == 1 || level == 2) && !hasKey) {
            for (const auto& key : keys)
                window.draw(key);
        }
        for (auto& enemy : enemies)
            window.draw(enemy.sprite);
        if (inputlocked)
            window.draw(tiredtext);
        window.draw(healthtext);
        window.display();
    }
};

///////////////////////////////////////////////////////////////////////////////
// main - simply instantiate Game and run
///////////////////////////////////////////////////////////////////////////////
int main() {
    Game game;
    return game.run();
}
