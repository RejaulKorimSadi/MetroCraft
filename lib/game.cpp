// MetroCraft by Md Rejaul Korim Sadi (231-115-349) and Kazi Abu Jafor Arif (231-115-068), CSE 402 CGIP Lab, Metropolitan University
#include "game.h"
#include "campus.h"
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <algorithm>

const char* DEVELOPER = "Sadi & Arif";
const char* DEV_NAMES = "Md Rejaul Korim Sadi  (231-115-349)        Kazi Abu Jafor Arif  (231-115-068)";
const char* DEV_INFO = "CSE 58th Batch   |   Metropolitan University, Sylhet";
const char* VERSION = "v1.0";

Game game;

static const char* SAVE_FILE = "world.dat";
static const float PI = 3.14159265f;

void say(const std::string& text) {
    game.message = text;
    game.messageTimer = 3.0f;
}

static void spawnPlayer() {
    campusSpawn(game.player.x, game.player.y, game.player.z);   // the entrance plaza, facing the campus
    game.player.vy = 0;
    game.player.yaw = 0;
    game.player.pitch = 10;
}

void newWorld(uint32_t seed) {
    delete game.world;
    game.world = new World(seed);
    game.world->generate();
    game.world->buildAllMeshes();
    spawnPlayer();
    campusSign(game.signX, game.signY, game.signZ);
    clearParticles();
}

static void capture(bool on) {
    game.mouseCaptured = on;
    glutSetCursor(on ? GLUT_CURSOR_NONE : GLUT_CURSOR_LEFT_ARROW);
    if (on) glutWarpPointer(game.winW / 2, game.winH / 2);
}

void startGame() {
    game.screen = PLAYING;
    game.introTimer = 6.0f;
    capture(true);
}

static void backToTitle() {
    game.screen = TITLE;
    game.titleTime = 0.0f;
    capture(false);
    game.input = Input();
}

static void updateView() {
    View& v = game.view;
    const Player& p = game.player;
    if (game.screen == TITLE) {
        // swing slowly from side to side in front of the roof signboard, looking at it
        float a = std::sin(game.titleTime * 0.12f) * 0.55f, cx = game.signX, cz = game.signZ;
        v.x = cx + std::sin(a) * 103.0f;
        v.z = cz + std::cos(a) * 103.0f;
        v.y = game.signY + 10.0f;
        v.yaw = std::atan2(cx - v.x, -(cz - v.z)) * 180.0f / PI;
        v.pitch = -15.0f;
        game.world->drawRange = 200.0f;
        v.fov = 70.0f;
    } else {
        v.x = p.x;
        v.y = p.y + Player::EYE + std::sin(game.walkPhase * 2.0f) * 0.06f * game.bobAmount;
        v.z = p.z;
        v.yaw = p.yaw;
        v.pitch = p.pitch;
        v.fov = game.fov;
        game.world->drawRange = 115.0f;
    }
}

float frameTime() {
    static int last = glutGet(GLUT_ELAPSED_TIME), fpsTimer = last;
    int now = glutGet(GLUT_ELAPSED_TIME);
    float dt = std::min((now - last) / 1000.0f, 0.1f);
    last = now;
    if (now - fpsTimer >= 1000) { game.fps = game.frames; game.frames = 0; fpsTimer = now; }
    return dt;
}

void updateGame(float dt) {
    game.clock += dt;
    if (game.messageTimer > 0) game.messageTimer -= dt;
    if (game.introTimer > 0) game.introTimer -= dt;
    if (game.swingTimer > 0) game.swingTimer -= dt;

    static float waterClock = 0;
    waterClock += dt;
    if (waterClock > 0.12f) { waterClock = 0; animateWater(game.atlas, game.clock); }

    if (game.screen == TITLE) {
        game.titleTime += dt;
    } else {
        Player& p = game.player;
        p.update(*game.world, game.input, dt);
        float speed = std::sqrt(p.vx * p.vx + p.vz * p.vz);
        bool walking = speed > 0.1f && p.onGround && !p.flying;
        game.bobAmount += ((walking ? 1.0f : 0.0f) - game.bobAmount) * std::min(1.0f, dt * 8.0f);
        if (walking) game.walkPhase += dt * speed * 1.7f;
        float targetFov = (game.input.sprint && speed > 0.1f) ? 78.0f : 70.0f;
        game.fov += (targetFov - game.fov) * std::min(1.0f, dt * 8.0f);
        updateParticles(dt);
    }
    game.world->rebuildDirty();
    updateView();
}

bool readyToPlay(int button) {
    if (game.screen == TITLE) {
        if (button == GLUT_LEFT_BUTTON) startGame();
        return false;
    }
    if (!game.mouseCaptured) {
        if (button == GLUT_LEFT_BUTTON) capture(true);
        return false;
    }
    return true;
}

bool lookingAround() {
    return game.mouseCaptured && game.screen == PLAYING;
}

RayHit lookAt() {
    float dx, dy, dz;
    const Player& p = game.player;
    p.lookDir(dx, dy, dz);
    return game.world->raycast(p.x, p.y + Player::EYE, p.z, dx, dy, dz, 6.0f);
}

