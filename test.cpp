#include<iostream>
#include<vector>
#include<unordered_map> 
#include<fstream>
#include<typeinfo>
#define ull unsigned long long

class Node{
        public:
                char character;
                ull frequency;
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

void get_char_frequency(std::vector<Node *>& nodes, std::ifstream &file){
        char ch;
        int i,flag;
        while(file.get(ch)){//get is used to include spaces, newlines
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

void build_huffman_tree(std::vector<Node *>& nodes, std::ifstream &file){
        int size;
        Node* min1;
        Node* min2;
        get_char_frequency(nodes, file);
        print_character_frequency(nodes);
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


void get_huffman_code(Node* node, std::string str, std::unordered_map<char,std::string> &m){
        if(node->left!=NULL){
                str.push_back('0');
                get_huffman_code(node->left, str, m);
                str.pop_back();
        }
        if(node->right!=NULL){
                str.push_back('1');
                get_huffman_code(node->right, str, m);
                str.pop_back();
        }
        if(node->left==NULL && node->right==NULL){
                m[node->character] = str;
        }
}

void print_huffman_code(std::unordered_map<char, std::string> m){
        //to do
        for(auto i: m){
               std::cout<<i.first<<" : "<<i.second<<std::endl;
        }
}

void encode(std::ifstream &file, std::unordered_map<char,std::string> m){
        std::ofstream out("hihihi.bin");
        unsigned char byte = 0, append=1;
        std::string str;
        int counter=0;
        char ch;
        while(file.get(ch)){
                str=m[ch];
                for(char i: str){
                        if(counter==8){
                                //std::cout<<counter;
                                counter=0;
                                out.put(byte);
                                byte=0;
                        }
                        if(i=='1'){
                                byte=byte<<1;
                                byte=byte|append;
                                std::cout<<i;     
                        }
                        else if(i=='0'){
                                byte=byte<<1;   
                                std::cout<<i;    
                        }
                        counter++; 
                }
        }
        if(counter!=8){
                byte=byte<<(8-counter);
                out.put(byte);
        }
        out.close();
}

void decode(std::ifstream &file, std::unordered_map<char,std::string> m, Node *node){
        unsigned char mask=128;
        char ch;
        Node *head = node;
        std::cout<<std::endl;
        while(file.get(ch)){
                for(int counter=0; counter<8; counter++){
                        if((head->left==NULL) && (head->right==NULL)){
                                        std::cout<<head->character;
                                        head=node;       
                        }
                        if(ch & mask){
                                //std::cout<<1;
                                if(head->right==NULL){
                                        std::cerr<<"Error no right child"<<std::endl;
                                        return;
                                }
                                else{
                                        head = head->right;
                                }
                        }
                        else{
                                //std::cout<<0;
                                if(head->left==NULL){
                                        std::cerr<<"Error no left child"<<std::endl;
                                        return;
                                }
                                else{
                                        head = head->left;
                                }
                        }
                        ch=ch<<1;
                }
        }
        
}


int main(int argc, char* argv[]){
        int size;
        std::string code;
        std::vector<Node *> nodes;
        std::unordered_map<char,std::string> m;
        std::ifstream file(argv[1]);
        if(file.is_open()){
                if(file.peek()==EOF){
                        std::cerr<<"File empty"<<std::endl;
                        return -1;
                }
                build_huffman_tree(nodes, file);
                get_huffman_code(nodes[0], code, m); //Now I have the huffman code inside the unordered map.       
                std::cout<<std::endl;
                print_huffman_code(m);
                file.clear();
                file.seekg(0,std::ios::beg);//to reach the beginning of the file to start compression.
                encode(file, m);//encoding using huffman-code
                file.close();
        }
        else{
                std::cerr<<"Cannot open file"<<std::endl;
                return -1;
        }
        std::ifstream dfile("hihihi.bin", std::ios::binary);//to decode
        if(dfile.is_open()){
                decode(dfile, m, nodes[0]);
                //Next don't decode the padded bits. Do something for that.
                dfile.close();
        }
        else{
                std::cerr<<"Cannot open encoded file"<<std::endl;
                return -1;
        } 
        
        
        m.clear();
        free_nodes(nodes);
        nodes.clear();
        return 0;

        
}
