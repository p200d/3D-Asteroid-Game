#include <cassert>
#include <cmath>
#include <string>
#include <iostream>
#include <time.h>
#include <sstream>
#include <chrono>
#include <iomanip>

#include "GetGlut.h"
#include "Sleep.h"
#include "ObjLibrary\ObjModel.h"
#include "ObjLibrary\DisplayList.h"
#include "ObjLibrary\Vector3.h"
#include "CoordinateSystem.h"
#include "KeyboardControls.h"
#include "PerlinNoiseField3.h"
#include "Entity.h"
#include "Asteroid2.h"
#include "Spaceship.h"
#include "Crystal.h"
#include "Drone.h"
#include "ObjLibrary\SpriteFont.h"

using namespace ObjLibrary;
using namespace std;
using namespace chrono;

namespace {
	double random1(){ return rand() / (RAND_MAX + 1.0); }
	double random2(double min_value, double max_value){
		assert(min_value <= max_value);
		return min_value + random1() * (max_value - min_value);
	}
}

void init();
void initDisplay();
void update();
void reshape(int w, int h);
void display();
void drawCrystals();
void drawDrones();
void drawDroneInfo(unsigned int index);
void drawAsteroids();
void drawCamera();
void drawAsteroidCoords(unsigned int index);
void drawSkybox();
void drawBlackHole();
void drawPlayerShip();
void drawAccretionDisk();
void initFramesAndUpdates();
void doGameUpdates(double delta_time);
void drawOverlays();
bool checkCollision(Asteroid2 ast1, Asteroid2 ast2);
void asteroidBounce(Asteroid2& ast1, Asteroid2& ast2);
bool checkPlayerCollision(Asteroid2 ast);
bool checkCrystalAstCollision(Crystal cryst, Asteroid2 ast);
void crystalBounce(Crystal& cryst, Asteroid2& ast);
bool checkPlayerCrystalCollision(Crystal cryst);
bool checkDroneAstCollision(Drone drone_check);
bool checkDroneTooClose(unsigned int index, unsigned int ast_index);

const unsigned int NUM_ASTEROIDS = 100;
const unsigned int NUM_ASTEROID_MODELS = 25;
bool GAME_OVER = false;
CoordinateSystem camera;
ObjModel disk;
ObjModel skybox;
DisplayList disk_list;
PerlinNoiseField3 perlin_noise;

Asteroid2 asteroids2[NUM_ASTEROIDS];
ObjModel asteroids2Object[NUM_ASTEROID_MODELS];
DisplayList asteroids2_list[NUM_ASTEROID_MODELS];

Spaceship player;
ObjModel player_obj;
DisplayList player_model_list;

unsigned int NUM_CRYSTALS_DRIFTING;
unsigned int NUM_CRYSTALS_COLLECTED;
vector<Crystal> crystal;
ObjModel crystal_obj;
DisplayList crystal_list;

unsigned int LIVING_DRONES = 5;
vector<Drone> drone;
ObjModel drone_model;
DisplayList drone_list;

string COORDS_STRING = "";

const string ASTEROID_PATHS[NUM_ASTEROID_MODELS] =
{
	"Models/AsteroidA.obj", "Models/AsteroidB.obj", "Models/AsteroidC.obj", "Models/AsteroidD.obj",
	"Models/AsteroidE.obj", "Models/AsteroidF.obj", "Models/AsteroidG.obj", "Models/AsteroidH.obj",
	"Models/AsteroidI.obj", "Models/AsteroidJ.obj", "Models/AsteroidK.obj", "Models/AsteroidL.obj",
	"Models/AsteroidM.obj", "Models/AsteroidN.obj", "Models/AsteroidO.obj", "Models/AsteroidP.obj",
	"Models/AsteroidQ.obj", "Models/AsteroidR.obj", "Models/AsteroidS.obj", "Models/AsteroidT.obj",
	"Models/AsteroidU.obj", "Models/AsteroidV.obj", "Models/AsteroidW.obj", "Models/AsteroidX.obj",
	"Models/AsteroidY.obj" 
};

SpriteFont font;
int window_width = 1020;
int window_height = 768;

//Frame & Physics Update Globals
int frame_count = 0; //Count how many frames have been displayed
system_clock::time_point start_time; //Time game started (initialize in init())
system_clock::time_point last_frame_time; //Keep track of time last frame was drawn
double smoothed_frames_per_second = 60; //Smooths how often instantaneous frame rate is updated. SET TO DESIRED (but plausible) VALUE.
int update_count = 0; //Count how many updates have occurred
system_clock::time_point next_update_time; //Keep track of time next update should happen

