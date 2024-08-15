/*

  This program provides cartesian type graph function

  Revisions
  rev     date        author      description
  1       12-24-2015  kasprzak    initial creation

  Updated by Bodmer to be an example for the library here:
  https://github.com/Bodmer/TFT_eSPI
  
*/

#include "lcd_espi.hpp"
#include "waveform.hpp"

// these are the only external variables used by the graph function
// it's a flag to draw the coordinate system only on the first call to the Graph() function
// and will mimize flicker
// also create some variables to store the old x and y, if you draw 2 graphs on the same display
// you will need to store ox and oy per each display


double ox = -999, oy = -999; // Force them to be off screen
bool display1 = true;
bool update1 = true;

/*

  function to draw a cartesian coordinate system and plot whatever data you want
  just pass x and y and the graph will be drawn

  huge arguement list
  &d name of your display object
  x = x data point
  y = y datapont
  dp = decimal point precision
  gx = x graph location (lower left)
  gy = y graph location (lower left)
  w = width of graph
  h = height of graph
  xlo = lower bound of x axis
  xhi = upper bound of x asis
  xinc = division of x axis (distance not count)
  ylo = lower bound of y axis
  yhi = upper bound of y asis
  yinc = division of y axis (distance not count)
  title = title of graph
  xlabel = x asis label
  ylabel = y asis label
  &redraw = flag to redraw graph on first call only
  color = plotted trace colour
*/


