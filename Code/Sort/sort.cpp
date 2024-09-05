#include <vector>
#include <algorithm> // ���� std::swap

void quickSort(std::vector<int>& vec, int left, int right) {
    if (left >= right) {
        return; // �������Σ�����Ҫ����
    }

    int pivot = vec[(left + right) / 2]; // ѡ���м�ֵ��Ϊ����
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
        quickSort(vec, left, j); // �ݹ�������벿��
    }
    if (i < right) {
        quickSort(vec, i, right); // �ݹ������Ұ벿��
    }
}

// ���ڳ�ʼ������
void quickSort(std::vector<int>& vec) {
    quickSort(vec, 0, vec.size() - 1);
}