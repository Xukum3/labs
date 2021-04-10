#include<stdio.h>

#ifndef TABLE_H
#define TABLE_H

//structs------------------------------------------------------
typedef struct InfoType{
  float f1;
  float f2;
  char* s1;
}InfoType;

typedef struct InfoType2{
  float f1;
  float f2;
  int len_s1;
}InfoType2;

typedef struct Item{
  int k1;
  int hash2;
  InfoType info;
  int ver;
  char* k2;
}Item;

typedef struct Item2{
  int alive;
  int k1;
  int hash2;
  int ver;
  long int offset_key1;
  long int offset_key2;
  long int offset_next;
  int len_k2;
  InfoType2 info;
}Item2;
//char len2
//char str1

typedef struct Table{
  FILE* ks1;
  FILE* ks2;
  FILE* items;
  int size1;
  int size2;
  int rsize1;
}Table;

//keyspaces---------------------------------------------------
typedef struct key1{
  int alive;
  long int offset_item;
  long int offset_next;
  long int offset_parent;
}key1;

typedef struct keyspace1{
  unsigned int val;
  long int offset_key1;
  int version;
  //key1* key;
}keyspace1;

typedef struct key2{
  int alive;
  int v2;
  long int offset_item;
  long int offset_next;
  long int offset_parent;
}key2;

typedef struct keyspace2{
  int version;
  long int offset_key2;
}keyspace2;

int bin_find(FILE* ks1, int size, unsigned int key, int* pos);
int hash(char* str, int size);

void Copy_Item(Item** dest, Item* source);
void NewItem(Table* table, Item2* item, char* k2, char* s1);

void free_item(long int ptr, Table* table);
void relink_1(Table* table, Item2* item, int pos);
void relink_2(Table* table, Item2* item);

void k1_erase_one(Table* table, int k1, int ver);
void k1_erase_all(Table* table, unsigned int key);

void k2_erase_one(Table* table, Item2* item, long int offs_item);
void k2_erase_all(Table* table, char* k2);
void k2_reorganize(Table* table, char* k2);

void k1_k2_erase(Table* table, int k1, char* k2);
void delete_all(Table* table);

void output_one(long int offset, Table* table);

void output_1(Table* table, unsigned int key);
void output_1_1(Table* table, unsigned int k1, int ver);

void output_2(Table* table, char* k2);
void output_2_2(Table* table, char* k2, int ver);

void output_1_2(Table* table, unsigned int k1, char* k2);

void output_all(Table* table);
void output_all_2(Table* table);

#endif