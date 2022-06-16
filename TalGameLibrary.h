/************** Tal Game Library BETA ***************************************/
/* January 2018                                                             */
/* Written by Dr Tal Alon 					                                */
/* Not fully debugged and tested - so the                                   */
/*     functions might not work properly under                              */
/*     certain circumstances					                            */
/*                                                                          */
/* Aimed for my game developing students only                               */ 
/*                                                                          */
/* Update December 2019 - Simplified Functional Interface					*/
/*--------------------------------------------------------------------------*/
#include "toolbox.h" 



// Use the TL_QuickRotateLoad function to load a bitmap for later rotation
// using this library (instead of GetBitmapFromFile). Perform the load long
// before the first rotation is due, as the process of loading can take some time.
// This funtion generates a fake bitmap handle that will only work for this library's functions   
extern int TL_QuickRotateLoad(char filename[], int *TL_bitmap_id);


// Use TL_QuickRotateDiscard to free the memory allocated by TL_QuickRotateLoad
// for a certain loaded bitmap (instead of DiscardBitmap)
extern int TL_QuickRotateDiscard(int TL_bitmap_id);


// Use TL_CanvasDrawBitmap to actually draw a rotated image
// the angle should be provided in degrees (rotation is clockwise)
// the image must be preloaded using TL_QuickRotateLoad
// also the image will be drawn without scaling
// the function returns a new bounding rectangle that can be used for collision detection
extern Rect TL_CanvasDrawBitmap(int panelhandle, int control, int TL_bitmap_id,  Point center,double angle_degrees);  




// The TL_GetAngleFromPoints function is fed a vector starting 
// from the center of the image to another point on the canvas
// (maybe where the mouse if pointing) and return the angle (degrees)
// between this vector and the Y axis 
// Can be used to rotate an image to point to that direction
extern double TL_GetAngleFromPoints(Point center, Point tip);
  

// The SpitSprites function takes a bitmap containin several sprites
// and fills the supplied bitmap_arr with new bitmaps, each contains only
// one sprite image
//
// 'horizontal' - specify the number of sprite images in every line
// 'vertical'   - specify the number of sprite images in every column
//
// Important Note  : This function allocates memory for all new bitmaps,
//                   you must use DiscardBitmap to free it when you are done.
extern void TL_SpitSprites(int bitmapID,
						int horizontal,
                        int vertical,
						int *bitmap_arr);
