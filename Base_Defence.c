/*----------------------------------------------------------------------------*/
/* Include files                                                              */
/*----------------------------------------------------------------------------*/
#include <ansi_c.h>
#include <cvirte.h>
#include <userint.h>
#include "Base_Defence.h"
#include "bass.h"
#include "TalGameLibrary.h"

/*----------------------------------------------------------------------------*/
/* Defines                                                                    */
/*----------------------------------------------------------------------------*/
//sounds
HSTREAM sound_SND[30];
#define DEFAULT_VOLUME 0.1
#define CLICK_SOUND 11

#define DEV 0

#define CANVAS_HEIGHT 760
#define CANVAS_WIDTH 1200

#define dt 0.000001
#define TIMER_TICK 0.02

#define START_LEVEL 1
#define WAVE_DELAY 9

#define SMALL_TOWER_POWER 1
#define SMALL_TOWER_RANGE 70
#define SMALL_TOWER_COST 100

#define MEDIUM_TOWER_POWER 1
#define MEDIUM_TOWER_RANGE 90
#define MEDIUM_TOWER_COST 400

#define LARGE_TOWER_POWER 2
#define LARGE_TOWER_RANGE 60
#define LARGE_TOWER_COST 600

#define MISSILE_TOWER_POWER 4
#define MISSILE_TOWER_RANGE 50
#define MISSILE_TOWER_COST 10000

#define CENTER_Y 85
#define CENTER_X 35

#define ENEMY1_SLOWER 3
#define ENEMY1_VELOCITY 1000//500
#define ENEMY1_HEALTH 190

#define ENEMY2_SLOWER 5
#define ENEMY2_VELOCITY 1200//600
#define ENEMY2_HEALTH 250

#define ENEMY3_SLOWER 5
#define ENEMY3_VELOCITY 650
#define ENEMY3_HEALTH 500

/*----------------------------------------------------------------------------*/
/* 						Module-globals                                        */
/*----------------------------------------------------------------------------*/

static int panelHandle, wait_panelHandle, name_panelHandle, leaderboard_panelHandle, info_panelHandle, dev_panelHandle;
FILE *fp;

// counters
int time_counter=0, levelTimer, secCounter=0;

//images
int welcome, logo, worlds, map1, map11, sidebar; 

//general variables
int level, levelStart, startup;
int mX, mY, l_pressed=0, drag, dragflag=0;

//sounds
int numOfSounds=16, playOnceVictory=1;

//_______________________player_________________________________//
int numOfPlayers, newHighScore=0;
typedef struct
{
	int score;
	char name[100];
	char date[50];
	char time[50];
} players;
players player[6], nowPlayer;

//_______________________map_________________________________//
// map data file
char waypointfile[300];
//maps waypoints arrays
int waypointsmap1[17][2];
int numberofWP;
//tiles - eligeble ground for placing tower
int numOfMapTiles;
int Map_Top_L, Map_Top_Y;
int tileHight, tileWidth;
int tileGap;
int tileOK;
int tileNum;
//int tileAvailability[][];

typedef struct
{
	Rect box;
	Point center;
	int state;
	int ok;
} tiles;
tiles tile[1000];



//_______________________towers_________________________________//
int numOfTowers=0;
typedef struct
{
	int pic, picBlu, picRed, TL_pic;
	int x, y; // the coordinates of the pivot
	int width, hight;
	Point pivot;
	int drag;
	int power, fireRate, range, cost;
	Rect box, rangeBox, rangeCircle;
} towerTypes;
towerTypes towType[4]; 

typedef struct
{
	int type;
	int pic, picBlu, picRed, TL_pic;
	int x, y; // the coordinates of the pivot
	double angle;
	int width, hight;
	int place, onGround;
	int enemyInRange, targetLock, fire;
	int power, fireRate, range, cost;
	Point target, origin;
	Rect box, rangeBox, rangeCircle;
} towers;
towers tower[30];
int radius=20;


//_______________________enemies_________________________________//
int x=CANVAS_WIDTH/2, y=CANVAS_HEIGHT/2, width=50, hight=50; //Test enemy
int numOfEnemies, enemiesLeft;
int level_enemies[2][1000]={0};
typedef struct
{
	//position & speed
	double posX, posY, Xa, Xb, Ya, Yb;
	double vector, vectorX, vectorY;
	int velocity;
	double vX, vY;
	int waypoint, nextWp;
	//image
	int type;
	int pic, pic1, pic2, pic3, pic1Left, pic2Left, pic3Left;
	int hight, width, frame_count, slower;
	int anim_state;
	//player state
	int alive, dead, health, startDelay, goFlag, underBrige;	
	Rect box, rangeBox;
} enemies;
enemies enemy[1000];

//______________________Sprites_________________________//
typedef struct
{
	int hight, width;
	int frame_count, anim_state, frame;
	int state;
	int slower;
	int pic, TL_pic;
} sprites;
sprites enemy_type[6], explosion;


/*----------------------------------------------------------------------------*/
/* 					Internal function prototypes                              */
/*----------------------------------------------------------------------------*/
void checkTowerPlacement();
int intersect(int turret, i);
void setTower(int i);
void towTypeReset();
void setEnemies();
void move_enemies();
void tower_move();
void update();
void draw();
void checkScore();
void endOfGameScoreCheck();
void sortLeaderboard();
void playLaserInUpdate();
void resetGame();
void terminate();
//________________________initialization_________________
void setwaypoints();
//****forTest****
void markwaypoints();
void markground();
//***************
void loadLeaderBoard();
void loadLevel(int level);
void loadTowers();
void loadAnimations();
void loadMap();
void loadSounds();
void initialize()
{
	if (startup==0)
		DisplayPanel (wait_panelHandle);
	BASS_Init( -1,44100, 0,0,NULL);
	loadSounds();
	BASS_ChannelPlay(sound_SND[0],TRUE);

	GetBitmapFromFile ("Images\\map_grass.png",&map1); //1200*760
	GetBitmapFromFile ("Images\\map_grass_bridge.png",&map11); //1200*760
	GetBitmapFromFile ("Images\\sidebar.png",&sidebar); //1200*760
	
	loadLeaderBoard();
	loadTowers();
	loadAnimations();
	loadMap();
	level=START_LEVEL;
	loadLevel(level);
	setEnemies();
	levelStart=0;
	levelTimer=0;
	startup=1;
	nowPlayer.score=SMALL_TOWER_COST;
	HidePanel(wait_panelHandle);
}


