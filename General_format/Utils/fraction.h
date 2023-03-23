#include <stdint.h>

typedef struct{
    uint32_t data;
}fraction, *fp;

int radix_point = 8;

void Set_RadixPoint(int val){
    radix_point = val;
}

void i2fr(int src, fraction* dst){
    dst->data = src << radix_point;
}

void f2fr(float src, fraction* dst){
    dst->data = src * (1 << radix_point);
}

void d2fr(double src, fraction* dst){
    dst->data = src * (1 << radix_point);
}

fraction fr_add(fraction a, fraction b){
    fraction c;
    c.data = a.data + b.data;
    return c;
}

void fr_acc(fraction* a, fraction* b){
    a->data = a->data+b->data;
}

void fr_mac(fraction* a, fraction* b, fraction* c){
    uint64_t product;
    product = b->data * c->data;
    a->data = product >> (radix_point/2);
}

fraction fr_sub(fraction a, fraction b){
    fraction c;
    c.data = a.data - b.data;
    return c;
}

void fr_mul(fraction* a, fraction* b, fraction* dst){
    fraction c;
    uint64_t product;
    product = a->data * b->data;
    dst->data = product >> (radix_point/2);
}


