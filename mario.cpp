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

    struct Player{
        sf::Vector2f pos = {100.f, 100.f};
        sf::Vector2f vel = {0.f, 0.f};
        sf::Vector2f size = {40.f, 60.f};

        bool onGround = false;

        float gravity = 1500.f; 
        float moveSpeed = 280.f;
        float accel = 1800.f;
        float friction = 2000.f;
        float downHoldTime = 0.f;
        float dropTimer = 0.f;
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

int main(){
    sf::Clock clock;
    sf::Event ev;
    Player player;
    sf::RenderWindow window(sf::VideoMode({960,540}), "MARIO Prototype");
    
    window.setFramerateLimit(120);

    sf::RectangleShape rec;
    rec.setSize(player.size);

    vector<Platform> platforms;
    
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

    platforms.push_back(p1);
    platforms.push_back(p2);
    platforms.push_back(p3);


    while(window.isOpen()){
        float dt = clock.restart().asSeconds();

        
        while(window.pollEvent(ev)){
            if(ev.type == sf::Event::Closed){
                window.close();
            }
            if(ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Space && player.onGround == true){
                player.vel.y = -800.f; 
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

                } else if(player.vel.x < 0 && prevLeft >= pfRight && left <= pfRight
                        && bottom > pfTop && top < pfBottom){
                        
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
                
                if(player.vel.y > 0 && prevBottom <= pfTop && bottom >= pfTop &&
                    right > pfLeft && left < pfRight){
                    player.pos.y = pfTop - player.size.y;
                    player.vel.y = 0;

                    player.onGround = true;
                    bottom = player.pos.y + player.size.y;

                }else if(player.vel.y < 0 && prevTop >= pfBottom && top <= pfBottom &&
                    right > pfLeft && left < pfRight){
                    player.pos.y = pfBottom;
                    player.vel.y = 0;
                }  
            } else if(pf.type == PlatformType::OneWay){
                    float pfTop = pf.shape.getPosition().y;

                    if(player.dropTimer <= 0.f && player.vel.y > 0 && prevBottom <= pfTop && 
                        bottom >= pfTop && right > pfLeft && left < pfRight){

                        player.pos.y = pfTop - player.size.y;
                        player.vel.y = 0;
                        player.onGround = true;
                        bottom = player.pos.y + player.size.y;
                    }
                }
        }
        float floorY = 480.f;
        bottom = player.pos.y + player.size.y;
        if(bottom >= floorY){
            player.pos.y = floorY - player.size.y;
            player.vel.y = 0.f;
            player.onGround = true;
        } 


        rec.setPosition(player.pos);

        window.clear(sf::Color::Black);
        for(auto& pf : platforms){
            window.draw(pf.shape);
        }
        window.draw(rec);
        window.display();
        
    }
}