/*----------------------------------------------------------------------------*/
/* 							MAIN and UIR									  */
/*----------------------------------------------------------------------------*/
int main (int argc, char *argv[])
{
	srand(time(0));
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "Base_Defence.uir", PANEL)) < 0)
		return -1;
	if ((wait_panelHandle = LoadPanel (0, "Base_Defence.uir", WAIT_PANEL)) < 0)
		return -1;
	if ((leaderboard_panelHandle = LoadPanel (0, "Base_Defence.uir", LEAD_PANEL)) < 0)
		return -1;
	if ((name_panelHandle = LoadPanel (0, "Base_Defence.uir", NAME_PANEL)) < 0)
		return -1;
	if ((info_panelHandle = LoadPanel (0, "Base_Defence.uir", INFO_PANEL)) < 0)
		return -1;
	if ((dev_panelHandle = LoadPanel (0, "Base_Defence.uir", DEV_PANEL)) < 0)
		return -1;
	
	initialize();
	DisplayPanel (panelHandle);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	DiscardPanel (wait_panelHandle);
	DiscardPanel (leaderboard_panelHandle);
	DiscardPanel (name_panelHandle);
	DiscardPanel (info_panelHandle);
	DiscardPanel (dev_panelHandle);
	return 0;
}
int CVICALLBACK TimerFuncSec (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_TIMER_TICK:
			secCounter++;
			if (startup==1 && secCounter==2)
			{
				SetCtrlAttribute (panelHandle, PANEL_PICTURE_LOGO, ATTR_VISIBLE, 0);
				SetCtrlAttribute (panelHandle, PANEL_START_BUTTON, ATTR_VISIBLE, 1);
				SetCtrlAttribute (panelHandle, PANEL_PICTURE_METAL_FRAME, ATTR_VISIBLE, 1);
				SetCtrlAttribute (panelHandle, PANEL_PICTURE_METAL_FRAME_2, ATTR_VISIBLE, 1);
				SetCtrlAttribute (panelHandle, PANEL_VOL_SHOW_BUTTON, ATTR_VISIBLE, 1);
				SetCtrlAttribute (panelHandle, PANEL_LEAD_BUTTON, ATTR_VISIBLE, 1);
				SetCtrlAttribute (panelHandle, PANEL_INFO_BUTTON, ATTR_VISIBLE, 1);
				SetCtrlAttribute (panelHandle, PANEL_EXIT_BUTTON, ATTR_VISIBLE, 1);
				startup=0;
			}
			if (levelStart)
			{
				levelTimer++;
				checkScore(nowPlayer.score);
				SetCtrlVal (panelHandle, PANEL_LEVEL_TEXT, level);
				switch (WAVE_DELAY-levelTimer)
				{
					case 1:
						//stop
						BASS_ChannelStop(sound_SND[1]);
						//play
						BASS_ChannelPlay(sound_SND[9],TRUE);
						//hide
						SetCtrlAttribute (panelHandle, PANEL_WAVE_COUNTDOWN, ATTR_VISIBLE, 0);
						//show
						SetCtrlAttribute (panelHandle, PANEL_ENEMIES_COMING, ATTR_VISIBLE, 1);
						break;
					case 0:
						//play
						BASS_ChannelPlay(sound_SND[7],TRUE);
						BASS_ChannelPlay(sound_SND[2],TRUE);
						break;
					case -1:
						//hide
						SetCtrlAttribute (panelHandle, PANEL_ENEMIES_COMING, ATTR_VISIBLE, 0);
						break;
					default:
						break;
				}
			}
			break;
	}
	return 0;
}
int CVICALLBACK TimerFunc (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_TIMER_TICK:
			time_counter++;
			update();
			draw();	
			break;
	}
	return 0;
}
void update()
{
	//update enemy position
	if (levelTimer>=WAVE_DELAY && levelStart)
	{
		//change animation frame & check if enemy in range
		for (int i=0; i<=numOfEnemies-1; i++)
		{
			//change animation frame
			if ( (time_counter%enemy[i].slower) ==1)
				enemy[i].anim_state++;
			if ( (time_counter%explosion.slower) ==1)
				explosion.anim_state++;
			
			
			if (levelTimer>=(enemy[i].startDelay+WAVE_DELAY)  && enemy[i].goFlag==0)
			{
				enemy[i].goFlag=1;
				BASS_ChannelPlay(sound_SND[rand()%2+15],TRUE);
			}
			else if (i==0 && enemy[i].goFlag==0)
			{
				enemy[i].goFlag=1;
				BASS_ChannelPlay(sound_SND[rand()%2+15],TRUE);
			}
			
			
			if (enemy[i].goFlag)
			{
				if (enemy[i].nextWp)
				{
					//position
					enemy[i].Xa = enemy[i].posX = waypointsmap1[enemy[i].waypoint][0];
					enemy[i].Ya = enemy[i].posY = waypointsmap1[enemy[i].waypoint][1];
					enemy[i].Xb=waypointsmap1[enemy[i].waypoint+1][0];
					enemy[i].Yb=waypointsmap1[enemy[i].waypoint+1][1];
					//calc relative vector
					enemy[i].vectorX = enemy[i].Xb - enemy[i].Xa;
					enemy[i].vectorY = enemy[i].Yb - enemy[i].Ya;
					//vector speed
					enemy[i].vector=sqrt( pow(enemy[i].vectorX, 2) + pow(enemy[i].vectorY, 2) );
					enemy[i].vX=( enemy[i].velocity * (enemy[i].vectorX/enemy[i].vector));
					enemy[i].vY=( enemy[i].velocity * (enemy[i].vectorY/enemy[i].vector));

					enemy[i].nextWp = 0;
				}

				//update live position with high resolution
				for (int j=0; j<1000; j++)
				{
					enemy[i].posX += enemy[i].vX*dt;
					enemy[i].posY += enemy[i].vY*dt;
					enemy[i].vectorX = enemy[i].Xb - enemy[i].posX;
					enemy[i].vectorY = enemy[i].Yb - enemy[i].posY;
				}
				//make enemy bounding box
				enemy[i].rangeBox = enemy[i].box = MakeRect(enemy[i].posY-CENTER_Y , enemy[i].posX-CENTER_X , 100, 80);
				RectGrow (&enemy[i].rangeBox, -20, -12);
				
				//change next WP when it gets to current WP
				if (enemy[i].posX >= enemy[i].Xb - 7 &&
						enemy[i].posX <= enemy[i].Xb + 7 &&
						enemy[i].posY >= enemy[i].Yb - 7 &&
						enemy[i].posY <= enemy[i].Yb + 7 )
				{
					if (enemy[i].waypoint==numberofWP-1)
					{
						enemy[i].velocity=0;
						BASS_ChannelPlay(sound_SND[6],TRUE);
						SetCtrlAttribute (panelHandle, PANEL_DEFEAT, ATTR_VISIBLE, 1);
						SetCtrlAttribute (panelHandle, PANEL_DIMM, ATTR_VISIBLE, 1);
						SetCtrlAttribute (panelHandle, PANEL_BACK_BUTTON, ATTR_VISIBLE, 1);
						levelStart=0;
					}
					enemy[i].nextWp=TRUE;
					enemy[i].waypoint++;
				}
				//check if in range
				for (int j=0; j<=numOfTowers; j++)
				{
					/*if (i==tower[j].enemyInRange)
					{
						if (!enemy[i].alive)
							tower[j].enemyInRange=numOfEnemies+4;
					}*/
					if (tower[j].fire==0 && enemy[i].alive==1)
					{
						if(RectIntersection (enemy[i].rangeBox , tower[j].rangeBox , 0))
						{
							tower[j].enemyInRange=i;
							tower[j].fire = TRUE;
							playLaserInUpdate();
							tower[j].target = MakePoint(
												  enemy[i].posX ,
												  enemy[i].posY-30 );
							tower[j].angle=TL_GetAngleFromPoints(tower[j].origin, tower[j].target );
						}
					}
					else if (i == tower[j].enemyInRange)
					{
						if (enemy[i].alive==0)
						{
							tower[j].fire=FALSE;
							tower[j].enemyInRange=numOfEnemies+4;
							BASS_ChannelStop(sound_SND[10]);
						}
						//no  intersection
						else if (!RectIntersection (enemy[i].rangeBox, tower[j].rangeBox , 0))
						{
							tower[j].fire=FALSE;
							tower[j].enemyInRange=numOfEnemies+4;
							BASS_ChannelStop(sound_SND[10]);
						}
						//intersection -fire=true
						else
						{
							playLaserInUpdate();
							tower[j].target = MakePoint(
												  enemy[i].posX ,
												  enemy[i].posY-30 );
							tower[j].angle=TL_GetAngleFromPoints(tower[j].origin, tower[j].target );
							enemy[i].health -= tower[j].power;
							if (enemy[i].health<=0 && enemy[i].alive)
							{
								enemy[i].alive=0;
								tower[j].fire=FALSE;
								tower[j].enemyInRange=numOfEnemies+4;
								BASS_ChannelStop(sound_SND[10]);
								enemiesLeft--;
								nowPlayer.score+=100;
								SetCtrlVal (panelHandle, PANEL_SCORE, nowPlayer.score);
							}
						}
					}
				}
			}
			if (enemy[i].alive==0 && enemy[i].vY!=0)
			{
				BASS_ChannelPlay(sound_SND[14],TRUE);
				enemy[i].vX=enemy[i].vY=0;
			}
			if (!enemiesLeft && playOnceVictory)
			{
				BASS_ChannelStop(sound_SND[2]);
				BASS_ChannelStop(sound_SND[10]);
				if ((time_counter%100)==1)
				{
					BASS_ChannelPlay(sound_SND[5],TRUE);
					playOnceVictory = 0;
				}
				SetCtrlAttribute (panelHandle, PANEL_VICTORY, ATTR_VISIBLE, 1);
				SetCtrlAttribute (panelHandle, PANEL_DIMM, ATTR_VISIBLE, 1);
				SetCtrlAttribute (panelHandle, PANEL_BACK_BUTTON, ATTR_VISIBLE, 1);
				SetCtrlAttribute (panelHandle, PANEL_CONTINUE_BUTTON, ATTR_VISIBLE, 1);
			}
		}
	}
	else
	{
		if ((WAVE_DELAY-levelTimer)>1)
			SetCtrlAttribute (panelHandle, PANEL_WAVE_COUNTDOWN, ATTR_VISIBLE, 1);
		SetCtrlVal (panelHandle, PANEL_WAVE_COUNTDOWN, WAVE_DELAY-levelTimer);
	}
}
void draw()
{
	CanvasStartBatchDraw (panelHandle, PANEL_CANVAS);
	CanvasClear (panelHandle, PANEL_CANVAS, VAL_ENTIRE_OBJECT);
	//draw Map
	CanvasDrawBitmap (panelHandle, PANEL_CANVAS, map1, VAL_ENTIRE_OBJECT,MakeRect (0, 0, CANVAS_HEIGHT, CANVAS_WIDTH));
	CanvasDrawBitmap (panelHandle, PANEL_CANVAS, sidebar, VAL_ENTIRE_OBJECT,MakeRect (0, 1200, CANVAS_HEIGHT, 250));
	//draw enemies
	
	//loop accending
	for (int i=numOfEnemies-1; i>=0; i--)
	{
		if (enemy[i].alive)
		{
			CanvasDrawBitmap (panelHandle, PANEL_CANVAS, enemy[i].pic,
							  MakeRect( 0,
										(enemy[i].anim_state % enemy[i].frame_count) * enemy[i].width ,
										enemy[i].hight ,
										enemy[i].width),
							  enemy[i].box);
			//Map bridge
			if (enemy[i].waypoint==3)
				CanvasDrawBitmap (panelHandle, PANEL_CANVAS, map11, VAL_ENTIRE_OBJECT,MakeRect (0, 0, CANVAS_HEIGHT, CANVAS_WIDTH));
		}
		
	}
	//loop decending
	for (int i=0; i<=numOfEnemies-1; i++)
	{
		if (!enemy[i].alive && !enemy[i].dead)
		{	
			CanvasDrawBitmap (panelHandle, PANEL_CANVAS, explosion.pic,
							  MakeRect(0 ,
									  (explosion.anim_state) * explosion.width,
									  explosion.hight,
									  explosion.width),
							 enemy[i].box);
			explosion.frame++;
			if (explosion.frame==14)
			{
				enemy[i].dead=1;
				explosion.frame=0;
			}
		}
		else if (enemy[i].alive)
		{	
			CanvasDrawBitmap (panelHandle, PANEL_CANVAS, enemy[i].pic,
								  MakeRect( 0,
											(enemy[i].anim_state % enemy[i].frame_count) * enemy[i].width ,
											enemy[i].hight ,
											enemy[i].width),
								  enemy[i].box);
			
			if(DEV)
				CanvasDrawRect (panelHandle, PANEL_CANVAS, enemy[i].rangeBox, VAL_DRAW_FRAME);
			//Map bridge
			if (enemy[i].waypoint==9)
				CanvasDrawBitmap (panelHandle, PANEL_CANVAS, map11, VAL_ENTIRE_OBJECT,MakeRect (0, 0, CANVAS_HEIGHT, CANVAS_WIDTH));
		}
		if (DEV)
			CanvasDrawOval (panelHandle, PANEL_CANVAS, MakeRect(enemy[i].posY, enemy[i].posX, 10,10), VAL_DRAW_INTERIOR);
	}


	//draw towers with rotation
	for (int i=0; i<=numOfTowers; i++)
	{
		//rotating towers
		if (tower[i].type<=2)
		{
			TL_CanvasDrawBitmap(panelHandle, PANEL_CANVAS, tower[i].TL_pic, tower[i].origin, tower[i].angle);
		}
		//missile tower
		else
		{
			CanvasDrawBitmap (panelHandle, PANEL_CANVAS, tower[i].pic, VAL_ENTIRE_OBJECT,tower[i].box);
		}
		SetCtrlAttribute (panelHandle, PANEL_CANVAS, ATTR_PEN_COLOR, VAL_GRAY);
		SetCtrlAttribute (panelHandle, PANEL_CANVAS, ATTR_PEN_STYLE, VAL_DOT);
		SetCtrlAttribute (panelHandle, PANEL_CANVAS, ATTR_PEN_WIDTH, 1);
		CanvasDrawOval (panelHandle, PANEL_CANVAS, tower[i].rangeCircle ,VAL_DRAW_FRAME);
		if (tower[i].fire)
		{
			SetCtrlAttribute (panelHandle, PANEL_CANVAS, ATTR_PEN_COLOR, VAL_RED);
			SetCtrlAttribute (panelHandle, PANEL_CANVAS, ATTR_PEN_WIDTH, 3);
			CanvasDrawLine (panelHandle, PANEL_CANVAS, MakePoint(tower[i].x, tower[i].y), tower[i].target);
		}
		
	}
	//tower bank
	for (int i=0; i<=3; i++)
	{
		CanvasDrawBitmap (panelHandle, PANEL_CANVAS, towType[i].pic, VAL_ENTIRE_OBJECT,MakeRect (
							  towType[i].y-towType[i].pivot.y ,
							  towType[i].x-towType[i].width/2 ,
							  towType[i].hight ,
							  towType[i].width));
		if (dragflag)
		{
		CanvasDrawOval (panelHandle, PANEL_CANVAS, MakeRect(
							towType[drag].y-towType[drag].range-radius ,
							towType[drag].x-towType[drag].range-radius ,
							(towType[drag].range+radius)*2 ,
							(towType[drag].range+radius)*2 ),
						VAL_DRAW_FRAME);
		MakeRect( towType[drag].y-towType[drag].range ,
							   towType[drag].x-towType[drag].range ,
							   towType[drag].range*2 ,
							   tower[drag].range*2);
		}
	}
	
	//if user is dev 1 for tests
	if (DEV)
	{
		//color tiles
		for (int i=0; i<=numOfMapTiles; i++)
		{
			if (tile[i].state==2)
			{
				SetCtrlAttribute (panelHandle, PANEL_CANVAS, ATTR_PEN_COLOR, VAL_GREEN);
				CanvasDrawRect (panelHandle, PANEL_CANVAS, tile[i].box, VAL_DRAW_FRAME);
			}
		}
		for (int i=0; i<=3; i++)
		{
			CanvasDrawRect (panelHandle, PANEL_CANVAS, towType[i].box, VAL_DRAW_FRAME);
		}
		//draw waypoints and rectangles
		SetCtrlAttribute (panelHandle, PANEL_CANVAS, ATTR_PEN_FILL_COLOR, VAL_RED);
		markwaypoints();
		markground();
	}
	
	CanvasEndBatchDraw (panelHandle, PANEL_CANVAS);
}
int CVICALLBACK canvasFunc (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int freeTiles=0;
	switch (event)
	{
		case EVENT_LEFT_CLICK:
			GetRelativeMouseState (panelHandle, PANEL_CANVAS, &mX, &mY, &l_pressed,0,0);
			if (dragflag==0)
			{
				for (int i=0; i<=3; i++)
				{
					if (RectContainsPoint (towType[i].box , MakePoint(mX,mY)))
					{
						drag=i;
						dragflag=1;
					}
				}
			}	
			
		case EVENT_MOUSE_POINTER_MOVE:
			GetRelativeMouseState (panelHandle, PANEL_CANVAS, &mX, &mY, &l_pressed,0,0);
			if (l_pressed && dragflag)
			{
				towType[drag].x=mX;
				towType[drag].y=mY;
				for (int i=0; i<=numOfMapTiles; i++)
				{
					if(RectContainsPoint (tile[i].box,MakePoint(mX,mY)) && tile[i].state!=1)
					{
						tile[i].state=2;
						if (tile[i].ok)
						{
							towType[drag].pic=towType[drag].picBlu;
							tileOK=1;
							tileNum=i;
						}
						else
						{
							towType[drag].pic=towType[drag].picRed;
							tileOK=0;
						}
						break;
					}
					else
					{
						tile[i].state=0;
						freeTiles++;
					}
				}
			}
			else
				dragflag=0;
				if (tileOK && l_pressed==0 )
				{
					towTypeReset();
					setTower(drag);
					tileNum=0;
					tileOK=0;
					drag=0;
				}
				else if (tileOK==0 && l_pressed==0 )
				{
					towTypeReset();
				}
	}
	return 0;
}