unsigned int UPDATES_PER_SECOND = 60; //Desired update rate
const double SECONDS_PER_PHYSICS = 1.0 / UPDATES_PER_SECOND;
microseconds DELTA_TIME(1000000 / UPDATES_PER_SECOND);
const milliseconds FRAME_TIME_MIN(5); //Minimum frame duration
const unsigned int MAXIMUM_UPDATES_PER_FRAME = 10;

duration<double> last_update_duration(DELTA_TIME);
system_clock::time_point last_update_time; //Keep track of when last update was performed.
const milliseconds UPDATE_TIME_MIN(5);
double smoothed_updates_per_second = 60;

int main(int argc, char* argv[])
{
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(0, 0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutCreateWindow("Space Game");
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyUp);
	glutSpecialFunc(special);
	glutSpecialUpFunc(specialUp);
	glutIdleFunc(update);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);

	//printPerlinNoise();

	init();

	glutMainLoop();  //Infinite loop
	return 1;
}

void init()
{
	font.load("Models/Font.bmp");
	skybox.load("Models/Skybox.obj");
	disk.load("Models/Disk.obj");
	disk_list = disk.getDisplayList(); //Copy information to VRAM

	player_obj.load("Models/Sagittarius.obj"); //Init player model
	player_model_list = player_obj.getDisplayList();

	crystal_obj.load("Models/Crystal.obj");
	crystal_list = crystal_obj.getDisplayList();

	for (unsigned int i = 0; i < LIVING_DRONES; i++) { //Init drone models
		drone_model.load("Models/Grapple.obj");
		drone_list = drone_model.getDisplayList();
	}

	for (unsigned int i = 0; i < LIVING_DRONES; i++) { //Init drone positions
		drone.push_back(Drone(player.getDronePosition(i), Vector3(30, 0, 0)));
	}

	srand((unsigned int)time(NULL));

	for (int i = 0; i < NUM_ASTEROID_MODELS; i++) { //Init asteroid models
		asteroids2Object[i].load(ASTEROID_PATHS[i]);
		for (unsigned int v = 0; v < asteroids2Object[i].getVertexCount(); v++) { //Set asteroid bumpiness
			Vector3 old_vertex = asteroids2Object[i].getVertexPosition(v);
			double randomVertices = random2(1, 6);
			double randomX = random2(.5, .9);
			double randomY = random2(.5, .9);
			double randomZ = random2(.5, .9);

			if(v % unsigned int(randomVertices) == 0)
			asteroids2Object[i].setVertexPosition(v,
				asteroids2Object[i].getVertexPosition(v).x * randomX,
				asteroids2Object[i].getVertexPosition(v).y * randomY,
				asteroids2Object[i].getVertexPosition(v).z * randomZ
			);
		}
		asteroids2_list[i] = asteroids2Object[i].getDisplayList(); //Assign each asteroid model to asteroid_list DisplayList [load to VRAM]
	}
	for (int i = 0; i < NUM_ASTEROIDS; i++) { //Init asteroids
		if (i == NUM_ASTEROIDS - 2) { //Collision asteroids for A4-A
			asteroids2[i].initAsteroid(Vector3(-1500, 200, -500), 300, 150, Vector3::getRandomUnitVector(), 0.0041);
			asteroids2[i].setVelocity(Vector3(0, 0, 30));
		}
		else if (i == NUM_ASTEROIDS - 1) {
			asteroids2[i].initAsteroid(Vector3(-1500, 200, 500), 150, 75, Vector3::getRandomUnitVector(), 0.0041);
			asteroids2[i].setVelocity(Vector3(0, 0, -30));
		}
		else {
			double outer_radius = random2(50, 400);
			double inner_ratio = random2(.1, .5);
			double inner_radius = outer_radius * inner_ratio;
			double distance = random2(2000, 8000);
			Vector3 position = Vector3::getRandomUnitVector() * distance;
			Vector3 rotation_axis;
			rotation_axis = rotation_axis.getRandomUnitVector();
			double rotation_rate = random2(0, 0.0041); //[0-.25] radians per seconds

			asteroids2[i].initAsteroid(position, outer_radius, inner_radius, rotation_axis, rotation_rate);
		}
		asteroids2[i].setDisplayListNumber(rand() % (NUM_ASTEROID_MODELS));
	}
	
	initFramesAndUpdates();
	initDisplay();
}

