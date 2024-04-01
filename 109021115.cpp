#define FILE_EXTENSION ".txt"
#include<fstream>
#include<string>
#include<cstring>
#include<vector>
#include<iostream>
#include <algorithm>

using namespace std;
using ll = long long ;

// Utility Func

// string parser : output vector of strings (words) after parsing
vector<string> word_parse(vector<string> tmp_string){
	vector<string> parse_string;
	for(auto& word : tmp_string){
		string new_str;
    	for(auto &ch : word){
			if(isalpha(ch))
				new_str.push_back(ch);
		}
		parse_string.emplace_back(new_str);
	}
	return parse_string;
}

vector<string> split(const string& str, const string& delim) {
	vector<string> res;
	if("" == str) return res;
	//先將要切割的字串從string型別轉換為char*型別
	char * strs = new char[str.length() + 1] ; //不要忘了
	strcpy(strs, str.c_str());

	char * d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while(p) {
		string s = p; //分割得到的字串轉換為string型別
		res.push_back(s); //存入結果陣列
		p = strtok(NULL, d);
	}

	return res;
}

bool check(string s,const vector< vector<int> >&trie,const vector< vector<int> >&trie_rev,
           const vector<bool>&is_terminal){
	ll trav;
	if(s[0]=='*'){
		reverse(s.begin(),s.end());
		trav=0;
		ll len=s.size();
		for(int i=1;i<len-1;i++){
			if(s[i]<'a') s[i]+=32;
			if(trie_rev[trav][s[i]-'a']==0){
                return 0;
            }
            trav=trie_rev[trav][s[i]-'a'];
		}
		return 1;
	}
	else{
		ll st=0;
		ll ed=s.size()-1;
		if(s[0]=='"'){
			st++; ed--;
		}
		trav=0;
		for(int i=st;i<=ed;i++){
			if(s[i]<'a') s[i]+=32;
			if(trie[trav][s[i]-'a']==0){
                return 0;
            }
            trav=trie[trav][s[i]-'a'];
		}
		if(s[0]=='"' && is_terminal[trav]==0) return 0;
		else return 1;
	}
}


