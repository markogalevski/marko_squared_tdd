#include "mapping.h"

node_t *mapping_create_node_impl(int8_t x, int8_t y, cell_t *walls, node_t *prev)
{
  static uint32_t current_length = 0;
  node_t *fresh_node = (node_t *)malloc(sizeof(node_t));
  fresh_node->x_coord = x;
  fresh_node->y_coord = y;
  fresh_node->walls = *walls;
  if (prev != NULL)
    {
      fresh_node->prev = prev;
    }
  fresh_node->list_length = ++current_length;
  return fresh_node;
}

node_t * (*mapping_create_node)(int8_t x, int8_t y, cell_t *walls, node_t *prev) = mapping_create_node_impl;

node_t *mapping_create_node_fake(int8_t x, int8_t y, cell_t *walls, node_t *prev)
{
  return prev;
}
