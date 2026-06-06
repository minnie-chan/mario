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
    };
    struct Coin{
        sf::Vector2f pos;

        bool collected = false;
    };
    struct CheckPoint{
        sf::RectangleShape shape;
        bool activated = false;
    };
    struct Hazard{
        sf::RectangleShape shape;
    };
    struct Enemy{
        sf::RectangleShape shape;
        sf::Sprite sprite;

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
        sf::Vector2f size = {40.f, 60.f};
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

        float jumpForce = -550.f;
        float superJumpForce = -700.f;

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


    sf::Texture playerTexture;
    sf::Texture coinTexture;
    sf::Font font;
    sf::Texture go;
    sf::Texture goombaTexture;

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

    sf::Sprite playerSprite(playerTexture);
    sf::Sprite coinSprite(coinTexture);
    sf::Sprite ene(go);
    sf::Sprite goom(goombaTexture);
    
    playerSprite.setScale({2.5f,2.5f});

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

    sf::Text a;
    a.setFont(font);
    a.setCharacterSize(30);
    a.setFillColor(sf::Color::Black);

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::White);

    sf::Vector2u texSize = playerTexture.getSize();

    float spriteOffsetX = 0.f;
    float spriteOffsetY = 2.f;

    /*ene.setScale({
        40.f / (float)go.getSize().x,
        40.f / (float)go.getSize().y
    });
    playerSprite.setScale({
        player.size.x / (float)texSize.x,
        player.size.y / (float)texSize.y
    });*/

    /*playerSprite.setOrigin({
        texSize.x / 2.f,
        0.f
    });*/

    vector<Platform> platforms;
    vector<Coin> coins;
    vector<Hazard> haz;
    vector<CheckPoint> check;
    vector<Enemy> evil;
    vector<Goal> goals;

    Goal flag;
    flag.shape.setSize({40.f,120.f});
    flag.shape.setPosition({900.f,460.f});
    flag.shape.setFillColor(sf::Color::White);
    goals.push_back(flag);

    Coin c1;
    c1.pos = {350.f,250.f};
    coinSprite.setScale({0.09f,0.09f});
    coins.push_back(c1);

    Platform p1;
    p1.shape.setSize({200.f, 10.f});
    p1.shape.setPosition({200.f, 450.f});
    p1.shape.setFillColor(sf::Color::Green);
    p1.type = PlatformType::Solid;

    Platform p2;
    p2.shape.setSize({200.f, 10.f});
    p2.shape.setPosition({400.f, 300.f});
    p2.shape.setFillColor(sf::Color::Green);
    p2.type = PlatformType::OneWay;

    Platform p3;
    p3.shape.setSize({200.f, 10.f});
    p3.shape.setPosition({600.f, 200.f});
    p3.shape.setFillColor(sf::Color::Green);
    p3.type = PlatformType::Solid;

    Platform ground;
    ground.shape.setSize({4000.f,80.f});
    ground.shape.setPosition({-200.f,580.f});
    ground.shape.setFillColor(sf::Color(100,200,100));
    ground.type = PlatformType::Solid;

    Hazard bad;
    bad.shape.setSize({50.f,50.f});
    bad.shape.setPosition({350.f,400.f});
    bad.shape.setFillColor({sf::Color::Red});

    CheckPoint ch;
    ch.shape.setSize({100.f,50.f});
    ch.shape.setPosition({600.f,500.f});
    ch.shape.setFillColor(sf::Color::Yellow);

    Enemy e;
    e.shape.setSize({40.f,40.f});
    e.shape.setPosition({500.f,540.f});
    
    e.sprite.setScale({2.f, 2.f});
    e.sprite.setPosition(e.shape.getPosition());
    e.sprite.setTexture(goombaTexture);
    e.leftBound = 400.f;
    e.rightBound = 700.f;
    evil.push_back(e);
    
    check.push_back(ch);
    haz.push_back(bad);
    platforms.push_back(ground);
    platforms.push_back(p1);
    platforms.push_back(p2);
    platforms.push_back(p3);

    
    while(window.isOpen()){
        float dt = clock.restart().asSeconds();//this means time passed sicne last frame 

        
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
            } // jump height/event loop 
        }
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
        

        float left = player.pos.x;
        float right = player.pos.x + player.size.x;

        float prevLeft = prevX;
        float prevRight = prevX + player.size.x;

        float bottom = player.pos.y + player.size.y;

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
        for(auto& hz : haz){
            sf::FloatRect playerBounds = getPlayerBounds(player);
            if(playerBounds.intersects(hz.shape.getGlobalBounds())){
                player.pos = player.respawnPoint;
                player.vel = {0.f,0.f};
            }
        }
        for(auto& ch : check){
            sf::FloatRect playerBounds = getPlayerBounds(player);
            if(!ch.activated && playerBounds.intersects(ch.shape.getGlobalBounds())){
                player.respawnPoint = ch.shape.getPosition();
                ch.activated = true;
                ch.shape.setFillColor(sf::Color::Green);
                scores += 500;
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

        float floorY = 580.f;

        bottom = player.pos.y + player.size.y;

        if(bottom >= floorY){
            player.pos.y = floorY - player.size.y;
            player.vel.y = 0.f;
            player.onGround = true;
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


        playerSprite.setPosition({
            player.pos.x + player.size.x / 2.f + spriteOffsetX,
            player.pos.y + spriteOffsetY
        });

        float cameraX = camera.getCenter().x;
        float cameraY = 270.f;

        float playerCenterX = player.pos.x + player.size.x / 2.f;

        if(playerCenterX > cameraX + 80.f){
            cameraX = playerCenterX - 80.f;
        } else if(playerCenterX < cameraX - 80.f){
            cameraX = playerCenterX + 80.f;
        }

        camera.setCenter({cameraX, cameraY});
        window.setView(camera);

        window.clear(sf::Color(135, 206, 235));

        for(auto& pf : platforms){
            window.draw(pf.shape);
        }
        for(auto& hz : haz){
            window.draw(hz.shape);
        }
        for(auto& ch: check){
            window.draw(ch.shape);
        }
        for(auto& e : evil){
            if(e.alive || e.dying){
                window.draw(e.sprite);
            }
        }
        for(auto& g: goals){
            window.draw(g.shape);
        }
        
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
        a.setString("Score: " + to_string(scores));
        a.setPosition({10.f,40.f});
        window.draw(a);
        
        scoreText.setString("Coins: " + to_string(score));

        window.draw(scoreText);

        window.display();
    }
}