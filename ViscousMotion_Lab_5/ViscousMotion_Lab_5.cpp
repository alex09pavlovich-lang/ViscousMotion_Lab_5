#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <optional>
#include <string>
#include <vector>

namespace {
constexpr unsigned W = 1100, H = 720;
constexpr float dt = 1.f / 120.f;
const sf::Color bg(18, 23, 34), panel(29, 37, 53), cyan(70, 214, 239), muted(116, 139, 166);

// Compact 5x7 bitmap font: no external font assets required.
const std::array<const char*, 7>& glyph(char ch) {
    static const std::array<const char*,7> blank = {"00000","00000","00000","00000","00000","00000","00000"};
    static const std::array<std::array<const char*,7>, 10> digits = {{
        {{"01110","10001","10011","10101","11001","10001","01110"}},
        {{"00100","01100","00100","00100","00100","00100","01110"}},
        {{"01110","10001","00001","00010","00100","01000","11111"}},
        {{"11110","00001","00001","01110","00001","00001","11110"}},
        {{"00010","00110","01010","10010","11111","00010","00010"}},
        {{"11111","10000","10000","11110","00001","00001","11110"}},
        {{"01111","10000","10000","11110","10001","10001","01110"}},
        {{"11111","00001","00010","00100","01000","01000","01000"}},
        {{"01110","10001","10001","01110","10001","10001","01110"}},
        {{"01110","10001","10001","01111","00001","00001","11110"}}
    }};
    static const std::array<const char*,7> dot = {"00000","00000","00000","00000","00000","01100","01100"};
    static const std::array<const char*,7> minus = {"00000","00000","00000","11111","00000","00000","00000"};
    static const std::array<const char*,7> colon = {"00000","01100","01100","00000","01100","01100","00000"};
    if (ch >= '0' && ch <= '9') return digits[ch-'0'];
    if (ch == '.') return dot;
    if (ch == '-') return minus;
    if (ch == ':') return colon;
    return blank;
}
void number(sf::RenderWindow& win, const std::string& s, float x, float y, float scale=3.f, sf::Color color=sf::Color::White) {
    sf::RectangleShape pixel({scale, scale}); pixel.setFillColor(color);
    for(char ch : s) {
        const auto& g = glyph(ch);
        for(int r=0;r<7;++r) for(int c=0;c<5;++c) if(g[r][c]=='1') {
            pixel.setPosition({x+c*scale, y+r*scale}); win.draw(pixel);
        }
        x += 6*scale;
    }
}
std::string fmt(float v, int precision=2) {
    char buf[40]; std::snprintf(buf,sizeof(buf),"%.*f",precision,v); return buf;
}
void rect(sf::RenderWindow& win, sf::Vector2f pos, sf::Vector2f size, sf::Color color) {
    sf::RectangleShape shape(size); shape.setPosition(pos); shape.setFillColor(color); win.draw(shape);
}
struct Model {
    float mass=1.f, resistance=.45f, initialSpeed=10.f;
    float x=0.f, v=10.f, time=0.f;
    bool running=false;
    std::vector<sf::Vector2f> samples;
    void reset() { x=0; v=initialSpeed; time=0; running=false; samples.clear(); samples.push_back({0,v}); }
    void step() {
        // Exact time-step update for m*dv/dt = -k*v, constant k and m.
        float decay=std::exp(-resistance/mass*dt);
        float oldV=v;
        v*=decay;
        x += resistance>0.00001f ? (oldV-v)*mass/resistance : oldV*dt;
        time+=dt;
        if(samples.empty() || time-samples.back().x >= .04f) samples.push_back({time,v});
    }
};
void graph(sf::RenderWindow& win, const Model& m) {
    const float gx=55, gy=385, gw=990, gh=255;
    rect(win,{gx,gy},{gw,gh},panel);
    for(int i=0;i<=5;++i) {
        rect(win,{gx,gy+i*gh/5},{gw,1},sf::Color(48,62,80));
        rect(win,{gx+i*gw/5,gy},{1,gh},sf::Color(48,62,80));
    }
    const float horizon=std::max(10.f,m.time);
    const float ymax=std::max(1.f,m.initialSpeed);
    sf::VertexArray line(sf::PrimitiveType::LineStrip);
    for(auto p : m.samples) {
        if(p.x < horizon-10.f) continue;
        const float t = p.x - std::max(0.f,horizon-10.f);
        line.append(sf::Vertex({gx+gw*t/10.f, gy+gh-gh*std::clamp(p.y/ymax,0.f,1.f)}, cyan));
    }
    if(line.getVertexCount()>1) win.draw(line);
    number(win,fmt(ymax,1),gx+7,gy+8,2,muted);
    number(win,"0",gx+7,gy+gh-19,2,muted);
    number(win,fmt(horizon,1),gx+gw-80,gy+gh-19,2,muted);
}
}
int main() {
    sf::RenderWindow window(sf::VideoMode({W,H}), "Viscous Motion | SPACE: run/pause | R: reset | UP/DOWN: drag | LEFT/RIGHT: mass | A/Z: initial speed");
    window.setFramerateLimit(60);
    Model model; model.reset();
    sf::Clock clock;
    float accumulator=0.f;
    while(window.isOpen()) {
        while(const std::optional event=window.pollEvent()) {
            if(event->is<sf::Event::Closed>()) window.close();
            if(const auto* key=event->getIf<sf::Event::KeyPressed>()) {
                switch(key->code) {
                    case sf::Keyboard::Key::Escape: window.close(); break;
                    case sf::Keyboard::Key::Space: model.running=!model.running; break;
                    case sf::Keyboard::Key::R: model.reset(); accumulator=0; break;
                    case sf::Keyboard::Key::Up: model.resistance=std::min(3.f,model.resistance+.05f); model.reset(); accumulator=0; break;
                    case sf::Keyboard::Key::Down: model.resistance=std::max(0.f,model.resistance-.05f); model.reset(); accumulator=0; break;
                    case sf::Keyboard::Key::Right: model.mass=std::min(10.f,model.mass+.25f); model.reset(); accumulator=0; break;
                    case sf::Keyboard::Key::Left: model.mass=std::max(.25f,model.mass-.25f); model.reset(); accumulator=0; break;
                    case sf::Keyboard::Key::A: model.initialSpeed=std::min(25.f,model.initialSpeed+1.f); model.reset(); accumulator=0; break;
                    case sf::Keyboard::Key::Z: model.initialSpeed=std::max(1.f,model.initialSpeed-1.f); model.reset(); accumulator=0; break;
                    default: break;
                }
            }
        }
        float frame=std::min(clock.restart().asSeconds(),.1f);
        if(model.running) {
            accumulator+=frame;
            while(accumulator>=dt) { model.step(); accumulator-=dt; }
        } else accumulator=0;
        window.clear(bg);
        rect(window,{35,30},{1030,330},panel);
        rect(window,{55,292},{990,3},muted);
        // Horizontal track: position scaled to fit visible area.
        float displayX=75.f+std::fmod(model.x*12.f,930.f);
        sf::CircleShape body(24.f); body.setOrigin({24.f,24.f});
        body.setPosition({displayX,267.f}); body.setFillColor(cyan); window.draw(body);
        // Dashboard: mass, drag, initial speed / time, speed, acceleration / distance.
        number(window,fmt(model.mass),70,65,3);
        number(window,fmt(model.resistance),270,65,3);
        number(window,fmt(model.initialSpeed),510,65,3);
        number(window,fmt(model.time),70,125,3,cyan);
        number(window,fmt(model.v),270,125,3,cyan);
        number(window,fmt(-model.resistance*model.v/model.mass),510,125,3,cyan);
        number(window,fmt(model.x),760,125,3,cyan);
        // Label-free legend: 1=mass, 2=drag, 3=initial speed, 4=time, 5=speed, 6=acceleration, 7=distance.
        number(window,"1:",70,45,2,muted); number(window,"2:",270,45,2,muted);
        number(window,"3:",510,45,2,muted); number(window,"4:",70,105,2,muted);
        number(window,"5:",270,105,2,muted); number(window,"6:",510,105,2,muted);
        number(window,"7:",760,105,2,muted);
        number(window,model.running?"1":"0",970,65,3,model.running?cyan:muted);
        graph(window,model);
        window.display();
    }
}
