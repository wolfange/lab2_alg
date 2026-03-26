#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MIN_N 16
#define DATA_LEN 128
#define EPS 1e-9

typedef struct {
    double *keys;
    char (*data)[DATA_LEN];
    int n;
} Table;

int cmp_double(double a, double b) {
    if (fabs(a - b) < EPS) {
        return 0;
    }
    return (a < b) ? -1 : 1;
}

int init_table(Table *t, int n) {
    t->keys = (double *)malloc((size_t)n * sizeof(double));
    t->data = (char(*)[DATA_LEN])malloc((size_t)n * sizeof(*t->data));
    t->n = n;

    if (t->keys == NULL || t->data == NULL) {
        free(t->keys);
        free(t->data);
        t->keys = NULL;
        t->data = NULL;
        t->n = 0;
        return 0;
    }

    return 1;
}

void free_table(Table *t) {
    free(t->keys);
    free(t->data);
    t->keys = NULL;
    t->data = NULL;
    t->n = 0;
}

void copy_table(Table *dst, const Table *src) {
    for (int i = 0; i < src->n; i++) {
        dst->keys[i] = src->keys[i];
        strcpy(dst->data[i], src->data[i]);
    }
}

void print_table(const Table *t, const char *title) {
    printf("\n%s\n", title);
    printf("------------------------------------------------------------\n");
    printf("| %-3s | %-12s | %-35s |\n", "#", "key", "data");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < t->n; i++) {
        printf("| %-3d | %-12.6f | %-35s |\n", i, t->keys[i], t->data[i]);
    }

    printf("------------------------------------------------------------\n");
}

void swap_rows(Table *t, int i, int j) {
    if (i == j) {
        return;
    }

    char data_tmp[DATA_LEN];
    double key_tmp = t->keys[i];
    t->keys[i] = t->keys[j];
    t->keys[j] = key_tmp;

    memcpy(data_tmp, t->data[i], DATA_LEN);
    memcpy(t->data[i], t->data[j], DATA_LEN);
    memcpy(t->data[j], data_tmp, DATA_LEN);
}

// Просеивание вниз для max-heap
void sift_down(Table *t, int root, int end) {
    while (2 * root + 1 <= end) {
        int child = 2 * root + 1;
        int max_idx = root;

        if (t->keys[child] > t->keys[max_idx]) {
            max_idx = child;
        }
        if (child + 1 <= end && t->keys[child + 1] > t->keys[max_idx]) {
            max_idx = child + 1;
        }

        if (max_idx == root) {
            return;
        }

        swap_rows(t, root, max_idx);
        root = max_idx;
    }
}

void heap_sort(Table *t) {
    if (t->n <= 1) {
        return;
    }

    for (int start = (t->n - 2) / 2; start >= 0; start--) {
        sift_down(t, start, t->n - 1);
    }

    for (int end = t->n - 1; end > 0; end--) {
        swap_rows(t, 0, end);
        sift_down(t, 0, end - 1);
    }
}

void reverse_table(Table *t) {
    for (int i = 0; i < t->n / 2; i++) {
        swap_rows(t, i, t->n - 1 - i);
    }
}

void shuffle_table(Table *t) {
    for (int i = t->n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap_rows(t, i, j);
    }
}

int binary_search_first(const Table *t, double key) {
    int left = 0;
    int right = t->n - 1;
    int pos = -1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = cmp_double(t->keys[mid], key);

        if (cmp == 0) {
            pos = mid;
            right = mid - 1;
        } else if (cmp < 0) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return pos;
}

void search_in_sorted_table(const Table *t) {
    double key;

    printf("\nВведите ключи для двоичного поиска (0 для выхода):\n");
    while (1) {
        printf("> ");
        if (scanf("%lf", &key) != 1) {
            return;
        }

        if (fabs(key) < EPS) {
            break;
        }

        int idx = binary_search_first(t, key);
        if (idx == -1) {
            printf("Ключ %.6f не найден\n", key);
        } else {
            printf("\nНайденные элементы:\n");
            for (int i = idx; i < t->n && cmp_double(t->keys[i], key) == 0; i++) {
                printf("[%d] key=%.6f data=%s\n\n", i, t->keys[i], t->data[i]);
            }
        }
    }
}

void run_case(const char *name, Table *t) {
    print_table(t, name);
    heap_sort(t);
    print_table(t, "После сортировки:");
    search_in_sorted_table(t);
}

int main(void) {
    int n;
    Table base;
    Table sorted_case;
    Table reversed_case;
    Table random_case;

    srand((unsigned int)time(NULL));

    printf("Лабораторная работа №2\n");
    printf("Введите количество элементов (минимум %d): ", MIN_N);

    if (scanf("%d", &n) != 1 || n < MIN_N) {
        printf("Ошибка: нужно ввести целое число >= %d\n", MIN_N);
        return 1;
    }

    if (!init_table(&base, n) || !init_table(&sorted_case, n) || !init_table(&reversed_case, n) ||
        !init_table(&random_case, n)) {
        printf("Ошибка выделения памяти\n");

        free_table(&base);
        free_table(&sorted_case);
        free_table(&reversed_case);
        free_table(&random_case);
        return 1;
    }

    printf("\nВведите записи таблицы:\n");
    printf("Для каждой записи вводите: вещественный ключ и строку данных.\n");

    for (int i = 0; i < n; i++) {
        printf("\nЗапись %d\n", i + 1);
        printf("Ключ: ");
        if (scanf("%lf", &base.keys[i]) != 1) {
            printf("Ошибка ввода ключа\n");

            free_table(&base);
            free_table(&sorted_case);
            free_table(&reversed_case);
            free_table(&random_case);
            return 1;
        }

        printf("Данные (без пробелов): ");
        if (scanf("%127s", base.data[i]) != 1) {
            printf("Ошибка ввода данных\n");

            free_table(&base);
            free_table(&sorted_case);
            free_table(&reversed_case);
            free_table(&random_case);
            return 1;
        }
    }

    copy_table(&sorted_case, &base);
    heap_sort(&sorted_case);

    copy_table(&reversed_case, &sorted_case);
    reverse_table(&reversed_case);

    copy_table(&random_case, &base);
    shuffle_table(&random_case);

    printf("\nСлучай 1: уже отсортировано\n");
    run_case("Исходная таблица:", &sorted_case);

    printf("\nСлучай 2: обратный порядок\n");
    run_case("Исходная таблица:", &reversed_case);

    printf("\nСлучай 3: случайный порядок\n");
    run_case("Исходная таблица:", &random_case);

    free_table(&base);
    free_table(&sorted_case);
    free_table(&reversed_case);
    free_table(&random_case);

    return 0;
}
