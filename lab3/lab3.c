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
insert                          done
delete by key1                  done
delete by key1 + ver            done
delete by key2                  done
reorganize by k2                done
delete by 1 + 2                 done
delete all                      done

output all                      done
output by 1                     done
output by 1 + ver               done
output by 2                     done
output by 2 + ver               done
output by 1 + 2                 done

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
//--------------------------------------------------------------

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

int hash(char* str, int size){
  int hs = 0;
  for(int i = 0; i < strlen(str); i++){
    hs = hs^str[i] + hs;
  }
  return abs(hs%size);
}

//-------------------------------------------------------------------

void Copy_Item(Item** dest, Item* source){
  *dest = (Item*)malloc(sizeof(Item));
  memcpy(*dest, source, sizeof(Item));

  (*dest)->k2=(char*)malloc(strlen(source->k2) + 1);
  memcpy((*dest)->k2, source->k2, strlen(source->k2) + 1);

  (*dest)->info.s1=(char*)malloc(strlen(source->info.s1) + 1);
  memcpy((*dest)->info.s1, source->info.s1, strlen(source->info.s1) + 1);
}

void NewItem(Table* table, Item* item){
  //insert at 1st keyspace 
  int place = bin_find(table->ks1, table->rsize1, item->k1);
  unsigned int is_new = 1;
  Item* insert = NULL;

  if (place < 0){                       
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

      //in the begin
      table->ks1[i].key = (key1*)malloc(sizeof(key1));
      Copy_Item(&(table->ks1[i].key->item), item);
      
      table->ks1[i].key->next = NULL;
      table->ks1[i].version = 0;      
      table->ks1[i].key->parent = NULL;
      table->ks1[i].key->item->ptr1 = table->ks1[i].key;

      insert = table->ks1[i].key->item;
    }
  }

  else{
    key1* ptr = table->ks1[place].key;
    unsigned int is_find = 0;
    //поиск по второму ключу
    do{
      //если есть элемент с совпадающим вторым ключом
      if(strcmp(ptr->item->k2, item->k2) == 0){
        is_find = 1;
        is_new = 0;

        item->ver=ptr->item->ver + 1;
        item->next = ptr->item;

        Item* tmp;
        Copy_Item(&tmp, item);

        ptr->item = tmp;
        ptr->item->ptr1 = ptr;
        ptr->item->ptr2 = ptr->item->next->ptr2;
          
        ptr->item->ptr2->item = ptr->item;
        insert = ptr->item;
        break;
      }
      ptr = ptr->next;
    }while(ptr != NULL);

    if(is_find == 0){
      //insert in begin of list 
      key1* nw = (key1*)malloc(sizeof(key1));

      item->next = NULL;
      item->ver = 0;

      nw->next = table->ks1[place].key;
      nw->next->parent = nw;
      nw->parent = NULL;

      table->ks1[place].version += 1;

      Copy_Item(&(nw->item), item);

      nw->item->ptr1 = nw;
      table->ks1[place].key = nw;
      insert = table->ks1[place].key->item;
    }
  }
  //insert in keyspace2
  if(insert != NULL){
    if(table->ks2[item->hash2].key == NULL){
      table->ks2[item->hash2].key= (key2*)malloc(sizeof(key2));
      table->ks2[item->hash2].key->item = insert;
      table->ks2[item->hash2].key->next = NULL;
      table->ks2[item->hash2].key->parent = NULL;
      table->ks2[item->hash2].version = 0;
      table->ks2[item->hash2].key->v2 = 0;
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
        table->ks2[item->hash2].key->v2 = 0;
        
        //seek the element version
        key2* ptr2 = table->ks2[item->hash2].key->next;
        while(ptr2 != NULL){
          if(strcmp(table->ks2[item->hash2].key->item->k2, ptr2->item->k2) == 0){
            table->ks2[item->hash2].key->v2 = ptr2->v2 + 1;
            break;
          }
          ptr2 = ptr2->next;
        }
      }
    }
  }
}
//---------------------------------------------------------------

void free_item(Item* ptr){
  Item* itptr = ptr;
  Item* itptr_nxt;
  while(itptr != NULL){
    itptr_nxt = itptr->next;
    free(itptr->k2);
    free(itptr->info.s1);
    free(itptr);
    itptr = itptr_nxt;
  }
}

