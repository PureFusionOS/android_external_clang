// RUN: %clang_cc1 -analyze -analyzer-check-objc-mem -analyzer-experimental-checks -verify -analyzer-constraints=basic %s
// RUN: %clang_cc1 -analyze -analyzer-check-objc-mem -analyzer-experimental-checks -verify -analyzer-constraints=range %s
#include <limits.h>

// These are used to trigger warnings.
typedef typeof(sizeof(int)) size_t;
void *malloc(size_t);
void free(void *);
#define NULL ((void*)0)

//---------------
//  Plus/minus
//---------------

void separateExpressions (int a) {
  int b = a + 1;
  --b;

  char* buf = malloc(1);
  if (a != 0 && b == 0)
    return; // no-warning
  free(buf);
}

void oneLongExpression (int a) {
  // Expression canonicalization should still allow this to work, even though
  // the first term is on the left.
  int b = 15 + a + 15 - 10 - 20;

  char* buf = malloc(1);
  if (a != 0 && b == 0)
    return; // no-warning
  free(buf);
}

//---------------
//  Comparisons
//---------------

// Equality and inequality only
void eq_ne (unsigned a) {
  char* b = NULL;
  if (a == UINT_MAX)
    b = malloc(1);
  if (a+1 != 0)
    return; // no-warning
  if (a-1 != UINT_MAX-1)
    return; // no-warning
  free(b);
}

void ne_eq (unsigned a) {
  char* b = NULL;
  if (a != UINT_MAX)
    b = malloc(1);
  if (a+1 == 0)
    return; // no-warning
  if (a-1 == UINT_MAX-1)
    return; // no-warning
  free(b);
}


// Simple order comparisons with no adjustment
void baselineGT (unsigned a) {
  char* b = NULL;
  if (a > 0)
    b = malloc(1);
  if (a == 0)
    return; // no-warning
  free(b);
}

void baselineGE (unsigned a) {
  char* b = NULL;
  if (a >= UINT_MAX)
    b = malloc(1);
  if (a == UINT_MAX)
    free(b);
  return; // no-warning
}

void baselineLT (unsigned a) {
  char* b = NULL;
  if (a < UINT_MAX)
    b = malloc(1);
  if (a == UINT_MAX)
    return; // no-warning
  free(b);
}

void baselineLE (unsigned a) {
  char* b = NULL;
  if (a <= 0)
    b = malloc(1);
  if (a == 0)
    free(b);
  return; // no-warning
}


// Adjustment gives each of these an extra solution!
void adjustedGT (unsigned a) {
  char* b = NULL;
  if (a-1 > UINT_MAX-1)
    b = malloc(1);
  return; // expected-warning{{leak}}
}

void adjustedGE (unsigned a) {
  char* b = NULL;
  if (a-1 >= UINT_MAX-1)
    b = malloc(1);
  if (a == UINT_MAX)
    free(b);
  return; // expected-warning{{leak}}
}

void adjustedLT (unsigned a) {
  char* b = NULL;
  if (a+1 < 1)
    b = malloc(1);
  return; // expected-warning{{leak}}
}

void adjustedLE (unsigned a) {
  char* b = NULL;
  if (a+1 <= 1)
    b = malloc(1);
  if (a == 0)
    free(b);
  return; // expected-warning{{leak}}
}


// Tautologies
void tautologyGT (unsigned a) {
  char* b = malloc(1);
  if (a > UINT_MAX)
    return; // no-warning
  free(b);
}

void tautologyGE (unsigned a) {
  char* b = malloc(1);
  if (a >= 0)
    free(b);
  return; // no-warning
}

void tautologyLT (unsigned a) {
  char* b = malloc(1);
  if (a < 0)
    return; // no-warning
  free(b);
}

void tautologyLE (unsigned a) {
  char* b = malloc(1);
  if (a <= UINT_MAX)
    free(b);
  return; // no-warning
}