/*----------------------------------------------------------------------------*/
/*                       TOWERS & ENEMIES									  */
/*----------------------------------------------------------------------------*/
void setTower(int i)
{
	tower[numOfTowers].type=i;
	tower[numOfTowers].TL_pic=towType[i].TL_pic;
	tower[numOfTowers].pic=towType[i].pic;
	tower[numOfTowers].origin = MakePoint (tile[tileNum].center.x, tile[tileNum].center.y);
	tower[numOfTowers].x=tile[tileNum].center.x;
	tower[numOfTowers].y=tile[tileNum].center.y;
	tower[numOfTowers].power=towType[i].power;
	tower[numOfTowers].fireRate=towType[i].fireRate;
	tower[numOfTowers].range=towType[i].range;
	tower[numOfTowers].box=towType[i].box;
	tower[numOfTowers].rangeBox=towType[i].rangeBox;
	tower[numOfTowers].cost=towType[i].cost;
	tower[numOfTowers].place=0;
	tower[numOfTowers].enemyInRange=0;
	tower[numOfTowers].fire=0;
	tile[tileNum].state=1;
	tile[tileNum].ok=0;
	tower[numOfTowers].rangeCircle =
		tower[numOfTowers].rangeBox =
			MakeRect( tower[numOfTowers].y-tower[numOfTowers].range ,
							   tower[numOfTowers].x-tower[numOfTowers].range ,
							   tower[numOfTowers].range*2 ,
							   tower[numOfTowers].range*2);
	RectGrow (&tower[numOfTowers].rangeCircle,
			  tower[numOfTowers].range/3 ,
			  tower[numOfTowers].range/3);
	nowPlayer.score-=towType[i].cost;
	SetCtrlVal (panelHandle, PANEL_SCORE, nowPlayer.score);
	numOfTowers++;
}
void towTypeReset()
{
	towType[0].x=towType[1].x=towType[2].x=towType[3].x=1255; 	//initial locations
	towType[0].y=156;
	towType[1].y=292;
	towType[2].y=440;
	towType[3].y=570;
	for (int i=0; i<=3; i++)
	{
		towType[i].pic=towType[i].picBlu;
	}
}
void loadAnimations()
{
	// load enemy pics using loop
	char enemy_pic_path[200];		
	for(int i=0; i<=5; i++)
	{
		sprintf(enemy_pic_path,"Images\\enemy%d.png",i);
		GetBitmapFromFile(enemy_pic_path, &enemy_type[i].pic);
		enemy_type[i].hight=500;
		enemy_type[i].width=400;
		enemy_type[i].state=0;
	}
	enemy_type[0].frame_count = 10;
	enemy_type[0].slower = 1;
	
	enemy_type[2].frame_count = 8;
	enemy_type[2].slower = 5;
	
	enemy_type[4].frame_count = 9;
	enemy_type[4].slower = 5;

	GetBitmapFromFile("Images\\explosion.png",&explosion.pic);
	explosion.anim_state=0;
	explosion.frame_count=14;
	explosion.hight=70;
	explosion.width=100;
	explosion.slower=4;
	explosion.frame=0;
}
void setEnemies()
{
	for (int i=0; i<numOfEnemies; i++)
	{
		enemy[i].type=(rand()%3)+1;
		if (i>=1)
		{
			if (i>=2)
				while (enemy[i].type==enemy[i-1].type || enemy[i].type==enemy[i-2].type)
				{
					enemy[i].type=(rand()%3)+1;
				}
			else
				while (enemy[i].type==enemy[i-1].type)
				{
					enemy[i].type=(rand()%3)+1;
				}
		}
		else 
			enemy[i].type=1;
		
		enemy[i].hight = 500;
		enemy[i].width=400;
		enemy[i].anim_state=0;
		enemy[i].posX = 0;
		enemy[i].posY = 0;
		enemy[i].nextWp=1;
		enemy[i].waypoint=0;
		enemy[i].alive=1;
		enemy[i].dead=0;
		enemy[i].startDelay=(rand()%6)+2+(i*2);
		enemy[i].goFlag=0;
		switch (enemy[i].type)
		{
			case 1:
				enemy[i].pic = enemy_type[0].pic;
				enemy[i].frame_count = 10;
				enemy[i].slower = ENEMY1_SLOWER;
				enemy[i].velocity=ENEMY1_VELOCITY;
				enemy[i].health=ENEMY1_HEALTH;		
				
				break;
			case 2:
				enemy[i].pic = enemy_type[2].pic;
				enemy[i].frame_count = 8;
				enemy[i].slower = ENEMY2_SLOWER;
				enemy[i].velocity=ENEMY2_VELOCITY;
				enemy[i].health=ENEMY2_HEALTH;

				break;
			case 3:
				enemy[i].pic = enemy_type[4].pic;
				enemy[i].frame_count = 9;
				enemy[i].slower = ENEMY3_SLOWER;
				enemy[i].velocity=ENEMY3_VELOCITY;
				enemy[i].health=ENEMY3_HEALTH;
				break;
		}
	}
	for (int i=0; i<=numOfEnemies; i++)
	{
		enemy[i].rangeBox = enemy[i].box = MakeRect(enemy[i].posY-CENTER_Y , enemy[i].posX-CENTER_X , 100, 80);
	}

}
void loadTowers()
{
	char towerPicLoad[100];
	
	//load images for rotation
	for (int i=0 ;	i<=3; i++)
	{
			sprintf(towerPicLoad,"Images\\tower-%d.png",i);
			GetBitmapFromFile(towerPicLoad, &towType[i].picBlu);
			if (i<=2)
				TL_QuickRotateLoad(towerPicLoad,&towType[i].TL_pic);
			sprintf(towerPicLoad,"Images\\tower-%d-red.png",i);
			GetBitmapFromFile(towerPicLoad, &towType[i].picRed);
			towType[i].pic=towType[i].picBlu;
	}
	 towTypeReset();
	//pivot at 30,15 px
	towType[0].pivot=MakePoint(15,30);
	towType[0].hight=50;
	towType[0].width=30;
	towType[0].power=SMALL_TOWER_POWER;
	towType[0].range=SMALL_TOWER_RANGE;
	towType[0].cost=SMALL_TOWER_COST;
				   
	//pivot at 38, 20 px
	towType[1].pivot=MakePoint(20,38);
	towType[1].hight=62;
	towType[1].width=40;
	towType[1].power=MEDIUM_TOWER_POWER;
	towType[1].range=MEDIUM_TOWER_RANGE;
	towType[1].cost=MEDIUM_TOWER_COST;

	//pivot at 56, 25
	towType[2].pivot=MakePoint(25,56);
	towType[2].hight=80;
	towType[2].width=50;
	towType[2].power=LARGE_TOWER_POWER;
	towType[2].range=LARGE_TOWER_RANGE;
	towType[2].cost=LARGE_TOWER_COST;

	//pivot -> middle
	towType[3].hight=towType[3].width=50;
	towType[3].pivot=MakePoint(towType[3].hight/2, towType[3].hight/2);
	towType[3].power=MISSILE_TOWER_POWER;
	towType[3].range=MISSILE_TOWER_RANGE;
	towType[3].cost=MISSILE_TOWER_COST;
		
	for (int i=0; i<=3; i++)
	{
		towType[i].box=MakeRect(
						   towType[i].y-towType[i].pivot.y,
						   towType[i].x -towType[i].pivot.x,
						   towType[i].hight ,
						   towType[i].width);
	}
}

