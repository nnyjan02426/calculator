#include <iostream>
#include <string>
using namespace std;

typedef struct node{
    double data;
    struct node *left_child;
    struct node *right_child;
    struct node *parent;
    struct node *next;
    struct node *previous;
    bool is_operator;
}Node;

typedef struct stack{
    double data;
    stack *below;
}Stack;


class calculator {
    string input;
    int input_len;
    public:
        Node *input_head; //input (ListNode) 第一項之地址
        Node *input_end;  //input (ListNode) 最後一項之地址
        Node *tree_top;   //binary tree最頂部之地址
        Stack *stack_top; //stack的最上層

        Node* create_input_node(string data, Node *previous){ //建立node來存input中的資料
            Node *new_node = new Node;
            if(previous==NULL){
                input_head = new_node;
            }

            double data_d;
            switch(data[0]){
                case '+':
                    new_node->is_operator = 1;
                    data_d = int('+');
                    break;
                case '-':
                    new_node->is_operator = 1;
                    data_d = int('-');
                    break;
                case '*':
                    new_node->is_operator = 1;
                    data_d = int('*');
                    break;
                case '/':
                    new_node->is_operator = 1;
                    data_d = int('/');
                    break;
                case '(':
                    new_node->is_operator = 1;
                    data_d = int('(');
                    break;
                case ')':
                    new_node->is_operator = 1;
                    data_d = int(')');
                    break;
                default:
                    new_node->is_operator = 0;
                    data_d = stod(data);
                    break;
            }

            new_node->data = data_d;
            new_node->left_child = NULL;
            new_node->right_child = NULL;
            new_node->parent = NULL;
            new_node->previous = previous;
            new_node->next = NULL;

            return new_node;
        }

        void get_valid_input(){ //使用者輸入算式
            while(1){ //當使用者輸入合格的算式，將結束loop
                cout << endl << "輸入算式： ";
                getline(cin, input);
                //input.erase(remove(input.begin(), input.end(), ' '), input.end()); //刪去空白，此行可加可不加
                input_len = input.length();

                //檢查算式是否有誤，若無錯誤，則結束loop
                cout << "Checking if it is valid input..." << endl;
                if(check_if_valid_input()){
                    break;
                }
            }
            
            cout << "Calculating..." << endl;
        }

        bool check_if_valid_input(){ //檢查算式是否有錯誤，並告訴使用者是哪裡有誤
            /*可能錯誤：
                1. 左右括號數目不相同 
                2. 連續出現二個運算符號
                3. 無運算符號
                4. 出現數字，括號，運算符號外的字元
                5. 運算符號後無數字或左括弧
            */
            int brackets_cout = 0;
            bool has_operator = 0;
            for(int i=0; i<input_len; i++){
                switch (input[i]) {
                    case '(':
                        brackets_cout++;
                        break;
                    case ')':
                        if(brackets_cout){
                            brackets_cout--;
                        }
                        else{
                            cout << "[Invalid Input] left and right brackets doesn't match." << endl;
                            return 0;
                        }
                        break;
                    case '+': case '-': case '*': case '/':
                        has_operator = 1;
                        if(input[i+1] && ((input[i+1] == '+')||(input[i+1] == '-')||(input[i+1] == '*')||(input[i+1] == '/'))){
                            cout << "[Invalid Input] two operators appear in a row." << endl;
                            return 0;
                        }
                        else if((input[i+1] && (input[i+1] == ')')) || !input[i+1]){
                            cout << "[Invalid Input] no numbers after an operator." << endl;
                            return 0;
                        }
                        break;
                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '.':
                        break;
                    default:
                        cout << "[Invalid Input] entered unknown character." << endl;
                        return 0;
                        break;
                }
            }

            if(!has_operator){
                cout << "[Invalid Input] no operator in the input." << endl;
                return 0;
            }
            else if(brackets_cout){
                cout << "[Invalid Input] left and right brackets doesn't match." << endl;
                return 0;
            }
            else{
                cout << "[Valid Input]" << endl;
                return 1;
            }
        }

        void input_to_node(){ //將input中的數字，符號換為listnode, 以便分辨每一項(主要是為了方便分辨數字)
            Node *previous = NULL;
            //Node *next = NULL;
            Node *current;
            string str = "";

            for(int i=0; i<input_len; i++){
                switch(input[i]){
                    case '+': case '-': case '*': case '/': case '(': case ')':
                        if(str!=""){
                            current = create_input_node(str, previous);
                            if(previous){
                                previous->next = current;
                            }
                            previous = current;
                            str = "";
                        }
                        current = create_input_node(string(1, input[i]), previous);
                        if(previous){
                            previous->next = current;
                        }
                        previous = current;
                        break;
                    default:
                        str += input[i];
                        break;
                }
            }

            if(str!=""){
                current = create_input_node(str, previous);
                previous->next = current;
            }
            input_end = current;
        }

        void print_input(){ //測試用 for testing
            cout << "Operaters:";
            cout << " '+'=" << int('+');
            cout << " '-'=" << int('-');
            cout << " '*'=" << int('*');
            cout << " '/'=" << int('/');
            cout << " '('=" << int('(');
            cout << " ')'=" << int(')') << endl;

            Node *current = input_head;
            while(current){
                cout << "Data: " << current->data;
                if(current->previous){
                    cout << "  Previous: " << current->previous->data;
                }
                if(current->next){
                    cout << "  Next: " << current->next->data;
                }
                cout << "  Operator: " << current->is_operator << endl;
                current = current->next;
                cout << endl;
            }
        }

