#include "Arduino.h"
#include "Stack.h"

// Default Constructor (capacity 20)
Stack::Stack() {
  _top = -1;
  _capacity = 20;
  _stack[_capacity];
}

void Stack::push(float operand) {
  _stack[++_top] = operand;
}

float Stack::pop() {
  return _stack[_top--];
}

float Stack::peek() {
  return _stack[_top];
}

boolean Stack::isFull() {
  return _top == _capacity - 1;
}

boolean Stack::isEmpty() {
  return _top == -1;
}