/*----------------------------------------------------------------------------*/
/*                       		 LEVEL										  */
/*----------------------------------------------------------------------------*/
int CVICALLBACK StartFunc (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
			SetCtrlAttribute (panelHandle, PANEL_TIMER_SECONDS, ATTR_ENABLED, 1);
			//click
			BASS_ChannelPlay(sound_SND[CLICK_SOUND],TRUE);
			//stop
			BASS_ChannelStop(sound_SND[0]);
			//play
			BASS_ChannelPlay(sound_SND[1],TRUE);
			//hide
			SetCtrlAttribute (panelHandle, PANEL_START_BUTTON, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_PICTURE_METAL_FRAME, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_PICTURE_METAL_FRAME_2, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_VOL_SHOW_BUTTON, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LEAD_BUTTON, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_INFO_BUTTON, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_PICTURE_WELCOME, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_VOLUME_SLIDE, ATTR_VISIBLE, 0);
			//show
			//SetCtrlAttribute (panelHandle, PANEL_CANVAS, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_TEXT_BKG, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_SCORE, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_LEVEL_TEXT, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_BACK_BUTTON_2, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_LOCK_5, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_LOCK_6, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_LOCK_7, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_LOCK_8, ATTR_VISIBLE, 1);
			SetCtrlVal (panelHandle, PANEL_LEVEL_TEXT, level);
			
			//move exit button
			SetCtrlAttribute (panelHandle, PANEL_EXIT_BUTTON, ATTR_LEFT, 1380);
			SetCtrlAttribute (panelHandle, PANEL_EXIT_BUTTON, ATTR_TOP, 715);
			SetCtrlAttribute (panelHandle, PANEL_EXIT_BUTTON, ATTR_HEIGHT, 50);
			SetCtrlAttribute (panelHandle, PANEL_EXIT_BUTTON, ATTR_WIDTH, 50);
			//flags
			levelTimer=0;
			//game timer
			SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
			if (level==1)
			{
				SetCtrlAttribute (panelHandle, PANEL_DIMM, ATTR_VISIBLE, 1);
				DisplayPanel (info_panelHandle);
			}
			else
				levelStart=1;
			break;
	}
	return 0;
}
void loadLevel(int level)
{
	levelTimer=0;
	playOnceVictory=1;
	numOfEnemies=enemiesLeft=(rand()%(level*level)+1);
	if (level==1)
		numOfEnemies=1;
	/*for (int i=0; i<=numOfEnemies; i++)
	{
		level_enemies[0][i]=i;
		if (level==1)
			level_enemies[1][i]=1;
		else
			level_enemies[1][i]=(rand()%3);
	}*/
	for (int i=0; i<=numOfTowers; i++)
	{
	tower[i].fire=FALSE;
	}
	setEnemies();
}
void loadMap(int level)
{
	char dataload[100], mapFilePath[100];
	int k=0, numOfCols,numOfRows;
	level=1;
	//load avalble ground on map & make rectangles
	sprintf( mapFilePath,"Map_Data\\map%d-data.csv",level);
	fp= fopen("Map_Data\\map1-data.csv", "r");		
	fgets (dataload, 50, fp);
	fgets (dataload, 50, fp);
	//top coordinates for sarting tiles
	fgets (dataload, 50, fp);
	sscanf (dataload, "%d,%d", &Map_Top_L, &Map_Top_Y );
	//tile diamentions
	fgets (dataload, 50, fp);
	fgets (dataload, 50, fp);
	sscanf (dataload, "%d, %d, %d",&tileWidth, &tileHight, &tileGap  );
	//number of tiles rows & cols
	fgets (dataload, 50, fp);
	fgets (dataload, 50, fp);
	sscanf (dataload, "%d,%d", &numOfCols, &numOfRows );

	//set tile boxes
	for(int i=0;i<numOfCols;i++)
	{
		for(int j=0;j<numOfRows;j++)
		{
			tile[k].box=MakeRect(
							Map_Top_Y + j*(tileHight+tileGap) ,
							Map_Top_L + i*(tileWidth+tileGap) ,
							tileHight ,
							tileWidth );
			RectCenter(tile[k].box, &tile[k].center);
			k++;
		}																							
	}
	numOfMapTiles = k-1;
	k=0;
	
	//reset tile states
	for (int i=0; i<=numOfMapTiles; i++)
	{
		tile[i].ok=0;
		tile[i].state=0;
	}
	
	//set avalable tiles for placing towers
	fgets (dataload, 50, fp);
	while (fgets(dataload, 50, fp) != 0)
	{
		sscanf (dataload, "%d", &k);
		tile[k].ok=1;
		if (!k)
		{
			tile[k].ok=0;
			break;
		}
	}
	
	//load waypoints to arry
	fgets (dataload, 50, fp);
	fgets (dataload, 50, fp);
	fgets (dataload, 50, fp);
	while (fgets(dataload, 50, fp) != 0)
	{
		sscanf (dataload, "%d,%d", &waypointsmap1[k][0], &waypointsmap1[k][1]);
		k++;
	}
	numberofWP=k-1;
	k=0;
	fclose(fp);
}
int CVICALLBACK nextLevelFunc (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			BASS_ChannelPlay(sound_SND[CLICK_SOUND],TRUE);
			BASS_ChannelPlay(sound_SND[1],TRUE);
			level++;
			loadLevel(level);
			setEnemies();
			SetCtrlAttribute (panelHandle, PANEL_VICTORY, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_DIMM, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_BACK_BUTTON, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_CONTINUE_BUTTON, ATTR_VISIBLE, 0);
			break;
	}
	return 0;
}

