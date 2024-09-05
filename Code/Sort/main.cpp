#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "sort.h" // 引入排序算法的头文件

// 声明全局变量
std::vector<int> data;

// 函数用于从文件中读取数据
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
    // 调用函数，从文件中加载数据
    loadDataFromFile("int_data_sample.dat");

    // 调用 sort.cpp 中的排序函数
    quickSort(data);

    // 打印数据，以验证内容
    for (int num : data) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    return 0;
}