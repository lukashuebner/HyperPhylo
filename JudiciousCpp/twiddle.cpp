/*twiddle.c - generate all combinations of M elements drawn without replacement
  from a set of N elements.  This routine may be used in two ways:
  (0) To generate all combinations of M out of N objects, let a[0..N-1]
      contain the objects, and let c[0..M-1] initially be the combination
      a[N-M..N-1].  While twiddle(&x, &y, &z, p) is false, set c[z] = a[x] to
      produce a new combination.
  (1) To generate all sequences of 0's and 1's containing M 1's, let
      b[0..N-M-1] = 0 and b[N-M..N-1] = 1.  While twiddle(&x, &y, &z, p) is
      false, set b[x] = 1 and b[y] = 0 to produce a new sequence.
  In either of these cases, the array p[0..N+1] should be initialised as
  follows:
    p[0] = N+1
    p[1..N-M] = 0
    p[N-M+1..N] = 1..M
    p[N+1] = -2
    if M=0 then p[1] = 1
  In this implementation, this initialisation is accomplished by calling
  inittwiddle(M, N, p), where p points to an array of N+2 ints.

  Coded by Matthew Belmonte <mkb4@Cornell.edu>, 23 March 1996.  This
  implementation Copyright (c) 1996 by Matthew Belmonte.  Permission for use and
  distribution is hereby granted, subject to the restrictions that this
  copyright notice and reference list be included in its entirety, and that any
  and all changes made to the program be clearly noted in the program text.

  This software is provided 'as is', with no warranty, express or implied,
  including but not limited to warranties of merchantability or fitness for a
  particular purpose.  The user of this software assumes liability for any and
  all damages, whether direct or consequential, arising from its use.  The
  author of this implementation will not be liable for any such damages.

  Reference:

  Phillip J Chase, `Algorithm 382: Combinations of M out of N Objects [G6]',
  Communications of the Association for Computing Machinery 13:6:368 (1970).

  The returned indices x, y, and z in this implementation are decremented by 1,
  in order to conform to the C language array reference convention.  Also, the
  parameter 'done' has been replaced with a Boolean return value.
*/

#include <vector>
#include <cassert>
#include "twiddle.h"

int attr_x;
int attr_y;
int attr_z;
std::vector<int> p;
std::vector<int> mask;
std::set<hElem> combination;
// Base set for combinations
std::vector<hElem> elements;

int twiddle(int *x, int *y, int *z) {
    int i, j, k;
    j = 1;

    while (p[j] <= 0) {
        j++;
    }

    if (p[j - 1] == 0) {
        for (i = j - 1; i != 1; i--) {
            p[i] = -1;
        }

        p[j] = 0;
        *x = *z = 0;
        p[1] = 1;
        *y = j - 1;
    } else {
        if (j > 1) {
            p[j - 1] = 0;
        }

        do {
            j++;
        } while (p[j] > 0);

        k = j - 1;
        i = j;

        while (p[i] == 0) {
            p[i++] = -1;
        }

        if (p[i] == -1) {
            p[i] = p[k];
            *z = p[k] - 1;
            *x = i - 1;
            *y = k - 1;
            p[k] = -1;
        } else {
            if (i == p[0]) {
                return (1);
            } else {
                p[j] = p[i];
                *z = p[i] - 1;
                p[i] = 0;
                *x = j - 1;
                *y = i - 1;
            }
        }
    }
    return (0);
}

void setBaseSet(const std::set<hElem> &baseSet) {
    assert(baseSet.size() <= INT32_MAX);
    assert(!baseSet.empty());

    // Convert set to vector for O(1) indexed access
    elements = std::vector<hElem>(baseSet.begin(), baseSet.end());
}

void initCombinationGenerator(int k) {
    assert(!elements.empty());

    auto n = (int) elements.size();

    assert(k > 0);
    assert(k <= n);


    p = std::vector<int>((size_t) n + 2);
    mask = std::vector<int>((size_t) n);
    attr_x = 0;
    attr_y = 0;
    attr_z = 0;

    // INIT
    int i;
    p[0] = n + 1;

    for (i = 1; i != n - k + 1; i++) {
        p[i] = 0;
    }

    while (i != n + 1) {
        p[i] = i + k - n;
        i++;
    }

    p[n + 1] = -2;
    if (k == 0) {
        p[1] = 1;
    }

    // Generate first combination
    for (i = 0; i != n - k; i++) {
        mask[i] = 0;
    }

    while (i != n) {
        mask[i++] = 1;
        combination.insert(elements[i - 1]);
    }
}

std::set<hElem> getNextCombination() {
    if (combination.empty()) {
        return combination;
    }

    std::set<hElem> nextCombination = combination;

    if (!twiddle(&attr_x, &attr_y, &attr_z)) {
        mask[attr_x] = 1;
        mask[attr_y] = 0;
        combination.clear();

        int i;
        for (i = 0; i != elements.size(); i++) {
            if (mask[i]) {
                combination.insert(elements[i]);
            } else {
            }
        }

        return nextCombination;
    } else {
        combination.clear();
        return nextCombination;
    }
}