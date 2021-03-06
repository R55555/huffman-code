#include<iostream>
#include<vector>
#include<sstream>

class Node{
        public:
                char character;
                int frequency;
                Node * left;
                Node * right;  
                
        Node(char c, int f, Node* l, Node* r){
                character = c;
                frequency = f;
                left = l;
                right = r;
        }
                   
};

void free_nodes(std::vector<Node *>& nodes){
        for(int i=0; i<nodes.size(); i++){
                free(nodes[i]);
                nodes[i] = NULL;
        }
}

void get_char_frequency(std::vector<Node *>& nodes, std::string str){
        char ch;
        int i,flag;
        std::stringstream strstream;
        strstream << str;
        while(strstream>>ch){
                flag = 0;
                for(i=0; i<nodes.size(); i++){
                        if(nodes[i]->character==ch){
                                nodes[i]->frequency+=1;
                                flag = 1;
                                continue;
                        }
                }
                if(!flag){
                        Node* node = new Node(ch,1,NULL,NULL);
                        nodes.push_back(node);
 
                }
                
        }
}

void min_heapify(std::vector<Node *>& nodes, int i, int size){
	int l,r,smallest;
	l=2*i+1;
	r=2*i+2;
	Node * temp;

	smallest = i;
	if(l<size && ((nodes[l]->frequency)<(nodes[smallest]->frequency)))
		smallest = l;
	if(r<size && ((nodes[r]->frequency)<(nodes[smallest]->frequency)))
		smallest = r;
	if(smallest != i){
		temp = nodes[i];
		nodes[i]=nodes[smallest];
		nodes[smallest]=temp;
		min_heapify(nodes,smallest,size);
	}
}

void build_min_heap(std::vector<Node *>& nodes, int size){
        for(int i=(size-1)/2; i>=0; i--)
                min_heapify(nodes, i, size);
           
}       
Node* extract_min(std::vector<Node *>& nodes, int &size){
        Node * min  = nodes[0];
        if(size>0){
                nodes[0] = nodes[size-1];
                nodes[size-1] = min;
                size--;
                return min;
        }

        std::cout<<"Tree underflow"<<std::endl;
        return NULL;
        
        
}

void print_character_frequency(std::vector<Node *>& nodes){
        for(int i=0; i<nodes.size(); i++){
                std::cout<<i<<" : "<<nodes[i]->character<<" - "<<nodes[i]->frequency<<std::endl;
        }
}

void build_huffman_tree(std::vector<Node *>& nodes, std::string str){
        int size;
        Node* min1;
        Node* min2;
        get_char_frequency(nodes, str);
        size=nodes.size();

        build_min_heap(nodes,size);
        while(size>1){
               min1 = extract_min(nodes,size);
               build_min_heap(nodes,size);
               min2 = extract_min(nodes,size);
               build_min_heap(nodes,size);
               Node* node = new Node('z',(min1->frequency+min2->frequency), min1, min2);
               nodes[size]=node;
               size++;
               build_min_heap(nodes,size);
        }
        min1=NULL;
        min2=NULL;
        
}

void get_huffman_code(Node* node, std::string str){
        if(node->left!=NULL){
                str.push_back('0');
                get_huffman_code(node->left, str);
                str.pop_back();
        }
        if(node->right!=NULL){
                str.push_back('1');
                get_huffman_code(node->right, str);
                str.pop_back();
        }
        if(node->left==NULL && node->right==NULL){
                std::cout<<node->character<<" : "<<node->frequency<<" :: "<<str<<std::endl;
        }
}



int main(){
        int size;
        std::string str, code;
        std::vector<Node *> nodes;
        getline(std::cin, str);
        build_huffman_tree(nodes, str);
        get_huffman_code(nodes[0], code);
        free_nodes(nodes);
        nodes.clear();

        

}
