#include <stdio.h>
#include <stdlib.h>

// 定义斐波那契堆中的节点结构
typedef struct Node {
    int key;                         // 节点的键值
    struct Node *parent, *child, *left, *right; // 指向父节点、子节点和兄弟节点的指针
    unsigned degree : 7;              // 节点的度数，使用7位足以存储最大度数
    unsigned mark : 1;                // 标记节点是否被切断过
} Node;

// 定义斐波那契堆结构
typedef struct {
    Node *min;                       // 指向最小节点的指针
    int size;                         // 堆中节点的数量
} FibonacciHeap;

void consolidate(FibonacciHeap* heap);

// 创建一个新节点
Node* createNode(int key) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->key = key;
    node->parent = node->child = NULL;
    node->left = node->right = node; // 初始化为一个循环链表
    node->degree = 0;
    node->mark = 0;
    return node;
}

// 创建一个空的斐波那契堆
FibonacciHeap* createHeap() {
    FibonacciHeap* heap = (FibonacciHeap*)malloc(sizeof(FibonacciHeap));
    heap->min = NULL;
    heap->size = 0;
    return heap;
}

// 向斐波那契堆中插入一个新节点
void insert(FibonacciHeap *heap, int key) {
    Node* node = createNode(key);
    // 如果堆为空，新节点就是最小节点
    if (heap->min == NULL) {
        heap->min = node;
    } else {
        // 将新节点插入到根节点列表中
        node->right = heap->min->right;
        node->left = heap->min;
        heap->min->right->left = node;
        heap->min->right = node;
        // 更新最小节点
        if (key < heap->min->key) {
            heap->min = node;
        }
    }
    heap->size++;
}

// 合并两个斐波那契堆
void merge(FibonacciHeap *heap1, FibonacciHeap *heap2) {
    if (heap2->min == NULL) return;
    if (heap1->min == NULL) {
        *heap1 = *heap2;
        free(heap2);
        return;
    }
    // 合并两个堆的根节点列表
    heap1->min->left->right = heap2->min->right;
    heap2->min->right->left = heap1->min->left;
    heap1->min->left = heap2->min;
    heap2->min->right = heap1->min;
    // 更新最小节点
    if (heap2->min->key < heap1->min->key) {
        heap1->min = heap2->min;
    }
    free(heap2);
}

// 提取最小节点
Node* extractMin(FibonacciHeap *heap) {
    Node *z = heap->min;
    if (z == NULL) return NULL;

    // 将z的子节点添加到根节点列表
    if (z->child != NULL) {
        Node *x = z->child;
        do {
            Node *next = x->right;
            x->parent = NULL;
            insert(heap, x->key);
            x = next;
        } while (x != z->child);
    }

    // 从根节点列表中移除z
    z->left->right = z->right;
    z->right->left = z->left;
    if (z == z->right) {
        heap->min = NULL;
    } else {
        heap->min = z->right;
        // 合并所有度数相同的节点
        consolidate(heap);
    }
    heap->size--;
    return z;
}

// 维护堆结构的辅助函数
void consolidate(FibonacciHeap *heap) {
    Node *A[50], *x, *y, *w;
    int i, l;
    for (i = 0; i < 50; i++) A[i] = NULL;

    // 将所有根节点按度数分组
    for (x = heap->min; x != x->right; x = x->right) {
        l = x->degree;
        while (A[l] != NULL) {
            y = A[l];
            if (x->key > y->key) {
                Node *temp = x;
                x = y;
                y = temp;
            }
            // 合并两个子堆
            w = x->child;
            y->parent = x->parent;
            x->child = y;
            x->degree++;
            while (w != y->right) {
                w->parent = x;
                w = w->right;
            }
            y->right->left = w->left;
            w->left->right = y->right;
            w->left = y;
            y->right = w;
            A[l] = NULL;
            l++;
        }
        A[l] = x;
    }
    heap->min = NULL;
    for (i = 0; i < 50; i++) {
        if (A[i] != NULL) {
            A[i]->left = A[i]->right = A[i];
            if (heap->min == NULL || A[i]->key < heap->min->key) {
                heap->min = A[i];
            }
        }
    }
}

// 斐波那契堆排序
void fibonacciHeapSort(FibonacciHeap *heap) {
    Node *minNode;
    while ((minNode = extractMin(heap)) != NULL) {
        printf("%d ", minNode->key);
        free(minNode);
    }
}

int main() {
    FibonacciHeap *heap = createHeap();
    int i;
    // 生成随机数据并插入斐波那契堆
    for (i = 0; i < 100000; i++) {
        int key = rand() % 1000;
        insert(heap, key);
    }

    printf("Sorted keys:\n ");
    fibonacciHeapSort(heap);
    printf("\n ");

    // 释放斐波那契堆占用的内存
    while (extractMin(heap) != NULL);
    free(heap);
    return 0;
}