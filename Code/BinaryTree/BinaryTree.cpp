template<typename T>
class BinaryTree {
public:
    // 构造函数，使用成员初始化列表来初始化root指针
    BinaryTree() : root(nullptr) {} // 初始化root为nullptr

    // 析构函数，负责释放所有节点的内存
    ~BinaryTree() {
        clear(root); // clear函数用于递归释放内存
    }

    // 其他成员函数和变量:增删改查 目前仅完成增加节点的部分 但也可见递归在树中的丰富应用.

    void addNode(T val, TreeNode* parent = nullptr) {
        if (parent == nullptr) {
            // 如果没有提供父节点，并且树为空，则创建根节点, 否则就是按照默认方案加入一个节点
            if (root == nullptr) {
                root = new TreeNode(val);
            }
            else {
                // 这里可以添加逻辑来找到一个合适的位置作为父节点
                // 例如，可以在根节点或树中的其他位置添加新节点
                // 这取决于想要实现的二叉树类型（如二叉搜索树、AVL树等）
                // 以下是一个示例，将新节点作为根节点的子节点（具体取决于二叉树类型）
                // 需要根据二叉树类型来实现具体的逻辑
                addNode(val, root); // 递归调用以找到一个合适的父节点
                return;
            }
        }
        else {
            // 这里实现将新节点添加到 parent 的子节点的逻辑
            // 这同样取决于你的二叉树类型
            // 以下是一个简单的示例，将新节点添加到 parent 的左侧 如果parent的孩子已满则顺延至其孩子的孩子
            // 假设我们总是添加到左侧
            if (parent->left == nullptr) {
                parent->left = new TreeNode(val);
            }
            else
            {
                if (parent->right ==nullptr)
                {
                    parent->right = new TreeNode(val);
                }
                else
                {
                    addNode(val, parent->left);
                }
            }
        }
    }

    void delete(TreeNode* node) {
        //删除某一个节点以及该节点下的子树 尤其需要注意的是内存释放的问题
        clear(node);
    }

private:
    TreeNode* root; // 根节点的指针

    // 递归释放树中所有节点的内存的辅助函数
    void clear(TreeNode* node) {
        if (node) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

    // TreeNode结构体定义
    struct TreeNode {
        T value;
        TreeNode* parent;
        TreeNode* left;
        TreeNode* right;

        TreeNode(T val) : value(val), left(nullptr), right(nullptr) {}
    };
};