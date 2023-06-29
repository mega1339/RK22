#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

#define MAX_LINE_LENGTH 1024 // максимальная длина строки и поля в файле
#define MAX_FIELD_LENGTH 256

cJSON* parseCSV(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open file: %s\n", filename);
        return NULL;
    }

    char line[MAX_LINE_LENGTH]; // массив в котором будут строки из файла
    cJSON* root = cJSON_CreateArray(); // объект представляющий собой массив типа cJSON
    cJSON* fields = NULL; //просматривается информация о полях CSV файла 
    int isFirstLine = 1; // первая строка в CSV файле

    while (fgets(line, sizeof(line), file)) { // с помощью fgets мы считываем строки из CSV файла и заносим их в массив lines
        // Удаляем символ новой строки
        line[strcspn(line, "\n")] = '\0'; // заменяем символ конца строки на нулевой символ, чтобы удалить его

        char* token; // нужен для разделения строки на поля по запятым
        char* field; // будет хранить значение каждого поля
        int fieldIndex = 0; // хранит текущий индекс поля
        cJSON* item = cJSON_CreateObject(); // item это объект типа cJSON хранящий в себе строку из csv файла

        // Разделяем строку по запятым
        token = strtok(line, ",");
        while (token != NULL) {
            field = (char*)malloc(MAX_FIELD_LENGTH);
            strncpy(field, token, MAX_FIELD_LENGTH);

            if (isFirstLine) {
                // Создаем массив для имен полей
                if (!fields) {
                    fields = cJSON_CreateArray();
                }

                // Добавляем имя поля в массив fields
                cJSON_AddItemToArray(fields, cJSON_CreateString(field));
            } else {
                // Добавляем значение в объект item
                cJSON_AddItemToObject(item, cJSON_GetArrayItem(fields, fieldIndex)->valuestring, cJSON_CreateString(field));
                fieldIndex++;
            }

            token = strtok(NULL, ",");
            free(field);
        }

        if (!isFirstLine) {
            // Добавляем объект item в массив root
            cJSON_AddItemToArray(root, item);
        }

        isFirstLine = 0;
    }

    fclose(file);
    cJSON_AddItemToObject(root, "fields", fields); // Добавляем массив fields в объект root
    return root;
}

int main() {
    cJSON* json = parseCSV("example.csv");
    if (json) {
        FILE* jsonFile = fopen("output.json", "w");
        if (jsonFile) {
            char* jsonString = cJSON_Print(json);
            fprintf(jsonFile, "%s\n", jsonString);
            fclose(jsonFile);
            cJSON_free(jsonString);
        } else {
            printf("Failed to open output file.\n");
        }
        
        cJSON_Delete(json);
    }

    return 0;
}
