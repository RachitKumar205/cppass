#include <data.h>
#include <stdio.h>
#include <stdlib.h>

int convertCharToNumber(char ch) {
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    } else if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    } else {
        return -1;
    }
}

char convertNumberToChar(int n) {
    if (n >= 0 && n <= 9) {
        return n + '0';
    } else if (n >= 10 && n <= 15) {
        return n - 10 + 'A';
    } else {
        return 0;
    }
}

DataNode* createNode(char value) {
    DataNode* newNode = (DataNode*)malloc(sizeof(DataNode));
    newNode->number = value;
    newNode->next = NULL;
    return newNode;
}

long long linkedListToDecimal(Data src) {
    long long result = 0;
    DataNode* current = src.data;

    while (current != NULL) {
        result = result * src.base + convertCharToNumber(current->number);
        current = current->next;
    }

    if (src.sign == 1 && src.base == 2 && src.len == src.number_bits &&
        convertCharToNumber(src.data->number) == 1) {
        result -= (1LL << src.number_bits);
    }

    return result;
}

Data convert_to_base_n(Data src, unsigned char n) {
    Data new_data;
    new_data.base = n;
    new_data.sign = src.sign;
    new_data.number_bits = src.number_bits;
    new_data.data = NULL;
    new_data.len = 0;

    long long decimal = linkedListToDecimal(src);

    if (decimal == 0) {
        new_data.data = createNode('0');
        new_data.len = 1;
        return new_data;
    }

    DataNode* head = NULL;
    long long abs_decimal = (decimal < 0) ? -decimal : decimal;

    while (abs_decimal > 0) {
        char digit = convertNumberToChar(abs_decimal % n);
        DataNode* newNode = createNode(digit);

        if (head == NULL) {
            head = newNode;
        } else {
            newNode->next = head;
            head = newNode;
        }
        new_data.len++;
        abs_decimal /= n;
    }

    while (head != NULL && head->number == '0' && new_data.len > 1) {
        DataNode* temp = head;
        head = head->next;
        free(temp);
        new_data.len--;
    }

    new_data.data = head;
    return new_data;
}

Data convert_int_to_data(int number, unsigned char base, unsigned char number_bits) {
    Data new_data;
    new_data.base = base;
    new_data.number_bits = number_bits;
    new_data.sign = 1;
    new_data.data = NULL;
    new_data.len = 0;

    if (number == 0) {
        new_data.data = createNode('0');
        new_data.len = 1;
        return new_data;
    }

    unsigned int uint_value;
    if (number < 0) {
        //uint_value = (1u << number_bits) + number;
        //uint_value = (unsigned int)(number & ((1u << number_bits) - 1));
        unsigned int mask = (number_bits == 32) ? 0xFFFFFFFFu : ((1u << number_bits) - 1);
        uint_value = (unsigned int)(number & mask);
    } else {
        uint_value = number;
    }

    DataNode* head = NULL;
    int data_len = 0;
    while (uint_value > 0 && data_len < number_bits) {
        unsigned int remainder = uint_value % base;
        DataNode* newNode = createNode(convertNumberToChar(remainder));

        if (head == NULL) {
            head = newNode;
        } else {
            newNode->next = head;
            head = newNode;
        }
        data_len++;
        uint_value /= base;
    }

    while (head != NULL && head->number == '0' && data_len > 1) {
        DataNode* temp = head;
        head = head->next;
        free(temp);
        data_len--;
    }

    new_data.data = head;
    new_data.len = data_len;
    return new_data;
}

Data left_shift(Data src, int n) {
    Data new_data;
    new_data.base = 2;
    new_data.sign = src.sign;
    new_data.number_bits = src.number_bits;

    Data binary = (src.base != 2) ? convert_to_base_n(src, 2) : src;

    long long decimal = linkedListToDecimal(binary);
    decimal = decimal << n;

    return convert_int_to_data(decimal, 2, src.number_bits);
}

Data right_shift(Data src, int n) {
    Data new_data;
    new_data.base = 2;
    new_data.sign = src.sign;
    new_data.number_bits = src.number_bits;

    Data binary = (src.base != 2) ? convert_to_base_n(src, 2) : src;

    long long decimal = linkedListToDecimal(binary);
    decimal = decimal >> n;

    return convert_int_to_data(decimal, 2, src.number_bits);
}