        Node* brackets_to_tree(Node *head){ //遇到括弧先計算
            int bracket_cout = 1;
            Node *left_bracket_ptr = head; //儲存左括弧的地址
            Node *right_bracket_ptr = left_bracket_ptr;  //儲存右括弧的地址
            head = head->next;

            //找出與此左括弧對應的右括弧
            while(bracket_cout){
                right_bracket_ptr = right_bracket_ptr->next;
                if(right_bracket_ptr->data == int(')')){
                    bracket_cout--;
                }
                else if(right_bracket_ptr->data == int('(')){
                    bracket_cout++;
                }
            }

            //將括弧內的算式轉為tree，並將其頂端存為new_node
            Node *new_node = create_tree(left_bracket_ptr->next, right_bracket_ptr->previous); //將括號內算式轉為binary tree
            
            //使new_node連入input(LinkedList)
            if(left_bracket_ptr->previous){
                left_bracket_ptr->previous->next = new_node;
                new_node->previous = left_bracket_ptr->previous;
            }
            else{
                new_node->previous = NULL;
            }
            if(right_bracket_ptr->next){
                right_bracket_ptr->next->previous = new_node;
                new_node->next = right_bracket_ptr->next;
            }
            else{
                new_node->next = NULL;
            }
            delete left_bracket_ptr;
            delete right_bracket_ptr;

            return new_node;
        }

        Node* create_tree(Node *head, Node *end){ //將input（中序式）轉為 binary tree
            // left operator right (e.g. 2 + 3)
            Node *left;
            Node *operater;
            Node *right;

            while(1){
                if(head->data == int('(')){ //若欲括弧，則先將括弧轉為tree
                    left = brackets_to_tree(head); 
                }
                else{
                    left = head;
                }
                
                if(left->next){
                    operater = left->next;

                    if(operater->next){
                        if(operater->next->data == int('(')){
                            right = brackets_to_tree(operater->next);
                        }
                        else{
                            right = operater->next;
                        }
                        
                    }
                    else{
                        return left;
                    }
                
                    
                    //先乘除後加減
                    if(((operater->data == int('+')) || (operater->data == int('-'))) && right->next){
                        if((right->next->data == int('*')) || (right->next->data == int('/'))){
                            Node *new_end;
                            if(right->next->next->data == int('(')){
                                new_end = brackets_to_tree(right->next->next);
                            }
                            else{
                                new_end = right->next->next;
                            }
                            right = create_tree(right, new_end);
                            if((new_end == end) || ((new_end->next->data == int(')')) && (new_end->next==end))){
                                end = right;
                            }
                        }
                    }

                    //製作樹
                    //使left成為operater的child, 並脫離原本的linkedlist
                    operater->left_child = left;
                    if(left->previous){
                        operater->previous = left->previous;
                        left->previous->next = operater;
                    }
                    left->parent = operater;
                    left->previous = NULL;
                    left->next = NULL;

                    //使right成為operater的child, 並脫離原本的linkedlist
                    operater->right_child = right;
                    if(right->next){
                        operater->next = right->next;
                        right->next->previous = operater;
                    }
                    right->parent = operater;
                    right->previous = NULL;
                    right->next = NULL;

                    //將operater，即為新tree的top設為下一回的left(會於每個loop的一開始將left設為head)
                    head = operater;

                    //若right為linkedlist中的最後一項，代表已將linkedlist轉為tree，則結束loop
                    if(right == end){
                        break;
                    }
                }
                else{ //若left = brackets_to_tree() 且無下一項時，將treetop定為left，結束loop
                    head = left;
                    break;
                }    
            }
            //已將linkedlist轉為tree，結束函式並回傳tree top
            return head;
        }

        void tree_to_stack(Node *tree_node){ //將tree轉為stack, 若tree node的data為運算符號, 則從stack中拿出最上方的二個數字做計算
            if(tree_node->left_child){ //binary tree中，有left_child就會有right_child
                tree_to_stack(tree_node->left_child);
                tree_to_stack(tree_node->right_child);
            }
            
            if(!(tree_node->is_operator)){ //若此node的data為數字，則將data存入stack
                Stack *stack = new Stack;
                stack->data = tree_node->data;
                stack->below = stack_top;
                stack_top = stack;
            }
            else{ //若此node的data為運算符號，則從stack中拿出兩個數做計算
                switch(char(int(tree_node->data))){
                    case '+':
                        stack_top->below->data = stack_top->below->data + stack_top->data;
                        break;
                    case '-':
                        stack_top->below->data = stack_top->below->data - stack_top->data;
                        break;
                    case '*':
                        stack_top->below->data = stack_top->below->data * stack_top->data;
                        break;
                    case '/':
                        stack_top->below->data = stack_top->below->data / stack_top->data;
                        break;
                    default:
                        break;
                }
                if(stack_top->below){
                    Stack *tmp = stack_top;
                    stack_top = stack_top->below;
                    delete tmp;
                }
            }

        }
};

int main(){
    calculator Calculator;
    Calculator.get_valid_input(); //使用者輸入算式, 並檢查算式是否有誤，若有誤，使用者將再次輸入算式
    Calculator.input_to_node(); //將算式轉為LinkedList

    Node* tree_top = Calculator.create_tree(Calculator.input_head, Calculator.input_end); //將input由linkedlist轉為tree，並回傳tree_top的地址
    Calculator.tree_to_stack(tree_top); //將tree轉為stack，若遇到運算符號，則從stack中拿出最上方的二個數做計算
    double result = Calculator.stack_top->data; //算式的計算結果儲存於stack_top，因此項為private，故使用get_result() 來得到此值
    
    cout << "Result: " << result << endl;
}