void breakBlock(const RayHit& hit) {
    if (!hit.hit) return;
    Block target = game.world->get(hit.x, hit.y, hit.z);
    game.swingTimer = 0.25f;
    if (target == BEDROCK) return;
    spawnBreakParticles(hit.x, hit.y, hit.z, target);
    game.world->set(hit.x, hit.y, hit.z, AIR);
}

void placeBlock(const RayHit& hit) {
    if (!hit.hit) return;
    World& w = *game.world;
    Block held = game.hotbar[game.selected];
    int x = hit.px, y = hit.py, z = hit.pz;
    if (isPlant(w.get(hit.x, hit.y, hit.z))) { x = hit.x; y = hit.y; z = hit.z; }   // building onto a flower replaces it
    Block there = w.get(x, y, z), below = w.get(x, y - 1, z);
    bool roomFree = there == AIR || there == WATER || isPlant(there);
    bool soilOk = !isPlant(held) || below == GRASS || below == DIRT;
    if (roomFree && soilOk && (isPlant(held) || !game.player.overlapsBlock(x, y, z))) {
        w.set(x, y, z, held);
        game.swingTimer = 0.25f;
    }
}

void pickBlock(const RayHit& hit) {
    if (!hit.hit) return;
    Block target = game.world->get(hit.x, hit.y, hit.z);
    if (target == BEDROCK || target == WATER) return;
    game.hotbar[game.selected] = target;
    say(std::string("Picked ") + blockName(target));
}

bool scrollHotbar(int button) {
    if (button == 3) { game.selected = (game.selected + 8) % 9; return true; }   // wheel up
    if (button == 4) { game.selected = (game.selected + 1) % 9; return true; }   // wheel down
    return false;
}

static void moveKey(unsigned char key, bool down) {
    if (key >= 1 && key <= 26) key = 'a' + key - 1;   // holding Ctrl turns W into code 23, so undo that
    Input& in = game.input;
    switch (std::tolower(key)) {
    case 'w': in.forward = down; break;
    case 's': in.back = down; break;
    case 'a': in.left = down; break;
    case 'd': in.right = down; break;
    case ' ': in.jump = down; break;
    }
}

static void pressKey(unsigned char key, bool down) {
    if (game.screen == TITLE) {
        if (!down) return;
        if (key == 27) { delete game.world; std::exit(0); }
        if (key == 13 || key == ' ') startGame();
        if (std::tolower(key) == 'n') { newWorld((uint32_t)std::time(nullptr)); game.titleTime = 0.9f; }
        return;
    }
    moveKey(key, down);
    if (!down) return;
    game.input.sprint = (glutGetModifiers() & GLUT_ACTIVE_CTRL) != 0;
    switch (std::tolower(key)) {
    case 27:
        if (game.showAbout) game.showAbout = false;
        else if (game.mouseCaptured) capture(false);
        else backToTitle();
        break;
    case 'f':
        game.player.flying = !game.player.flying;
        game.player.vy = 0;
        say(game.player.flying ? "Flying" : "Walking");
        break;
    case 't': game.timeOfDay = (game.timeOfDay + 1) % 3; break;
    case 'h': game.showHelp = !game.showHelp; break;
    case 'i': game.showAbout = !game.showAbout; break;
    case 'n':
        newWorld((uint32_t)std::time(nullptr));
        say("New world created");
        break;
    default:
        if (key >= '1' && key <= '9') game.selected = key - '1';
    }
}

static void pressSpecial(int key, bool down) {
    if (game.screen == TITLE) return;
    Player& p = game.player;
#ifdef GLUT_KEY_SHIFT_L
    if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R) { game.input.sneak = down; return; }
    if (key == GLUT_KEY_CTRL_L || key == GLUT_KEY_CTRL_R) { game.input.sprint = down; return; }
#endif
    if (!down) return;
    switch (key) {
    case GLUT_KEY_F1: game.showHelp = !game.showHelp; break;
    case GLUT_KEY_F2: game.shotPending = true; break;
    case GLUT_KEY_F3: game.showDebug = !game.showDebug; break;
    case GLUT_KEY_F5: {
        float s[5] = {p.x, p.y, p.z, p.yaw, p.pitch};
        say(game.world->save(SAVE_FILE, s) ? "World saved to world.dat" : "Could not save the world");
        break;
    }
    case GLUT_KEY_F9: {
        float s[5];
        if (game.world->load(SAVE_FILE, s)) {
            p.x = s[0]; p.y = s[1]; p.z = s[2]; p.yaw = s[3]; p.pitch = s[4];
            p.vy = 0;
            clearParticles();
            say("World loaded");
        } else say("No saved world found");
        break;
    }
    }
}

void keyDown(unsigned char key, int, int) { pressKey(key, true); }
void keyUp(unsigned char key, int, int) { pressKey(key, false); }
void specialDown(int key, int, int) { pressSpecial(key, true); }
void specialUp(int key, int, int) { pressSpecial(key, false); }
