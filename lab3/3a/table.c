#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int bin_find(keyspace1* ks1, int size, unsigned int key, int* pos){
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

  if(size > 0 && key > ks1[size - 1].val)
    *pos = size;
  else
    *pos = l;
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
  int rpos;
  int place = bin_find(table->ks1, table->rsize1, item->k1, &rpos);
  printf("\n        real position %d %d\n", rpos, place);
  unsigned int is_new = 1;
  Item* insert = NULL;

  if (place < 0){                       
    if (table->rsize1 == table->size1){
      printf("No place\n");                //return NULL
    }
    else{
      printf("\nyes1\n");
      int i = table->rsize1++;
      /*table->ks1[i].val = item->k1;
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
      }*/
      if(rpos == table->rsize1 - 1){
        printf("\nyes\n");
        table->ks1[i].val = item->k1;
      }
      else{
        printf("\nno\n");
        printf("\n%d\n", rpos);
        memmove(table->ks1 + rpos + 1, table->ks1 + rpos, (table->rsize1 - rpos - 1)*sizeof(keyspace1));
        table->ks1[rpos].val = item->k1;
      }
      i = rpos;
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
      //table->ks2[item->hash2].version = 0;
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

        //table->ks2[item->hash2].version += 1;

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
  int rpos;
  int pos = bin_find(table->ks1, table->rsize1, k1, &rpos);
  if(pos < 0){
    printf("\nThis key dont exist\n");
  }
  else{
    if(table->ks1[pos].version < ver){
      printf("\nThis version dont exist\n");
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
      //table->ks2[ptr->item->hash2].version -= 1;
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
  int rpos;
  int pos = bin_find(table->ks1, table->rsize1, key, &rpos);
  if(pos < 0){
    printf("\nThis key dont exist\n");
  }
  else{
    key1* ptr = table->ks1[pos].key;
    key1* ptr_nxt;
    while(ptr != NULL){
      //delete key from ks2
      relink_2(table, ptr);
      //table->ks2[ptr->item->hash2].version -= 1;
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
  int rpos;
  int pos = bin_find(table->ks1, table->rsize1, ptr->item->k1, &rpos);
  relink_1(table, ptr->item->ptr1, pos);
  relink_2(table, ptr->item->ptr1);

  free(ptr->item->ptr1);

  //delete from ks2

  //table->ks2[ptr->item->hash2].version -= 1;
  
  free_item(ptr->item);
}


void k2_erase_all(Table* table, char* k2){
  int hash2 = hash(k2, table->size2);
  if(table->ks2[hash2].key == NULL){
    printf("\nThis key dont exist\n");
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
    printf("\nThis key dont exist\n");
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
  if(is_find == 0){
    printf("\nThis key dont exist\n");
  }
}
//-----------------------
void k1_k2_erase(Table* table, int k1, char* k2){
  int rpos;
  int pos = bin_find(table->ks1, table->rsize1, k1, &rpos);
  if(pos < 0){
    printf("\nThis item dont exist\n");
  }
  else{
    key1* ptr = table->ks1[pos].key;
    while(strcmp(ptr->item->k2, k2) != 0){
      ptr = ptr->next;
      if(ptr == NULL){
        printf("\nThis item dont exist\n");
        return;
      }
    }

    //erase from ks2
    relink_2(table, ptr);
    //table->ks2[ptr->item->hash2].version -= 1;
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
  int rpos;
  int pos = bin_find(table->ks1, table->rsize1, key, &rpos);
  if(pos < 0){
    printf("\nThis key dont exist\n");
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
  int rpos;
  int pos = bin_find(table->ks1, table->rsize1, k1, &rpos);
  if(pos < 0){
    printf("\nThis key dont exist\n");
  }
  else{
    if(table->ks1[pos].version < ver){
      printf("\nThis version dont exist\n");
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
    printf("\nThis key dont exist\n");
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
          printf("\nThis version dont exist\n");
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
    printf("\nThis item dont exist\n");
  }
}

void output_1_2(Table* table, unsigned int k1, char* k2){
  int rpos;
  int pos = bin_find(table->ks1, table->rsize1, k1, &rpos);
  unsigned int is_find = 0;
  if(pos < 0){
    printf("\nThis key dont exist\n");
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
      printf("\nThis item dont exist\n");
    }
  }
}

void output_all(Table* table){
  if(table->rsize1 == 0){
    printf("\nTable is empty\n");
    return;
  }
  for(int i = 0; i < table->rsize1; i++){
    key1* ptr = table->ks1[i].key;
    while(ptr != NULL){
      output_one(ptr->item);
      ptr = ptr->next; 
    }
  }
}