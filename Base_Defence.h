/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  DEV_PANEL                        1
#define  DEV_PANEL_EXIT_BUTTON            2       /* control type: pictButton, callback function: exitAbout */
#define  DEV_PANEL_PICTURE                3       /* control type: picture, callback function: (none) */
#define  DEV_PANEL_TEXTMSG_4              4       /* control type: textMsg, callback function: (none) */
#define  DEV_PANEL_TEXTMSG_5              5       /* control type: textMsg, callback function: (none) */
#define  DEV_PANEL_TEXTMSG_3              6       /* control type: textMsg, callback function: (none) */
#define  DEV_PANEL_TEXTMSG_2              7       /* control type: textMsg, callback function: (none) */
#define  DEV_PANEL_PICTURE_2              8       /* control type: picture, callback function: (none) */

#define  INFO_PANEL                       2
#define  INFO_PANEL_EXIT_BUTTON           2       /* control type: pictButton, callback function: exitHelp */
#define  INFO_PANEL_PICTURE               3       /* control type: picture, callback function: (none) */
#define  INFO_PANEL_TEXTMSG_2             4       /* control type: textMsg, callback function: (none) */
#define  INFO_PANEL_TEXTMSG_3             5       /* control type: textMsg, callback function: (none) */
#define  INFO_PANEL_TEXTMSG               6       /* control type: textMsg, callback function: (none) */
#define  INFO_PANEL_TEXTMSG_8             7       /* control type: textMsg, callback function: (none) */
#define  INFO_PANEL_TEXTMSG_5             8       /* control type: textMsg, callback function: (none) */
#define  INFO_PANEL_TEXTMSG_7             9       /* control type: textMsg, callback function: (none) */
#define  INFO_PANEL_TEXTMSG_4             10      /* control type: textMsg, callback function: (none) */
#define  INFO_PANEL_TEXTMSG_6             11      /* control type: textMsg, callback function: (none) */
#define  INFO_PANEL_DOCUMENTATION         12      /* control type: command, callback function: docShow */
#define  INFO_PANEL_DEMO_VIDEO            13      /* control type: command, callback function: demoShow */
#define  INFO_PANEL_ABOUT_BUTTON          14      /* control type: command, callback function: developerShow */

#define  LEAD_PANEL                       3
#define  LEAD_PANEL_TABLE                 2       /* control type: table, callback function: (none) */
#define  LEAD_PANEL_EXIT_BUTTON           3       /* control type: pictButton, callback function: exitLeader */
#define  LEAD_PANEL_LEADER_BKG            4       /* control type: picture, callback function: (none) */
#define  LEAD_PANEL_PAPER                 5       /* control type: picture, callback function: (none) */
#define  LEAD_PANEL_PICTURE               6       /* control type: picture, callback function: (none) */

#define  NAME_PANEL                       4
#define  NAME_PANEL_TEXTMSG_3             2       /* control type: textMsg, callback function: (none) */
#define  NAME_PANEL_PLAYER_NAME           3       /* control type: string, callback function: writeToLeaderboard */
#define  NAME_PANEL_NAME_CONFIRM          4       /* control type: pictButton, callback function: writeToLeaderboard */
#define  NAME_PANEL_PICTURE               5       /* control type: picture, callback function: (none) */
#define  NAME_PANEL_PICTURE_2             6       /* control type: picture, callback function: (none) */
#define  NAME_PANEL_BACK_BUTTON           7       /* control type: pictButton, callback function: backNameFunc */
#define  NAME_PANEL_SCORE                 8       /* control type: numeric, callback function: (none) */

#define  PANEL                            5       /* callback function: ExitFunc */
#define  PANEL_CANVAS                     2       /* control type: canvas, callback function: canvasFunc */
#define  PANEL_INFO_BUTTON                3       /* control type: pictButton, callback function: aboutFunc */
#define  PANEL_CONTINUE_BUTTON            4       /* control type: pictButton, callback function: nextLevelFunc */
#define  PANEL_BACK_BUTTON_2              5       /* control type: pictButton, callback function: backFunc */
#define  PANEL_BACK_BUTTON                6       /* control type: pictButton, callback function: backFunc */
#define  PANEL_EXIT_BUTTON                7       /* control type: pictButton, callback function: exitMain */
#define  PANEL_PICTURE_WELCOME            8       /* control type: picture, callback function: (none) */
#define  PANEL_PICTURE_METAL_FRAME_2      9       /* control type: picture, callback function: (none) */
#define  PANEL_PICTURE_METAL_FRAME        10      /* control type: picture, callback function: (none) */
#define  PANEL_PICTURE_LOGO               11      /* control type: picture, callback function: (none) */
#define  PANEL_START_BUTTON               12      /* control type: pictButton, callback function: StartFunc */
#define  PANEL_WAVE_COUNTDOWN             13      /* control type: numeric, callback function: (none) */
#define  PANEL_LEVEL_TEXT                 14      /* control type: numeric, callback function: (none) */
#define  PANEL_SCORE                      15      /* control type: numeric, callback function: (none) */
#define  PANEL_TIMER_SECONDS              16      /* control type: timer, callback function: TimerFuncSec */
#define  PANEL_TIMER                      17      /* control type: timer, callback function: TimerFunc */
#define  PANEL_LOCK_3                     18      /* control type: picture, callback function: (none) */
#define  PANEL_LOCK_2                     19      /* control type: picture, callback function: (none) */
#define  PANEL_VICTORY                    20      /* control type: picture, callback function: (none) */
#define  PANEL_TEXT_BKG                   21      /* control type: picture, callback function: (none) */
#define  PANEL_VOLUME_SLIDE               22      /* control type: scale, callback function: volume */
#define  PANEL_DIMM                       23      /* control type: picture, callback function: (none) */
#define  PANEL_LOCK_4                     24      /* control type: picture, callback function: (none) */
#define  PANEL_LEAD_BUTTON                25      /* control type: pictButton, callback function: showLeaderboard */
#define  PANEL_DEFEAT                     26      /* control type: picture, callback function: (none) */
#define  PANEL_ENEMIES_COMING             27      /* control type: picture, callback function: (none) */
#define  PANEL_VOL_SHOW_BUTTON            28      /* control type: pictButton, callback function: showVolSlider */
#define  PANEL_LOCK_1                     29      /* control type: picture, callback function: (none) */
#define  PANEL_LOCK_7                     30      /* control type: picture, callback function: (none) */
#define  PANEL_LOCK_8                     31      /* control type: picture, callback function: (none) */
#define  PANEL_LOCK_6                     32      /* control type: picture, callback function: (none) */
#define  PANEL_LOCK_5                     33      /* control type: picture, callback function: (none) */

#define  WAIT_PANEL                       6
#define  WAIT_PANEL_TEXTMSG               2       /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

#define  CTRLARRAY                        1

     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK aboutFunc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK backFunc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK backNameFunc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK canvasFunc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK demoShow(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK developerShow(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK docShow(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK exitAbout(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ExitFunc(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK exitHelp(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK exitLeader(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK exitMain(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK nextLevelFunc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK showLeaderboard(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK showVolSlider(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK StartFunc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimerFunc(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimerFuncSec(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK volume(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK writeToLeaderboard(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif