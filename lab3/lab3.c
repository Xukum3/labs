/*Ключами являются:
"an unsigned integer" - в первом пространстве ключей;
"a string with variable length (null-terminated)" - во втором пространстве ключей.
Информация представляет собой "a record of two float numbers and a string with variable length (null-terminated)".
#----------------------------------------------------------------------
В таблице могут быть элементы с одинаковым составными ключами (key1, key2) и разными номерами версий
(номер версии элемента формируется как порядковый номер элемента в последовательности элементов
с одинаковыми ключами, определяемый при включении элемента в таблицу).
#----------------------------------------------------------------------
Первое пространство ключей организовано как упорядоченная таблица, организованная вектором;
Максимальный размер пространства ключей ограничен msize1, определяемый при инициализации таблицы.

В пространстве могут находиться несколько элементов с одинаковыми ключами и разными номерами версий

В данном пространстве ключей предусмотрены следующие особые операции:
- поиск в таблице всех версий элемента, заданного ключом, или конкретной (заданной) версии элемента,
также заданного своим ключом: результатом поиска должна быть новая таблица, содержащая найденные элементы;
- удаление из таблицы всех элементов с заданным ключом или элемента определенной версии, также заданного своим ключом.
#---------------------------------------------------------------------
Второе пространство ключей организовано как перемешанная таблица, использующая перемешивание сцеплением.
Максимальный размер массива указателей ограничен msize2, определяемый при инициализации таблицы.

В пространстве могут находиться несколько элементов с одинаковыми ключами и разными номерами версий

В данном пространстве ключей предусмотрены следующие особые операции:
- поиск в таблице всех версий элемента, заданного ключом, или конкретной (заданной) версии элемента,
также заданного своим ключом. Результатом поиска должна быть новая таблица, содержащая найденные элементы;
- "чистка таблицы" (или реорганизация таблицы) – удаление из таблицы всех версий элементов, кроме последних.
#---------------------------------------------------------------------
Предусмотреть следующие операции:
- включение нового элемента в таблицу с соблюдением ограничений на уникальность ключей в соответствующих ключевых пространствах
                            (см. предыдущие пункты задания) и уникальности составного ключа (key1, key2);
- поиск в таблице элемента, заданного составным ключом;
- удаление из таблицы элемента, заданного составным ключом;
- поиск в таблице элемента по любому заданному ключу; результатом поиска должна быть копии всех найденных элементов со значениями ключей;
- удаление из таблицы всех элементов, заданного ключом в одном из ключевых пространств;
- вывод содержимого таблицы на экран (или текстовый файл); при этом формат вывода должен соответствовать приведенной выше структуре элемента таблицы;
- особые операции, определяемые в соответствующем пространстве ключей.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//structs------------------------------------
typedef struct InfoType{
  int n1;
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


/*void ItemErase(){
}
*/

//keyspace1--------------------------------------------------------------------------------------------
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
}key2;

typedef struct keyspace2{
    int version;
    key2* key;
}keyspace2;

int bin_find(keyspace1* ks1, int size, int key){
  int l = 0, r = size - 1;
  int m;
  while(l <= r){
    m = (l+r)/2;
    if(ks1[m].val == key){
      return m;
    }
    if (ks1[m].val < key){
      l = m + 1;
    }
    else{
      r = m - 1;
    }
  }
  return -1;
}

/*void k1_find_ver(){
}*/