/*----------------------------------------------------------------------------*/
/*                       		 SOUNDS										  */
/*----------------------------------------------------------------------------*/
void loadSounds()
{
	char soundload[50];
	for (int i=0; i<=numOfSounds; i++)
	{
		sprintf(soundload,"Sound\\%d.mp3", i);
		sound_SND[i] = BASS_StreamCreateFile(FALSE,soundload,0,0,0);
		if (i==10 || i==0)
			sound_SND[i] = BASS_StreamCreateFile(FALSE,soundload,0,0,BASS_MUSIC_LOOP);
		BASS_ChannelSetAttribute(sound_SND[i], BASS_ATTRIB_VOL, DEFAULT_VOLUME);
	}
}
void playLaserInUpdate()
{
	if ((time_counter%100)==1)
		BASS_ChannelPlay(sound_SND[10],TRUE);
}
int CVICALLBACK volume (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	double volume;
	switch (event)
	{
		case EVENT_VAL_CHANGED:
			GetCtrlVal (panelHandle, PANEL_VOLUME_SLIDE, &volume);
			for (int i=0; i<=numOfSounds; i++)
			{
				BASS_ChannelSetAttribute(sound_SND[i], BASS_ATTRIB_VOL, volume);
			}
			break;
	}
	return 0;
}
int CVICALLBACK showVolSlider (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int showSlider;
	switch (event)
	{
		case EVENT_COMMIT:
			BASS_ChannelPlay(sound_SND[CLICK_SOUND],TRUE);
			GetCtrlVal (panelHandle, PANEL_VOL_SHOW_BUTTON, &showSlider);
			SetCtrlAttribute (panelHandle, PANEL_VOLUME_SLIDE, ATTR_VISIBLE, 1-showSlider);
			break;
	}
	return 0;
}

