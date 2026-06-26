#include <iostream>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <vector>
using namespace std;

    enum class PlatformType{
        OneWay,
        Solid
    };

    struct Platform{
        sf::RectangleShape shape;
        PlatformType type;

        bool isQuestionBlock = false;
        bool used = false;
    };
    struct PopupCoin{
        sf::Vector2f pos;
        float timer = 0.f;
        bool acitve = true;
    };
    struct QuestionBlock{
        sf::RectangleShape shape;
        bool used = false;
    };
    struct Coin{
        sf::Vector2f pos;

        bool collected = false;
    };
    struct Hazard{
        sf::RectangleShape shape;
    };
    struct Enemy{
        sf::RectangleShape shape;
        sf::Sprite sprite;

        sf::Vector2f spawnPoint;

        float speed = 100.f;
        int direction = -1;
        float leftBound;
        float rightBound;
        float animTimer = 0.f;
        float deathTimer = 0.f;

        bool alive = true;
        bool walkFrame = false;
        bool dying = false;
    };
    struct Goal{
        sf::RectangleShape shape;
        bool reached = false;
    };

    struct Player{
        sf::Vector2f pos = {100.f, 100.f};
        sf::Vector2f vel = {0.f, 0.f};
        sf::Vector2f size = {38.f, 45.f};
        sf::Vector2f respawnPoint = {100.f,100.f};

        bool onGround = false;
        bool jumping = false;
        bool canSuperJump = false;
        bool usedSuperJump = false;
        bool holdingJump = false;
        
        float gravity = 1500.f; 
        float moveSpeed = 280.f;

        float accel = 1800.f;
        float friction = 2000.f;

        float downHoldTime = 0.f;
        float dropTimer = 0.f;

        float jumpForce = -650.f;
        float superJumpForce = -800.f;

        float spaceHoldTime = 0.f;
        float superJumpHoldTime = 0.15f;

        float jumptime = 0.f;
        float superJumpWindow = 0.18f;

        float superJumpDelay = 0.10f;


    };

    float approach(float cur, float target, float maxDelta){
        if(cur < target){
            cur += maxDelta;
            if(cur > target){
                cur = target;
            }
        }else if(cur > target){
            cur -= maxDelta;
            if(cur < target){
                cur = target;
            }
        }
        return cur;
    }
    sf::FloatRect getPlayerBounds(Player& player){
        return sf::FloatRect(
            player.pos.x,
            player.pos.y,
            player.size.x,
            player.size.y
        );
    }

