#include <iostream>
#include <stack>
using namespace std ;

class MyStack {
	public:
		void push (int x) {
			// push into the original stack and maintain the aux st with max and its count
			st.push(x);
			if (!aux_st.empty() ){
				if (aux_st.top().first > x) { //do nothing
				}
				else if( aux_st.top().first < x ){ // add and init count to 1
					aux_st.push( make_pair(x,1) );
				} else { //equals 
					aux_st.top().second += 1;
				}
			} else {
				aux_st.push( make_pair(x,1) );
			}
		}
 
		int  pop () {
			if (empty()){
				return -1; //ideally throw exception
			}
			// pop from original stack and update the helper stack count
			// no need to return anything for now as the problem does not need it
			st.pop();
			aux_st.top().second -= 1 ;
			if ( 0 == aux_st.top().second ) {
				aux_st.pop();
			}

		}

		bool empty() {
			return st.empty();
		}
		
		void printStack(){
			cout << "STACK :" << std::endl;
			#if 0 
			auto x = st.begin();
			for ( ; x++ ; x != st.end() ) {
				cout << *x << std::endl ;
			}
			#endif
		}
		void printMax() {
			cout << "MAX: " <<aux_st.top().first << " COUNT: " << aux_st.top().second << std::endl;
		}
				
	private:
		stack<int> st; //original stack
		stack< pair<int,int> > aux_st; //helper stack to store the current max and its occurence count
};

int main() {
        MyStack mStack;
	mStack.push(2);
	mStack.push(3);
	mStack.push(4);
	mStack.push(4);

	mStack.printStack();
	mStack.printMax();	
	return 0;
}