/*----------------------------------------------------------------------------*/
/*                        LEADERBOARD & SCORES								  */
/*----------------------------------------------------------------------------*/
void loadLeaderBoard()
{
	int i=0;
	char lineload[100];
	fp= fopen("Leader_Board\\Scores.csv", "r");	
	fgets (lineload, 50, fp);
	for (i; i<=5; i++)
	{
		fgets(lineload, 50, fp);
		sscanf (lineload, "%[^,], %d, %[^,], %[^\n]", player[i].name, &player[i].score, player[i].date, player[i].time);
		SetTableCellVal (leaderboard_panelHandle, LEAD_PANEL_TABLE, MakePoint(1,i+1), player[i].name);
		SetTableCellVal (leaderboard_panelHandle, LEAD_PANEL_TABLE, MakePoint(2,i+1), player[i].score);
		SetTableCellVal (leaderboard_panelHandle, LEAD_PANEL_TABLE, MakePoint(3,i+1), player[i].date);
		SetTableCellVal (leaderboard_panelHandle, LEAD_PANEL_TABLE, MakePoint(4,i+1), player[i].time);
	}
	fclose(fp);
	numOfPlayers=i-1;
}
void sortLeaderboard()
{
	//SortTableCells (leaderboard_panelHandle, LEAD_PANEL_TABLE, MakeRect (1, 1, numOfPlayers+1, 3), VAL_COLUMN_MAJOR, 2, 0, 0, 0);
	players sortPlayer;
	for (int i=0; i<=5; i++)
	{
		for (int j=1+i; j<=5; j++)
		{
			if (player[j].score>player[i].score)
			{
				strcpy(sortPlayer.name, player[i].name);
				strcpy(sortPlayer.date, player[i].date);
				strcpy(sortPlayer.time, player[i].time);
				sortPlayer.score=player[i].score;
				
				strcpy(player[i].name, player[j].name);
				strcpy(player[i].date, player[j].date);
				strcpy(player[i].time, player[j].time);
				player[i].score=player[j].score;
				
				strcpy(player[j].name, sortPlayer.name);
				strcpy(player[j].date, sortPlayer.date);
				strcpy(player[j].time, sortPlayer.time);
				player[j].score=sortPlayer.score;
			}
		}
	}
}
int CVICALLBACK writeToLeaderboard (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			int day, month, year, hour, minute, second;
			//1.name
			GetCtrlVal (name_panelHandle, NAME_PANEL_PLAYER_NAME, player[5].name);
			//2.score
			player[5].score=nowPlayer.score;
			//3.date
			sscanf(DateStr(), "%d-%d-%d", &month, &day, &year);
			sprintf(player[5].date,"%d/%d/%d", day, month, year);
			//4.time
			sscanf(TimeStr(), "%d:%d:%d", &hour, &minute, &second);
			sprintf(player[5].time,"%d:%d", hour, minute);
			
			sortLeaderboard();
			fp= fopen("Leader_Board\\Scores.csv", "w");
			fprintf(fp,"name,score,date,time\n");
			for (int i=0; i<=5; i++)
			{
				fprintf(fp,"%s,%d,%s,%s\n",player[i].name ,player[i].score, player[i].date, player[i].time);
				SetTableCellVal (leaderboard_panelHandle, LEAD_PANEL_TABLE, MakePoint(1,i+1), player[i].name);
				SetTableCellVal (leaderboard_panelHandle, LEAD_PANEL_TABLE, MakePoint(2,i+1), player[i].score);
				SetTableCellVal (leaderboard_panelHandle, LEAD_PANEL_TABLE, MakePoint(3,i+1), player[i].date);
				SetTableCellVal (leaderboard_panelHandle, LEAD_PANEL_TABLE, MakePoint(4,i+1), player[i].time);
			}
			fclose(fp);
			newHighScore=0;
			nowPlayer.score=SMALL_TOWER_COST;
			HidePanel(name_panelHandle);
			resetGame();
			DisplayPanel(leaderboard_panelHandle);
			
			break;
	}
	return 0;
}
void checkScore(int score)
{
	SetCtrlVal (panelHandle, PANEL_SCORE, nowPlayer.score);
	if (score>=MISSILE_TOWER_COST)
	{
		SetCtrlAttribute (panelHandle, PANEL_LOCK_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute (panelHandle, PANEL_LOCK_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute (panelHandle, PANEL_LOCK_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute (panelHandle, PANEL_LOCK_4, ATTR_VISIBLE, 0);
	}
	if (score<MISSILE_TOWER_COST && score>=LARGE_TOWER_COST)
	{
		SetCtrlAttribute (panelHandle, PANEL_LOCK_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute (panelHandle, PANEL_LOCK_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute (panelHandle, PANEL_LOCK_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute (panelHandle, PANEL_LOCK_4, ATTR_VISIBLE, 1);
	}
	if (score<LARGE_TOWER_COST && score>=MEDIUM_TOWER_COST)
	{
		SetCtrlAttribute (panelHandle, PANEL_LOCK_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute (panelHandle, PANEL_LOCK_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute (panelHandle, PANEL_LOCK_3, ATTR_VISIBLE, 1);
		SetCtrlAttribute (panelHandle, PANEL_LOCK_4, ATTR_VISIBLE, 1);
	}
	if (score<MEDIUM_TOWER_COST && score>=SMALL_TOWER_COST)
	{
		SetCtrlAttribute (panelHandle, PANEL_LOCK_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute (panelHandle, PANEL_LOCK_2, ATTR_VISIBLE, 1);
		SetCtrlAttribute (panelHandle, PANEL_LOCK_3, ATTR_VISIBLE, 1);
		SetCtrlAttribute (panelHandle, PANEL_LOCK_4, ATTR_VISIBLE, 1);
	}
	if (score<SMALL_TOWER_COST)
	{
		SetCtrlAttribute (panelHandle, PANEL_LOCK_1, ATTR_VISIBLE, 1);
		SetCtrlAttribute (panelHandle, PANEL_LOCK_2, ATTR_VISIBLE, 1);
		SetCtrlAttribute (panelHandle, PANEL_LOCK_3, ATTR_VISIBLE, 1);
		SetCtrlAttribute (panelHandle, PANEL_LOCK_4, ATTR_VISIBLE, 1);
	}
	
}
void endOfGameScoreCheck()
{
	if (nowPlayer.score>player[5].score)
		newHighScore=1;
	else
		newHighScore=0;
	if (newHighScore)
	{
		DisplayPanel(name_panelHandle);
		SetCtrlVal (name_panelHandle, NAME_PANEL_SCORE, nowPlayer.score);
	}
}
int CVICALLBACK showLeaderboard (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	int showLeaderboard;
	switch (event)
	{
		case EVENT_COMMIT:
			BASS_ChannelPlay(sound_SND[CLICK_SOUND],TRUE);
			GetCtrlVal (panelHandle, PANEL_LEAD_BUTTON, &showLeaderboard);
			switch (showLeaderboard)
			{
				case 0:
					HidePanel(leaderboard_panelHandle);
					break;
				case 1:
					DisplayPanel(leaderboard_panelHandle);
					loadLeaderBoard();
					break;
			}
			break;
	}
	return 0;
}
int CVICALLBACK backNameFunc (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			resetGame();
			break;
	}
	return 0;
}

/*----------------------------------------------------------------------------*/
/*                              OTHER BUTTONS								  */
/*----------------------------------------------------------------------------*/
int CVICALLBACK aboutFunc (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			BASS_ChannelPlay(sound_SND[CLICK_SOUND],TRUE);
			DisplayPanel(info_panelHandle);
			break;
	}
	return 0;
}
int CVICALLBACK backFunc (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			BASS_ChannelPlay(sound_SND[CLICK_SOUND],TRUE);
			resetGame();
			break;
	}
	return 0;
}
int CVICALLBACK developerShow (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			BASS_ChannelPlay(sound_SND[CLICK_SOUND],TRUE);
			DisplayPanel(dev_panelHandle);
			break;
	}
	return 0;
}
int CVICALLBACK docShow (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			BASS_ChannelPlay(sound_SND[CLICK_SOUND],TRUE);
			OpenDocumentInDefaultViewer ("c:\\Users\\Roy\\Documents\\Programing_Mechatronics\\Final_Project\\Game_Documentation.pdf", VAL_NO_ZOOM);
			break;
	}
	return 0;
}
int CVICALLBACK demoShow (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			BASS_ChannelPlay(sound_SND[CLICK_SOUND],TRUE);
			OpenDocumentInDefaultViewer ("DEMO_VIDEO.mp4", VAL_NO_ZOOM);
			for (int i=0; i<=numOfSounds; i++)
			{
				BASS_ChannelSetAttribute(sound_SND[i], BASS_ATTRIB_VOL, 0.0);
			}
			SetCtrlVal (panelHandle, PANEL_VOLUME_SLIDE, 0.0);
			MessagePopup ("Note", "Sounds are muted. \nYou can raise the volume at the sound button on the bottom left of the screen.");
			break;
	}
	return 0;
}
int CVICALLBACK exitAbout (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			BASS_ChannelPlay(sound_SND[CLICK_SOUND],TRUE);
			HidePanel(dev_panelHandle);
			break;
	}
	return 0;
}
int CVICALLBACK ExitFunc (int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			BASS_ChannelPlay(sound_SND[CLICK_SOUND],TRUE);
			QuitUserInterface(0);
			break;
	}
	return 0;
}
int CVICALLBACK exitLeader (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			BASS_ChannelPlay(sound_SND[CLICK_SOUND],TRUE);
			HidePanel(leaderboard_panelHandle);
			SetCtrlVal (panelHandle, PANEL_LEAD_BUTTON, 0);
			break;
	}
	return 0;
}
int CVICALLBACK exitMain (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			BASS_ChannelPlay(sound_SND[CLICK_SOUND],TRUE);
			QuitUserInterface (0);
			break;
	}
	return 0;
}
int CVICALLBACK exitHelp (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			BASS_ChannelPlay(sound_SND[CLICK_SOUND],TRUE);
			HidePanel(info_panelHandle);
			if (time_counter>>0 && level==1)
			{
				levelStart=1;
				nowPlayer.score=SMALL_TOWER_COST;
				SetCtrlVal (panelHandle, PANEL_SCORE, nowPlayer.score);
			}
			SetCtrlAttribute (panelHandle, PANEL_DIMM, ATTR_VISIBLE, 0);
			break;
	}
	return 0;
}