void initDisplay()
{
	glClearColor(0.5, 0.5, 0.5, 0.0);
	glColor3f(0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glutPostRedisplay();
}

void update()
{
	system_clock::time_point current_time = system_clock::now();
	for (unsigned int i = 0; i < MAXIMUM_UPDATES_PER_FRAME && next_update_time < current_time; i++)
	{
		double delta_time = SECONDS_PER_PHYSICS;
		doGameUpdates(delta_time);
		update_count++;
		next_update_time += DELTA_TIME;
		last_update_time = current_time;
		current_time = system_clock::now();
	}
	sleep(duration<double>(DELTA_TIME).count()); //Sleep based on desired update rate

	glutPostRedisplay();
}

void reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	window_width = w;
	window_height = h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)(w) / (GLdouble)(h), 1.0, 100000.0);
	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

void display()
{
	sleep(getSlowFrames()); //Slows framerate, default 0
	frame_count++; //Increment number of frames displayed

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//Clear the screen - any drawing before here will not display at all

	glLoadIdentity();

	drawCamera();
	drawSkybox();
	drawBlackHole();
	if(!GAME_OVER) {
		drawPlayerShip();
		player.drawFuturePath(SECONDS_PER_PHYSICS);
		drawDrones();
		for (unsigned int i = 0; i < LIVING_DRONES; i++) { //Draw drones
			drone[i].drawFuturePath(SECONDS_PER_PHYSICS);
			if (tToggled()) {
				drawDroneInfo(i);
			}
		}
	}
	drawAsteroids();
	drawOverlays();
	drawAccretionDisk(); //Draw last
	drawCrystals();

	// send the current image to the screen - any drawing after here will not display
	glutSwapBuffers();
}

void drawDroneInfo(unsigned int index) {
	glPushMatrix(); //Draw future drone position
		glColor3d(0.0, 1.0, 0.0);
		glTranslated(drone[index].future_position.x, drone[index].future_position.y, drone[index].future_position.z);
		glScaled(2, 2, 2);
		glutWireOctahedron();
	glPopMatrix();

	glPushMatrix(); //Draw relative escort position
		glColor3d(0.0, 1.0, 0.0);
		glTranslated(player.getDronePosition(index).x, player.getDronePosition(index).y, player.getDronePosition(index).z);
		glScaled(2, 2, 2);
		glutWireOctahedron();
	glPopMatrix();
}

void drawCrystals() {
	for (unsigned int i = 0; i < NUM_CRYSTALS_DRIFTING; i++) {
		glPushMatrix();
			glTranslated(crystal[i].getPosition().x, crystal[i].getPosition().y, crystal[i].getPosition().z);
			glScaled(crystal[i].getRadius(), crystal[i].getRadius(), crystal[i].getRadius());
			crystal_list.draw();
		glPopMatrix();
	}
}

void drawDrones() {
	for(unsigned int i = 0; i < LIVING_DRONES; i++) {
		glPushMatrix();
			glTranslated(drone[i].getPosition().x, drone[i].getPosition().y, drone[i].getPosition().z);
			glScaled(drone[i].getRadius(), drone[i].getRadius(), drone[i].getRadius());
			drone_list.draw();
		glPopMatrix();
	}
}

//Draws all asteroids
void drawAsteroids() {
	for (int index = 0; index < NUM_ASTEROIDS; index++) { //Draw asteroids
		glPushMatrix(); //Draw asteroid shape
			glColor3d(0.0, 0.0, 0.0);
			glTranslated(asteroids2[index].getPosition().x, asteroids2[index].getPosition().y, asteroids2[index].getPosition().z); //Set position from origin
			glScaled(asteroids2[index].getRadius(), asteroids2[index].getRadius(), asteroids2[index].getRadius());

			asteroids2[index].local_coords.applyDrawTransformations(); //Apply rotation
			asteroids2_list[asteroids2[index].getDisplayListNumber()].draw(); //Display asteroid model
		glPopMatrix();

		if (tToggled()) //T is toggled on
			drawAsteroidCoords(index);

		if (tToggled() && !asteroids2[index].safe) {
			glPushMatrix(); //Draw "too close" sphere around asteroid
				glColor3d(1.0, 1.0, 1.0);
				glTranslated(asteroids2[index].getPosition().x, asteroids2[index].getPosition().y, asteroids2[index].getPosition().z);
				glutWireSphere(asteroids2[index].getRadius() + 10, 30, 30);
			glPopMatrix();
		}
	}
}

