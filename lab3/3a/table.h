#ifndef TABLE_H
#define TABLE_H

//structs------------------------------------------------------
typedef struct InfoType{
  float f1;
  float f2;
  char* s1;
}InfoType;

typedef struct Item{
  int k1;
  char* k2;
  int hash2;
  InfoType info;
  int ver;
  struct Item* next;
  struct key1* ptr1;
  struct key2* ptr2;
}Item;

typedef struct Table{
  struct keyspace1* ks1;
  struct keyspace2* ks2;
  int size1;
  int size2;
  int rsize1;
}Table;

//keyspaces---------------------------------------------------
typedef struct key1{
  Item* item;
  struct key1* next;
  struct key1* parent;
}key1;

typedef struct keyspace1{
  int version;
  unsigned int val;
  key1* key;
}keyspace1;

typedef struct key2{
    Item* item;
    struct key2* next;
    struct key2* parent;
    int v2;
}key2;

typedef struct keyspace2{
    int version;
    key2* key;
}keyspace2;

int bin_find(keyspace1* ks1, int size, unsigned int key, int* pos);
int hash(char* str, int size);

void Copy_Item(Item** dest, Item* source);
void NewItem(Table* table, Item* item);

void free_item(Item* ptr);
void relink_1(Table* table, key1* ptr, int pos);
void relink_2(Table* table, key1* ptr);

void k1_erase_one(Table* table, int k1, int ver);
void k1_erase_all(Table* table, unsigned int key);

void k2_erase_one(Table* table, key2* ptr);
void k2_erase_all(Table* table, char* k2);
void k2_reorganize(Table* table, char* k2);

void k1_k2_erase(Table* table, int k1, char* k2);
void delete_all(Table* table);

void output_one(Item* itptr);

void output_1(Table* table, unsigned int key);
void output_1_1(Table* table, unsigned int k1, int ver);

void output_2(Table* table, char* k2);
void output_2_2(Table* table, char* k2, int ver);

void output_1_2(Table* table, unsigned int k1, char* k2);

void output_all(Table* table);

#endif