#include "mapping.h"

node_t *mapping_create_node_impl(int8_t x, int8_t y, cell_t *walls, node_t *prev)
{
  node_t *fresh_node = (node_t *)malloc(sizeof(node_t));
  fresh_node->x_coord = x;
  fresh_node->y_coord = y;
  fresh_node->walls = *walls;
  if (prev != NULL)
    {
      prev->prev = fresh_node;
    }
  return fresh_node;
}

node_t * (*mapping_create_node)(int8_t x, int8_t y, cell_t *walls, node_t *prev) = mapping_create_node_impl;

node_t *mapping_create_node_fake(int8_t x, int8_t y, cell_t *walls, node_t *prev)
{
  return(NULL);
}
