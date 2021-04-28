#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int bin_find(FILE* ks1, int size, unsigned int key, int* pos){
  int l = 0, r = size - 1;
  int m;
  int cur = 0;

  while(l <= r){
    m = (l+r)/2;
    fseek(ks1, m*sizeof(keyspace1), SEEK_SET);
    fread(&cur, sizeof(int), 1, ks1);
    if(cur == key){
      *pos = m;
      return m;
    }
    if (cur < key){
      l = m + 1;
    }
    else{
      r = m - 1;
    }
  }

  fseek(ks1, (size-1)*sizeof(keyspace1), SEEK_SET);
  fread(&cur, sizeof(int), 1, ks1);

  if(key > cur)
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
void NewItem(Table* table, Item2* it2, char* k2, char* s1){
  //insert at 1st keyspace 
  int rpos = 0;
  int place = bin_find(table->ks1, table->rsize1, it2->k1, &rpos);
  unsigned int is_new = 1;

  if (place < 0){                       
    if (table->rsize1 == table->size1){
      printf("No place\n");                //return NULL
      return;
    }
    else{
      fseek(table->items, 0, SEEK_END);
      fseek(table->ks1, 0, SEEK_END);
      fseek(table->ks2, 0, SEEK_END);

      it2->ver = 0;
      it2->offset_next = 0;
      it2->offset_key1 = (long int)ftell(table->ks1);
      it2->offset_key2 = (long int)ftell(table->ks2);

      key1 k1 = {0};
      k1.alive = 1;
      k1.offset_next = 0;
      k1.offset_parent = 0;
      k1.offset_item = ftell(table->items);

      fseek(table->ks1, 0, SEEK_END);
      fwrite(&k1, sizeof(key1), 1, table->ks1);

      keyspace1 nw = {0};

      nw.val = it2->k1;
      nw.offset_key1 = it2->offset_key1;
      nw.version = 0;
      int i = table->rsize1++;

      //add key1 as new elem
      if(rpos == table->rsize1 - 1){
        fseek(table->ks1, rpos*sizeof(keyspace1), SEEK_SET);
        fwrite(&nw, sizeof(keyspace1), 1, table->ks1);
      }
      else{
        //memmove(table->ks1 + rpos + 1, table->ks1 + rpos, (table->rsize1 - rpos - 1)*sizeof(keyspace1));
        keyspace1* movable;
        fseek(table->ks1, rpos*sizeof(keyspace1), SEEK_SET);
        movable = (keyspace1*)calloc((table->rsize1 - rpos - 1), sizeof(keyspace1));
        fread(movable, sizeof(keyspace1), (table->rsize1 - rpos - 1), table->ks1);
        fseek(table->ks1, rpos*sizeof(keyspace1), SEEK_SET);
        fwrite(&nw, sizeof(keyspace1), 1, table->ks1);
        fwrite(movable, sizeof(keyspace1), (table->rsize1 - rpos - 1), table->ks1);
        free(movable);
        //memmove
      }
    }
  }
  
  else{
    keyspace1 ksptr;
    fseek(table->ks1, place*sizeof(keyspace1), SEEK_SET);
    fread(&ksptr, sizeof(keyspace1), 1, table->ks1);

    key1 ptr;
    fseek(table->ks1, ksptr.offset_key1, SEEK_SET);
    fread(&ptr, sizeof(key1), 1, table->ks1);

    long int cur_ptr_offs = ksptr.offset_key1;

    key2 ptrkey2;
    unsigned int is_find = 0;
    //поиск по второму ключу
    do{
      Item2 itptr;
      fseek(table->items, ptr.offset_item, SEEK_SET);
      fread(&itptr, sizeof(Item2), 1, table->items);

      char* ptrk2 = (char*)calloc(itptr.len_k2, 1);
      fread(ptrk2, 1, itptr.len_k2, table->items);
      //если есть элемент с совпадающим вторым ключом
      if(strcmp(ptrk2, k2) == 0){
        free(ptrk2);
        is_find = 1;
        is_new = 0;
    
        it2->ver = itptr.ver + 1;
        it2->offset_next = ptr.offset_item;

        fseek(table->items, 0, SEEK_END);
        ptr.offset_item = ftell(table->items);
        
        it2->offset_key1 = itptr.offset_key1;
        it2->offset_key2 = itptr.offset_key2;


        fseek(table->ks2, itptr.offset_key2, SEEK_SET);
        fread(&ptrkey2, sizeof(key2), 1, table->ks2);
        ptrkey2.offset_item = ptr.offset_item;
        fseek(table->ks2, 0-sizeof(key2), SEEK_CUR);
        fwrite(&ptrkey2, sizeof(key2), 1, table->ks2);
        
        fseek(table->ks1, cur_ptr_offs, SEEK_SET);
        fwrite(&ptr, sizeof(key1), 1, table->ks1);


        fseek(table->items, 0, SEEK_END);
        fwrite(it2, sizeof(Item2), 1, table->items);
        fwrite(k2, 1, it2->len_k2, table->items);
        fwrite(s1, 1, it2->info.len_s1, table->items);
        break;
      }
      free(ptrk2);
      if(ptr.offset_next == 0){
        break;
      }
      cur_ptr_offs = ptr.offset_next;
      fseek(table->ks1, ptr.offset_next, SEEK_SET);
      fread(&ptr, sizeof(key1), 1, table->ks1);
    }while(1);

    if(is_find == 0){
      //insert in begin of list 

      fseek(table->ks1, 0, SEEK_END);
      fseek(table->ks2, 0, SEEK_END);

      it2->offset_next = 0;
      it2->offset_key1 = (long int)ftell(table->ks1);
      it2->offset_key2 = (long int)ftell(table->ks2);
      it2->ver = 0;


      key1 nw = {0};
      nw.offset_next = ksptr.offset_key1;
      nw.offset_parent = 0;
      nw.alive = 1;

      //nw->next->parent = nw;
      key1 old;
      fseek(table->ks1, ksptr.offset_key1, SEEK_SET);
      fread(&old, sizeof(key1), 1, table->ks1);
      old.offset_parent = it2->offset_key1;

      fseek(table->ks1, -sizeof(key1), SEEK_CUR);
      fwrite(&old, sizeof(key1), 1, table->ks1);       
      //nw->next->parent = nw;

      ksptr.version += 1;
      ksptr.offset_key1 = it2->offset_key1;

      //Copy_Item(&(nw->item), item);
      fseek(table->items, 0, SEEK_END);
      nw.offset_item = (long int)ftell(table->items);

      fseek(table->ks1, 0, SEEK_END);
      fwrite(&nw, sizeof(key1), 1, table->ks1);

      fseek(table->ks1, place*sizeof(keyspace1), SEEK_SET);
      fwrite(&ksptr, sizeof(keyspace1), 1, table->ks1);
    }
  }
  //insert in keyspace2
  fseek(table->ks2, (it2->hash2)*sizeof(keyspace2), SEEK_SET);
  keyspace2 ksptr2;
  fread(&ksptr2, sizeof(keyspace2), 1, table->ks2);
  if(ksptr2.offset_key2 == 0){
    key2 ks2 = {0};
    ksptr2.offset_key2 = it2->offset_key2;
    ksptr2.version = 0;

    ks2.alive = 1;
    ks2.offset_parent = 0;
    ks2.offset_next = 0;
    ks2.v2 = 0;
    
    fseek(table->items, 0, SEEK_END);
    ks2.offset_item = (long int)ftell(table->items);

    fseek(table->ks2, 0, SEEK_END);
    fwrite(&ks2, sizeof(key2), 1, table->ks2);

    fseek(table->ks2, (it2->hash2)*sizeof(keyspace2), SEEK_SET);
    fwrite(&ksptr2, sizeof(keyspace2), 1, table->ks2);

    fseek(table->items, 0, SEEK_END);
    fwrite(it2, sizeof(Item2), 1, table->items);
    fwrite(k2, 1, it2->len_k2, table->items);
    fwrite(s1, 1, it2->info.len_s1, table->items);
  }
  else{
    if(is_new != 0){
      key2 nw2 = {0};

      nw2.offset_parent = 0;
      nw2.v2 = 0;

      //nw2->next->parent = nw2;
      key2 old;
      fseek(table->ks2, ksptr2.offset_key2, SEEK_SET);
      fread(&old, sizeof(key2), 1, table->ks2);
      old.offset_parent = it2->offset_key2;

      fseek(table->ks2, ksptr2.offset_key2, SEEK_SET);
      fwrite(&old, sizeof(key2), 1, table->ks2);  
      //nw2->next->parent = nw2; 

      fseek(table->items, 0, SEEK_END);
      nw2.offset_item = (long int)ftell(table->items);
      nw2.offset_next = ksptr2.offset_key2;

      ksptr2.offset_key2 = it2->offset_key2;
      ksptr2.version += 1;
        
      //seek the element version
      while(1){
        Item2 itptr;
        fseek(table->items, old.offset_item, SEEK_SET);
        fread(&itptr, sizeof(Item2), 1, table->items);
      
        char* ptrk2 = (char*)calloc(itptr.len_k2, 1);
        fread(ptrk2, 1, itptr.len_k2, table->items);

        if(strcmp(k2, ptrk2) == 0){
          nw2.v2 = old.v2 + 1;
          free(ptrk2);
          break;
        }
        free(ptrk2);
        if(old.offset_next == 0)
          break;
        fseek(table->ks2, old.offset_next, SEEK_SET);
        fread(&old, sizeof(key2), 1, table->ks2);
      }

      fseek(table->ks2, 0, SEEK_END);
      fwrite(&nw2, sizeof(key2), 1, table->ks2);

      fseek(table->ks2, (it2->hash2)*sizeof(keyspace2), SEEK_SET);
      fwrite(&ksptr2, sizeof(keyspace2), 1, table->ks2);

      fseek(table->items, 0, SEEK_END);
      fwrite(it2, sizeof(Item2), 1, table->items);
      fwrite(k2, 1, it2->len_k2, table->items);
      fwrite(s1, 1, it2->info.len_s1, table->items);
    }
  }
}
//---------------------------------------------------------------

