// MetroCraft by Md Rejaul Korim Sadi (231-115-349) and Kazi Abu Jafor Arif (231-115-068), CSE 402 CGIP Lab, Metropolitan University
#ifndef CAMPUS_H
#define CAMPUS_H

class World;

// the campus is a flat lawn at this height, with hills all around it
const int CAMPUS_GROUND = 26;
const int CAMPUS_X0 = 16, CAMPUS_X1 = 175, CAMPUS_Z0 = 26, CAMPUS_Z1 = 187;

float campusEdge(int x, int z);   // 0 on the campus, rising smoothly to 1 out on the hills
void buildCampus(World& w);       // the buildings, field, pond, signs and trees

void campusSpawn(float& x, float& y, float& z);   // the entrance plaza
void campusSign(float& x, float& y, float& z);    // centre of the university signboard on the roof

#endif