/*----------------------------------------------------------------------------*/
/*                                   TESTS									  */
/*----------------------------------------------------------------------------*/
int CVICALLBACK Test (int panel, int control, int event, void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			//update();
			//draw();
			SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
			break;
	}
	return 0;
}
void markwaypoints()
{
	for (int j=0; j<=numberofWP; j++) //mark waypoints
	{
		CanvasDrawOval (panelHandle, PANEL_CANVAS, MakeRect (waypointsmap1[j][1]-10, waypointsmap1[j][0]-10, 20, 20), VAL_DRAW_FRAME_AND_INTERIOR);
	}
}
void markground()
{
	char text[10]= {0};
	for (int j=0; j<=numOfMapTiles; j++) //ground rectangles
	{
		if (tile[j].ok)
		{
			SetCtrlAttribute (panelHandle, PANEL_CANVAS, ATTR_PEN_COLOR, VAL_GREEN);
			CanvasDrawRect (panelHandle, PANEL_CANVAS, tile[j].box, VAL_DRAW_INTERIOR);
		}
		else
			CanvasDrawRect (panelHandle, PANEL_CANVAS, tile[j].box, VAL_DRAW_FRAME);
		sprintf (text, "%d", j);
		CanvasDrawTextAtPoint (panelHandle, PANEL_CANVAS, text, VAL_APP_META_FONT, MakePoint(tile[j].box.left,tile[j].box.top), VAL_UPPER_LEFT);
	}
}


