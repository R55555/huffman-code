#include<iostream>
#include<vector>
#include<unordered_map> 
#include<fstream>
#define ull unsigned long
#define head_size int  
#define byte_count int

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

class Node_decode{
        public:
                char character;
                Node_decode* left;
                Node_decode* right; 
                 
        Node_decode(char c, Node_decode* l, Node_decode* r){
                character = c;
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

void free_huffman_tree(Node* node){//to free huffman tree during encode
        if(node->left!=NULL){
                free_huffman_tree(node->left);
        }
        free(node->left);
        node->left=NULL;
        if(node->right!=NULL){
                free_huffman_tree(node->right);
        }
        free(node->right);
        node->right=NULL;
} 

void free_huffman_tree(Node_decode* node){//to free huffman tree during decode
        if(node->left!=NULL){
                free_huffman_tree(node->left);
        }
        free(node->left);
        node->left=NULL;
        if(node->right!=NULL){
                free_huffman_tree(node->right);
        }
        free(node->right);
        node->right=NULL;
}

void get_char_frequency(std::vector<Node *>& nodes, std::ifstream &file){
        char ch;
        int i,flag;
        while(file.get(ch)){//get is used to include spaces, newlines...
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
        //print_character_frequency(nodes); //to print the character frequency
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

Node_decode* build_huffman_tree_from_map(std::unordered_map<char, std::string> m){ 
        Node_decode* node = new Node_decode('a', NULL, NULL); //'a'is just a placeholder.
        Node_decode *head, *temp;
        char ch;
        head = node;

        for(auto i : m){
                ch = i.first;
                for(char c : i.second){
                        if(c=='1'){
                                if(node->right==NULL){
                                        temp = new Node_decode(ch, NULL, NULL);
                                        node->right = temp; 
                                }
                                node=node->right;                                
                        } else {
                                if(node->left==NULL){
                                        temp = new Node_decode(ch, NULL, NULL);
                                        node->left = temp; 
                                }
                                node=node->left;                                
                        }
                }
                node->character = ch; // to place the character at the leaves
                node = head;
        }
        temp = NULL;
        return node;
}

void get_huffman_code(Node_decode* node, std::string str, std::unordered_map<char,std::string> &m){//for testing the code
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

void print_huffman_code(std::unordered_map<char, std::string> m){//for testing the code
        for(auto i: m){
               std::cout<<i.first<<" : "<<i.second<<std::endl;
        }
}

int decode(std::string file_to_decode){
        unsigned char data_mask=128;
        unsigned char counter;
        std::unordered_map<char,std::string> m; 
        std::string str;
        char ch, byte, mask;//can reuse any two. Do it if every test passes.
        head_size size;
        byte_count count;
        ull last_byte, offset; 
        
        std::ifstream file(file_to_decode, std::ios::binary);
        std::ofstream out("output.txt", std::ios::binary);
        
        if(!file.is_open() || !out.is_open()){
                std::cerr<<"Cannot open file to decode"<<std::endl;
                return -1;
        }      

        std::cout<<std::endl<<"Decoding..."<<std::endl<<std::endl;
        
        //header decode start
        
        file>>size;
        std::cout<<"Header size : "<<size<<std::endl;
        for(head_size i=0; i<size; i++){ //to decode the data to an unordered_map m
                str="";
                file.get(ch);
                //std::cout<<ch<<" ";
                file.read((char *)&count, sizeof(byte_count));
                //std::cout<<count<<" : ";
                for(byte_count j=0; j<count; j++){
                        file.get(byte);
                        for(counter = 0; counter<8; counter++){
                                if(byte & data_mask){
                                        //std::cout<<1;
                                        str.push_back('1');
                                } else {
                                        //std::cout<<0;
                                        str.push_back('0');
                                }
                                byte = byte<<1;
                        }
                }
                file.get(byte);
                file.get(mask);
                while(mask & data_mask){
                        if(byte & data_mask){
                                //std::cout<<1;
                                str.push_back('1');
                        } else {
                                //std::cout<<0;
                                str.push_back('0');
                        }
                        mask = mask<<1;
                        byte = byte<<1;
                }
                m[ch]=str;
                
        }

        //now we have character and huffman code in unordered_map
        //header decode end
        
        //Next we have to build huffman tree from the unordered map
        
        //Build huffman tree begin
        Node_decode* node = build_huffman_tree_from_map(m); //complete
        std::unordered_map<char,std::string> test;
        //Build huffman tree end
        
        offset=file.tellg();
        file.seekg(-1, std::ios::end);
        last_byte=file.tellg(); // get the last byte - the last byte is having padding information
        file.get(mask);
        file.seekg(offset, std::ios::beg);
        
        //decode data starts
        Node_decode *head = node;
        while(file.get(ch)&&(file.tellg()<last_byte)){
                for(counter=0; counter<8; counter++){
                        if(ch & data_mask){
                                //std::cout<<1;
                                if(head->right==NULL){
                                        std::cerr<<"Error no right child"<<std::endl;
                                        return -1;
                                }
                                else{
                                        head = head->right;
                                }
                        }
                        else{
                                //std::cout<<0;
                                if(head->left==NULL){
                                        std::cerr<<"Error no left child"<<std::endl;
                                        return -1;
                                }
                                else{
                                        head = head->left;
                                }
                        }
                        if((head->left==NULL) && (head->right==NULL)){
                                        std::cout<<head->character;
                                        out.put(head->character);
                                        head=node;       
                        }
                        ch=ch<<1;
                }
        }
        while(mask & data_mask){
                if(ch & data_mask){
                                //std::cout<<1;
                        if(head->right==NULL){
                                std::cerr<<"Error no right child"<<std::endl;
                                return -1;
                        }
                        else{
                                head = head->right;
                        }
                }
                else{
                        //std::cout<<0;
                        if(head->left==NULL){
                                std::cerr<<"Error no left child"<<std::endl;
                                return -1;
                        }
                        else{
                                head = head->left;
                        }
                }
                if((head->left==NULL) && (head->right==NULL)){
                        std::cout<<head->character;
                        out.put(head->character);
                        head=node;       
                }
                ch=ch<<1;
                mask=mask<<1;
        
        }
        free_huffman_tree(node); //deleting the huffman tree
        //decode data ends
        
        std::cout<<std::endl<<std::endl<<"Decoding ends"<<std::endl;
        
        out.close();
        file.close();
        return 1;
        
}

int encode(std::string file_to_encode){
        std::string code;
        std::vector<Node *> nodes;//to build huffman tree;
        std::unordered_map<char,std::string> m;
        std::ifstream file(file_to_encode, std::ios::binary);
        ull offset;
        
        if(!file.is_open()){
                std::cerr<<"Cannot open file"<<std::endl;
                return -1;
        }
        if(file.peek()==EOF){
                        std::cerr<<"File empty"<<std::endl;
                        return -1;
        }
        
        build_huffman_tree(nodes, file);
        get_huffman_code(nodes[0], code, m); //Now I have the huffman code inside the unordered_map m.       
        file.clear();
        file.seekg(0,std::ios::beg);//to reach the beginning of the file to start encoding.
        
        //encoding starts here
        std::ofstream out("encoded.huff");
        unsigned char counter, byte, mask, lastbits, append=1, data_mask=128;
        std::string str;
        char ch;
        head_size size;
        byte_count count,j;
        
        std::cout<<"Encoding..."<<"\n"; 
     
        //header begin 
        
        //1. enter the total number of characters (size). 
        //2. add the character
        //3. add the size of huffamn code in bits divided by 8(integer division) to get bytes that are not padded
        //4. add the final byte of the huffman code with padding
        //5. add the mask to identify the padded bits
        //6. if no more characters then end, else goto step 2.
        
        
        size=m.size();
        out<<size;
        
        for(auto i: m){
                ch=i.first; 
                out.put(ch);
                
                count = (i.second.size())/8;
                out.write(reinterpret_cast<const char *>(&count), sizeof(byte_count));
                counter = 0;
                
                for(j=0; j<(count*8); j++){
                        char c = i.second[j];
                        byte=byte<<1;
                        if(c=='1'){
                                byte=byte|append;
                        }
                        counter++;
                        if(counter==8){
                                out.put(byte);
                                byte=0;
                                counter=0;
                        }
                }
                lastbits=i.second.size()-j; //to get the size of last unpadded bits.
                for(unsigned char iter=0; iter<lastbits; iter++){
                        char c = i.second[j+iter];
                        byte=byte<<1;
                        if(c=='1'){
                                byte=byte|append;
                        }
                }
                        
                byte=byte<<(8-lastbits);//padding extra bits with 0
                mask=0;
                for(unsigned char iter=0; iter<lastbits; iter++){
                        mask = mask>>1;
                        mask = mask|data_mask;
                }
                out.put(byte);       
                out.put(mask);                 
        }
        
        //data encoding starts
        //huffman code is written as bits.
        counter=0; byte=0;
        while(file.get(ch)){
                str=m[ch];
                for(char i: str){
                        byte=byte<<1;
                        if(i=='1'){
                                byte=byte|append;   
                        }
                        counter++; 
                        if(counter==8){
                                counter=0;
                                out.put(byte);
                                byte=0;
                        }
                }
        }
        if(counter!=8){//padding extra bits
                byte=byte<<(8-counter);
                out.put(byte);
                byte=0;
               
                for(unsigned char iter=0; iter<counter; iter++){
                        byte=byte>>1;
                        byte = byte|data_mask;
                }
                out.put(byte);
        } 
        //data encoding ends
        
        file.close();
        out.close();
        
        std::cout<<"Encoding complete"<<"\n";
        
        m.clear();

        free_huffman_tree(nodes[0]);
        nodes[0]=NULL;
        
        nodes.clear();
        return 1;
        
}


int main(int argc, char* argv[]){
        
        if(argc==1 || argc>2){
                std::cerr<<"Invalid number of arguments"<<std::endl;
                return -1;
        }
                
        encode(argv[1]);
        decode("encoded.huff");
        
        return 0;

        
}