void free_item(long int it_offs, Table* table){
  Item2 ptr;
  while(1){
    fseek(table->items, it_offs, SEEK_SET);
    fread(&ptr, sizeof(Item2), 1, table->items);
    ptr.alive = 0;
    fseek(table->items, it_offs, SEEK_SET);
    fwrite(&ptr, sizeof(Item2), 1, table->items);

    if(ptr.offset_next == 0)
      return;

    it_offs = ptr.offset_next;
  }
}

void relink_1(Table* table, Item2* item, int pos){
  key1 ptr1;
  key1 parent;
  key1 child;
  keyspace1 kptr;
  fseek(table->ks1, pos*sizeof(keyspace1), SEEK_SET);
  fread(&kptr, sizeof(keyspace1), 1, table->ks1);
  //kptr.version -= 1;

  fseek(table->ks1, item->offset_key1, SEEK_SET);
  fread(&ptr1, sizeof(key1), 1, table->ks1);
  ptr1.alive = 0;

  if(ptr1.offset_parent != 0){
    fseek(table->ks1, ptr1.offset_parent, SEEK_SET);
    fread(&parent, sizeof(key1), 1, table->ks1);
    parent.offset_next = ptr1.offset_next;
    fseek(table->ks1, ptr1.offset_parent, SEEK_SET);
    fwrite(&parent, sizeof(key1), 1, table->ks1);

    if(ptr1.offset_next != 0){
      fseek(table->ks1, ptr1.offset_next, SEEK_SET);
      fread(&child, sizeof(key1), 1, table->ks1);
      child.offset_parent = ptr1.offset_parent;
      fseek(table->ks1, ptr1.offset_next, SEEK_SET);
      fwrite(&child, sizeof(key1), 1, table->ks1);
    }
    kptr.version -= 1;
    fseek(table->ks1, pos*sizeof(keyspace1), SEEK_SET);
    fwrite(&kptr, sizeof(keyspace1), 1, table->ks1);
  }
  //in the begin
  else{
    if(ptr1.offset_next != 0){
      kptr.offset_key1 = ptr1.offset_next;
      kptr.version -= 1;
      fseek(table->ks1, ptr1.offset_next, SEEK_SET);
      fread(&child, sizeof(key1), 1, table->ks1);
      child.offset_parent = 0;
      fseek(table->ks1, ptr1.offset_next, SEEK_SET);
      fwrite(&child, sizeof(key1), 1, table->ks1);

      fseek(table->ks1, pos*sizeof(keyspace1), SEEK_SET);
      fwrite(&kptr, sizeof(keyspace1), 1, table->ks1);
    }
    else{
      //memmove(table->ks1 + pos, table->ks1 + pos + 1, (table->rsize1 - pos - 1) * sizeof(keyspace1));
      keyspace1* movable;
      movable = (keyspace1*)calloc((table->rsize1 - pos - 1), sizeof(keyspace1));
      fseek(table->ks1, (pos + 1)*sizeof(keyspace1), SEEK_SET);
      fread(movable, sizeof(keyspace1), (table->rsize1 - pos - 1), table->ks1);
      fseek(table->ks1, pos*sizeof(keyspace1), SEEK_SET);
      fwrite(movable, sizeof(keyspace1), (table->rsize1 - pos - 1), table->ks1);
      free(movable);
      //memmove
      
      table->rsize1 -= 1;
    }
  }
  fseek(table->ks1, item->offset_key1, SEEK_SET);
  fwrite(&ptr1, sizeof(key1), 1, table->ks1);
}


