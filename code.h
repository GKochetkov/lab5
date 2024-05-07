#ifndef CODE_H
#define CODE_H

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "structs.h"

unsigned char *ToString(wchar_t *str) {
    short flag = 0;
    int len = wcslen(str);
    if (str[wcslen(str)-1] != '\n') {
        flag = 1;
    }

    unsigned char *res = (unsigned char*)malloc((len+1+flag)*sizeof(unsigned char));

    for (int i = 0; i < len; i++) {
        *(res+i) = *(str+i);
    }
    if (flag)
        *(res+len) = '\n';
    *(res+len+flag) = '\0';

    //puts("ToString done.");
    return res;
}

int* GetFrequency(FILE *in) {
    int *res = (int*)malloc(256*sizeof(int));
    wchar_t buffer[MAX_STR_LENGTH];

    for (int i = 0; i < 256; i++)
        *(res+i) = 0;

    while (fgetws(buffer, MAX_STR_LENGTH, in) != NULL) {
        unsigned char *str = ToString(buffer);
        int len = strlen(str);
        for (int i = 0; i < len; i++) {
            *(res+str[i]) += 1;
        }
        free(str);
    }

    //printf("GetFrequency done.\n");
    return res;
}

STACK* CreateStack(FILE *in, int *codeSize) {
    STACK *res = CreateS(NULL);

    int *freq = GetFrequency(in);
    for (unsigned char i = 0; i < 255; i++) {
        if (*(freq+i) != 0) {
            Insert(res, CreateS(CreateN(i, *(freq+i))));
            *codeSize += 1;
            //printf("Byte %d in stack.\n", i);
        }
    }

    free(freq);
    //printf("CreateStack done(%d).\n", *codeSize);
    return res;
}

NODE* CreateTree(STACK *first) {
    while (first -> next -> next != NULL)
        Insert(first, CreateS(CombineFromStack(first)));

    NODE *res = first -> next -> node;
    free(first -> next);
    free(first -> node);
    free(first);
    //printf("CreateTree done.\n");
    return res;
}

char* AddBit(char *str, char bit) {
    int len = strlen(str);
    char *res = (char*)malloc((len+2)*sizeof(char));

    for (int i = 0; i < len; i++)
        *(res+i) = *(str+i);
    *(res+len) = bit;
    *(res+len+1) = '\0';

    return res;
}

void SetCode(NODE *cur, char* code, char** codes) {
    if (cur -> byte != 0)
        *(codes + cur -> byte) = code;
    else {
        SetCode(cur -> left, AddBit(code, '0'), codes);
        SetCode(cur -> right, AddBit(code, '1'), codes);
    }
}

char* CodeStr(unsigned char *str, char **codes) {
    char *res = (char*)malloc(MAX_STR_LENGTH*sizeof(char));
    *res = '\0';

    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        strcat(res, *(codes+(*(str+i))));
    }

    return res;
}

char* CompactStr(char *str) {
    char syms[64] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ&#";
    int lenS = strlen(str);

    int lenR = lenS + 6 - (lenS % 6);
    char *nStr = (char*)malloc((lenR+1)*sizeof(char));
    *nStr = '\0';
    strcat(nStr, str);
    for (int i = lenS; i < lenR; i++)
        *(nStr+i) = '0';
    *(nStr+lenR) = '\0';

    int numGroups = lenR / 6;
    char *res = (char*)malloc((numGroups+1)*sizeof(char));

    for (int i = 0; i < numGroups; i++) {
        char group[7];
        strncpy(group, nStr + i * 6, 6);
        group[6] = '\0';
        int dec = strtol(group, NULL, 2);
        res[i] = syms[dec];
    }
    *(res+numGroups) = '\0';

    free(nStr);
    return res;
}

void Code(char *inputName, FILE *output) {
    int codeSize = 0;

    FILE *input = fopen(inputName, "r");

    STACK *codeStack = CreateStack(input, &codeSize);
    if (codeStack -> next == NULL) {
        free(codeStack);
        fclose(input);
        return;
    }
    NODE *codeTree = CreateTree(codeStack);

    char **codes = (char**)malloc(256*sizeof(char*));
    for(int i = 0; i < 256; i++)
        *(codes+i) = NULL;

    char *code = (char*)malloc(sizeof(char));
    *code = '\0';
    SetCode(codeTree, code, codes);

    fprintf(output, "%d\n", codeSize);
    for (int i = 0; i < 256; i++)
        if (*(codes+i) != NULL) {
            fprintf(output, "%d ", i);
            fputs(*(codes+i), output);
            fprintf(output, "\n");
        }

    //printf("Start coding.\n");
    freopen(inputName, "r", input);

    wchar_t buffer[MAX_STR_LENGTH];
    while (fgetws(buffer, MAX_STR_LENGTH, input) != NULL) {
        unsigned char *str = ToString(buffer);
        char *coded = CodeStr(str, codes);
        //puts(coded);
        char *compacted = CompactStr(coded);
        fputs(compacted, output);
        fprintf(output, "\n");
        free(str);
        free(coded);
        free(compacted);
    }

    fclose(input);
}

#endif //CODE_H
