函数对象（Function Object），也称为仿函数（Functor），是一种在C++中实现多态操作的机制。函数对象是重载了函数调用操作符`operator()`的类的实例。它们可以被当作函数来使用，但它们还可以携带状态（即数据成员）。

本质上是个对象, 但是看起来行为像一个函数.

### 特点
1. **对象作为函数使用**：函数对象可以像普通函数一样被调用。
2. **可携带状态**：与普通函数不同，函数对象可以拥有成员变量，这些变量可以在函数对象的多次调用之间保持状态。
3. **可重载操作符**：函数对象通过重载`operator()`实现调用，可以有参数和返回值。
4. **多态性**：函数对象可以模拟函数的行为，实现多态。

### 使用场景
- **算法的自定义行为**：在STL算法中，经常需要传入自定义的行为，如排序、查找等。
- **状态的封装**：当需要在多次调用中保持某些状态时，可以使用函数对象。
- **回调机制**：在需要延迟或异步执行某些操作时，函数对象可以作为回调。

### 示例
下面是一个简单的函数对象示例，它计算两个整数的和：

```cpp
#include <iostream>

// 函数对象类
class SumFunctor {
public:
    // 重载()运算符，使其可以像函数一样被调用
    int operator()(int a, int b) const {
        return a + b;
    }
};

int main() {
    SumFunctor sum;  // 创建函数对象的实例
    int result = sum(5, 3);  // 使用函数对象，相当于调用sum(5, 3)
    std::cout << "Sum: " << result << std::endl;  // 输出结果
    return 0;
}
```

### 与Lambda表达式的比较
从C++11开始，Lambda表达式提供了一种更简洁的方式来定义匿名函数对象。Lambda表达式可以捕获外围作用域的变量，并且可以非常方便地用于STL算法。例如，使用Lambda表达式计算两个整数的和：

```cpp
#include <iostream>

int main() {
    auto sum = [](int a, int b) { return a + b; };
    int result = sum(5, 3);
    std::cout << "Sum: " << result << std::endl;
    return 0;
}
```

Lambda表达式通常用于简单的场景，而函数对象则更适用于需要封装更多状态或需要多次复用的情况。

### 函数对象在STL中的应用
在C++ STL中，函数对象广泛用于算法，如`std::sort`、`std::for_each`、`std::find_if`等。例如，使用函数对象来自定义排序算法的行为：

```cpp
#include <algorithm>
#include <vector>
#include <iostream>

class MyObject {
public:
    int value;
    MyObject(int v) : value(v) {}
};

bool compareMyObject(MyObject a, MyObject b) {
    return a.value < b.value;
}

int main() {
    std::vector<MyObject> vec = {MyObject(3), MyObject(1), MyObject(2)};
    std::sort(vec.begin(), vec.end(), compareMyObject);
    for (const auto& obj : vec) {
        std::cout << obj.value << " ";
    }
    return 0;
}
```

在这个例子中，`compareMyObject`是一个函数对象，用于自定义`std::sort`算法的比较逻辑。