void relink_2(Table* table, Item2* item){
  key2 ptr2;
  key2 parent;
  key2 child;

  fseek(table->ks2, item->offset_key2, SEEK_SET);
  fread(&ptr2, sizeof(key2), 1, table->ks2);
  ptr2.alive = 0;

  if(ptr2.offset_parent != 0){
    fseek(table->ks2, ptr2.offset_parent, SEEK_SET);
    fread(&parent, sizeof(key2), 1, table->ks2);
    parent.offset_next = ptr2.offset_next;
    fseek(table->ks2, ptr2.offset_parent, SEEK_SET);
    fwrite(&parent, sizeof(key2), 1, table->ks2);

    if(ptr2.offset_next != 0){
      fseek(table->ks2, ptr2.offset_next, SEEK_SET);
      fread(&child, sizeof(key2), 1, table->ks2);
      child.offset_parent = ptr2.offset_parent;

      fseek(table->ks2, ptr2.offset_next, SEEK_SET);
      fwrite(&child, sizeof(key2), 1, table->ks2);
    }

    char* ref_str = (char*)calloc(item->len_k2, 1);
    fread(ref_str, 1, item->len_k2, table->items);

    key2 ptr = {0};
    ptr = parent;
    long int cur_offset = ptr2.offset_parent;

    while(1){
      Item2 itptr;
      fseek(table->items, parent.offset_item, SEEK_SET);
      fread(&itptr, sizeof(Item2), 1, table->items);
      
      char* ptrk2 = (char*)calloc(itptr.len_k2, 1);
      fread(ptrk2, 1, itptr.len_k2, table->items);

      if(strcmp(ref_str, ptrk2) == 0){
        parent.v2--;
      }
      fseek(table->ks2, cur_offset, SEEK_SET);
      fwrite(&parent, sizeof(key2), 1, table->ks2);

      free(ptrk2);
      if(parent.offset_parent == 0)
        break;

      cur_offset = parent.offset_parent;
      fseek(table->ks2, parent.offset_parent, SEEK_SET);
      fread(&parent, sizeof(key2), 1, table->ks2);
    }
    free(ref_str);
  }
  else{
    keyspace2 kptr;
    fseek(table->ks2, (item->hash2) * sizeof(keyspace2), SEEK_SET);
    fread(&kptr, sizeof(keyspace2), 1, table->ks2);
    if(ptr2.offset_next != 0){
      fseek(table->ks2, ptr2.offset_next, SEEK_SET);
      fread(&child, sizeof(key2), 1, table->ks2);

      kptr.offset_key2 = ptr2.offset_next;
      child.offset_parent = 0;
      fseek(table->ks2, ptr2.offset_next, SEEK_SET);
      fwrite(&child, sizeof(key2), 1, table->ks2);
    }
    else{
      kptr.offset_key2 = 0;
    }
    fseek(table->ks2, (item->hash2) * sizeof(keyspace2), SEEK_SET);
    fwrite(&kptr, sizeof(keyspace2), 1, table->ks2);
  }

  fseek(table->ks2, item->offset_key2, SEEK_SET);
  fwrite(&ptr2, sizeof(key2), 1, table->ks2);
}

