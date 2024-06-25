template<typename T>
class BinaryTree {
public:
    // ���캯����ʹ�ó�Ա��ʼ���б�����ʼ��rootָ��
    BinaryTree() : root(nullptr) {} // ��ʼ��rootΪnullptr

    // ���������������ͷ����нڵ���ڴ�
    ~BinaryTree() {
        clear(root); // clear�������ڵݹ��ͷ��ڴ�
    }

    // ������Ա�����ͱ���:��ɾ�Ĳ� Ŀǰ��������ӽڵ�Ĳ��� ��Ҳ�ɼ��ݹ������еķḻӦ��.

    void addNode(T val, TreeNode* parent = nullptr) {
        if (parent == nullptr) {
            // ���û���ṩ���ڵ㣬������Ϊ�գ��򴴽����ڵ�, ������ǰ���Ĭ�Ϸ�������һ���ڵ�
            if (root == nullptr) {
                root = new TreeNode(val);
            }
            else {
                // �����������߼����ҵ�һ�����ʵ�λ����Ϊ���ڵ�
                // ���磬�����ڸ��ڵ�����е�����λ������½ڵ�
                // ��ȡ������Ҫʵ�ֵĶ��������ͣ��������������AVL���ȣ�
                // ������һ��ʾ�������½ڵ���Ϊ���ڵ���ӽڵ㣨����ȡ���ڶ��������ͣ�
                // ��Ҫ���ݶ�����������ʵ�־�����߼�
                addNode(val, root); // �ݹ�������ҵ�һ�����ʵĸ��ڵ�
                return;
            }
        }
        else {
            // ����ʵ�ֽ��½ڵ���ӵ� parent ���ӽڵ���߼�
            // ��ͬ��ȡ������Ķ���������
            // ������һ���򵥵�ʾ�������½ڵ���ӵ� parent ����� ���parent�ĺ���������˳�����亢�ӵĺ���
            // ��������������ӵ����
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
        //ɾ��ĳһ���ڵ��Լ��ýڵ��µ����� ������Ҫע������ڴ��ͷŵ�����
        clear(node);
    }

private:
    TreeNode* root; // ���ڵ��ָ��

    // �ݹ��ͷ��������нڵ���ڴ�ĸ�������
    void clear(TreeNode* node) {
        if (node) {
            clear(node->left);
            clear(node->right);
            delete node;
        }
    }

    // TreeNode�ṹ�嶨��
    struct TreeNode {
        T value;
        TreeNode* parent;
        TreeNode* left;
        TreeNode* right;

        TreeNode(T val) : value(val), left(nullptr), right(nullptr) {}
    };
};