void NewItem(Table* table, Item* item){
  //insert at 1st keyspace                 to return table->ks1[i].key->item or NULL
  int place = bin_find(table->ks1, table->rsize1, item->k1);
  unsigned int is_new = 1;
  Item* insert = NULL;

  if (place < 0){                       //вставка нового элемента в кс1 
    if (table->rsize1 == table->size1){
      printf("no place*\n");                //return NULL
    }
    else{
      int i = table->rsize1++;
      table->ks1[i].val = item->k1;
      keyspace1 tmp;
      while(i > 0){
        if(table->ks1[i-1].val > table->ks1[i].val){
          tmp = table->ks1[i-1];
          table->ks1[i-1] = table->ks1[i];
          table->ks1[i] = tmp;
          --i;
        }
        else{
          break;
        }
      }
      item->next = NULL;
      item->ver = 0;

      //в начало
      table->ks1[i].key = (key1*)malloc(sizeof(key1));
                                                                //copy item
      table->ks1[i].key->item = (Item*)malloc(sizeof(Item));
      memcpy(table->ks1[i].key->item, item, sizeof(Item));
                                                                //copy key2
      table->ks1[i].key->item->k2 = (char*)malloc(strlen(item->k2)+1);
      memcpy(table->ks1[i].key->item->k2, item->k2, strlen(item->k2)+1);

      table->ks1[i].key->next = NULL;
      table->ks1[i].version = 0;             //return table->ks1[i].key->item
      table->ks1[i].key->parent = NULL;
      table->ks1[i].key->item->ptr1 = table->ks1[i].key;

      insert = table->ks1[i].key->item;
    }
  }

  else{
    key1* ptr = table->ks1[place].key;
    unsigned int is_find = 0;
    //поиск по второму ключу
    while(1){
        //если есть элемент с совпадающим вторым ключом
        if(strcmp(ptr->item->k2, item->k2) == 0){
            is_find = 1;
            is_new = 0;

            item->ver=ptr->item->ver + 1;

            item->next = ptr->item;

            Item* tmp;
            tmp = (Item*)malloc(sizeof(Item));
            memcpy(tmp, item, sizeof(Item));

            tmp->k2=(char*)malloc(strlen(item->k2) + 1);
            memcpy(tmp->k2, item->k2, strlen(item->k2) + 1);

            ptr->item = tmp;
            ptr->item->ptr1 = ptr;
            ptr->item->ptr2 = ptr->item->next->ptr2;
            ptr->item->next->ptr2->item = ptr->item;
            insert = ptr->item;
            break;
        }
        if(ptr->next == NULL){
          break;
        }
        ptr = ptr->next;
    }
    if(is_find == 0){
      //insert in begin of list 
      key1* nw = (key1*)malloc(sizeof(key1));

      item->next = NULL;
      item->ver = 0;

      nw->next = table->ks1[place].key;
      nw->next->parent = nw;
      nw->parent = NULL;

      table->ks1[place].version += 1;
      //copy elem (dest, item)
      nw->item = (Item*)malloc(sizeof(Item));
      memcpy(nw->item, item, sizeof(Item));

      nw->item->k2 = (char*)malloc(strlen(item->k2) + 1);
      memcpy(nw->item->k2, item->k2, strlen(item->k2) + 1);

      nw->item->ptr1 = nw;

      table->ks1[place].key = nw;

      insert = table->ks1[place].key->item;

    }
    //-------------------------------
  }
  //insert in keyspace2
  if(insert != NULL){
    if(table->ks2[item->hash2].key == NULL){
      table->ks2[item->hash2].key= (key2*)malloc(sizeof(key2));
      table->ks2[item->hash2].key->item = insert;
      table->ks2[item->hash2].key->next = NULL;
      table->ks2[item->hash2].key->parent = NULL;
      table->ks2[item->hash2].version = 0;
      table->ks2[item->hash2].key->item->ptr2 = table->ks2[item->hash2].key;
    }
    else{
      if(is_new != 0){
        key2* nw2 = (key2*)malloc(sizeof(key2));

        nw2->item = insert;
        nw2->item->ptr2 = nw2;

        nw2->next = table->ks2[item->hash2].key;
        nw2->next->parent = nw2;
        nw2->parent = NULL;

        table->ks2[item->hash2].version += 1;

        table->ks2[item->hash2].key = nw2;
      }
    }
  }

}
/*
void k1_search_one(){
}
void k1_search_many(){
}

void k1_erase_uno(){
}
void k1_update_ver(){
}

void k1_erase_all(){
}
*/
//keyspace2-------------------------------------------------------------------------------------


int hash(char* str, int size){
  int hs = 0;
  for(int i = 0; i < strlen(str); i++){
    hs = hs^str[i] + hs;
  }
  return abs(hs%size);
}

/*
void k2_search_uno(){
}
void k2_reorginize(){
}*/

void output_1(Table* table){
  for(int i = 0; i < table->rsize1; i++){
    printf("\n\n%d:\n", table->ks1[i].val);
    key1* ptr = table->ks1[i].key;
    Item* itptr;
    int j = 0;
    while(ptr != NULL){
      printf(" %d\n",table->ks1[i].version-j);
      j++;
      itptr = ptr->item;
      while(itptr != NULL){
        printf("hash: %d, key2:%s, v:%d  ", itptr->hash2, itptr->k2, itptr->ver);
        itptr = itptr->next;
      }
      printf("\n");
      ptr = ptr->next; 
    }
  }
}

void output_2(Table* table){
  for(int i = 0; i < table->size2; i++){
    if(table->ks2[i].key != NULL){
      printf("\n\nhash: %d\n", i);
      key2* ptr = table->ks2[i].key;
      Item* itptr;
      int j = 0;
      while(ptr != NULL){
        printf(" %d\n",table->ks2[i].version-j);
        j++;
        itptr = ptr->item;
        while(itptr != NULL){
          printf("k1: %d, key2:%s, v:%d;  ", itptr->k1, itptr->k2, itptr->ver);
          itptr = itptr->next;
        }
        printf("\n");
        ptr = ptr->next;
      }
    }
  }
}

//main-------------------------------------------------------------------------------------------------
void check(){
  printf("yes\n");
}

int main()
{
  int n = 0;
  keyspace1* ks1;
  keyspace2* ks2;
  int size1, size2;
  scanf("%d%d", &size1, &size2);
  ks1 = (keyspace1*)malloc(size1 * sizeof(keyspace1));
  ks1[0].val = -1;
  ks2 = (keyspace2*)malloc(size2 * sizeof(keyspace2));
  for (int i = 0; i < size2; i++) {
        ks2[i].key = NULL;
  }
  Table table;
  table.ks1 = ks1;
  table.ks2 = ks2;
  table.size1 = size1;
  table.size2 = size2;
  table.rsize1 = 0;

  int key1;
  char* key2;

  InfoType info;
  Item new;
  while(n != -1){
    key2 = (char*)malloc(50);

    scanf("%d", &key1);
    scanf("%s", key2);
    n = scanf("%d", &(info.n1));
    //считали информацию об элементе
    new.k1 = key1;
    new.k2 = key2;
    new.hash2 = hash(key2, size2);
    new.info = info;
    NewItem(&table, &new);
    output_1(&table);
    printf("#-----------------------");
    output_2(&table);

    free(key2);
  }

  return 0;
}
