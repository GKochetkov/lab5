#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <wchar.h>
#include "structs.h"


char *SymToCode(char sym) {
    char syms[64] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ&#";
    int dec = 0;
    while (syms[dec] != sym)
        dec++;

    char *res = (char*)malloc(7*sizeof(char));
    for (int i = 0; i < 6; i++)
        *(res+i) = '0';
    *(res+6) = '\0';

    int i = 5;
    while (dec > 0 && i >= 0) {
        *(res+i) = (dec % 2) + '0';
        i--;
        dec /= 2;
    }

    return res;
}

char** GetCodes(FILE *input) {
    char **res = (char**)malloc(256*sizeof(char*));
    for(int i = 0; i < 256; i++) {
        *(res+i) = NULL;
    }
    char buffer[MAX_STR_LENGTH];

    int codeSize;
    fscanf(input, "%d\n", &codeSize);

    for (int i = 0; i < codeSize; i++) {
        int curByte;
        fscanf(input, "%d ", &curByte);
        fgets(buffer, MAX_STR_LENGTH, input);

        int len = strlen(buffer);
        char *code = (char*)malloc((len+1)*sizeof(char));
        strcpy(code, buffer);
        *(code+len-1) = '\0';
        *(res+curByte) = code;
        //puts(code);
    }

    //puts("GetCodes done.");
    return res;
}

NODE* AttachNode(NODE *cur, unsigned char byte, char *code, int i) {
    if (*(code+i) == '\0') {
        cur -> byte = byte;
        //printf("%d attached.\n", byte);
    } else
        AttachNode(SafeAdd(cur, *(code+i)), byte, code, i+1);
}

char* DecompactStr(char *str) {
    int lenS = strlen(str);
    int lenR = lenS*6;

    char *res = (char*)malloc((lenR+1)*sizeof(char));
    *res = '\0';

    for (int i = 0; i < lenS; i++) {
        char *code = SymToCode(*(str+i));
        strcat(res, code);
        free(code);
    }

    *(res+lenR) = '\0';
    return res;
}

void DecodeStr(char *str, NODE* first, FILE *out) {
    wchar_t decoded[MAX_STR_LENGTH];
    int indx = 0;
    NODE *cur = first;

    int len = strlen(str);
    int i;
    for (i = 0; i < len; i++) {
        if (*(str+i) == '0')
            cur = cur -> left;
        else
            cur = cur -> right;
        if (cur -> byte != 0) {
            decoded[indx] = cur -> byte;
            indx++;
            if (cur -> byte == 10)
                break;
            cur = first;
        }
    }

    fputws(decoded, out);
    for (int j = 0; j <= i; j++)
        decoded[j] = '\0';
}

void Decode(char *inputName, FILE *output) {
    FILE *input = fopen(inputName, "r");
    char buffer[MAX_STR_LENGTH];

    NODE *codeTree = CreateN(0, 0);
    char **codes = GetCodes(input);

    for (unsigned char i = 0; i < 255; i++)
        if (*(codes+i) != NULL)
            AttachNode(codeTree, i, *(codes+i), 0);

    while(fgets(buffer, MAX_STR_LENGTH, input) != NULL) {
        char *decomp = DecompactStr(buffer);
        DecodeStr(decomp, codeTree, output);
        free(decomp);
    }
}

#endif //DECODE_H
