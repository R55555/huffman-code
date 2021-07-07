#include<iostream>
#include<vector>
#include<unordered_map> 
#include<fstream>
#include<typeinfo>
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

int decode(std::string file_to_decode, std::unordered_map<char,std::string> m, Node *node){
        unsigned char data_mask=128, extra_mask=1;
        unsigned char counter;
        //std::unordered_map<char,std::string> m; required in the next step to get from headers
        char ch, end;
        head_size size;
        byte_count count;
        ull last_byte;
        
        std::ifstream file(file_to_decode, std::ios::binary);
        std::ifstream file_end(file_to_decode, std::ios::binary);
        if(!file.is_open() || !file_end.is_open()){
                std::cerr<<"Cannot open file to decode"<<std::endl;
                return -1;
        }      
        //std::cout<<std::endl<<offset<<std::endl;
        
        //header decode start
        
        std::cout<<std::endl<<std::endl;
        file>>size;
        std::cout<<size;
        for(head_size i;i<size; i++){
                file.get(ch);
                std::cout<<"'"<<ch<<":";
                file>>count;
                std::cout<<count<<"'";
                for(byte_count iter=0; iter<count; iter++){
                        file.get(ch);
                        for(counter=0; counter<8; counter++){
                                if(ch&data_mask){
                                        std::cout<<1;
                                } else {
                                        std::cout<<0;
                                }
                                ch=ch<<1;
                        }
                }
                file.get(ch); // to get the mask if extra bytes is present
                file.get(end); //the last byte
                while(ch&data_mask){
                        if(end&data_mask){      
                                std::cout<<1;
                        } else {
                                std::cout<<0;
                        }
                        end=end<<1;
                        ch=ch<<1;
                }        
        }
        
        //header decode end
        
        file_end.seekg(-1, std::ios::end);
        last_byte=file_end.tellg(); // get the last byte - the last byte is having padding information
        file_end.get(end);
        
        //decode data starts
        Node *head = node;
        std::cout<<std::endl;
        while(file.get(ch)&&/*file_next.get(next_ch)*/(file.tellg()<last_byte)){
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
                                        head=node;       
                        }
                        ch=ch<<1;
                }
        }
        while(end & data_mask){
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
                        head=node;       
                }
                ch=ch<<1;
                end=end<<1;
        
        }
        //decode data ends
        
        file_end.close();
        file.close();
        return 1;
        
}

int encode(std::string file_to_encode){
        std::string code;
        std::vector<Node *> nodes;
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
        std::cout<<std::endl;
        print_huffman_code(m); //to print huffman code
        file.clear();
        file.seekg(0,std::ios::beg);//to reach the beginning of the file to start compression.
        
        //encoding starts here
        std::ofstream out("hihihi.bin");
        unsigned char counter, byte, mask, append=1, data_mask=128;
        std::string str;
        char ch;
        head_size size;
        byte_count count,j;
        
        
     
        //header begin 
        
        //1. enter the total number of characters size. 
        //2. add the character
        //3. add the size of huffamn code in bits divided by 8(integer division) to get bytes that are not padded
        //4. add the mask to identify the padded bits
        //5. add the final byte of the huffman code with padding
        //6. if no more characters then end, else goto step 2.
        
        size=m.size();
        out<<size;     
        for(auto i: m){
                out<<i.first;   
                std::cout<<"'"<<i.first<<":";
                count = (i.second.size())/8;
                out<<count;
                std::cout<<count<<"'";
                counter = 0;
                
                for(j=0; j<count*8; j++){//problem found here and solved
                        char c = i.second[j];
                        if(c=='1'){
                                byte=byte<<1;
                                std::cout<<1;
                                byte=byte|append;
                        } else if(c=='0'){
                                byte=byte<<1;
                                std::cout<<0;
                        }
                        counter++;
                        if(counter==8){
                                out.put(byte);
                                byte=0;
                                counter=0;
                        }
                }
                
                for(unsigned char iter=0; iter<((i.second.size())-j); iter++){
                        char c = i.second[j+iter];
                        if(c=='1'){
                                byte=byte<<1;
                                std::cout<<1;
                                byte=byte|append;
                        } else {
                                byte=byte<<1;
                                std::cout<<0;
                        }
                }
                        
                byte=byte<<(8-((i.second.size())-j));
                mask=0;
                for(unsigned char iter=0; iter<((i.second.size())-j); iter++){
                        mask = mask>>1;
                        mask = mask|data_mask;
                }
                out.put(mask);
                out.put(byte);       
                 
                
        }
        
        std::cout<<std::endl<<"Pointer pos after head : "<<out.tellp()<<std::endl;
       
        //header end
       
        offset=out.tellp(); // to test decode
        
        //data encoding starts
        //huffman code is written as bits.
        counter=0; byte=0;
        while(file.get(ch)){
                str=m[ch];
                for(char i: str){
                        if(i=='1'){
                                byte=byte<<1;
                                byte=byte|append;
                                //std::cout<<i;     
                        }
                        else if(i=='0'){
                                byte=byte<<1;   
                                //std::cout<<i;    
                        }
                        counter++; 
                        if(counter==8){
                                //std::cout<<counter;
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
        
        decode("hihihi.bin", m, nodes[0]); //to test decode
       
        
        m.clear();
        free_nodes(nodes);
        nodes.clear();
        
        return (8-counter); //returns the number of padded bits. It was used in testing
        
}


int main(int argc, char* argv[]){
        int padding; //to store the no. of padding bits. For testing.
        
        if(argc==1 || argc>2){
                std::cerr<<"Invalid number of arguments"<<std::endl;
                return -1;
        }
                
        padding = encode(argv[1]);
        
        return 0;

        
}