void drawCamera() {
	Vector3 look_at; //Draw camera
	look_at.set(player.getPosition().x, player.getPosition().y, player.getPosition().z);// + player.local_coords.getForward(); //Set up look_at position of camera
	gluLookAt(
		player.getPosition().x + (player.local_coords.getForward().x * -20 + player.local_coords.getUp().x * 5), // (x, y, z) camera position
		player.getPosition().y + (player.local_coords.getForward().y * -20 + player.local_coords.getUp().y * 5),
		player.getPosition().z + (player.local_coords.getForward().z * -20 + player.local_coords.getUp().z * 5),
		player.getPosition().x + player.local_coords.getForward().x, //(x, y, z) camera look_at position
		player.getPosition().y + player.local_coords.getForward().y,
		player.getPosition().z + player.local_coords.getForward().z,
		player.local_coords.getUp().x, player.local_coords.getUp().y, player.local_coords.getUp().z //(x, y, z) camera up direction
	);
}

//Takes an index of the asteroids array and displays local coordinate axes
void drawAsteroidCoords(unsigned int index) {
	glPushMatrix();
		glTranslated(asteroids2[index].getPosition().x, asteroids2[index].getPosition().y, asteroids2[index].getPosition().z);
		asteroids2[index].local_coords.applyDrawTransformations();
		glBegin(GL_LINES);
		glColor3d(1.0, 0.0, 0.0);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(asteroids2[index].getRadius() * 2, 0.0, 0.0); //Size of axis is radius*2 so it's visible, but not too large (scales with asteroid size)
		glColor3d(0.0, 1.0, 0.0);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, asteroids2[index].getRadius() * 2, 0.0);
		glColor3d(0.0, 0.0, 1.0);
		glVertex3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, 0.0, asteroids2[index].getRadius() * 2);
		glEnd();
	glPopMatrix();
}

void drawSkybox() {
	glPushMatrix(); //Skybox matrix
		glTranslated(player.getPosition().x, player.getPosition().y, player.getPosition().z);
		glScaled(600.0, 600.0, 600.0); //60% of "far clipping plane", see gluPerspective above //REMOVE?
		glDepthMask(GL_FALSE); //disable writing to depth buffer, "since the depth buffer is not being used, the distance to skybox is recorded internally, so everything appears in front of it.
		skybox.draw();
		glDepthMask(GL_TRUE); //re-enable writing to depth buffer
	glPopMatrix();
}

void drawBlackHole() {
	glPushMatrix(); //Draw black hole at origin
		glTranslated(0.0, 0.0, 0.0);
		glColor3d(0.0, 0.0, 0.0);
		glutSolidSphere(50.0, 60, 60); //radius = 50m
	glPopMatrix();
}

void drawPlayerShip() {
	glPushMatrix(); //Draw player ship
		glTranslated(player.getPosition().x, player.getPosition().y, player.getPosition().z);
		glScaled(player.getRadius(), player.getRadius(), player.getRadius());
		glColor3d(1.0, 0.0, 0.0);
		player.local_coords.applyDrawTransformations();
		player_model_list.draw();
	glPopMatrix();
}

void drawAccretionDisk() {
	glPushMatrix(); //Draw accretion disk
		glTranslated(0.0, 0.0, 0.0);
		glScaled(10000, 1.0, 10000); //length/width = 10,000m
		disk_list.draw();
	glPopMatrix();
}

void initFramesAndUpdates() {
	start_time = system_clock::now(); //Start 'time game started'
	last_frame_time = start_time;
	next_update_time = start_time;
	last_update_time = start_time;
}

