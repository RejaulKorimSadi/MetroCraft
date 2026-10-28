// MetroCraft by Md Rejaul Korim Sadi (231-115-349) and Kazi Abu Jafor Arif (231-115-068), CSE 402 CGIP Lab, Metropolitan University
// the Metropolitan University campus, built block by block on the flat ground world.cpp leaves in the middle
#include "campus.h"
#include "world.h"
#include <cmath>
#include <cstring>
#include <algorithm>

static const int G = CAMPUS_GROUND;

float campusEdge(int x, int z) {
    int dx = std::max({CAMPUS_X0 - x, x - CAMPUS_X1, 0});
    int dz = std::max({CAMPUS_Z0 - z, z - CAMPUS_Z1, 0});
    float t = std::min(std::max(dx, dz) / 16.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

void campusSpawn(float& x, float& y, float& z) { x = 96.5f; y = G + 1.01f; z = 181.5f; }

void campusSign(float& x, float& y, float& z) { x = 96.5f; y = G + 41.5f; z = 47.0f; }

static void box(World& w, int x0, int y0, int z0, int x1, int y1, int z1, Block b) {
    for (int x = x0; x <= x1; x++)
        for (int y = y0; y <= y1; y++)
            for (int z = z0; z <= z1; z++) w.set(x, y, z, b);
}

// ---------------------------------------------------------------- block letters

// a slimmer 3-wide set for long words on small walls
static const char* narrowRow(char c, int row) {
    static const char* C[5] = {".11", "1..", "1..", "1..", ".11"};
    static const char* E[5] = {"111", "1..", "11.", "1..", "111"};
    static const char* F[5] = {"111", "1..", "11.", "1..", "1.."};
    static const char* L[5] = {"1..", "1..", "1..", "1..", "111"};
    static const char* O[5] = {"111", "1.1", "1.1", "1.1", "111"};
    static const char* R[5] = {"11.", "1.1", "11.", "1.1", "1.1"};
    static const char* T[5] = {"111", ".1.", ".1.", ".1.", ".1."};
    static const char* X[5] = {"1.1", "1.1", ".1.", "1.1", "1.1"};
    switch (c) {
    case 'C': return C[row]; case 'E': return E[row]; case 'F': return F[row]; case 'L': return L[row];
    case 'O': return O[row]; case 'R': return R[row]; case 'T': return T[row]; case 'X': return X[row];
    }
    return nullptr;
}

// 5-row letters; each one is 3 to 5 blocks wide
static const char* letterRow(char c, int row, bool narrow = false) {
    if (narrow && narrowRow(c, row)) return narrowRow(c, row);
    static const char* A[5] = {".11.", "1..1", "1111", "1..1", "1..1"};
    static const char* B[5] = {"111.", "1..1", "111.", "1..1", "111."};
    static const char* D[5] = {"111.", "1..1", "1..1", "1..1", "111."};
    static const char* E[5] = {"1111", "1...", "111.", "1...", "1111"};
    static const char* F[5] = {"1111", "1...", "111.", "1...", "1..."};
    static const char* I[5] = {"111", ".1.", ".1.", ".1.", "111"};
    static const char* L[5] = {"1...", "1...", "1...", "1...", "1111"};
    static const char* M[5] = {"1...1", "11.11", "1.1.1", "1...1", "1...1"};
    static const char* N[5] = {"1..1", "11.1", "1.11", "1..1", "1..1"};
    static const char* O[5] = {".11.", "1..1", "1..1", "1..1", ".11."};
    static const char* P[5] = {"111.", "1..1", "111.", "1...", "1..."};
    static const char* R[5] = {"111.", "1..1", "111.", "1.1.", "1..1"};
    static const char* S[5] = {".111", "1...", ".11.", "...1", "111."};
    static const char* T[5] = {"11111", "..1..", "..1..", "..1..", "..1.."};
    static const char* U[5] = {"1..1", "1..1", "1..1", "1..1", ".11."};
    static const char* V[5] = {"1...1", "1...1", "1...1", ".1.1.", "..1.."};
    static const char* Y[5] = {"1...1", ".1.1.", "..1..", "..1..", "..1.."};
    switch (c) {
    case 'A': return A[row]; case 'B': return B[row]; case 'D': return D[row]; case 'E': return E[row];
    case 'F': return F[row]; case 'I': return I[row]; case 'L': return L[row]; case 'M': return M[row];
    case 'N': return N[row]; case 'O': return O[row]; case 'P': return P[row]; case 'R': return R[row];
    case 'S': return S[row]; case 'T': return T[row]; case 'U': return U[row]; case 'V': return V[row];
    case 'Y': return Y[row];
    }
    return "..";
}

static int textWidth(const char* text, bool narrow = false) {
    int w = 0;
    for (const char* c = text; *c; c++) w += (int)std::strlen(letterRow(*c, 0, narrow)) + 1;
    return w - 1;
}

// writes text upright from its top-left block, stepping (dx, dz) for each column so it can face any side
static void writeText(World& w, const char* text, int x, int yTop, int z, int dx, int dz, Block b, bool narrow = false) {
    for (const char* c = text; *c; c++) {
        int width = (int)std::strlen(letterRow(*c, 0, narrow));
        for (int row = 0; row < 5; row++)
            for (int col = 0; col < width; col++)
                if (letterRow(*c, row, narrow)[col] == '1') w.set(x + col * dx, yTop - row, z + col * dz, b);
        x += (width + 1) * dx;
        z += (width + 1) * dz;
    }
}

// ---------------------------------------------------------------- trees

static void palm(World& w, int x, int z, int height) {
    int y = G + 1, top = y + height;
    box(w, x, y, z, x, top - 1, z, LOG);
    w.set(x, top, z, LEAVES);
    static const int DIRS[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    for (int d = 0; d < 8; d++) {
        int reach = d < 4 ? 3 : 2;
        for (int i = 1; i <= reach; i++) w.set(x + DIRS[d][0] * i, i == reach ? top - 1 : top, z + DIRS[d][1] * i, LEAVES);
    }
}

// ---------------------------------------------------------------- the main academic building

// seven storeys, read left to right as you face it: a solid red brick panel, a tall strip of dark
// windows in the middle, then brick and window bands with a white banner; the ground floor is glass and open
static void mainBuilding(World& w) {
    const int x0 = 63, x1 = 129, z0 = 34, z1 = 52, floors = 7, roof = G + 4 * floors;
    const int midL = 87, midR = 105;
    for (int f = 0; f <= floors; f++) box(w, x0, G + 4 * f, z0, x1, G + 4 * f, z1, CONCRETE);

    for (int f = 0; f < floors; f++) {
        int y = G + 4 * f;
        for (int x = x0; x <= x1; x++) {
            bool column = x == x0 || x == midL || x == midR || x == x1;
            for (int r = 1; r <= 3; r++) {
                Block front = CONCRETE;
                if (column) front = CONCRETE;
                else if (f == 0) front = x < 73 ? BRICK : (x < midR ? GLASS : AIR);
                else if (x < midL) front = BRICK;
                else if (x < midR) front = (x == 92 || x == 96 || x == 100) ? CONCRETE : WINDOW;
                else front = r == 1 ? BRICK : WINDOW;
                Block back = (x - x0) % 8 == 0 ? CONCRETE : (f == 0 || r == 1 ? BRICK : WINDOW);
                w.set(x, y + r, z1, front);
                w.set(x, y + r, z0, back);
            }
        }
        for (int z = z0 + 1; z < z1; z++) {
            bool column = (z - z0) % 6 == 0;
            for (int r = 1; r <= 3; r++) {
                Block side = column ? CONCRETE : (f == 0 || r == 1 ? BRICK : WINDOW);
                w.set(x0, y + r, z, side);
                w.set(x1, y + r, z, side);
            }
        }
    }
    box(w, x0 + 1, G + 4, z1, midL - 1, roof - 1, z1, BRICK);   // the big brick panel runs over the floor lines
    box(w, 94, G + 1, z1, 98, G + 2, z1, AIR);                  // front door

    // the white banner on the right, with MU on it
    box(w, 110, G + 9, z1, 124, G + 23, z1, CONCRETE);
    box(w, 111, G + 10, z1, 123, G + 10, z1, PAINT_RED);
    box(w, 111, G + 22, z1, 123, G + 22, z1, PAINT_RED);
    writeText(w, "MU", 112, G + 18, z1, 1, 0, PAINT_BLUE);

    for (int x = x0 + 8; x < x1; x += 8)
        for (int z : {40, 46}) box(w, x, G + 1, z, x, roof - 1, z, CONCRETE);

    // stairs inside, one flight per storey, turning back and forth, the last one up onto the roof
    for (int f = 0; f < floors; f++) {
        int y = G + 4 * f;
        bool even = f % 2 == 0;
        int za = even ? 50 : 48;
        for (int k = 1; k <= 3; k++) {
            int x = even ? 93 + k : 97 - k;
            box(w, x, y + 1, za, x, y + k, za + 1, CONCRETE);
        }
        box(w, 94, y + 4, za, 96, y + 4, za + 1, AIR);
    }

    // roof edge and the white sunshade frame on the top right
    for (int x = x0; x <= x1; x++) { w.set(x, roof + 1, z0, CONCRETE); w.set(x, roof + 1, z1, CONCRETE); }
    for (int z = z0; z <= z1; z++) { w.set(x0, roof + 1, z, CONCRETE); w.set(x1, roof + 1, z, CONCRETE); }
    for (int x : {106, 114, 122, 128})
        for (int z : {38, 52}) box(w, x, roof + 1, z, x, roof + 3, z, CONCRETE);
    for (int x = 106; x <= 128; x += 2) box(w, x, roof + 4, 38, x, roof + 4, 53, CONCRETE);
    box(w, 106, roof + 4, 38, 128, roof + 4, 38, CONCRETE);
    box(w, 106, roof + 4, 53, 128, roof + 4, 53, CONCRETE);
}

// the white signboard on the roof with the university's name in red, standing on three legs
static void universitySign(World& w) {
    const char* lines[2] = {"METROPOLITAN", "UNIVERSITY"};
    const int x0 = 64, x1 = 128, z = 47, bottom = G + 34, top = bottom + 15;
    for (int x : {66, 96, 126}) box(w, x, G + 29, z - 1, x, bottom - 1, z, CONCRETE);
    box(w, x0, bottom, z - 1, x1, top, z, CONCRETE);
    for (int i = 0; i < 2; i++) {
        int start = x0 + (x1 - x0 + 1 - textWidth(lines[i])) / 2;
        writeText(w, lines[i], start, top - 2 - i * 7, z, 1, 0, i ? PAINT_BLACK : PAINT_RED);
    }
}

// "DEVELOPED BY / SADI AND ARIF" on a board on the west side of the field, facing it like a scoreboard
static void developerSign(World& w) {
    const char* lines[2] = {"DEVELOPED BY", "SADI AND ARIF"};
    const int x = 32, z0 = 61, z1 = 123, bottom = G + 4, top = bottom + 15;
    for (int z : {z0 + 2, z0 + 31, z0 + 60}) box(w, x - 1, G + 1, z, x - 1, bottom - 1, z, CONCRETE);
    box(w, x - 1, bottom, z0, x, top, z1, CONCRETE);
    box(w, x, top, z0, x, top, z1, PAINT_GREEN);
    box(w, x, bottom, z0, x, bottom, z1, PAINT_GREEN);
    for (int i = 0; i < 2; i++) {
        int start = z1 - (z1 - z0 + 1 - textWidth(lines[i])) / 2;
        writeText(w, lines[i], x, top - 2 - i * 7, start, 0, -1, i ? PAINT_RED : PAINT_BLACK);
    }
}

// the Centre For Excellence, first building on the right inside the gate, its front facing the road
static void centreForExcellence(World& w) {
    const int x0 = 144, x1 = 172, z0 = 64, z1 = 138;
    for (int y : {G, G + 6, G + 12}) box(w, x0, y, z0, x1, y, z1, CONCRETE);
    box(w, x0 - 1, G + 12, z0 - 1, x1 + 1, G + 12, z1 + 1, CONCRETE);
    for (int y = G + 1; y <= G + 11; y++) {
        if (y == G + 6) continue;
        bool upper = y > G + 6;
        for (int z = z0; z <= z1; z++) {
            bool mullion = z == z0 || z == z1 || (z - z0) % 4 == 0;
            Block b = mullion ? CONCRETE : (upper || (y >= G + 2 && y <= G + 4) ? GLASS : CONCRETE);
            w.set(x0, y, z, b);
            w.set(x1, y, z, b);
        }
        for (int x = x0; x <= x1; x++) {
            bool mullion = x == x0 || x == x1 || (x - x0) % 4 == 0;
            Block b = mullion ? CONCRETE : (upper || (y >= G + 2 && y <= G + 4) ? GLASS : CONCRETE);
            w.set(x, y, z0, b);
            w.set(x, y, z1, b);
        }
    }

    // the red tiled hip roof over the glass upper floor
    for (int k = 0; k < 8; k++) box(w, x0 - 1 + 2 * k, G + 13 + k, z0 - 1 + 2 * k, x1 + 1 - 2 * k, G + 13 + k, z1 + 1 - 2 * k, ROOF_TILE);

    // left: a tall white frame holding a grid of glass
    box(w, 140, G + 1, 66, 143, G + 12, 77, CONCRETE);
    for (int y = G + 2; y <= G + 11; y++)
        for (int z = 67; z <= 76; z++) {
            bool bar = y == G + 6 || (z - 66) % 4 == 0;
            w.set(140, y, z, bar ? CONCRETE : GLASS);
            if (!bar) box(w, 141, y, z, 143, y, z, AIR);
        }

    // middle: the white name wall with the red i
    box(w, 142, G + 1, 80, 143, G + 14, 127, CONCRETE);
    box(w, 142, G + 3, 82, 142, G + 11, 82, PAINT_RED);
    w.set(142, G + 13, 82, PAINT_RED);
    writeText(w, "CENTRE FOR", 142, G + 13, 85, 0, 1, PAINT_GREEN, true);
    writeText(w, "EXCELLENCE", 142, G + 7, 85, 0, 1, PAINT_GREEN, true);

    // the doorway with its green and white balloon arch, and a railing on each side of the step up
    box(w, x0, G + 1, 129, x0, G + 2, 130, AIR);
    w.set(x0, G + 3, 129, GLASS);
    w.set(x0, G + 3, 130, GLASS);
    for (int y = G + 1; y <= G + 5; y++)
        for (int z = 128; z <= 131; z++)
            if (z == 128 || z == 131 || y == G + 5) w.set(140, y, z, (y + z) % 2 ? PAINT_GREEN : CONCRETE);
    box(w, 141, G, 128, 143, G, 131, CONCRETE);
    for (int z : {127, 132})
        for (int x = 136; x <= 139; x++) w.set(x, G + 1, z, x % 2 ? CONCRETE : AIR);

    // right: a glass box that hangs out over the ground
    box(w, 139, G + 4, 132, 143, G + 12, 138, CONCRETE);
    for (int y = G + 5; y <= G + 11; y++) {
        for (int z = 133; z <= 137; z++) {
            bool bar = y == G + 8 || z == 135;
            w.set(139, y, z, bar ? CONCRETE : GLASS);
        }
        for (int x = 140; x <= 142; x++) w.set(x, y, 138, GLASS);
        box(w, 140, y, 133, 143, y, 137, AIR);
    }
    box(w, 140, G + 5, 133, 143, G + 5, 137, CONCRETE);

    // the solid white corner on the far side with the green round logo
    box(w, 162, G + 1, z1, x1, G + 11, z1, CONCRETE);
    for (int y = G + 5; y <= G + 11; y++)
        for (int x = 163; x <= 171; x++)
            if (std::fabs(std::hypot(x - 167.0f, y - (G + 8.0f)) - 2.5f) < 0.8f) w.set(x, y, z1 + 1, PAINT_GREEN);
}

// ---------------------------------------------------------------- the rest of the campus

// the entrance gate: a white beam with the university's name across the road, on two pillars
static void entranceGate(World& w) {
    const char* top = "METROPOLITAN";
    const char* bottom = "UNIVERSITY";
    const int x0 = 63, x1 = 129, z0 = 140, z1 = 141, width = x1 - x0 + 1;
    box(w, x0, G + 8, z0, x1, G + 20, z1, CONCRETE);
    box(w, x0, G + 1, z0, x0 + 3, G + 7, z1, CONCRETE);
    box(w, x1 - 3, G + 1, z0, x1, G + 7, z1, CONCRETE);
    box(w, x0, G + 1, z0 - 1, x0 + 3, G + 2, z1 + 1, BRICK);
    box(w, x1 - 3, G + 1, z0 - 1, x1, G + 2, z1 + 1, BRICK);
    writeText(w, top, x0 + (width - textWidth(top)) / 2, G + 19, z1 + 1, 1, 0, PAINT_BLACK);
    writeText(w, bottom, x0 + (width - textWidth(bottom)) / 2, G + 13, z1 + 1, 1, 0, PAINT_RED);
    for (int x : {x0 - 6, x1 + 2}) {
        box(w, x, G + 1, z1 + 1, x + 4, G + 1, z1 + 2, LEAVES);
        for (int k = 0; k < 5; k += 2) w.set(x + k, G + 1, z1 + 3, k == 2 ? ROSE : DANDELION);
    }
}

// a row of flag poles behind the gate; the first one on the right flies the flag of Bangladesh
static void flags(World& w) {
    const Block colours[5] = {PAINT_RED, PAINT_GREEN, PAINT_BLUE, PAINT_YELLOW, CONCRETE};
    const int z = 134;
    box(w, 106, G + 1, z, 106, G + 16, z, CONCRETE);
    box(w, 107, G + 12, z, 113, G + 16, z, PAINT_GREEN);
    box(w, 109, G + 13, z, 111, G + 15, z, PAINT_RED);
    int i = 0;
    for (int x = 46; x <= 134; x += 8) {
        if (x > 82 && x < 122) continue;
        box(w, x, G + 1, z, x, G + 12, z, CONCRETE);
        box(w, x + 1, G + 10, z, x + 3, G + 12, z, colours[i++ % 5]);
    }
}

// the playing field with a red running track around it, like the one in the campus plan
static void field(World& w) {
    const float ax = 74, bx = 118, cz = 96;
    for (int x = 50; x <= 142; x++)
        for (int z = 72; z <= 120; z++) {
            float px = std::max(ax, std::min(bx, (float)x)), d = std::hypot(x - px, z - cz);
            if (d > 16 && d <= 20) w.set(x, G, z, std::fabs(d - 18) < 0.5f ? CONCRETE : TRACK);
        }
    for (int x = 68; x <= 124; x++) { w.set(x, G, 86, CONCRETE); w.set(x, G, 106, CONCRETE); }
    for (int z = 86; z <= 106; z++) { w.set(68, G, z, CONCRETE); w.set(124, G, z, CONCRETE); w.set(96, G, z, CONCRETE); }
    for (int x = 90; x <= 102; x++)
        for (int z = 90; z <= 102; z++)
            if (std::fabs(std::hypot(x - 96.0f, z - 96.0f) - 5.0f) < 0.5f) w.set(x, G, z, CONCRETE);
    for (int x : {68, 124}) {
        box(w, x, G + 1, 93, x, G + 3, 93, CONCRETE);
        box(w, x, G + 1, 99, x, G + 3, 99, CONCRETE);
        box(w, x, G + 3, 93, x, G + 3, 99, CONCRETE);
    }
}

// the pond just inside the entrance, right of the road, with a concrete ghat stepping down into it
static void pond(World& w) {
    box(w, 110, G - 1, 146, 146, G, 168, WATER);
    box(w, 104, G, 150, 109, G, 164, CONCRETE);
    box(w, 102, G, 155, 103, G, 159, CONCRETE);
    for (int k = 0; k < 3; k++) box(w, 110 + k, G - 2, 150, 110 + k, G - k, 164, CONCRETE);
}

// the smaller white two-storey building with red trim, on the left as you walk in
static void whiteBuilding(World& w) {
    const int x0 = 26, x1 = 50, z0 = 148, z1 = 172;
    for (int f = 0; f <= 2; f++) box(w, x0, G + 4 * f, z0, x1, G + 4 * f, z1, CONCRETE);
    for (int f = 0; f < 2; f++) {
        int y = G + 4 * f;
        for (int r = 1; r <= 3; r++) {
            for (int x = x0; x <= x1; x++) {
                Block b = (r >= 2 && (x - x0) % 3 != 0) ? WINDOW : CONCRETE;
                w.set(x, y + r, z0, b);
                w.set(x, y + r, z1, b);
            }
            for (int z = z0; z <= z1; z++) {
                Block b = (r >= 2 && (z - z0) % 3 != 0) ? WINDOW : CONCRETE;
                w.set(x0, y + r, z, b);
                w.set(x1, y + r, z, b);
            }
        }
    }
    box(w, x1, G + 1, 165, x1, G + 3, 169, GLASS);
    for (int x = x0; x <= x1; x++) { w.set(x, G + 8, z0, PAINT_RED); w.set(x, G + 8, z1, PAINT_RED); w.set(x, G + 9, z0, PAINT_RED); w.set(x, G + 9, z1, PAINT_RED); }
    for (int z = z0; z <= z1; z++) { w.set(x0, G + 8, z, PAINT_RED); w.set(x1, G + 8, z, PAINT_RED); w.set(x0, G + 9, z, PAINT_RED); w.set(x1, G + 9, z, PAINT_RED); }
}

// the little gazebo with a green roof beside the main building
static void gazebo(World& w) {
    box(w, 143, G, 56, 149, G, 62, CONCRETE);
    for (int x : {144, 148})
        for (int z : {57, 61}) box(w, x, G + 1, z, x, G + 3, z, CONCRETE);
    for (int i = 0; i < 4; i++) box(w, 143 + i, G + 4 + i, 56 + i, 149 - i, G + 4 + i, 62 - i, PAINT_GREEN);
}

static void paths(World& w) {
    box(w, 70, G, 168, 122, G, 186, BRICK);    // entrance plaza
    box(w, 92, G, 128, 100, G, 167, BRICK);    // the main road to the field
    box(w, 91, G, 128, 91, G, 167, CONCRETE);
    box(w, 101, G, 128, 101, G, 167, CONCRETE);
    box(w, 51, G, 166, 69, G, 168, CONCRETE);  // to the white building's door
    box(w, 40, G, 122, 138, G, 127, BRICK);    // the walk along the south of the track
    box(w, 40, G, 64, 43, G, 121, CONCRETE);   // side paths up to the building
    box(w, 139, G, 64, 141, G, 138, BRICK);    // the paving in front of the Centre For Excellence
    box(w, 50, G, 53, 142, G, 57, BRICK);      // the plaza in front of the building
    box(w, 92, G, 58, 100, G, 75, BRICK);      // and the walk from its door to the track
}

// the lawn in front of the building: flower rows, oaks on the left, palms on the right
static void frontLawn(World& w) {
    for (int x = 50; x <= 142; x += 2) {
        if (x > 88 && x < 104) continue;
        w.set(x, G + 1, 58, x % 4 ? ROSE : DANDELION);
        w.set(x, G + 1, 63, x % 4 ? DANDELION : ROSE);
    }
    for (int x : {54, 66, 78}) w.placeTree(x, G + 1, 61, 0.5f);
    for (int x : {114, 122, 130, 138}) palm(w, x, 61, 6);
}

void buildCampus(World& w) {
    paths(w);
    field(w);
    pond(w);
    mainBuilding(w);
    universitySign(w);
    developerSign(w);
    centreForExcellence(w);
    entranceGate(w);
    flags(w);
    whiteBuilding(w);
    gazebo(w);
    frontLawn(w);

    for (int z : {172, 180}) { palm(w, 66, z, 7); palm(w, 126, z, 7); }
    const int palms[][2] = {{54, 158}, {54, 184}, {18, 156}, {150, 150}, {150, 164}, {170, 150}};
    for (const auto& p : palms) palm(w, p[0], p[1], 6);
    const int oaks[][2] = {{46, 68}, {46, 118}, {24, 60}, {24, 100}, {174, 56},
                           {62, 186}, {130, 186}, {24, 136}, {160, 180}};
    for (const auto& o : oaks) w.placeTree(o[0], G + 1, o[1], 0.5f);
}
