#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "sort.h" // ���������㷨��ͷ�ļ�

// ����ȫ�ֱ���
std::vector<int> data;

// �������ڴ��ļ��ж�ȡ����
void loadDataFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    int number;
    while (file >> number) {
        data.push_back(number);
    }

    file.close();
}

int main() {
    // ���ú��������ļ��м�������
    loadDataFromFile("int_data_sample.dat");

    // ���� sort.cpp �е�������
    quickSort(data);

    // ��ӡ���ݣ�����֤����
    for (int num : data) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    return 0;
}