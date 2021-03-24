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

todo
output all
insert                          done
delete by key1                  done
delete by key1 + ver
reorginize
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//structs------------------------------------
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

int hash(char*, int);

int bin_find(keyspace1* ks1, int size, unsigned int key){
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

      table->ks1[i].key->item->info.s1 = (char*)malloc(strlen(item->info.s1)+1);
      memcpy(table->ks1[i].key->item->info.s1, item->info.s1, strlen(item->info.s1)+1);

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

            tmp->info.s1=(char*)malloc(strlen(item->info.s1) + 1);
            memcpy(tmp->info.s1, item->info.s1, strlen(item->info.s1) + 1);

            ptr->item = tmp;
            ptr->item->ptr1 = ptr;
            ptr->item->ptr2 = ptr->item->next->ptr2;
            //обновить указатель на итемы
            ptr->item->ptr2->item = ptr->item;
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

      nw->item->info.s1 = (char*)malloc(strlen(item->info.s1) + 1);
      memcpy(nw->item->info.s1, item->info.s1, strlen(item->info.s1) + 1);

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
}*/

void k1_erase_uno(Table* table, unsigned int key, int ver){
  key1* ptr = table->ks1->key;
}

void free_item(Item* item){
  free(item->k2);
  free(item->info.s1);
  free(item);
}

void check();
void output_2(Table*);
void output_1(Table*);

void k1_erase_all(Table* table, unsigned int key){
  int pos = bin_find(table->ks1, table->rsize1, key);
  if(pos < 0){
    printf("\nТакого ключа нет\n");
  }
  else{
    key1* ptr = table->ks1[pos].key;
    key1* ptr_nxt;
    while(ptr != NULL){
      //delete key from ks2
      if(ptr->item->ptr2->parent != NULL){
        ptr->item->ptr2->parent->next = ptr->item->ptr2->next;
        if(ptr->item->ptr2->next != NULL){
          ptr->item->ptr2->next->parent = ptr->item->ptr2->parent;
        }
      }
      else{
        if(ptr->item->ptr2->next != NULL){
          table->ks2[ptr->item->hash2].key = ptr->item->ptr2->next;
          table->ks2[ptr->item->hash2].key->parent = NULL;
        }
        else{
          table->ks2[ptr->item->hash2].key = NULL;
        }
      }
      table->ks2[ptr->item->hash2].version -= 1;
      free(ptr->item->ptr2);

      //delete from ks1
      Item* itptr = ptr->item;
      Item* itptr_nxt;
      while(itptr != NULL){
        itptr_nxt = itptr->next;
        free_item(itptr);
        itptr = itptr_nxt;
      }
      ptr_nxt = ptr->next;
      free(ptr);
      ptr = ptr_nxt;
      output_2(table);
    }
    check();
    memmove(table->ks1 + pos, table->ks1 + pos + 1, (table->rsize1 - pos - 1)*sizeof(keyspace1));
    table->rsize1 -= 1;
  }
}

void k2_erase_one(Table* table, key2* ptr){
  int pos = bin_find(table->ks1, table->rsize1, ptr->item->k1);
  if(ptr->item->ptr1->parent != NULL){
    ptr->item->ptr1->parent->next = ptr->item->ptr1->next;
    if(ptr->item->ptr1->next != NULL){
        ptr->item->ptr1->next->parent = ptr->item->ptr1->parent;
    }
    table->ks1[pos].version -= 1;
  }
  //in the begin
  else{
    if(ptr->item->ptr1->next != NULL){
      table->ks1[pos].key = ptr->item->ptr1->next;
      table->ks1[pos].key->parent = NULL;
      table->ks1[pos].version -= 1;
    }
    else{
      memmove(table->ks1 + pos, table->ks1 + pos + 1, (table->rsize1 - pos - 1) * sizeof(keyspace1));
      table->rsize1 -= 1;
    }
  }
  free(ptr->item->ptr1);

  //delete from ks2

  if(ptr->parent != NULL){
    ptr->parent->next = ptr->next;
    if(ptr->next != NULL){
      ptr->next->parent = ptr->parent;
    }
  }
  else{
    if(ptr->next != NULL){
      table->ks2[ptr->item->hash2].key = ptr->next;
      table->ks2[ptr->item->hash2].key->parent = NULL;
    }
    else{
      table->ks2[ptr->item->hash2].key = NULL;
    }
  }
  table->ks2[ptr->item->hash2].version -= 1;
  
  Item* itptr = ptr->item;
    Item* itptr_nxt;
    while (itptr != NULL) {
        itptr_nxt = itptr->next;
        free_item(itptr);
        itptr = itptr_nxt;
    }
}


