/*Ключами являются:
"an unsigned integer" - в первом пространстве ключей;
"a string with variable length (null-terminated)" - во втором пространстве ключей.
Информация представляет собой "a record of two float numbers and a string with variable length (null-terminated)".
#----------------------------------------------------------------------
Первое пространство ключей организовано как упорядоченная таблица, организованная вектором;
Максимальный размер пространства ключей ограничен msize1, определяемый при инициализации таблицы.
#---------------------------------------------------------------------
Второе пространство ключей организовано как перемешанная таблица, использующая перемешивание сцеплением.
Максимальный размер массива указателей ограничен msize2, определяемый при инициализации таблицы.
#----------------------------------------------------------------------
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

#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_str(int *n){
	char buf[80];
	char* res = NULL;
	int len = 0;
	do{
		*n = scanf("%80[^\n]", buf);
    if(*n < 0){
      if(!res){
        res = realloc(res, len + 2);
        res[len] = ' ';
        len += 1;
      }
    }
    else if(*n > 0){
			int chunk_len = strlen(buf);
			int str_len = len + chunk_len;
			res = realloc(res,str_len + 1);
			memcpy(res + len, buf, chunk_len);
			len = str_len;
		}
		else{
			scanf("%*c");
		}
	}while(*n > 0);

	if(len > 0) res[len] = '\0';
	else res = calloc(1,sizeof(char));
	return res;
}

void get_num(int* x){
    int p;
    do{
        p = scanf("%d", x);
        if(p <= 0){
            printf("Error. Try again\n");
            scanf("%*[^\n]");
        }
    }while(p <= 0);
    scanf("%*c");
}

void get_float(float* x){
    int p;
    do{
        p = scanf("%f", x);
        if(p <= 0){
            printf("Error. Try again\n");
            scanf("%*[^\n]");
        }
    }while(p <= 0);
    scanf("%*c");
}
//----------------------------------------------------------

void Make_Table(Table* table){
  int n;
  char* name1;
  char* name2;
  printf("Enter names of keyspaces files:\n");
  name1 = get_str(&n);
  name2 = get_str(&n);

  int size1, size2;
  printf("Enter size of keyspace 1 and keyspace 2:\n");
  get_num(&size1);
  get_num(&size2);

  table->size1 = size1;
  table->size2 = size2;
  table->rsize1 = 0;
  FILE* ks1 = fopen(name1, "w+b");
  FILE* ks2 = fopen(name2, "w+b");

  table->ks1 = ks1;
  table->ks2 = ks2;

  keyspace1 ks11 = {0};
  ks11.offset_key1 = 0;
  ks11.val = 0;
  ks11.version = 0;
  for(int i = 0; i < size1; i++){
    fwrite(&ks11, sizeof(keyspace1), 1, table->ks1);
  }
  //fwrite(&ks11, sizeof(keyspace1), size1, ks1);

  keyspace2 ks12 = {0};
  ks12.version = 0;
  ks12.offset_key2 = 0;
  for(int i = 0; i < size2; i++){
    fwrite(&ks12, sizeof(keyspace2), 1, table->ks2);
  }
  //fwrite(&ks12, sizeof(keyspace2), size2, ks2);
  int a = 0;

  rewind(table->items);
  fwrite(&(table->size1), sizeof(int), 1, table->items);
  fwrite(&a, sizeof(int), 1, table->items);
  fwrite(&(table->size2), sizeof(int), 1, table->items);

  a = strlen(name1) + 1;
  fwrite(&a, sizeof(int), 1, table->items);
  a = strlen(name2) + 1;
  fwrite(&a, sizeof(int), 1, table->items);

  fwrite(name1, 1, strlen(name1) + 1, table->items);
  fwrite(name2, 1, strlen(name2) + 1, table->items);
  rewind(table->items);

  free(name1);
  free(name2);

}
//------------- ---------------------------------------------
void insert(Table* table){
  InfoType2 info = {0};
  int n;
  int key1;
  char* key2;
  char* s1;
  Item2 nw;

  printf("Enter key1 and key2:\n");
  get_num(&key1);
  key2 = get_str(&n);

  printf("Enter info\n");
  get_float(&(info.f1));
  get_float(&(info.f2));
  s1 = get_str(&n);
  info.len_s1 = strlen(s1) + 1;

  nw.alive = 1;
  nw.k1 = key1;
  nw.len_k2 = strlen(key2) + 1;
  nw.hash2 = hash(key2, table->size2);
  nw.info = info;
  NewItem(table, &nw, key2, s1);
  free(key2);
  free(s1);
}

void delete(Table *table){
  int n;
  int ans;
  int key1;
  char* key2;
  int ver;
  printf("Enter:\n\t1:erase all by key1\n\t2:erase all by key2\n\t3:reorganize by key2\n\t");
  printf("4:delete by key 1 + version\n\t5:delete by combination of keys\n\t0:delete all\n\t-1:return\n"); 
  while(1){
    printf("Enter a command\n");
    get_num(&ans);

    if(ans == 1){
      printf("Enter key1\n");
      get_num(&key1);
      k1_erase_all(table, key1);
      break;
    }

    else if(ans == 2){
      printf("Enter key2\n");
      key2 = get_str(&n);
      k2_erase_all(table, key2);
      free(key2);
      break;
    }

    else if(ans == 3){
      printf("Enter key2\n");
      key2 = get_str(&n);
      k2_reorganize(table, key2);
      free(key2);
      break;
    }

    else if(ans == 4){
      printf("Enter key1 and version\n");
      get_num(&key1);
      get_num(&ver);
      k1_erase_one(table, key1, ver);
      break;
    }

    else if(ans == 5){
      printf("Enter key1 and key2\n");
      get_num(&key1);
      key2 = get_str(&n);
      k1_k2_erase(table, key1, key2);
      free(key2);
      break;
    }

    else if(ans == 0){
      printf("Full delete\n");
      delete_all(table);
      break;
    }


    else if(ans == -1){
      break;
    }
    else{
      printf("\nPlease try again\n");
    }
  }
}

void output(Table* table){
  int n;
  int ans;
  int key1;
  char* key2;
  int ver;
  printf("Enter:\n\t1:output all\n\t2:output by key1\n\t3:output by key1 with version\n\t");
  printf("4:output by key2\n\t5:output by key2 with version\n\t6:output by combination of key1 and k2\n\t");
  printf("-1:return\n");
  
  while(1){
    printf("Enter a command\n");
    get_num(&ans);
    if(ans == 1){
      output_all(table);
      break;
    }

    else if(ans == 2){
      printf("Enter key1\n");
      get_num(&key1);
      output_1(table, key1);
      break;
    }

    else if(ans == 3){
      printf("Enter key1 and version\n");
      get_num(&key1);
      get_num(&ver);
      output_1_1(table, key1, ver);
      break;
    }

    else if(ans == 4){
      printf("Enter key2\n");
      key2 = get_str(&n);
      output_2(table, key2);
      free(key2);
      break;
    }

    else if(ans == 5){
      printf("Enter key2 and version\n");
      key2 = get_str(&n);
      get_num(&ver);
      output_2_2(table, key2, ver);
      free(key2);
      break;
    }

    else if(ans == 6){
      printf("Enter key1 and key 2\n");
      get_num(&key1);
      key2 = get_str(&n);
      output_1_2(table, key1, key2);
      free(key2);
      break;
    }
    else if(ans == -1){
      break;
    }
    else{
      printf("\nPlease try again\n");
    }
  }
}

//-----------------------------------------------------------------------------

int main()
{
  Table table;
  int n;
  char* name;
  int cont = 0;
  int ans;

  printf("Enter name of table: ");
  name = get_str(&n);  
  FILE* file = fopen(name, "r+b");
  free(name);

	if(!file){
	  printf("\nNo such file\n");
		return 0;
	}
	fseek(file, 0, SEEK_END);
	if(!ftell(file)){
    char* if_new;
		printf("File is empty\n");
    printf("Do you want to create new table? [y][n]:\n");
    while(1){
      if_new = get_str(&n);
      if(strcmp(if_new, "y") == 0){
        cont = 1;
        table.items = file;
        Make_Table(&table);
        free(if_new);
        break;
      }
      else if(strcmp(if_new, "n") == 0){
        printf("\nGoodbye!\n");
        free(if_new);
        fclose(file);
        break;
      }
    }
	}
  else{
    cont = 1;
    table.items = file;
    rewind(file);
    fread(&(table.size1), sizeof(int), 1, file);
    fread(&(table.rsize1), sizeof(int), 1, file);
    fread(&(table.size2), sizeof(int), 1, file);
    int s1, s2;
    fread(&s1, sizeof(int), 1, file);
    fread(&s2, sizeof(int), 1, file);
    char* str1; 
    char* str2;
    str1 = (char*)calloc(s1, 1);
    str2 = (char*)calloc(s2, 1);

    fread(str1, 1, s1, file);
    fread(str2, 1, s2, file);

    table.ks1 = fopen(str1, "r+b");
    table.ks2 = fopen(str2, "r+b");
    free(str1);
    free(str2);
	}
  if(cont == 1){
    printf("\n\nWhat do you need?\n\t");
    printf("1:insert\n\t2:delete\n\t3:output\n\t4:exit\n\t0:repeat\n");
    while(1){
      printf("\nMAIN MENU:\n");
      printf("\nEnter a command:\n");
      get_num(&ans);
      if(ans == 1){
        insert(&table);
      }
      else if(ans == 2){
        delete(&table);
      }
      else if(ans == 3){
        output(&table);
      }
      else if(ans == 4){
        printf("\nThats all\n\n");
        fseek(table.items, sizeof(int), SEEK_SET);
        fwrite(&(table.rsize1), sizeof(int), 1, table.items);
        fclose(table.items);
        fclose(table.ks2);
        fclose(table.ks1);
        //delete_all(&table);
        break;
      }
      else if(ans == 0){
        printf("Commands. Press:\n\t1:insert\n\t2:delete\n\t3:output\n\t4:exit\n\t0:repeat\n");
      }
      else{
        printf("\nPlease try again\n");
      }
    }
  }
  return 0;
}