/*----------------------------------------------------------------------------*/
/* Shut down.						  										  */
/*----------------------------------------------------------------------------*/
void resetGame()
{
	//flags
	SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
	level=START_LEVEL;
	loadLevel(level);
	setEnemies();
	levelStart=0;
	levelTimer=0;
	
	
	//towers
	for (int i=0; i<=numOfTowers; i++)
	{
		memset(&tower[i], 0, sizeof(towers));
	}
	numOfTowers=0;

	//map
	loadMap(level);
	
	//sound
	for (int i=0; i<=numOfSounds; i++)
	{
		BASS_ChannelStop(sound_SND[i]);
	}
	BASS_ChannelPlay(sound_SND[0],TRUE);

	//hide
	SetCtrlAttribute (panelHandle, PANEL_ENEMIES_COMING, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_VICTORY, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_DEFEAT, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_DIMM, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_BACK_BUTTON, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_TEXT_BKG, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_WAVE_COUNTDOWN, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_BACK_BUTTON_2, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_SCORE, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_LEVEL_TEXT, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_LOCK_1, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_LOCK_2, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_LOCK_3, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_LOCK_4, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_CONTINUE_BUTTON, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_LOCK_5, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_LOCK_6, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_LOCK_7, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_LOCK_8, ATTR_VISIBLE, 0);
	HidePanel(name_panelHandle);
	HidePanel(leaderboard_panelHandle);
	HidePanel(info_panelHandle);

	//show
	SetCtrlAttribute (panelHandle, PANEL_START_BUTTON, ATTR_VISIBLE, 1);
	SetCtrlAttribute (panelHandle, PANEL_PICTURE_METAL_FRAME, ATTR_VISIBLE, 1);
	SetCtrlAttribute (panelHandle, PANEL_PICTURE_METAL_FRAME_2, ATTR_VISIBLE, 1);
	SetCtrlAttribute (panelHandle, PANEL_VOL_SHOW_BUTTON, ATTR_VISIBLE, 1);
	SetCtrlAttribute (panelHandle, PANEL_LEAD_BUTTON, ATTR_VISIBLE, 1);
	SetCtrlAttribute (panelHandle, PANEL_INFO_BUTTON, ATTR_VISIBLE, 1);
	SetCtrlAttribute (panelHandle, PANEL_PICTURE_WELCOME, ATTR_VISIBLE, 1);

	//move exit button
	SetCtrlAttribute (panelHandle, PANEL_EXIT_BUTTON, ATTR_LEFT, 1187);
	SetCtrlAttribute (panelHandle, PANEL_EXIT_BUTTON, ATTR_TOP, 674);
	SetCtrlAttribute (panelHandle, PANEL_EXIT_BUTTON, ATTR_HEIGHT, 87);
	SetCtrlAttribute (panelHandle, PANEL_EXIT_BUTTON, ATTR_WIDTH, 87);
	
	endOfGameScoreCheck();
	if (!newHighScore)
	{
		nowPlayer.score=SMALL_TOWER_COST;
	}
	
}
void terminate()
{
	for (int i=0; i<=numOfSounds; i++)
	{
		BASS_StreamFree(sound_SND[i]);
	}
	
	for (int i=0 ;	i<=3; i++)
	{
			DiscardBitmap (towType[i].picBlu);
			DiscardBitmap (towType[i].picRed);
			if (i<=2)
				TL_QuickRotateDiscard(towType[i].TL_pic);
	}
	for(int i=0; i<=5; i++)
	{
		DiscardBitmap (enemy_type[i].pic);
	}
	
	DiscardBitmap (map1);
	DiscardBitmap (map11);
	DiscardBitmap (sidebar);
	
	DiscardPanel (panelHandle);
	DiscardPanel (wait_panelHandle);
	DiscardPanel (leaderboard_panelHandle);
	DiscardPanel (name_panelHandle);
	DiscardPanel (info_panelHandle);
	DiscardPanel (dev_panelHandle);
}