//-----------------------------------------------------------
void k1_erase_one(Table* table, int k1, int ver){
  int rpos;
  int pos = bin_find(table->ks1, table->rsize1, k1, &rpos);
  if(pos < 0){
    printf("\nThis key dont exist\n");
  }
  else{
    keyspace1 kptr;
    fseek(table->ks1, pos*sizeof(keyspace1), SEEK_SET);
    fread(&kptr, sizeof(keyspace1), 1, table->ks1);

    if(kptr.version < ver){
      printf("\nThis version dont exist\n");
    }
    else{
      int j = 0;
      key1 ptr;
      fseek(table->ks1, kptr.offset_key1, SEEK_SET);
      fread(&ptr, sizeof(key1), 1, table->ks1);

      while(kptr.version - j != ver){
        fseek(table->ks1, ptr.offset_next, SEEK_SET);
        fread(&ptr, sizeof(key1), 1, table->ks1);
        ++j;
      }

      //erase from ks2
      Item2 item;
      fseek(table->items, ptr.offset_item, SEEK_SET);
      fread(&item, sizeof(Item2), 1, table->items);

      relink_2(table, &item);
      relink_1(table, &item, pos);
      free_item(ptr.offset_item, table);
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
    keyspace1 kptr;
    fseek(table->ks1, pos*sizeof(keyspace1), SEEK_SET);
    fread(&kptr, sizeof(keyspace1), 1, table->ks1);

    key1 ptr;
    fseek(table->ks1, kptr.offset_key1, SEEK_SET);
    fread(&ptr, sizeof(key1), 1, table->ks1);

    long int cur_offs = kptr.offset_key1;

    while(1){
      //delete key from ks2
      Item2 item;
      fseek(table->items, ptr.offset_item, SEEK_SET);
      fread(&item, sizeof(Item2), 1, table->items);

      relink_2(table, &item);

      //delete from ks1
      free_item(ptr.offset_item, table);
      
      ptr.alive = 0;
      fseek(table->ks1, cur_offs, SEEK_SET);
      fwrite(&ptr, sizeof(key1), 1, table->ks1);
      
      if(ptr.offset_next == 0)
        break;

      cur_offs = ptr.offset_next;
      fseek(table->ks1, ptr.offset_next, SEEK_SET);
      fread(&ptr, sizeof(key1), 1, table->ks1);
    }
    //memmove(table->ks1 + pos, table->ks1 + pos + 1, (table->rsize1 - pos - 1)*sizeof(keyspace1));
    keyspace1* movable;
    movable = (keyspace1*)calloc((table->rsize1 - pos - 1), sizeof(keyspace1));
    fseek(table->ks1, (pos + 1)*sizeof(keyspace1), SEEK_SET);
    fread(movable, sizeof(keyspace1), (table->rsize1 - pos - 1), table->ks1);
    fseek(table->ks1, pos*sizeof(keyspace1), SEEK_SET);
    fwrite(movable, sizeof(keyspace1), (table->rsize1 - pos - 1), table->ks1);
    free(movable);
    //memmove

    table->rsize1 -= 1;
  }
}
//------------------

void k2_erase_one(Table* table, Item2* item, long int offs_item){
  int rpos;
  int pos = bin_find(table->ks1, table->rsize1, item->k1, &rpos);
  relink_1(table, item, pos);
  relink_2(table, item);
  free_item(offs_item, table);
}


void k2_erase_all(Table* table, char* k2){
  int hash2 = hash(k2, table->size2);
  keyspace2 kptr;
  fseek(table->ks2, hash2*sizeof(keyspace2), SEEK_SET);
  fread(&kptr, sizeof(keyspace2), 1, table->ks2);

  if(kptr.offset_key2 == 0){
    printf("\nThis key dont exist\n");
    return;
  }

  key2 ptr2;
  fseek(table->ks2, kptr.offset_key2, SEEK_SET);
  fread(&ptr2, sizeof(key2), 1, table->ks2);
  long int offset_cur = kptr.offset_key2;
  unsigned int is_find = 0;

  while(1){
    //erase from key2;
    Item2 item;
    fseek(table->items, ptr2.offset_item, SEEK_SET);
    fread(&item, sizeof(Item2), 1, table->items);
    char* ky2;
    ky2 = (char*)calloc(item.len_k2, 1);
    fread(ky2, 1, item.len_k2, table->items);
    if(strcmp(ky2, k2) == 0){
      is_find = 1;
      k2_erase_one(table, &item, ptr2.offset_item);
    } 
    free(ky2);

    if(ptr2.offset_next == 0)
      break;
    
    offset_cur = ptr2.offset_next;

    fseek(table->ks2, ptr2.offset_next, SEEK_SET);
    fread(&ptr2, sizeof(key2), 1, table->ks2);
  }
  if(is_find == 0){
    printf("\nThis key dont exist\n");
  }
}

void k2_reorganize(Table* table, char* k2){
  int hash2 = hash(k2, table->size2);

  keyspace2 kptr;
  fseek(table->ks2, hash2*sizeof(keyspace2), SEEK_SET);
  fread(&kptr, sizeof(keyspace2), 1, table->ks2);

  if(kptr.offset_key2 == 0){
    printf("\nThis key dont exist\n");
    return;
  }

  key2 ptr;
  fseek(table->ks2, kptr.offset_key2, SEEK_SET);
  fread(&ptr, sizeof(key2), 1, table->ks2);
  long int offset_cur = kptr.offset_key2;
  unsigned int is_find = 0;

  while(1){
    //erase from key2;
    Item2 item;
    fseek(table->items, ptr.offset_item, SEEK_SET);
    fread(&item, sizeof(Item2), 1, table->items);
    char* ky2;
    ky2 = (char*)calloc(item.len_k2, 1);
    fread(ky2, 1, item.len_k2, table->items);
    if(strcmp(ky2, k2) == 0){
      if(is_find != 0){
        k2_erase_one(table, &item, ptr.offset_item);
      }
      else{
        is_find = 1;
        ptr.v2 = 0;
      }
    }
    free(ky2);
    if(ptr.offset_next == 0)
      break;
    
    offset_cur = ptr.offset_next;

    fseek(table->ks2, ptr.offset_next, SEEK_SET);
    fread(&ptr, sizeof(key2), 1, table->ks2);

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
    keyspace1 kptr;
    fseek(table->ks1, pos*sizeof(keyspace1), SEEK_SET);
    fread(&kptr, sizeof(keyspace1), 1, table->ks1);

    key1 ptr;
    fseek(table->ks1, kptr.offset_key1, SEEK_SET);
    fread(&ptr, sizeof(key1), 1, table->ks1);

    Item2 item;
    while(1){
      fseek(table->items, ptr.offset_item, SEEK_SET);
      fread(&item, sizeof(Item2), 1, table->items);
      char* kptr2 = (char*)calloc(item.len_k2, 1);
      fread(kptr2, 1, item.len_k2, table->items);
      if(strcmp(kptr2, k2) == 0){
        free(kptr2);
        break;
      }
      free(kptr2);
      if(ptr.offset_next == 0){
        printf("\nThis item dont exist\n");
        return;
      }

      fseek(table->ks1, ptr.offset_next, SEEK_SET);
      fread(&ptr, sizeof(key1), 1, table->ks1);
    }

    k2_erase_one(table, &item, ptr.offset_item);
  }
}

void delete_all(Table* table){
  int pos;
  while(table->rsize1 > 0){
    pos = (table->rsize1 - 1) / 2;

    keyspace1 kptr;
    fseek(table->ks1, pos*sizeof(keyspace1), SEEK_SET);
    fread(&kptr, sizeof(keyspace1), 1, table->ks1);
    k1_erase_all(table, kptr.val);
  }
}
//output-------------------------------------------------------------------------------------


void output_one(long int offset, Table* table){
  Item2 ptr;
  fseek(table->items, offset, SEEK_SET);
  fread(&ptr, sizeof(Item2), 1, table->items);
  char* k2 = calloc(ptr.len_k2, 1);
  fread(k2, 1, ptr.len_k2, table->items);
  fseek(table->items, -ptr.len_k2, SEEK_CUR);
    
  printf("key 1:%d, key2:%s\n", ptr.k1, k2);
  while(1){
    char* s1 = calloc(ptr.info.len_s1, 1);
    fseek(table->items, ptr.len_k2, SEEK_CUR);
    fread(s1, 1, ptr.info.len_s1, table->items);

    printf("\t%f, %f, %s\n", ptr.info.f1, ptr.info.f2, s1);

    free(s1);
    if(ptr.offset_next == 0)
      break;

    fseek(table->items, ptr.offset_next, SEEK_SET);
    fread(&ptr, sizeof(Item2), 1, table->items);
  }
  printf("\n");
  free(k2);
}

void output_1(Table* table, unsigned int key){
  int rpos;
  int pos = bin_find(table->ks1, table->rsize1, key, &rpos);
  if(pos < 0){
    printf("\nThis key dont exist\n");
  }
  else{
    keyspace1 kptr1;
    fseek(table->ks1, sizeof(keyspace1)*pos, SEEK_SET);
    fread(&kptr1, sizeof(keyspace1), 1, table->ks1);

    key1 ptr;
    fseek(table->ks1, kptr1.offset_key1, SEEK_SET);
    fread(&ptr, sizeof(key1), 1, table->ks1);

    while(1){
      output_one(ptr.offset_item, table);
      if(ptr.offset_next == 0)
        break;
      fseek(table->ks1, ptr.offset_next, SEEK_SET);
      fread(&ptr, sizeof(key1), 1, table->ks1);
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
    keyspace1 kptr1;
    fseek(table->ks1, sizeof(keyspace1)*pos, SEEK_SET);
    fread(&kptr1, sizeof(keyspace1), 1, table->ks1);

    if(kptr1.version < ver){
      printf("\nThis version dont exist\n");
    }
    else{
      int j = 0;
      key1 ptr;
      fseek(table->ks1, kptr1.offset_key1, SEEK_SET);
      fread(&ptr, sizeof(key1), 1, table->ks1);
      while(kptr1.version - j != ver){
        fseek(table->ks1, ptr.offset_next, SEEK_SET);
        fread(&ptr, sizeof(key1), 1, table->ks1);
        ++j;
      }
      output_one(ptr.offset_item, table);
    }
  }
}

void output_2(Table* table, char* k2){
  int pos = hash(k2, table->size2);
  unsigned int is_find = 0;

  keyspace2 kptr;
  fseek(table->ks2, sizeof(keyspace2)*pos, SEEK_SET);
  fread(&kptr, sizeof(keyspace2), 1, table->ks2);

  key2 ptr;
  fseek(table->ks2, kptr.offset_key2, SEEK_SET);
  fread(&ptr, sizeof(key2), 1, table->ks2);
  while(1){
    Item2 item;
    fseek(table->items, ptr.offset_item, SEEK_SET);
    fread(&item, sizeof(Item2), 1, table->items);
    char* kptr2 = (char*)calloc(item.len_k2, 1);
    fread(kptr2, 1, item.len_k2, table->items);
    if(strcmp(kptr2, k2) == 0){
      is_find = 1;
      output_one(ptr.offset_item, table);
    }
    free(kptr2);
    if(ptr.offset_next == 0)
      break;
    fseek(table->ks2, ptr.offset_next, SEEK_SET);
    fread(&ptr, sizeof(key2), 1, table->ks2);
  }
  if(is_find == 0){
    printf("\nThis key dont exist\n");
  }
}

void output_2_2(Table* table, char* k2, int ver){
  int pos = hash(k2, table->size2);
  unsigned int is_find = 0;
  
  keyspace2 kptr;
  fseek(table->ks2, sizeof(keyspace2)*pos, SEEK_SET);
  fread(&kptr, sizeof(keyspace2), 1, table->ks2);

  key2 ptr;
  fseek(table->ks2, kptr.offset_key2, SEEK_SET);
  fread(&ptr, sizeof(key2), 1, table->ks2);

  while(1){
    Item2 item;
    fseek(table->items, ptr.offset_item, SEEK_SET);
    fread(&item, sizeof(Item2), 1, table->items);
    char* kptr2 = (char*)calloc(item.len_k2, 1);
    fread(kptr2, 1, item.len_k2, table->items);
    if(strcmp(kptr2, k2) == 0){
      if(is_find == 0){
        is_find = 1;
        if(ptr.v2 < ver){
          printf("\nThis version dont exist\n");
          break;
        }
      }
      if(ptr.v2 == ver){
        output_one(ptr.offset_item, table);
        break;
      }
    }
    
    free(kptr2);
    if(ptr.offset_next == 0)
      break;
    fseek(table->ks2, ptr.offset_next, SEEK_SET);
    fread(&ptr, sizeof(key2), 1, table->ks2);
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
    keyspace1 kptr1;
    fseek(table->ks1, sizeof(keyspace1)*pos, SEEK_SET);
    fread(&kptr1, sizeof(keyspace1), 1, table->ks1);

    key1 ptr;
    fseek(table->ks1, kptr1.offset_key1, SEEK_SET);
    fread(&ptr, sizeof(key1), 1, table->ks1);
    

    while(1){
      Item2 item;
      fseek(table->items, ptr.offset_item, SEEK_SET);
      fread(&item, sizeof(Item2), 1, table->items);
      char* kptr2 = (char*)calloc(item.len_k2, 1);
      fread(kptr2, 1, item.len_k2, table->items);
      if(strcmp(kptr2, k2) == 0){
        is_find = 1;
        output_one(ptr.offset_item, table);
        break;
      }
      free(kptr2);

      if(ptr.offset_next == 0)
      break;
      fseek(table->ks1, ptr.offset_next, SEEK_SET);
      fread(&ptr, sizeof(key1), 1, table->ks1);
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
    keyspace1 kptr1;
    fseek(table->ks1, sizeof(keyspace1)*i, SEEK_SET);
    fread(&kptr1, sizeof(keyspace1), 1, table->ks1);

    key1 ptr;
    fseek(table->ks1, kptr1.offset_key1, SEEK_SET);
    fread(&ptr, sizeof(key1), 1, table->ks1);

    while(1){
      output_one(ptr.offset_item, table);
      if(ptr.offset_next == 0)
        break;
      fseek(table->ks1, ptr.offset_next, SEEK_SET);
      fread(&ptr, sizeof(key1), 1, table->ks1);
    }
  }
}

void output_all_2(Table* table){
  for(int i = 0; i < table->size2; i++){
    keyspace2 kptr2;
    fseek(table->ks2, sizeof(keyspace2)*i, SEEK_SET);
    fread(&kptr2, sizeof(keyspace2), 1, table->ks2);
    printf("\n%d\n", kptr2.offset_key2);
    if(kptr2.offset_key2 == 0)
      continue;

    key2 ptr;
    fseek(table->ks2, kptr2.offset_key2, SEEK_SET);
    fread(&ptr, sizeof(key2), 1, table->ks2);

    while(1){
      output_one(ptr.offset_item, table);
      if(ptr.offset_next == 0)
        break;
      fseek(table->ks2, ptr.offset_next, SEEK_SET);
      fread(&ptr, sizeof(key2), 1, table->ks2);
    }
  }
}