void doGameUpdates(double delta_time) {
	sleep(getSlowPhysics()); //Slows updates, default 0

	checkKeyPress(player);
	checkSpecialPress(player);

	if (getKnockCheck()) { //Knock crystals off near asteroid;
		setKnockCheck(false);

		for (int i = 0; i < NUM_ASTEROIDS; i++) { //Knock crystals check
			double distanceToAsteroid = player.getPosition().getDistance(asteroids2[i].getPosition());

			if (distanceToAsteroid < asteroids2[i].getRadius() + 200) { //Asteroid is close enough, knock crystals
				if (asteroids2[i].holdsCrystals()) { //Asteroid has crystals, knock
					asteroids2[i].knockCrystals(); //Flag no longer holds crystals
					for (int j = 0; j < 10; j++) { //Create 10 new crystals
						Vector3 direction_to_player = player.getPosition() - asteroids2[i].getPosition();
						direction_to_player.normalize();
						Vector3 position = Vector3(
							asteroids2[i].getPosition().x + asteroids2[i].getRadius() * direction_to_player.x,
							asteroids2[i].getPosition().y + asteroids2[i].getRadius() * direction_to_player.y,
							asteroids2[i].getPosition().z + asteroids2[i].getRadius() * direction_to_player.z
						);
						Vector3 velocity = Vector3::getRandomUnitVector() * 10;
						velocity += asteroids2[i].getVelocity();
						crystal.push_back(Crystal(position, velocity));
						NUM_CRYSTALS_DRIFTING++;
					}
				}
			}
		}
	}

	for (unsigned int i = 0; i < NUM_CRYSTALS_DRIFTING; i++) { //Check cystal->asteroid collisions
		for (unsigned int j = 0; j < NUM_ASTEROIDS; j++) {
			if (checkCrystalAstCollision(crystal[i], asteroids2[j])) {
				crystalBounce(crystal[i], asteroids2[j]);
			}
		}
	}

	for (unsigned int i = 0; i < NUM_CRYSTALS_DRIFTING; i++) { //Check player->crystal collisions
		if (checkPlayerCrystalCollision(crystal[i])) {
			crystal.erase(crystal.begin() + i);
			NUM_CRYSTALS_DRIFTING--;
			NUM_CRYSTALS_COLLECTED++;
		}
	}

	if (!GAME_OVER)
		player.update(delta_time);

	for (int i = 0; i < NUM_ASTEROIDS; i++) {
		asteroids2[i].multiplyDeltaTime(getDeltaMultiplier());
		asteroids2[i].update(delta_time);
	}

	for (int i = 0; i < NUM_ASTEROIDS; i++) { //Check asteroid->asteroid collisions
		for (int j = i + 1; j < NUM_ASTEROIDS; j++) {
			if (checkCollision(asteroids2[i], asteroids2[j])) { //Collision zone
				asteroidBounce(asteroids2[i], asteroids2[j]);
			}
		}
	}

	if (!GAME_OVER) {
		for (int i = 0; i < NUM_ASTEROIDS; i++) { //Check player->asteroid collisions
			if (checkPlayerCollision(asteroids2[i])) {
				gameOver();
				GAME_OVER = true;
			}
		}
	}

	for (unsigned int i = 0; i < NUM_CRYSTALS_DRIFTING; i++) { //Update crystals
		crystal[i].update(delta_time);
	}

	if (!GAME_OVER) {
		for (unsigned int i = 0; i < LIVING_DRONES; i++) { //Update drones
			if (checkDroneAstCollision(drone[i])) { //Check drone->asteroid collision and delete if collision
				drone.erase(drone.begin() + i);
				LIVING_DRONES--;
				for (unsigned int asteroid_index = 0; asteroid_index < NUM_ASTEROIDS; asteroid_index++) {
					asteroids2[asteroid_index].safe = true;
				}
				break;
			}
			else {
				for (unsigned int asteroid_index = 0; asteroid_index < NUM_ASTEROIDS; asteroid_index++) {
					if (checkDroneTooClose(i, asteroid_index)) { //Check if drone too close to asteroid
						asteroids2[asteroid_index].safe = false;
					}
					else {
						asteroids2[asteroid_index].safe = true;
					}
				}
				drone[i].update(delta_time);
				drone[i].AI(player.getDronePosition(i), player.getVelocity());
			}
		}
	}
}