int main(){
    sf::Clock clock;
    
    sf::Event ev;
    Player player;
    sf::RenderWindow window(sf::VideoMode({960,540}), "MARIO Prototype");
    sf::View camera = window.getDefaultView();
    camera.zoom(1.5f);
    camera.setCenter(player.pos);

    window.setFramerateLimit(120);
    int score = 0;
    int scores = 0;
    bool levelComplete = false;
    bool gameOver = false;

    sf::Texture playerTexture;
    sf::Texture coinTexture;
    sf::Font font;
    sf::Texture go;
    sf::Texture goombaTexture;
    sf::Texture bgTexture;

    if(!font.loadFromFile("arial.ttf")){
        cout << "failed to load font\n";
    }
    if(!playerTexture.loadFromFile("assets/player.png")){
        cout << "Failed to load player sprite\n";
    }
    if(!coinTexture.loadFromFile("assets/coin.png")){
        cout << "Failed to load player sprite\n";
    }
    if(!goombaTexture.loadFromFile("assets/goon.png")){
        cout << "failed to load player sprite\n";
    }
    if(!bgTexture.loadFromFile("assets/level.png")){
        cout << "failed to load player sprite\n";
    }

    sf::Sprite playerSprite(playerTexture);
    sf::Sprite coinSprite(coinTexture);
    sf::Sprite ene(go);
    sf::Sprite goom(goombaTexture);
    sf::Sprite levelSprite(bgTexture);

    float sX = 2.3f;
    float sY = 3.4f;
    levelSprite.setPosition({-240.f,-10.f});
    levelSprite.setScale({sX,sY});

    sf::IntRect frame0({0,7}, {18,18});
    sf::IntRect frame1({19,7}, {18,18});
    sf::IntRect frame2({37,7}, {18,18});
    sf::IntRect frame3({55,7}, {18,18});
    sf::IntRect jumpFrame({95,7}, {18,18});
    sf::IntRect takeoffFrame({75,7}, {18,18});

    goom.setScale({4.f,4.f});

    sf::IntRect frame_1({7,39}, {17,17});
    sf::IntRect frame_2({39,39},{17,17});
    sf::IntRect frame_3({7,63},{17,12});

    float animTimer = 0.f;
    float animSpeed = 0.15f;
    int currentFrame = 0;
    float airTime = 0.f;
    int lives = 3;
    int nextLifeScore = 3000;

    sf::Text livesText;
    livesText.setFont(font);
    livesText.setCharacterSize(30);
    livesText.setFillColor(sf::Color::Black);

    sf::Text a;
    a.setFont(font);
    a.setCharacterSize(30);
    a.setFillColor(sf::Color::Black);

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::White);

    sf::Vector2u texSize = playerTexture.getSize();


    /*ene.setScale({
        40.f / (float)go.getSize().x,
        40.f / (float)go.getSize().y
    });*/
    playerSprite.setScale({2.5f,2.5f});

    playerSprite.setOrigin({9.0f,0.0f});
    bool questionBlockUsed = false;

    vector<Platform> platforms;
    vector<Coin> coins;
    vector<Hazard> haz;
    vector<Enemy> evil;
    vector<Goal> goals;
    vector<PopupCoin> ppc;

    Platform secblok;
    secblok.shape.setSize({39.f, 50.f});
    secblok.shape.setPosition({2116.f,430.f});
    secblok.shape.setFillColor(sf::Color::Transparent);
    secblok.type = PlatformType::Solid;
    secblok.isQuestionBlock = true;
    platforms.push_back(secblok);

    Platform Q;
    Q.shape.setSize({39.f, 50.f});
    Q.shape.setPosition({345.f,480.f});
    Q.shape.setFillColor(sf::Color::Transparent);
    Q.type = PlatformType::Solid;
    Q.isQuestionBlock = true;

    Platform QB;
    QB.shape.setSize({39.f, 50.f});
    QB.shape.setPosition({530.f,480.f});
    QB.shape.setFillColor(sf::Color::Transparent);
    QB.type = PlatformType::Solid;
    QB.isQuestionBlock = true;

    Platform QBB;
    QBB.shape.setSize({39.f, 50.f});
    QBB.shape.setPosition({605.f,480.f});
    QBB.shape.setFillColor(sf::Color::Transparent);
    QBB.type = PlatformType::Solid;
    QBB.isQuestionBlock = true;

    Platform QBBB;
    QBBB.shape.setSize({39.f, 50.f});
    QBBB.shape.setPosition({570.f,263.f});
    QBBB.shape.setFillColor(sf::Color::Transparent);
    QBBB.type = PlatformType::Solid;
    QBBB.isQuestionBlock = true;

    Platform QBBBB;
    QBBBB.shape.setSize({39.f, 50.f});
    QBBBB.shape.setPosition({2630.f,480.f});
    QBBBB.shape.setFillColor(sf::Color::Transparent);
    QBBBB.type = PlatformType::Solid;
    QBBBB.isQuestionBlock = true;

    Platform B;
    B.shape.setSize({39.f, 50.f});
    B.shape.setPosition({3220.f,263.f});
    B.shape.setFillColor(sf::Color::Transparent);
    B.type = PlatformType::Solid;
    B.isQuestionBlock = true;

    Platform BB;
    BB.shape.setSize({39.f, 50.f});
    BB.shape.setPosition({3770.f,263.f});
    BB.shape.setFillColor(sf::Color::Transparent);
    BB.type = PlatformType::Solid;
    BB.isQuestionBlock = true;

    Platform BBB;
    BBB.shape.setSize({39.f, 50.f});
    BBB.shape.setPosition({3773.f,263.f});
    BBB.shape.setFillColor(sf::Color::Transparent);
    BBB.type = PlatformType::Solid;
    BBB.isQuestionBlock = true;

    Platform BBBB;
    BBBB.shape.setSize({39.f, 50.f});
    BBBB.shape.setPosition({3771.f,480.f});
    BBBB.shape.setFillColor(sf::Color::Transparent);
    BBBB.type = PlatformType::Solid;
    BBBB.isQuestionBlock = true;

    Platform BBBBQ;
    BBBBQ.shape.setSize({39.f, 50.f});
    BBBBQ.shape.setPosition({3662.f,480.f});
    BBBBQ.shape.setFillColor(sf::Color::Transparent);
    BBBBQ.type = PlatformType::Solid;
    BBBBQ.isQuestionBlock = true;

    Platform A;
    A.shape.setSize({39.f, 50.f});
    A.shape.setPosition({4509.f,263.f});
    A.shape.setFillColor(sf::Color::Transparent);
    A.type = PlatformType::Solid;
    A.isQuestionBlock = true;

    Platform AA;
    AA.shape.setSize({39.f, 50.f});
    AA.shape.setPosition({4540.f,263.f});
    AA.shape.setFillColor(sf::Color::Transparent);
    AA.type = PlatformType::Solid;
    AA.isQuestionBlock = true;

    Platform AAA;
    AAA.shape.setSize({39.f, 50.f});
    AAA.shape.setPosition({3883.f,480.f});
    AAA.shape.setFillColor(sf::Color::Transparentg);
    AAA.type = PlatformType::Solid;
    AAA.isQuestionBlock = true;

    platforms.push_back(Q);
    platforms.push_back(QB);
    platforms.push_back(QBB);
    platforms.push_back(QBBB);
    platforms.push_back(QBBBB);
    platforms.push_back(B);
    platforms.push_back(BB);
    platforms.push_back(BBB);
    platforms.push_back(BBBB);
    platforms.push_back(BBBBQ);
    platforms.push_back(A);
    platforms.push_back(AA);
    platforms.push_back(AAA);

    Platform block;
    block.shape.setSize({39.f, 50.f});
    block.shape.setPosition({495.f,480.f});
    block.shape.setFillColor(sf::Color::Transparent);
    block.type = PlatformType::Solid;

    Platform bloc;
    bloc.shape.setSize({39.f, 50.f});
    bloc.shape.setPosition({640.f,480.f});
    bloc.shape.setFillColor(sf::Color::Transparent);
    bloc.type = PlatformType::Solid; 
    
    Platform blo;
    blo.shape.setSize({39.f, 50.f});
    blo.shape.setPosition({2665.f,480.f});
    blo.shape.setFillColor(sf::Color::Transparent);
    blo.type = PlatformType::Solid;

    Platform bl;
    bl.shape.setSize({39.f, 50.f});
    bl.shape.setPosition({2592.f,480.f});
    bl.shape.setFillColor(sf::Color::Transparent);
    bl.type = PlatformType::Solid;

    Platform b;
    b.shape.setSize({295.f, 50.f});
    b.shape.setPosition({2705.f,263.f});
    b.shape.setFillColor(sf::Color::Transparent);
    b.type = PlatformType::Solid;

    Platform k;
    k.shape.setSize({110.f, 50.f});
    k.shape.setPosition({3110.f,263.f});
    k.shape.setFillColor(sf::Color::Transparent);
    k.type = PlatformType::Solid;

    Platform kc;
    kc.shape.setSize({39.f, 50.f});
    kc.shape.setPosition({3220.f,480.f});
    kc.shape.setFillColor(sf::Color::Transparent);
    kc.type = PlatformType::Solid;

    Platform kco;
    kco.shape.setSize({74.f, 50.f});
    kco.shape.setPosition({3440.f,480.f});
    kco.shape.setFillColor(sf::Color::Transparent);
    kco.type = PlatformType::Solid;

    Platform kcol;
    kcol.shape.setSize({39.f, 50.f});
    kcol.shape.setPosition({4103.f,480.f});
    kcol.shape.setFillColor(sf::Color::Transparent);
    kcol.type = PlatformType::Solid;

    Platform kcolb;
    kcolb.shape.setSize({110.f, 50.f});
    kcolb.shape.setPosition({4213.f,263.f});
    kcolb.shape.setFillColor(sf::Color::Transparent);
    kcolb.type = PlatformType::Solid;

    Platform O;
    O.shape.setSize({74.f, 50.f});
    O.shape.setPosition({4508.f,480.f});
    O.shape.setFillColor(sf::Color::Transparent);
    O.type = PlatformType::Solid;

    Platform OA;
    OA.shape.setSize({39.f, 50.f});
    OA.shape.setPosition({4470.f,263.f});
    OA.shape.setFillColor(sf::Color::Transparent);
    OA.type = PlatformType::Solid;

    Platform OAA;
    OAA.shape.setSize({39.f, 50.f});
    OAA.shape.setPosition({4579.f,263.f});
    OAA.shape.setFillColor(sf::Color::Transparent);
    OAA.type = PlatformType::Solid;

    platforms.push_back(block);
    platforms.push_back(bloc);
    platforms.push_back(blo);
    platforms.push_back(bl);
    platforms.push_back(b);
    platforms.push_back(k);
    platforms.push_back(kc);
    platforms.push_back(kco);
    platforms.push_back(kcol);
    platforms.push_back(kcolb);
    platforms.push_back(O);
    platforms.push_back(OA);
    platforms.push_back(OAA);

    Goal flag;
    flag.shape.setSize({40.f,120.f});
    flag.shape.setPosition({900.f,460.f});
    flag.shape.setFillColor(sf::Color::White);
    goals.push_back(flag);

    Coin c1;
    c1.pos = {350.f,250.f};
    coinSprite.setScale({0.09f,0.09f});
    coins.push_back(c1);

    Platform pipe;
    pipe.shape.setSize({80.f,100.f});
    pipe.shape.setPosition({790.f,590.f});
    pipe.shape.setFillColor(sf::Color::Transparent);
    pipe.type = PlatformType::Solid;
    platforms.push_back(pipe);

    Platform pipee;
    pipee.shape.setSize({80.f,200.f});
    pipee.shape.setPosition({1160.f,535.f});
    pipee.shape.setFillColor(sf::Color::Transparent);
    pipee.type = PlatformType::Solid;
    platforms.push_back(pipee);

    Platform pipeee;
    pipee.shape.setSize({80.f,230.f});
    pipee.shape.setPosition({1450.f,483.f});
    pipee.shape.setFillColor(sf::Color::Transparent);
    pipee.type = PlatformType::Solid;
    platforms.push_back(pipee);

    Platform pipeeee;
    pipeeee.shape.setSize({80.f,230.f});
    pipeeee.shape.setPosition({1855.f,483.f});
    pipeeee.shape.setFillColor(sf::Color::Transparent);
    pipeeee.type = PlatformType::Solid;
    platforms.push_back(pipeeee);

    float levelEndX = 7055.f; // adjust to flag/castle area
    float levelWidth =  bgTexture.getSize().x * sX;
    float groundY = 697;
    Platform ground;

    for(float x = 0; x < levelWidth; x += 64){
        if(x >= 2263.f && x < 2306.f){
            continue;
        }
        if(x >= 2881.f && x < 3038.f){
            continue;
        }

        ground.shape.setSize({64.f,64.f});
        ground.shape.setFillColor(sf::Color::Transparent);
        ground.shape.setPosition({x,groundY});
        ground.type = PlatformType::Solid;

        platforms.push_back(ground);
    }
    
    /*Hazard bad;
    bad.shape.setSize({50.f,50.f});
    bad.shape.setPosition({350.f,400.f});
    bad.shape.setFillColor({sf::Color::Red});

    haz.push_back(bad);
    */

    Enemy e;
    e.shape.setSize({40.f,40.f});
    e.shape.setPosition({300.f,660.f});
    e.sprite.setScale({2.f, 2.f});
    e.sprite.setPosition(e.shape.getPosition());
    e.sprite.setTexture(goombaTexture);
    e.spawnPoint = e.shape.getPosition();
    e.leftBound = 400.f;
    e.rightBound = 700.f;

    Enemy ee;
    ee.shape.setSize({40.f,40.f});
    ee.shape.setPosition({550.f,445.f});
    ee.sprite.setScale({2.f, 2.f});
    ee.sprite.setPosition(e.shape.getPosition());
    ee.sprite.setTexture(goombaTexture);
    ee.spawnPoint = e.shape.getPosition();
    ee.leftBound = 500.f;
    ee.rightBound = 650.f;

    Enemy eee;
    eee.shape.setSize({40.f,40.f});
    eee.shape.setPosition({300.f,660.f});
    eee.sprite.setScale({2.f, 2.f});
    eee.sprite.setPosition(e.shape.getPosition());
    eee.sprite.setTexture(goombaTexture);
    eee.spawnPoint = e.shape.getPosition();
    eee.leftBound = 871.f;
    eee.rightBound = 970.5f;

    Enemy eeee;
    eeee.shape.setSize({40.f,40.f});
    eeee.shape.setPosition({300.f,660.f});
    eeee.sprite.setScale({2.f, 2.f});
    eeee.sprite.setPosition(e.shape.getPosition());
    eeee.sprite.setTexture(goombaTexture);
    eeee.spawnPoint = e.shape.getPosition();
    eeee.leftBound = 999.f;
    eeee.rightBound = 1122.f;

    evil.push_back(e);
    evil.push_back(ee);
    evil.push_back(eee);
    evil.push_back(eeee);

    player.pos = {4399.f, 652.f};
    sf::Vector2f playerStartPos = player.pos;

    
    while(window.isOpen()){
        //update loop
        float dt = clock.restart().asSeconds();//this means time passed sicne last frame 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y)){
            cout << "Mario X: " << player.pos.x
                << " Mario Y: " << player.pos.y
                << '\n';
        }
        //Event
        while(window.pollEvent(ev)){
            if(ev.type == sf::Event::Closed){
                window.close();
            }
            if(ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Space && player.onGround == true){
                player.vel.y = player.jumpForce; 
                
                player.jumptime = 0.f;
                player.canSuperJump = true;
                player.usedSuperJump = false;
                player.holdingJump = true;
            }
            if(gameOver && ev.type == sf::Event::KeyPressed){
                if(ev.key.code == sf::Keyboard::R){
                    gameOver = false;
                    levelComplete = false;

                    lives = 3;
                    score = 0;
                    scores = 0;

                    player.pos = playerStartPos;
                    player.respawnPoint = playerStartPos;
                    player.vel = {0.f,0.f};
                    
                    for(auto& c : coins){
                        c.collected = false;
                    }

                    for(auto& e : evil){
                        e.alive = true;
                        e.dying = false;
                        e.deathTimer = 0.f;
                        e.direction = 1;

                        e.shape.setPosition(e.spawnPoint);
                        e.sprite.setPosition(e.spawnPoint);
                    }
                    for(auto& b: platforms){
                        if(b.isQuestionBlock){
                            b.used = false;
                            b.shape.setFillColor(sf::Color::Transparent);
                        }
                    }
                }
                if(ev.key.code == sf::Keyboard::Escape){
                    window.close();
                }
            } 
            // jump height/event loop 
        }
        //Reset State
        player.onGround = false;
        //input/movement
        if(!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
            player.holdingJump = false;
        }
        if(player.canSuperJump){
            player.jumptime += dt;

            if(player.jumptime >= player.superJumpDelay &&
                player.jumptime <= player.superJumpWindow &&
                player.holdingJump &&
                !player.usedSuperJump){

                player.vel.y = player.superJumpForce;
                player.usedSuperJump = true;
                player.canSuperJump = false;
            }
            
            if(player.jumptime > player.superJumpWindow){
                player.canSuperJump = false;
            }
        }


        float dir = 0.f;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
            dir -= 1.f;
        }
        
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
            dir += 1.f;
        }

        float target = dir * player.moveSpeed;

        if(dir != 0.f){
            player.vel.x = approach(player.vel.x, target,
            player.accel * dt);
        } else{
            player.vel.x = approach(player.vel.x, 0.f,
            player.friction * dt);
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && player.onGround == true){
                player.downHoldTime += dt;
            } else {
                player.downHoldTime = 0.f;
        }

        if(player.downHoldTime >= 0.5f){
            player.dropTimer = 0.2f;
            player.downHoldTime = 0.f;
        }

        if(player.dropTimer > 0.f){
            player.dropTimer -= dt;
        }

        float prevX = player.pos.x;
        player.pos.x += player.vel.x *dt;

        if (player.pos.x + player.size.x > levelEndX){
            player.pos.x = levelEndX - player.size.x;
            player.vel.x = 0.f;
        }
        if (player.pos.x < 0.f){
            player.pos.x = 0.f;
            player.vel.x = 0.f;
        }
        float left = player.pos.x;
        float right = player.pos.x + player.size.x;

        float prevLeft = prevX;
        float prevRight = prevX + player.size.x;

        float bottom = player.pos.y + player.size.y;
        
        // Collision
        for(auto& pf: platforms){
            if(pf.type == PlatformType::Solid){

                float pfLeft = pf.shape.getPosition().x;
                float pfRight = pf.shape.getPosition().x + pf.shape.getSize().x;
                float pfTop = pf.shape.getPosition().y;
                float pfBottom = pf.shape.getPosition().y + pf.shape.getSize().y;

                float top = player.pos.y;

                if(player.vel.x > 0 && prevRight <= pfLeft &&
                    right >= pfLeft && bottom > pfTop && top < pfBottom){

                    player.pos.x = pfLeft - player.size.x; 
                    player.vel.x = 0;

                } else if(player.vel.x < 0 && prevLeft >= pfRight &&
                        left <= pfRight && bottom > pfTop && top < pfBottom){
                        
                        player.pos.x = pfRight;
                        player.vel.x = 0;
                }
            }
        }
        
        float prevY = player.pos.y;
        
        player.vel.y +=  player.gravity * dt;        
        player.pos.y += player.vel.y *dt;        

        left = player.pos.x;
        right = player.pos.x + player.size.x;
        
        bottom = player.pos.y + player.size.y;
        float prevBottom = prevY + player.size.y;

        float top = player.pos.y;
        float prevTop = prevY;
        
        player.onGround = false;

        for(auto& pf : platforms){

            float pfLeft  = pf.shape.getPosition().x;
            float pfRight = pf.shape.getPosition().x + pf.shape.getSize().x;
            float pfTop   = pf.shape.getPosition().y;
            float pfBottom = pfTop + pf.shape.getSize().y;

            if(pf.type == PlatformType::Solid){
                
                if(player.vel.y > 0 && prevBottom <= pfTop &&
                    bottom >= pfTop && right > pfLeft && left < pfRight){

                    player.pos.y = pfTop - player.size.y;
                    player.vel.y = 0;

                    player.onGround = true;
                    bottom = player.pos.y + player.size.y;

                }else if(player.vel.y < 0 && prevTop >= pfBottom &&
                    top <= pfBottom && right > pfLeft && left < pfRight){
                            sf::FloatRect headSensor(
                                player.pos.x + 10.f,
                                player.pos.y,
                                player.size.x - 20.f,
                                8.f
                            );
                        if(pf.isQuestionBlock && !pf.used && headSensor.intersects(pf.shape.getGlobalBounds())){
                            pf.used = true;

                            PopupCoin pc;
                            pc.pos = {
                                pf.shape.getPosition().x,
                                pf.shape.getPosition().y - 50.f
                            };

                            ppc.push_back(pc);
                            
                            pf.shape.setFillColor(sf::Color::Blue);

                            score++;
                            scores += 100;

                            cout << "Coin!\n";
                        }
                    player.pos.y = pfBottom;
                    player.vel.y = 0;
                }  
            }
            else if(pf.type == PlatformType::OneWay){

                if(player.dropTimer <= 0.f &&
                    player.vel.y > 0 &&
                    prevBottom <= pfTop && 
                    bottom >= pfTop &&
                    right > pfLeft &&
                    left < pfRight){

                    player.pos.y = pfTop - player.size.y;
                    player.vel.y = 0;
                    player.onGround = true;

                    bottom = player.pos.y + player.size.y;
                }
            }
        }
        // GAme Rules

        if(player.pos.y > 900.f){
            player.pos = player.respawnPoint;
            player.vel = {0.f,0.f};
            lives--;
        }
        if(scores >= nextLifeScore){
            lives++;
            nextLifeScore += 3000;
        }
        for(auto& hz : haz){
            sf::FloatRect playerBounds = getPlayerBounds(player);
            if(playerBounds.intersects(hz.shape.getGlobalBounds())){
                player.pos = player.respawnPoint;
                player.vel = {0.f,0.f};
                lives--;
            }
        }
        if(dir > 0){//spiet right
            playerSprite.setScale({
                abs(playerSprite.getScale().x),
                playerSprite.getScale().y
            });
        } else if(dir < 0){//sprite left 
            playerSprite.setScale({
                -abs(playerSprite.getScale().x),
                playerSprite.getScale().y
            });
        }

        //animition
        if(!player.onGround){
            airTime += dt;

            if(airTime < 0.1f){
                playerSprite.setTextureRect(takeoffFrame);
            } else {
                playerSprite.setTextureRect(jumpFrame);
            } 
        }else if(dir != 0.f){
            animTimer += dt;
            airTime = 0.f;

            if(animTimer >= animSpeed){
                animTimer = 0.f;
                currentFrame++;

                if(currentFrame > 3){
                    currentFrame = 0;
                }
            }
            if(currentFrame == 0){
                playerSprite.setTextureRect(frame1);
            } else if(currentFrame == 1){
                playerSprite.setTextureRect(frame2);
            } else if(currentFrame == 2){
                playerSprite.setTextureRect(frame3);
            } else if(currentFrame == 3){
                playerSprite.setTextureRect(frame2);
            }
        } else {
            airTime = 0.f;
            playerSprite.setTextureRect(frame0);
            animTimer = 0.f;
            currentFrame = 0;
        }

        for(auto& pc : ppc){
            if(pc.acitve)
            {
                pc.pos.y -= 100.f * dt;

                pc.timer += dt;

                if(pc.timer > 0.5f)
                {
                    pc.acitve = false;
                }
            }
        }

        for(auto& e : evil){
            if(e.alive){
                e.shape.move({e.speed * e.direction * dt, 0.f});

                if(e.shape.getPosition().x <= e.leftBound){
                    e.direction = 1;
                }

                if(e.shape.getPosition().x >= e.rightBound){
                    e.direction = -1;
                }
                e.animTimer += dt;

                if(e.animTimer >= 0.2f){
                    e.animTimer = 0.f;
                    e.walkFrame = !e.walkFrame;
                }

                if(e.walkFrame){
                    e.sprite.setTextureRect(frame_1);
                } else{
                    e.sprite.setTextureRect(frame_2);
                }
                e.sprite.setPosition(e.shape.getPosition());
                
                sf::FloatRect playerBounds = getPlayerBounds(player);

                if(playerBounds.intersects(e.shape.getGlobalBounds())){
                    float enemyTop = e.shape.getPosition().y;

                    if(bottom <= enemyTop + 15.f && player.vel.y > 0){
                        e.alive = false;
                        e.dying = true;
                        e.deathTimer = 0.f;
                        scores += 300;
                        e.sprite.setTextureRect(frame_3);
                        player.vel.y = -800.f;
                    }else{
                        player.pos = player.respawnPoint;
                        player.vel = {0.f, 0.f};
                        lives--;
                    }
                }
            } else if(e.dying){
                e.deathTimer += dt;

                if(e.deathTimer >= 0.5f){
                    e.dying = false;
                }
            }
        }

        for(auto& g : goals){
            sf::FloatRect p = getPlayerBounds(player);
            if(!g.reached && p.intersects(g.shape.getGlobalBounds())){
                g.reached = true;
                levelComplete = true;
                scores += 1000;
                g.shape.setFillColor(sf::Color::Green);
            }

        }
        
        // camera/HUD
        float cameraX = camera.getCenter().x;
        float cameraY = 400.f;

        float playerCenterX = player.pos.x + player.size.x / 2.f;

        if(playerCenterX > cameraX + 80.f){
            cameraX = playerCenterX - 80.f;
        } 
        else if(playerCenterX < cameraX - 80.f){
            cameraX = playerCenterX + 80.f;
        }

        float halfScreen = window.getSize().x / 2.f;

        if(cameraX < halfScreen){
            cameraX = halfScreen;
        }
        if(cameraX > levelWidth - halfScreen){
            cameraX = levelWidth - halfScreen;
        }        
            window.clear(sf::Color(135, 206, 235));
        camera.setCenter({cameraX, cameraY});
        window.setView(camera);
        window.draw(levelSprite);
        // Draw
        for(auto& pf : platforms){
            window.draw(pf.shape);
        }
        for(auto& hz : haz){
            window.draw(hz.shape);
        }
        for(auto& e : evil){
            if(e.alive || e.dying){
                window.draw(e.sprite);
            }
        }
        for(auto& g: goals){
            window.draw(g.shape);
        }
        for(auto& pc : ppc){
            if(pc.acitve)
            {
                coinSprite.setPosition(pc.pos);
                window.draw(coinSprite);
            }
        }
        playerSprite.setPosition({
            player.pos.x + player.size.x / 2.5f,
            player.pos.y
        });
        window.draw(playerSprite);

        for(auto& coin : coins){
            float coinLeft = coin.pos.x;
            float coinRight = coin.pos.x + 32.f;

            float coinTop = coin.pos.y;
            float coinBottom = coin.pos.y + 32.f;

            if(!coin.collected && right > coinLeft && left < coinRight &&
               bottom > coinTop && top < coinBottom){

                coin.collected = true;
                score++;
                scores += 100;
            }
            if(!coin.collected){
                coinSprite.setPosition(coin.pos);
                window.draw(coinSprite);
            }
        }
        window.setView(window.getDefaultView());

        sf::Text winText;
        winText.setFont(font);
        winText.setString("LEVEL COMPLETE!");
        winText.setCharacterSize(40);
        winText.setFillColor(sf::Color::White);
        winText.setPosition({300.f, 220.f});
        sf::RectangleShape winBox;
        winBox.setSize({400.f, 200.f});
        winBox.setPosition({280.f, 170.f});
        winBox.setFillColor(sf::Color(0, 0, 0, 200));
        if(levelComplete){
            window.draw(winBox);
            window.draw(winText);    
        }
        sf::Text loseText;
        loseText.setFont(font);
        loseText.setString("!GAME OVER");
        loseText.setCharacterSize(40);
        loseText.setFillColor(sf::Color::White);
        loseText.setPosition({350.f,160.f});
        sf::RectangleShape gameoverbox;
        gameoverbox.setSize({400.f, 200.f});
        gameoverbox.setPosition({280.f, 170.f});
        gameoverbox.setFillColor(sf::Color::Black);
        if(lives == -1){
            gameOver = true;
            if(gameOver){
                window.draw(gameoverbox);
                window.draw(loseText);
                
            }
        }
        /*if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)){
            std::cout << "Mario X: " << player.pos.x << '\n';
        }*/
        livesText.setString("Lives: " + to_string(lives));
        livesText.setPosition({5.f,70.f});
        a.setString("Score: " + to_string(scores));
        a.setPosition({5.f,40.f});
        window.draw(a);
        window.draw(livesText);
        
        scoreText.setString("Coins: " + to_string(score));

        window.draw(scoreText);
        

        window.display();
    }
}