int main(int argc, char *argv[])
{

    // INPUT :
	// 1. data directory in data folder
	// 2. number of txt files
	// 3. output route

    string data_dir = argv[1] + string("/"); // data/ ...
	string query_dir = string(argv[2]); //query.txt
	string output = string(argv[3]); // output.txt

	// Read File & Parser Example
	ofstream ofs;
	ofs.open(output);

	fstream fi;
	string file, title_name, tmp;

	vector<string> tmp_string;

	//query process
	fi.open(query_dir,ios::in);
	ll query_cnt=0;
    vector<pair<string,bool>> query[10010]; //順著存,反著處理
	while(getline(fi,tmp)){
        tmp_string=split(tmp," ");

        query[query_cnt].emplace_back(tmp_string[0],1);
        for(int i=1;i<tmp_string.size();i+=2){
            if(tmp_string[i]=="+") query[query_cnt].emplace_back(tmp_string[i+1],1); //1 for and
            else query[query_cnt].emplace_back(tmp_string[i+1],0);
        }

        query_cnt++;
    }

	fi.close();

	vector<int>found[10010]; //f[i]={1,2,3} : ith query found 1,2,3 file
	vector<string> title_array(100010);



	//traverse all data files
	ll cur_data_num=0;
	while(1){
        string str_file_num;
		ll tmp_oper_num=cur_data_num;
		while(1){
            int last_bit=tmp_oper_num%10;
            str_file_num.push_back(last_bit+'0');
            tmp_oper_num/=10;
            if(tmp_oper_num==0) break;
        }
		reverse(str_file_num.begin(),str_file_num.end());
		string open_data_file=data_dir+str_file_num+".txt";


		bool file_been_queried=0;
		fi.open(open_data_file,ios::in);
		if(!fi){
			break;
		}

		getline(fi, title_name);
		tmp_string = split(title_name, " ");
		vector<string> title = word_parse(tmp_string);



		// GET CONTENT LINE BY LINE

		// 建tree   title&content
		vector<vector<int>> trie;
    	vector<bool> is_terminal;
		trie.emplace_back(vector<int>(26));
    	is_terminal.emplace_back(0);
		ll cnt_node=0;
    	ll cur_node_idx=0;
		//reversed
		vector<vector<int>> trie_rev;
    	vector<bool> is_terminal_rev;
		trie_rev.emplace_back(vector<int>(26));
    	is_terminal_rev.emplace_back(0);
		ll cnt_node_rev=0;
    	ll cur_node_idx_rev=0;

		while(getline(fi, tmp)){

			// GET CONTENT WORD VECTOR
			tmp_string = split(tmp, " ");

			// PARSE CONTENT
			vector<string> content = word_parse(tmp_string);
			for(string s:content){
				cur_node_idx=0;
				for(char c:s){
					if(c<'a') c+=32; //轉小寫
					if(trie[cur_node_idx][c-'a']==0) {
						trie[cur_node_idx][c-'a']=++cnt_node;
						trie.emplace_back(vector<int>(26));
						is_terminal.emplace_back(0);
            		}
					cur_node_idx=trie[cur_node_idx][c-'a'];
				}
				is_terminal[cur_node_idx]=1;
			}
			for(string s:content){
				reverse(s.begin(),s.end());
				cur_node_idx_rev=0;
				for(char c:s){
					if(c<'a') c+=32; //轉小寫
					if(trie_rev[cur_node_idx_rev][c-'a']==0) {
						trie_rev[cur_node_idx_rev][c-'a']=++cnt_node_rev;
						trie_rev.emplace_back(vector<int>(26));
						is_terminal_rev.emplace_back(0);
            		}
					cur_node_idx_rev=trie_rev[cur_node_idx_rev][c-'a'];
				}
				is_terminal_rev[cur_node_idx_rev]=1;
			}
		}
		for(string s:title){
			cur_node_idx=0;
			for(char c:s){
				if(c<'a') c+=32; //轉小寫
				if(trie[cur_node_idx][c-'a']==0) {
					trie[cur_node_idx][c-'a']=++cnt_node;
					trie.emplace_back(vector<int>(26));
					is_terminal.emplace_back(0);
            	}
				cur_node_idx=trie[cur_node_idx][c-'a'];
			}
			is_terminal[cur_node_idx]=1;
		}
		for(string s:title){
			reverse(s.begin(),s.end());
			cur_node_idx_rev=0;
			for(char c:s){
				if(c<'a') c+=32; //轉小寫
				if(trie_rev[cur_node_idx_rev][c-'a']==0) {
					trie_rev[cur_node_idx_rev][c-'a']=++cnt_node_rev;
					trie_rev.emplace_back(vector<int>(26));
					is_terminal_rev.emplace_back(0);
        		}
				cur_node_idx_rev=trie_rev[cur_node_idx_rev][c-'a'];
			}
			is_terminal_rev[cur_node_idx_rev]=1;
		}

		//query
		for(int i=0;i<query_cnt;i++){
			int ok=1;
			for(int j=query[i].size()-1;j>=0;j--){
				if(query[i][j].second==1 && check(query[i][j].first,trie,trie_rev,is_terminal)==0){
					ok=0;
					break;
				}
				else if(query[i][j].second==0 && check(query[i][j].first,trie,trie_rev,is_terminal)==1){
					ok=1;
					break;
				}
			}
			if(ok){
				found[i].push_back(cur_data_num);
				file_been_queried=1;
			}
		}


		if(file_been_queried){
			title_array[cur_data_num]=title_name;
		}

		// CLOSE FILE
		fi.close();
		cur_data_num++;
	}
		// from data_dir get file ....
		// eg : use 0.txt in data directory

		//fi.open("data/0.txt", ios::in);

		// GET TITLENAME


		// GET TITLENAME WORD ARRAY

	//print
    for(int i=0;i<query_cnt;i++){
		if(found[i].empty()){
			ofs<<"Not Found!\n";
		}
		else{
			for(auto it:found[i]){
				ofs<<title_array[it]<<'\n';
			}
		}
	}
	ofs.close();




}


// 1. UPPERCASE CHARACTER & LOWERCASE CHARACTER ARE SEEN AS SAME.
// 2. FOR SPECIAL CHARACTER OR DIGITS IN CONTENT OR TITLE -> PLEASE JUST IGNORE, YOU WONT NEED TO CONSIDER IT.
//    EG : "AB?AB" WILL BE SEEN AS "ABAB", "I AM SO SURPRISE!" WILL BE SEEN AS WORD ARRAY AS ["I", "AM", "SO", "SURPRISE"].
// 3. THE OPERATOR IN "QUERY.TXT" IS LEFT ASSOCIATIVE
//    EG : A + B / C == (A + B) / C

//

//////////////////////////////////////////////////////////