void relink_1(Table* table, key1* ptr, int pos){
  if(ptr->parent != NULL){
    ptr->parent->next = ptr->next;
    if(ptr->next != NULL){
      ptr->next->parent = ptr->parent;
    }
    table->ks1[pos].version -= 1;
  }
  //in the begin
  else{
    if(ptr->next != NULL){
      table->ks1[pos].key = ptr->next;
      table->ks1[pos].key->parent = NULL;
      table->ks1[pos].version -= 1;
    }
    else{
      memmove(table->ks1 + pos, table->ks1 + pos + 1, (table->rsize1 - pos - 1) * sizeof(keyspace1));
      table->rsize1 -= 1;
    }
  }
}

void relink_2(Table* table, key1* ptr){
  if(ptr->item->ptr2->parent != NULL){
    ptr->item->ptr2->parent->next = ptr->item->ptr2->next;
    if(ptr->item->ptr2->next != NULL){
      ptr->item->ptr2->next->parent = ptr->item->ptr2->parent;
    }

    key2* ptr2 = ptr->item->ptr2->parent;
    while(ptr2 != NULL){
      if(strcmp(ptr2->item->k2, ptr->item->k2) == 0){
        ptr2->v2 -= 1;
      }
      ptr2 = ptr2->parent;
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
}
//-----------------------------------------------------------
void k1_erase_one(Table* table, int k1, int ver){
  int pos = bin_find(table->ks1, table->rsize1, k1);
  if(pos < 0){
    printf("\nТакого ключа нет\n");
  }
  else{
    if(table->ks1[pos].version < ver){
      printf("\nЭлемента с такой версией нет\n");
    }
    else{
      int j = 0;
      key1* ptr = table->ks1[pos].key;
      while(table->ks1[pos].version - j != ver){
        ptr = ptr->next;
        ++j;
      }

      //erase from ks2
      relink_2(table, ptr);
      table->ks2[ptr->item->hash2].version -= 1;
      free(ptr->item->ptr2);
      //----------------------------
      free_item(ptr->item);
      //----------------------------
      
      relink_1(table, ptr, pos);
      free(ptr);
    }
  }
}

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
      relink_2(table, ptr);
      table->ks2[ptr->item->hash2].version -= 1;
      free(ptr->item->ptr2);

      //delete from ks1
      free_item(ptr->item);

      ptr_nxt = ptr->next;
      free(ptr);
      ptr = ptr_nxt;
    }
    memmove(table->ks1 + pos, table->ks1 + pos + 1, (table->rsize1 - pos - 1)*sizeof(keyspace1));
    table->rsize1 -= 1;
  }
}
//------------------

void k2_erase_one(Table* table, key2* ptr){

  int pos = bin_find(table->ks1, table->rsize1, ptr->item->k1);
  relink_1(table, ptr->item->ptr1, pos);
  relink_2(table, ptr->item->ptr1);

  free(ptr->item->ptr1);

  //delete from ks2

  table->ks2[ptr->item->hash2].version -= 1;
  
  free_item(ptr->item);
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
      if(is_find != 0){
        k2_erase_one(table, ptr);
        free(ptr);
      }
      else{
        is_find = 1;
        ptr->v2 = 0;
      } 
    } 
    ptr = ptr_nxt;
  }
}
//-----------------------
void k1_k2_erase(Table* table, int k1, char* k2){
  int pos = bin_find(table->ks1, table->rsize1, k1);
  if(pos < 0){
    printf("\nТакого ключа нет\n");
  }
  else{
    key1* ptr = table->ks1[pos].key;
    while(strcmp(ptr->item->k2, k2) != 0){
      ptr = ptr->next;
      if(ptr == NULL){
        printf("\nНе найдено\n");
        return;
      }
    }

    //erase from ks2
    relink_2(table, ptr);
    table->ks2[ptr->item->hash2].version -= 1;
    free(ptr->item->ptr2);
    //----------------------------
    free_item(ptr->item);
    //----------------------------
    relink_1(table, ptr, pos);
    free(ptr);
  }
}

void delete_all(Table* table){
  int pos;
  while(table->rsize1 > 0){
    pos = (table->rsize1 - 1) / 2;
    k1_erase_all(table, table->ks1[pos].val);
  }
  free(table->ks1);
  free(table->ks2);
}
//output-------------------------------------------------------------------------------------


void output_one(Item* itptr){
    printf("key 1:%d, key2:%s\n", itptr->k1, itptr->k2);
    while(itptr != NULL){
      printf("\t%f, %f, %s\n", itptr->info.f1, itptr->info.f2, itptr->info.s1);
      itptr = itptr->next;
    }
    printf("\n");
}