void Graph(TFT_eSprite *waveform, double x, double y, byte dp,
                           double gx, double gy, double w, double h,
                           double xlo, double xhi, double xinc,
                           double ylo, double yhi, double yinc,
                           char *title, char *xlabel, char *ylabel, char *y2label,
                           bool &redraw, unsigned int color) {

    double ydiv, xdiv;
    double i;
    double temp;
    int rot, newrot;

    // gcolor = graph grid colors
    // acolor = axes line colors
    // pcolor = color of your plotted data
    // tcolor = text color
    // bcolor = background color
    unsigned int gcolor = DKBLUE;
    unsigned int acolor = RED;
    unsigned int pcolor = color;
    unsigned int tcolor = WHITE;
    unsigned int bcolor = BLACK;

    if (redraw == true) {

        redraw = false;
        // initialize old x and old y in order to draw the first point of the graph
        // but save the transformed value
        // note my transform funcition is the same as the map function, except the map uses long and we need doubles
        //ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
        //oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

        waveform->fillSprite(TFT_BLACK);

        waveform->setTextDatum(MR_DATUM);

        // draw y scale
        for ( i = ylo; i <= yhi; i += yinc) {
            // compute the transform
            temp =  (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

            if (i == 0) {
                waveform->drawLine(gx, temp, gx + w, temp, acolor);
                waveform->setTextColor(acolor, bcolor);
                waveform->drawString(xlabel, (int)(gx + w) , (int)temp, 2);
            }
            else {
                waveform->drawLine(gx, temp, gx + w, temp, gcolor);
            }

            // draw the axis labels
            waveform->setTextColor(tcolor, bcolor);
            // precision is default Arduino--this could really use some format control
            waveform->drawFloat(i, dp, gx - 4, temp, 1);


            waveform->setTextColor(tcolor, bcolor);
            // precision is default Arduino--this could really use some format control
            waveform->drawFloat(i / 20.0, 2, gx + w + 32, temp, 1);
        }        


        // draw x scale
        for (i = xlo; i <= xhi; i += xinc) {

            // compute the transform
            temp =  (i - xlo) * ( w) / (xhi - xlo) + gx;
            if (i == 0) {
                waveform->drawLine(temp, gy, temp, gy - h, acolor);
                waveform->setTextColor(acolor, bcolor);
                waveform->setTextDatum(BC_DATUM);
                waveform->drawString(ylabel, (int)temp + 20, (int)(gy - h + 15) , 2);


                waveform->drawLine(temp + w, gy, temp + w, gy - h, TFT_GREEN);
                waveform->setTextColor(TFT_GREEN, bcolor);
                waveform->setTextDatum(BC_DATUM);
                waveform->drawString(y2label, (int)(temp + w - 15), (int)(gy - h + 15) , 2);
            }
            else {
                waveform->drawLine(temp, gy, temp, gy - h, gcolor);
            }

            // draw the axis labels
            waveform->setTextColor(tcolor, bcolor);
            waveform->setTextDatum(TC_DATUM);
            // precision is default Arduino--this could really use some format control
            waveform->drawFloat(i, dp, temp, gy + 7, 1);


            // waveform->setTextColor(tcolor, bcolor);
            // waveform->setTextDatum(TC_DATUM);
            // // precision is default Arduino--this could really use some format control
            // waveform->drawFloat(i, dp, temp + w, gy + 7, 1);

        }

        //now draw the graph labels
        waveform->setTextColor(tcolor, bcolor);
        waveform->drawString(title, (int)(gx + w / 2) , (int)(gy - h - 30), 4);
    }

    // the coordinates are now drawn, plot the data
    // the entire plotting code are these few lines...
    // recall that ox and oy are initialized above
    //x =  (x - xlo) * ( w) / (xhi - xlo) + gx;
    //y =  (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    //tft.drawLine(ox, oy, x, y, pcolor);
    // it's up to you but drawing 2 more lines to give the graph some thickness
    //tft.drawLine(ox, oy + 1, x, y + 1, pcolor);
    //tft.drawLine(ox, oy - 1, x, y - 1, pcolor);
    //ox = x;
    //oy = y;

}


void Trace(TFT_eSprite *waveform, double x,  double y,  byte dp,
           double gx, double gy,
           double w, double h,
           double xlo, double xhi, double xinc,
           double ylo, double yhi, double yinc,
           char *title, char *xlabel, char *ylabel, char *y2label,
           bool &update1, unsigned int color)
{

  double ydiv, xdiv;
  double i;
  double temp;
  int rot, newrot;

  //unsigned int gcolor = DKBLUE;   // gcolor = graph grid color
  unsigned int acolor = RED;        // acolor = main axes and label color
  unsigned int pcolor = color;      // pcolor = color of your plotted data
  unsigned int tcolor = WHITE;      // tcolor = text color
  unsigned int bcolor = BLACK;      // bcolor = background color

  // initialize old x and old y in order to draw the first point of the graph
  // but save the transformed value
  // note my transform funcition is the same as the map function, except the map uses long and we need doubles
  if (update1) {
    update1 = false;

    // reset the screen
    // waveform->fillSprite(TFT_BLACK);
    
    ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
    oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

    if ((ox < gx) || (ox > gx+w)) {update1 = true; return;}
    if ((oy < gy-h) || (oy > gy)) {update1 = true; return;}
    

    waveform->setTextDatum(MR_DATUM);

    // draw y scale
    for ( i = ylo; i <= yhi; i += yinc) {
      // compute the transform
      temp =  (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

      if (i == 0) {
        waveform->setTextColor(acolor, bcolor);
        waveform->drawString(xlabel, (int)(gx + w) , (int)temp - 10, 2);
      }
      // draw the axis labels
      waveform->setTextColor(tcolor, bcolor);
      // precision is default Arduino--this could really use some format control
      waveform->drawFloat(i, dp, gx - 4, temp, 1);

      waveform->setTextColor(tcolor, bcolor);
      // precision is default Arduino--this could really use some format control
      waveform->drawFloat(i / 20.0, 2, gx + w + 32, temp, 1);
    }

    // draw x scale
    for (i = xlo; i <= xhi; i += xinc) {

      // compute the transform
      temp =  (i - xlo) * ( w) / (xhi - xlo) + gx;
      if (i == 0) {
        waveform->setTextColor(acolor, bcolor);
        waveform->setTextDatum(BC_DATUM);
        waveform->drawString(ylabel, (int)temp + 20, (int)(gy - h + 15) , 2);

        waveform->drawLine(temp + w, gy, temp + w, gy - h, TFT_GREEN);
        waveform->setTextColor(TFT_GREEN, bcolor);
        waveform->setTextDatum(BC_DATUM);
        waveform->drawString(y2label, (int)(temp + w - 15), (int)(gy - h + 15) , 2);
      }

      // draw the axis labels
      waveform->setTextColor(tcolor, bcolor);
      waveform->setTextDatum(TC_DATUM);
      // precision is default Arduino--this could really use some format control
      waveform->drawFloat(i, dp, temp, gy + 7, 1);
    }

    //now draw the graph labels
    waveform->setTextColor(tcolor, bcolor);
    waveform->drawString(title, (int)(gx + w / 2) , (int)(gy - h - 30), 4);
  }

  // the coordinates are now drawn, plot the data
  // the entire plotting code are these few lines...
  // recall that ox and oy are initialized above
  x =  (x - xlo) * ( w) / (xhi - xlo) + gx;
  y =  (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;

  if ((x < gx) || (x > gx+w)) {update1 = true; return;}
  if ((y < gy-h) || (y > gy)) {update1 = true; return;}
    
    
  waveform->drawLine(ox, oy, x, y, pcolor);
  // it's up to you but drawing 2 more lines to give the graph some thickness
  //tft.drawLine(ox, oy + 1, x, y + 1, pcolor);
  //tft.drawLine(ox, oy - 1, x, y - 1, pcolor);
  ox = x;
  oy = y;

}


void create_sprite_waveform(sprites *spr){

    
    spr->waveform.setColorDepth(8);
    spr->waveform.createSprite(320, 240);
    spr->waveform.setTextSize(1);
    // spr->waveform.fillSprite(TFT_BLACK);
    // spr->waveform.pushSprite(0, 0);

    spr->waveform_background.setColorDepth(8);
    spr->waveform_background.createSprite(320, 240);
    spr->waveform_background.setTextSize(1);
    spr->waveform_background.fillSprite(TFT_BLACK);
    spr->waveform_background.pushSprite(0, 0);


    display1 = true;

    // Graph(&(spr->waveform_background), 0, 0, 0, 25, 220, 260, 215, 0, 200, 40, -18, 18, 6, "", "", "", "", display1, YELLOW);
}