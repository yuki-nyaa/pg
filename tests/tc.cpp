#include<yuki/Vector.hpp>
#include<cstdio>

struct Node{
    int i;
    int sum=0;
    yuki::Vector<Node*> children;
    size_t depth=0;
};

yuki::Vector<Node*> stack;

void traverse(Node* const node){
    stack.emplace_back(node);
    const size_t depth_start = stack.size();
    node->depth=depth_start;
    for(Node* const c : node->children){
        if(c->depth==0){
            traverse(c);
            node->sum += c->sum;
        }else if(c!=node){
            node->sum += c->sum;
        }
        if(c->depth < node->depth)
            node->depth = c->depth;
    }
    node->sum += node->i;
    if(node->depth==depth_start){
        while(1){
            Node* const pop = stack.pop_back_v();
            pop->depth = -1;
            if(pop==node)
                return;
            else
                pop->sum = node->sum;
        }
    }
}

int main(){
    {
    Node n1 = {1};
    Node n2 = {2};
    Node n3 = {3};
    Node n4 = {4};

    n1.children.emplace_back(&n3);
    n1.children.emplace_back(&n2);

    n2.children.emplace_back(&n4);
    n2.children.emplace_back(&n1);

    traverse(&n1);

    printf("%d %d %d %d\n",n1.sum,n2.sum,n3.sum,n4.sum);
    }

    {
    Node n1 = {1};
    Node n2 = {2};
    Node n3 = {3};
    Node n4 = {4};
    Node n5 = {5};
    Node n6 = {6};
    Node n7 = {7};

    n1.children.emplace_back(&n2);
    n1.children.emplace_back(&n5);

    n2.children.emplace_back(&n3);

    n3.children.emplace_back(&n2);
    n3.children.emplace_back(&n3);
    n3.children.emplace_back(&n4);

    n4.children.emplace_back(&n3);

    n5.children.emplace_back(&n6);

    n6.children.emplace_back(&n7);

    n7.children.emplace_back(&n3);
    n7.children.emplace_back(&n6);


    traverse(&n1);

    printf("%d %d %d %d %d %d %d\n",n1.sum,n2.sum,n3.sum,n4.sum,n5.sum,n6.sum,n7.sum);
    }
}
