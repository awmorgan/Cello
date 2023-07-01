#include "Cello.h"

int main(int argc, char **argv) {

  var items = new (Array, Int, $I(8), $I(5), $I(20), $I(15), $I(16), $I(98));

  /* Iterate over indices using "range" */
  foreach (i, range($I(len(items)))) {
    print("Item Range %i is %i\n", i, get(items, i));
  }

  /* Iterate over every other item with "slice" */
  foreach (item, slice(items, _, _, $I(2))) {
    print("Item Slice %i\n", item);
  }

  return 0;
}
