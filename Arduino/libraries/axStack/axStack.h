/*
    axStack.h - computes rolling average for simple discrete-based derivative calculations.
    Created by the Amber Lab at Caltech for Axo.
*/

#ifndef axStack_h
#define axStack_h

#include "Arduino.h"

class axStack
{
private:
  double *_members; // lack of generalizability is due to the get_average() function
                    // the class was really just made to generate simple means
  int _size;
  double _avg;
  int _position;

public:

  axStack();

  bool initialize(int size, double fillvalue);

  /**
      * Returns the average of the cycling stack
      * @return the average of all members of the int stack
      */
  double get_average();

  /**
     * Flushes the whole stack
     * @param fillvalue the replacement value for all members of the stack
     */
  void flush(double fillvalue);

  /**
     * Adds an integer to the beginning of the stack and removes the last member
     * @param member the member to be added
     */
  void enqueue(double member);
};

#endif