void drawOverlays() //Draw frame and update rates to screen
{
	//Frames elapsed string setup
	stringstream frame_count_ss;
	frame_count_ss << "Frame count: " << frame_count;

	//Time elapsed string setup
	system_clock::time_point current_time = system_clock::now();
	double game_duration = duration<double>(current_time - start_time).count();
	stringstream game_time_ss;
	game_time_ss << "Game running time: " << game_duration;

	//Average frame rate string setup
	double average_frames_per_second = frame_count / game_duration;
	stringstream average_frame_rate_ss;
	average_frame_rate_ss << "Average frame rate: " << average_frames_per_second;

	//Calculate instantaneous frame rate ("How long most recent frame took to display")
	system_clock::duration frame_duration_1 = current_time - last_frame_time;
	if (frame_duration_1 < FRAME_TIME_MIN) //Ensure instantaneous frame rate is at least min frame rate
		frame_duration_1 = FRAME_TIME_MIN;
	double frame_duration = duration<double>(frame_duration_1).count();
	double instantaneous_frames_per_second = 1.0f / frame_duration;
	stringstream instantaneous_frame_rate_ss;
	instantaneous_frame_rate_ss << "Instantaneous frame rate: " << instantaneous_frames_per_second;

	//Calculate smoothed frame rate (instantaneous frame rate 'normalized' to not update as frequently)
	smoothed_frames_per_second = 0.95f * smoothed_frames_per_second + 0.05f * instantaneous_frames_per_second;
	stringstream smoothed_frame_rate_ss;
	smoothed_frame_rate_ss << "Smoothed frame rate: " << setprecision(3) << smoothed_frames_per_second;

	//Set up update count string
	stringstream update_count_ss;
	update_count_ss << "Update count: " << setprecision(3) << update_count;

	//Calculate average update rate
	double average_updates_per_second = update_count / game_duration;
	stringstream average_update_rate_ss;
	average_update_rate_ss << "Update rate: " << setprecision(3) << average_updates_per_second;

	//Calculate instantaneous update rate
	system_clock::duration update_duration_1 = current_time - last_update_time;
	if(update_duration_1 < UPDATE_TIME_MIN) //Ensure instantaneous frame rate is at least min frame rate
		update_duration_1 = UPDATE_TIME_MIN;
	double update_duration = duration<double>(update_duration_1).count();
	double instantaneous_update_per_second = 1.0f / update_duration;
	stringstream instantaneous_update_rate_ss;
	instantaneous_update_rate_ss << "Instantaneous update rate: " << setprecision(3) << instantaneous_update_per_second;

	//Calculate smoothed update rate (instantaneous update rate 'normalized' to not update as frequently)
	smoothed_updates_per_second = 0.95f * smoothed_updates_per_second + 0.05f * instantaneous_update_per_second;
	stringstream smoothed_update_rate_ss;
	smoothed_update_rate_ss << "Smoothed physics rate: " << setprecision(3) << smoothed_updates_per_second;

	stringstream game_over_ss;
	game_over_ss << "GAME OVER";

	stringstream num_crystals_drifting_ss;
	num_crystals_drifting_ss << "Crystals drifting: " << NUM_CRYSTALS_DRIFTING;

	stringstream num_crystals_collected_ss;
	num_crystals_collected_ss << "Crystals collected: " << NUM_CRYSTALS_COLLECTED;

	stringstream living_drones_ss;
	living_drones_ss << "Living drones: " << LIVING_DRONES;

	SpriteFont::setUp2dView(window_width, window_height);
		//font.draw(frame_count_ss.str(), 16, 48); //Display elapsed frames
		//font.draw(game_time_ss.str(), 16, 16);	//Display elapsed time
		//font.draw(average_frame_rate_ss.str(), 16, 72); //Display average frame rate
		//font.draw(instantaneous_frame_rate_ss.str(), 16, 96); //Display instantaneous frame rate
		last_frame_time = current_time;
		font.draw(smoothed_frame_rate_ss.str(), 5, 10); //Display smoothed frame rate
		//font.draw(update_count_ss.str(), 16, 152); //Display number of updates
		//font.draw(average_update_rate_ss.str(), 5, 40); //Display average update rate
		//font.draw(instantaneous_update_rate_ss.str(), 5, 40); //Display instantenous update rate
		font.draw(smoothed_update_rate_ss.str(), 5, 40); //Display smoothed update rate
		font.draw(num_crystals_drifting_ss.str(), 5, 70);
		font.draw(num_crystals_collected_ss.str(), 5, 100);
		font.draw(living_drones_ss.str(), 5, 130);
		if (GAME_OVER) {
			font.draw(game_over_ss.str(), window_width/2 - 50, window_height/2 -10);
		}
	SpriteFont::unsetUp2dView();
}

bool checkCollision(Asteroid2 ast1, Asteroid2 ast2) {
	double collision_distance = 
		pow((ast1.getPosition().x - ast2.getPosition().x), 2)
		+ pow((ast1.getPosition().y - ast2.getPosition().y), 2)
		+ pow((ast1.getPosition().z - ast2.getPosition().z), 2);

	return(collision_distance < pow((ast1.getRadius() + ast2.getRadius()), 2));
}

