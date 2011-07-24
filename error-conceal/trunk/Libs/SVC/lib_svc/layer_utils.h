#ifndef _LAYER_UTILS_H_
#define _LAYER_UTILS_H_


int get_layer_id(char dqId,char* layer_id_table);
int get_same_dep_highest_layer_id(int layer_id,char* layer_id_table);
void add_layer_id(char dqId,int layer_id);
void insert_layer_id(char dqId,int layer_id);
#endif