void k2_erase_all(Table* table, char* k2){
  int hash2 = hash(k2, table->size2);
  if(table->ks2[hash2].key == NULL){
    printf("\nThis key isn't exist\n");
  }
  key2* ptr = table->ks2[hash2].key;
  key2* ptr_nxt;
  while(ptr != NULL){
    //erase from key2;
    ptr_nxt = ptr->next;
    if(strcmp(ptr->item->k2, k2) == 0){
      printf("\n    key:  %s\n", ptr->item->k2);
      k2_erase_one(table, ptr);
      free(ptr);
    } 
    ptr = ptr_nxt;
  }
}

void k2_reorganize(Table* table, char* k2){
  int hash2 = hash(k2, table->size2);
  if(table->ks2[hash2].key == NULL){
    printf("\nThis key isn't exist\n");
  }
  unsigned int is_find = 0;
  key2* ptr = table->ks2[hash2].key;
  key2* ptr_nxt;
  while(ptr != NULL){
    //erase from key2;
    ptr_nxt = ptr->next;
    if(strcmp(ptr->item->k2, k2) == 0){
      printf("\n    key:  %s\n", ptr->item->k2);
      if(is_find != 0){
        k2_erase_one(table, ptr);
        free(ptr);
      }
      else is_find = 1;
    } 
    ptr = ptr_nxt;
  }
}
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
void k2_reorganize(){
}*/

void output_1(Table* table){
  for(int i = 0; i < table->rsize1; i++){
    printf("\n\n%d:\n", table->ks1[i].val);
    key1* ptr = table->ks1[i].key;
    Item* itptr;
    int j = 0;
    while(ptr != NULL){
      printf("\npointer %p", ptr);
      printf("\nparent %p", ptr->parent);
      printf("\nchild %p", ptr->next);
      printf(" %d\n",table->ks1[i].version-j++);
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
        printf(" %d\n",table->ks2[i].version-j++);
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

  unsigned int key1;
  char* key2;

  InfoType info;
  Item new;
  int ans;
  while(n != -1){
    printf("Input oper: 1) new; 2)erase all by 1; 3)erase all by 2; 4)reorganize by key2; -1)exit\n");
    n = scanf("%d", &ans);
    if(ans == 1){
      key2 = (char*)malloc(50);
      scanf("%d", &key1);
      scanf("%s", key2);

      scanf("%f", &(info.f1));
      scanf("%f", &(info.f2));
      info.s1 = (char*)malloc(50);
      scanf("%s", info.s1);
      //считали информацию об элементе
      new.k1 = key1;
      new.k2 = key2;
      new.hash2 = hash(key2, size2);
      new.info = info;
      NewItem(&table, &new);
      free(key2);
      free(info.s1);
    }

    else if(ans == 2){
      scanf("%d", &key1);
      k1_erase_all(&table, key1);
    }

    else if(ans == 3){
      char* k2 = (char*)malloc(50);
      scanf("%s", k2);
      k2_erase_all(&table, k2);
      free(k2);
    }

    else if(ans == 4){
      char* k2 = (char*)malloc(50);
      scanf("%s", k2);
      k2_reorganize(&table, k2);
      free(k2);
    }

    else if(ans == -1){
      printf("\n\nThats all\n");
      break;
    }
    output_1(&table);
    printf("#-----------------------");
    output_2(&table);

  }
  free(ks1);
  free(ks2);

  return 0;
}