bool checkPlayerCollision(Asteroid2 ast) {
	double collision_distance =
		pow((player.getPosition().x - ast.getPosition().x), 2)
		+ pow((player.getPosition().y - ast.getPosition().y), 2)
		+ pow((player.getPosition().z - ast.getPosition().z), 2);

	return(collision_distance < pow((player.getRadius() + ast.getRadius()), 2));
}

void asteroidBounce(Asteroid2& ast1, Asteroid2& ast2) {
	Vector3 direction1to2 = ast2.getPosition() - ast1.getPosition();
	Vector3 average_velocity = (ast1.getMass() * ast1.getVelocity() + ast2.getMass() * ast2.getVelocity()) / (ast1.getMass() + ast2.getMass());
	Vector3 momentum_transfer1 = (ast1.getVelocity() - average_velocity).getProjectionSafe(direction1to2) * ast1.getMass();
	Vector3 momentum_transfer2 = (ast2.getVelocity() - average_velocity).getProjectionSafe(direction1to2) * ast2.getMass();
	if (momentum_transfer1.dotProduct(direction1to2) <= 0) //Moving towards each other, bounce
		return;

	ast1.setVelocity(average_velocity + (momentum_transfer2 / ast1.getMass()));
	ast2.setVelocity(average_velocity + (momentum_transfer1 / ast2.getMass()));
}

bool checkCrystalAstCollision(Crystal cryst, Asteroid2 ast) {
	double collision_distance =
		pow((cryst.getPosition().x - ast.getPosition().x), 2)
		+ pow((cryst.getPosition().y - ast.getPosition().y), 2)
		+ pow((cryst.getPosition().z - ast.getPosition().z), 2);

	return(collision_distance < pow((player.getRadius() + ast.getRadius()), 2));
}

void crystalBounce(Crystal& cryst, Asteroid2& ast) {
	Vector3 directionCrystToAst = ast.getPosition() - cryst.getPosition();
	Vector3 average_velocity = (cryst.getMass() * cryst.getVelocity() + ast.getMass() * ast.getVelocity()) / (cryst.getMass() + ast.getMass());
	Vector3 momentum_transfer1 = (cryst.getVelocity() - average_velocity).getProjectionSafe(directionCrystToAst) * cryst.getMass();
	Vector3 momentum_transfer2 = (ast.getVelocity() - average_velocity).getProjectionSafe(directionCrystToAst) * ast.getMass();
	if (momentum_transfer1.dotProduct(directionCrystToAst) <= 0) //Moving towards each other, bounce
		return;

	cryst.setVelocity(average_velocity + (momentum_transfer2 / cryst.getMass()));
	ast.setVelocity(average_velocity + (momentum_transfer1 / ast.getMass()));
}

bool checkPlayerCrystalCollision(Crystal cryst) {
	double collision_distance =
		pow((player.getPosition().x - cryst.getPosition().x), 2)
		+ pow((player.getPosition().y - cryst.getPosition().y), 2)
		+ pow((player.getPosition().z - cryst.getPosition().z), 2);

	return(collision_distance < pow((player.getRadius() + cryst.getRadius()), 2));
}

bool checkDroneAstCollision(Drone drone_check) {
	for (unsigned int i = 0; i < NUM_ASTEROIDS; i++) {
		double collision_distance =
			pow((drone_check.getPosition().x - asteroids2[i].getPosition().x), 2)
			+ pow((drone_check.getPosition().y - asteroids2[i].getPosition().y), 2)
			+ pow((drone_check.getPosition().z - asteroids2[i].getPosition().z), 2);

		if (collision_distance < pow((drone_check.getRadius() + asteroids2[i].getRadius()), 2))
			return true;
	}
	return false;
}

bool checkDroneTooClose(unsigned int index, unsigned int ast_index) {
	//ra + 10 * ||vd - va|| / ad
	double safe_distance;
	Vector3 combined_velocity = drone[index].getVelocity() - asteroids2[ast_index].getVelocity();
	safe_distance = (asteroids2[ast_index].getRadius() + 10 * combined_velocity.getNorm()) / 4;
	if (drone[index].getPosition().getDistance(asteroids2[ast_index].getPosition()) < safe_distance)
		return true; //Drone is too close to asteroid[i]

	return false; //Drone is not too close to any asteroid
}