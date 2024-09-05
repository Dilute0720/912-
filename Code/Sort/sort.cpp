#include <vector>
#include <algorithm> // 包含 std::swap

void quickSort(std::vector<int>& vec, int left, int right) {
    if (left >= right) {
        return; // 基本情形，不需要排序
    }

    int pivot = vec[(left + right) / 2]; // 选择中间值作为枢轴
    int i = left;
    int j = right;

    while (i <= j) {
        while (vec[i] < pivot) ++i;
        while (vec[j] > pivot) --j;

        if (i <= j) {
            std::swap(vec[i], vec[j]);
            ++i;
            --j;
        }
    }

    if (left < j) {
        quickSort(vec, left, j); // 递归排序左半部分
    }
    if (i < right) {
        quickSort(vec, i, right); // 递归排序右半部分
    }
}

// 用于初始化调用
void quickSort(std::vector<int>& vec) {
    quickSort(vec, 0, vec.size() - 1);
}