void output_1(Table* table, unsigned int key){
  int pos = bin_find(table->ks1, table->rsize1, key);
  if(pos < 0){
    printf("\nТакого ключа нет\n");
  }
  else{
    key1* ptr = table->ks1[pos].key;
    while(ptr != NULL){
      output_one(ptr->item);
      ptr = ptr->next; 
    }
  }
}

void output_1_1(Table* table, unsigned int k1, int ver){
  int pos = bin_find(table->ks1, table->rsize1, k1);
  if(pos < 0){
    printf("\nТакого ключа нет\n");
  }
  else{
    if(table->ks1[pos].version < ver){
      printf("\nЭлемента с такой версией нет\n");
    }
    else{
      int j = 0;
      key1* ptr = table->ks1[pos].key;
      while(table->ks1[pos].version - j != ver){
        ptr = ptr->next;
        ++j;
      }
      output_one(ptr->item);
    }
  }
}

void output_2(Table* table, char* k2){
  int pos = hash(k2, table->size2);
  unsigned int is_find = 0;
  
  key2* ptr = table->ks2[pos].key;
  while(ptr != NULL){
    if(strcmp(ptr->item->k2, k2) == 0){
      is_find = 1;
      output_one(ptr->item);
    }
    ptr = ptr->next;
  }
  if(is_find == 0){
    printf("\nТакого ключа нет\n");
  }
}

void output_2_2(Table* table, char* k2, int ver){
  int pos = hash(k2, table->size2);
  unsigned int is_find = 0;
  
  key2* ptr = table->ks2[pos].key;
  while(ptr != NULL){
    if(strcmp(ptr->item->k2, k2) == 0){
      if(is_find == 0){
        is_find = 1;
        if(ptr->v2 < ver){
          printf("\nЭлемента с такой версией нет\n");
          break;
        }
      }
      if(ptr->v2 == ver){
        output_one(ptr->item);
        break;
      }
    }
    ptr = ptr->next;
  }
  if(is_find == 0){
    printf("\nТакого ключа нет\n");
  }
}

void output_1_2(Table* table, unsigned int k1, char* k2){
  int pos = bin_find(table->ks1, table->rsize1, k1);
  unsigned int is_find = 0;
  if(pos < 0){
    printf("\nТакого ключа нет\n");
  }
  else{
    key1* ptr = table->ks1[pos].key;
    while(ptr != NULL){
      if(strcmp(ptr->item->k2, k2) == 0){
        output_one(ptr->item);
        is_find = 1;
        break;
      }
      ptr = ptr->next;
    }
    if(is_find == 0){
      printf("Такой комбинации ключей нет");
    }
  }
}

void output_all(Table* table){
  for(int i = 0; i < table->rsize1; i++){
    key1* ptr = table->ks1[i].key;
    while(ptr != NULL){
      output_one(ptr->item);
      ptr = ptr->next; 
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
    printf("Input oper: 1) new; 2)erase all by 1; 3)erase all by 2; \n4)reorganize by key2; 5)delete by 1 + ver  6)delete by 1 + 2 -1)exit\n");
    printf("7)output all; 8)output by k1; 9)output by k1 + ver; 10)output by k2; 11)output by k2 + ver; 12)output by k1 + k2; \n");
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

    else if(ans == 5){
      int key1;
      int ver;
      scanf("%d%d", &key1, &ver);
      k1_erase_one(&table, key1, ver);
    }

    else if(ans == 6){
      int key1;
      char* key2 = (char*)malloc(50);
      scanf("%d%s", &key1, key2);
      k1_k2_erase(&table, key1, key2);
      free(key2);
    }

    else if(ans == -1){
      printf("\n\nThats all\n");
      delete_all(&table);
      break;
    }

    else if(ans == 7){
      output_all(&table);
    }

    else if(ans == 8){
      int key1;
      scanf("%d", &key1);
      output_1(&table, key1);
    }

    else if(ans == 9){
      int key1;
      int ver;
      scanf("%d%d", &key1, &ver);
      output_1_1(&table, key1, ver);
    }


    else if(ans == 10){
      char* k2 = (char*)malloc(50);
      scanf("%s", k2);
      output_2(&table, k2);
      free(k2);
    }

    else if(ans == 11){
      int ver;
      char* k2 = (char*)malloc(50);
      scanf("%s%d", k2, &ver);
      output_2_2(&table, k2, ver);
      free(k2);
    }

    else if(ans == 12){
      int key1;
      char* key2 = (char*)malloc(50);
      scanf("%d%s", &key1, key2);
      output_1_2(&table, key1, key2);
      free(key2);
    }
  }
  return 0;
}
