#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// هيكل المخزن الدائري
typedef struct {
    char *buf;       // المصفوفة
    int size;        // الحجم الكلي
    int head;        // مكان الكتابة
    int tail;        // مكان القراءة
    int count;       // عدد العناصر
} CircularBuffer;

// تهيئة المخزن
void cb_init(CircularBuffer *cb, char *storage, int sz) {
    cb->buf = storage;
    cb->size = sz;
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
}

// دالة الكتابة: ترجع 1 إذا نجحت أو 0 إذا المخزن ممتلئ
int cb_write(CircularBuffer *cb, char ch) {
    if (cb->count == cb->size) {
        return 0;   // فشل بسبب overflow
    }
    cb->buf[cb->head] = ch;
    cb->head = (cb->head + 1) % cb->size;
    cb->count++;
    return 1;
}

// دالة القراءة: ترجع الحرف أو -1 لو فارغ
int cb_read(CircularBuffer *cb) {
    if (cb->count == 0) {
        return -1;
    }
    char val = cb->buf[cb->tail];
    cb->tail = (cb->tail + 1) % cb->size;
    cb->count--;
    return (int)val;
}
void cb_status(CircularBuffer *cb) {
    printf("debug: head=%d, tail=%d, count=%d\n", cb->head, cb->tail, cb->count);
}

int main() {
    char name[50];
    char full[100];

    printf("أدخل اسمك: ");
    fgets(name, sizeof(name), stdin);
// ازالة سطر
    int len = strlen(name);
    if (len > 0 && name[len-1] == '\n')
        name[len-1] = '\0';

    // دمج الاسم مع "CE-ESY"
    strcpy(full, name);
    strcat(full, "CE-ESY");
    printf("السلسلة الناتجة: \"%s\" (طولها %d)\n\n", full, (int)strlen(full));

    // ===== اختبار بحجم صغير (يسبب overflow) =====
    int small_size = 5;
    char small_storage[5];
    CircularBuffer cb_small;
    cb_init(&cb_small, small_storage, small_size);

    printf(">>> اختبار بحجم صغير (%d):\n", small_size);
    for (int i = 0; full[i] != '\0'; i++) {
        if (!cb_write(&cb_small, full[i])) {
            printf("تنبيه: فشلت كتابة '%c' - المخزن امتلأ (overflow)\n", full[i]);
            break;   // نتوقف عند أول overflow
        }
    }

    printf("المحارف المقروءة: \"");
    int ch;
    while ((ch = cb_read(&cb_small)) != -1)
        putchar((char)ch);
    printf("\"\n");

    if (cb_small.count == 0)
        printf("✅ المخزن الصغير فارغ.\n\n");
    else
        printf("باقي %d حرف ما نقرأ.\n\n", cb_small.count);

    // ===== اختبار بحجم كبير (يكفي النص كاملاً) =====
    int large_size = strlen(full) + 10; 
    char *large_storage = (char*) malloc(large_size * sizeof(char));
    if (large_storage == NULL) {
        printf("فشل تخصيص الذاكرة! لا توجد ذاكرة كافية.\n");
        return 1;
    }

    CircularBuffer cb_large;
    cb_init(&cb_large, large_storage, large_size);

    printf(">>> اختبار بحجم كبير (%d):\n", large_size);
    for (int i = 0; full[i] != '\0'; i++) {
        if (cb_write(&cb_large, full[i]) == 0) {
            printf("خطأ: المفروض ما يصير overflow هنا!\n");
        }
    }
    printf("تمت كتابة النجاح.\n");

    // نعرض الحالة
    cb_status(&cb_large);

    printf("المحارف المقروءة: \"");
    while ((ch = cb_read(&cb_large)) != -1)
        putchar((char)ch);
    printf("\"\n");

    if (cb_large.count == 0)
        printf("✅ المخزن الكبير فارغ تماماً.\n");
    else
        printf("باقي %d عنصر؟ خطأ منطقي!\n", cb_large.count);

    free(large_storage);
    return 0;
}