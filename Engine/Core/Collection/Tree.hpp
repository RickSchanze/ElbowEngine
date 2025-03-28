//
// Created by Echo on 2025/3/23.
//

#pragma once
#include "Core/TypeAlias.hpp"

template<typename T, UInt64 NumChildren>
class TreeNode {
public:
    enum class TraverseOrder { Pre, Post };

    static UInt64 ChildrenCount() { return NumChildren; }

    ~TreeNode() {
        for (UInt64 i = 0; i < NumChildren; ++i) {
            if (children_[i]) {
                Delete(children_[i]);
            }
        }
    }

    TreeNode() {
        for (UInt64 i = 0; i < NumChildren; ++i) {
            children_[i] = nullptr;
        }
    }

    explicit TreeNode(const T &data) : data_(data) {
        for (UInt64 i = 0; i < NumChildren; ++i) {
            children_[i] = nullptr;
        }
    }

    void SetData(const T &data) { data_ = data; }

    const T &GetData() const { return data_; }

    T &GetData() { return data_; }

    TreeNode *GetChild(UInt64 index) const { return children_[index]; }

    /**
     * 设置子节点
     * @param index
     * @param child
     * @param remove 为true则会删除原来的子节点
     */
    void SetChild(UInt64 index, TreeNode *child, bool remove = true) {
        if (remove && children_[index]) {
            Delete(children_[index]);
        }
        children_[index] = child;
    }

    /**
     * 设置子节点
     * @param index
     * @param child
     * @param remove 为true则会删除原来的子节点
     */
    void SetChild(UInt64 index, const T &child, bool remove = true) {
        if (remove && children_[index]) {
            Delete(children_[index]);
        }
        children_[index] = New<TreeNode>(child);
    }

    // DFS
    template<typename F>
    TreeNode *FindDFS(F &&f, TraverseOrder order = TraverseOrder::Pre) {
        if (order == TraverseOrder::Pre) {
            if (f(data_)) {
                return this;
            }
            for (Int64 i = 0; i < NumChildren; ++i) {
                if (auto child = GetChild(i)) {
                    if (TreeNode *result = child->FindDFS(f, order)) {
                        return result;
                    }
                }
            }
        } else if (order == TraverseOrder::Post) {
            if (f(data_)) {
                return this;
            }
            for (Int64 i = NumChildren - 1; i >= 0; ++i) {
                if (auto child = GetChild(i)) {
                    if (TreeNode *result = child->FindDFS(f, order)) {
                        return result;
                    }
                }
            }
        }
        return nullptr;
    }

private:
    T data_;
    TreeNode *children_[NumChildren];
};

template<typename T>
using BinaryTreeNode = TreeNode<T, 2>;

// 一些特定操作
template<typename T>
BinaryTreeNode<T> *Left(BinaryTreeNode<T> *node) {
    if (node) {
        return node->GetChild(0);
    }
    return nullptr;
}

template<typename T>
BinaryTreeNode<T> *Right(BinaryTreeNode<T> *node) {
    if (node) {
        return node->GetChild(1);
    }
    return nullptr;
}
