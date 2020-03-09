#ifndef _MAPPING_H
#define _MAPPING_H

#include <stdbool.h>
#include <stdint.h>
#define WALL_THRESHOLD 130

typedef struct
{
  bool west_wall;
  bool south_wall;
  bool east_wall;
  bool north_wall;
}cell_t;

typedef struct node node_t;

typedef struct node
{
  int8_t x_coord;
  int8_t y_coord;
  cell_t walls;
  node_t *prev;
  uint32_t list_length;
}node_t;
extern node_t *(*mapping_create_node)(int8_t x, int8_t y, cell_t *walls, node_t *prev);
node_t *mapping_create_node_fake(int8_t x, int8_t y, cell_t *walls, node_t *prev);


#endif
