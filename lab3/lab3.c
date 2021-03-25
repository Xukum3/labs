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

void insert(Table* table){
  InfoType info;
  int n;
  int key1;
  char* key2;
  Item new;
  printf("Enter key1 and key2:\n");
  get_num(&key1);
  key2 = get_str(&n);

  printf("Enter info\n");
  get_float(&(info.f1));
  get_float(&(info.f2));
  
  info.s1 = get_str(&n);

  new.k1 = key1;
  new.k2 = key2;
  new.hash2 = hash(key2, table->size2);
  new.info = info;
  NewItem(table, &new);
  free(key2);
  free(info.s1);
}

void delete(Table *table){
  int n;
  int ans;
  int key1;
  char* key2;
  int ver;
  printf("Enter 1 to erase all by key1;\t2 to erase all by key2;t\3 to reorganize by key2;\n");
  printf("4 to delete by key 1 + version;\t5 to delete by combination of keys;\n -1 to return;\n"); 
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
  printf("Enter 1 to output all;\t2 to output by key1;\t3 to output by key1 with version;\n");
  printf("4 to output by key2;\t5 to output by key2 with version;\t6 to output by combination of key1 and k2;\n");
  printf("-1 to return;\n");
  
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

int main()
{
  keyspace1* ks1;
  keyspace2* ks2;
  int size1, size2;
  printf("Enter size of keyspace 1 and keyspace 2:\n");
  get_num(&size1);
  get_num(&size2);

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

  int ans;

  printf("\n\nWhat do you need?\n");
  printf("1 for insert;\t2 for delete;\t3 for output;\t4 for exit;\t0 to repeat\n");
  while(1){
    printf("\nMAIN MENU:\n");
    printf("Enter a command\n");
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
      delete_all(&table);
      break;
    }
    else if(ans == 0){
      printf("Commands:1 for insert;\t2 for delete;\t3 for output;\t4 for exit;\t0 to repeat\n");
    }
    else{
      printf("\nPlease try again\n");
    }
  }
  return 0;
}
