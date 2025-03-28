#include <iostream>
#include <fstream>
#include "prior-art/tcm.h"
#include "GSS/GSS-origin.h" 
#include "prior-art/ListGraph.h"
#include <string>
#include <vector>
#include <chrono>
#include <assert.h>
#include <stdlib.h>
using namespace std;

// This demo file test the query accuracy of gss with 12 bit / 16 bit fingerprint and tcm. it supports edge query, 1-hope successor/ precursor query, node query
// reachability query and SSSP query. 

//Note that we donot add SSSP function to TCM, as its accuracy in basic operators are low, we donot expect it to support SSSP with an acceptable accuracy. Therefore 
// for SSSP query, the query result of TCM will be NA. 

// Besides, note that we donot actually use 12 bit fingerprint structure (in the bucket_12.h fiile) to implement gss_12. Instead we use the 16 bit fingerprint structure (in bucket_16.h,
// which basically uses data type `short' for fingerprints), but only uses 12 active bits as fingherprints. We hope to evaluate gss_12 and gss_16 in the same demo, while
// the fingerprint structure is fixed in the compilation. Thus we have to choose one structure to fit both gss_12 and gss_16. The memory usage of gss_12 is still computed 
//with 12bit-length fingerprints,

int main(int argc, char* argv[])
{
	/*ifstream fsetting("./data/lkml-setting.txt");
	unsigned int query_type = 6;
	string query_path = "./data/lkml-root.txt";
	string data_path = "./data/lkml-reply.txt";*/
	string setting_file, data_file, query_file;
	setting_file = argv[1];
	data_file = argv[2];
	query_file = argv[3];
	unsigned int query_type = atoi(argv[4]);
	ifstream fsetting(setting_file.c_str());
	


	unsigned int gss_r, gss_k;
	int round, t2g, use_ht, include_delete, nt_size; // eq_t2g is the times of memory usage of tcm against gss in edge query, and t2g is the times of memory usage of tcm against gss in other queries. 
	//Note that we only consider the memory of matrix part in the multiplication. The hash table for node ID is not always necessary.
	fsetting >> t2g >>gss_r>>gss_k>> include_delete >> use_ht >> nt_size >> round;
	cout << "gss address sequence lenghth " << gss_r << endl;
	cout << "gss candidate bucket number " << gss_k << endl;
	cout << "if the streaming graph include delete " << include_delete << endl;
	cout << "if using hash table to store the original IDs " << use_ht << endl;
	cout << "Memory usage of TCM is " << t2g <<" times larger than gss"<< endl;
	ifstream fin(data_file.c_str());
	ifstream fquery(query_file.c_str());

	// read data
	vector<string> src;
	vector<string> dst;
	vector<int> weight;
	string s, d, v;
	unsigned int w, t;
	while (fin >> s >> d)
	{
		fin >> w;
		src.push_back(s);
		dst.push_back(d);
		weight.push_back(w);
	}
	fin.close();
	
	// read query 
	vector<string> qs;
	vector<string> qd;
	if(query_type==0||query_type==5)
	{
		string s, d;
		while(fquery>>s>>d)
		{
			qs.push_back(s);
			qd.push_back(d);
		}
	}
	else
	{
		string v;
		while(fquery>>v){
			qs.push_back(v);
		}
	}
	cout<<qs.size()<<endl;
	fquery.close(); 
	



	ofstream fout("./output/query_result.txt");
	string headline = "Width GSS(fsize=12) GSS(fsize=16) TCM(";
	headline += to_string(t2g);
	headline += "*memory)";
	fout << headline << endl;


		cout<<endl;
	// insert linked list first, it will be used to produce standard results of the queries.
	list_graph* lg = new list_graph(nt_size);
	list_graph* rlg = new list_graph(nt_size);

	auto t1 = std::chrono::system_clock::now();
	for (int i = 0; i < src.size(); i++)
		lg->insert(src[i], dst[i], weight[i]);
	auto t2 = std::chrono::system_clock::now();
	double time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
	cout << "successor linked list insertion speed " << double(src.size()) / time << " edges per second " << endl;
	cout<<endl;

	t1 = std::chrono::system_clock::now();
	for (int i = 0; i < src.size(); i++)
		rlg->insert(dst[i], src[i], weight[i]);
	t2 = std::chrono::system_clock::now();
	time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
	cout << "precursor linked list insertion speed " << double(src.size()) / time << " edges per second" << endl;
	cout<<endl;

	cout<<"total memoy usage of linked list "<< ((double)(rlg->get_mem()+lg->get_mem()))/1024<<" KB"<<endl;
	cout<<endl;

	//query linked lists and store the query results. 

	vector<unsigned int> std;
	vector<unordered_map<string, int>> std_sssp; // used to store the result of sssp.

	if (query_type == 0) // edge query
	{
		t1 = std::chrono::system_clock::now();
		for (int i = 0; i < qs.size(); i++)
			std.push_back(lg->query(qs[i], qd[i]));
		t2 = std::chrono::system_clock::now();
		time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
		cout << "linked list edge query speed " << double(qs.size()) / time << " queries per second" << endl;
	}
	else if (query_type == 1)
	{
		vector<string> vec;
		t1 = std::chrono::system_clock::now();
		for (int i = 0; i < qs.size(); i++) {
			lg->successor(qs[i], vec);
			std.push_back(vec.size());
			vec.clear();
		}
		t2 = std::chrono::system_clock::now();
		time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
		cout << "linked list successor query speed " << double(qs.size()) / time << " queries per second" << endl;
	}
	else if (query_type == 2)
	{
		vector<string> vec;
		t1 = std::chrono::system_clock::now();
		for (int i = 0; i < qs.size(); i++) {
			rlg->successor(qs[i], vec);
			std.push_back(vec.size());
			vec.clear();
		}
		t2 = std::chrono::system_clock::now();
		time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
		cout << "linked list precursor query speed " << double(qs.size()) / time << " queries per second" << endl;

	}
	else if (query_type == 3)
	{
		t1 = std::chrono::system_clock::now();
		for (int i = 0; i < qs.size(); i++)
			std.push_back(lg->nodequery(qs[i]));
		t2 = std::chrono::system_clock::now();
		time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
		cout << "linked list successor node query speed " << double(qs.size()) / time << " queries per second" << endl;
	}
	else if (query_type == 4)
	{
		t1 = std::chrono::system_clock::now();
		for (int i = 0; i < qs.size(); i++)
			std.push_back(rlg->nodequery(qs[i]));
		t2 = std::chrono::system_clock::now();
		time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
		cout << "linked list precursor node query speed " << double(qs.size()) / time << " queries per second" << endl;
	}
	else if (query_type == 5)
	{
		t1 = std::chrono::system_clock::now();
		for (int i = 0; i < qs.size(); i++)
			std.push_back(lg->reach_query(qs[i], qd[i]));
		t2 = std::chrono::system_clock::now();
		time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
		cout << "linked list reachability query speed " << double(qs.size()) / time << " queries per second" << endl;
	}
	else if (query_type == 6)
	{
		t1 = std::chrono::system_clock::now();
		for (int i = 0; i < qs.size(); i++) {
			unordered_map<string, int> distance;
			lg->SSSP(qs[i], distance);
			std_sssp.push_back(distance);
		}
		t2 = std::chrono::system_clock::now();
		cout << "linked list SSSP query time: " << double(time) / qs.size() << " seconds per query" << endl;
	}


	// evaluate gss with 12 bit fingerprint, gss with 16 bit fingerprint and tcm with different matrix size.

	unsigned long long* row = new unsigned long long[round];
	for (int cur = 0; cur < 1; cur++)
	{
		fsetting >> row[cur];
		fout << row[cur] << ' ';
		cout<<endl<<endl<<endl;
		 
		cout <<"round "<<cur<<", matrix length of gss_16: "<<row[cur] << endl;
		cout<<endl;

		unsigned long long f16_matrix_size = row[cur] * row[cur] * room_num;
		unsigned long long f12_matrix_size = f16_matrix_size * 9 / 8;
		unsigned long long tcm_length = sqrt(f16_matrix_size * 9 / 16 * t2g);


		GSS* gss12 = new GSS(f12_matrix_size, gss_r, gss_k, use_ht, 12, nt_size);
		
		

		t1 = std::chrono::system_clock::now();
		for (int i = 0; i < src.size(); i++) {
			if (include_delete)
				gss12->insert_with_delete(src[i], dst[i], weight[i]);
			else
				gss12->insert(src[i], dst[i], weight[i]);
		}
		t2 = std::chrono::system_clock::now();

		time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
		cout << "GSS12 update speed " << double(src.size()) / time << " edges per second" <<endl;
		
			cout<<"GSS12 memory "<<((double)gss12->get_mem())/1024<<" KB"<<endl;
			cout<<"buffer size "<<gss12->get_buffer_size()<<endl;


		
		
		if(query_type == 0) // edge query
		{
			vector<unsigned int> ans;
			
			t1 = std::chrono::system_clock::now();
			for (int i = 0; i < qs.size(); i++) {
				if(include_delete)
					ans.push_back(gss12->query_with_delete(qs[i], qd[i]));
				else
					ans.push_back(gss12->query(qs[i], qd[i]));
			}
			t2 = std::chrono::system_clock::now();
			time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
		    cout << "GSS12 edge query speed " << double(qs.size()) / time << " queries per second" <<endl;
			
			double total = 0;
			int cnt = 0;

			for (int i = 0; i < qs.size(); i++) {
				if (std[i] != 0) {
					total += ((double)(ans[i] - std[i])) / std[i];
					cnt++;
				}
			}
			
			cout<<"ARE "<<total/cnt<< endl;
			fout<<total/cnt<<' ';
		}
		else if(query_type==1) // successor query
		{
			vector<unsigned int> ans;
			vector<string> vec;
			
			t1 = std::chrono::system_clock::now();
			for(int i=0;i<qs.size();i++){
				gss12->successor(qs[i], vec);
				ans.push_back(vec.size());
				vec.clear(); 
			}
			t2 = std::chrono::system_clock::now();
			time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
		    cout << "GSS12 successor query speed " << double(qs.size()) / time << " queries per second" <<endl;
		    
			
			unsigned int cnt = 0;
			double total = 0;
			for (int i = 0; i < qs.size(); i++) {
				if (std[i] != 0) {
					total += ((double)(std[i])) / ans[i];
					cnt++;
				}
			}
			cout<< "precision "<<total/cnt<< endl;
			fout<<total/cnt<<' ';
		}
		else if(query_type==2) // precursor query
		{
			vector<unsigned int> ans;
			vector<string> vec;

			t1 = std::chrono::system_clock::now();
			for (int i = 0; i < qs.size(); i++) {
				gss12->precursor(qs[i], vec);
				ans.push_back(vec.size());
				vec.clear();
			}
			t2 = std::chrono::system_clock::now();
			time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
			cout << "GSS12 precursor query speed " << double(qs.size()) / time << " queries per second" << endl;


			unsigned int cnt = 0;
			double total = 0;
			for (int i = 0; i < qs.size(); i++) {
				if (std[i] != 0) {
					total += ((double)(std[i])) / ans[i];
					cnt++;
				}
			}
			cout << "precision "<<total / cnt << endl;
			fout << total / cnt << ' ';
		}
		else if(query_type==3) // successor node value query
		{
			vector<unsigned int> ans;

			t1 = std::chrono::system_clock::now();
			for (int i = 0; i < qs.size(); i++)
				ans.push_back(gss12->SuccessorValueQuery(qs[i]));
			t2 = std::chrono::system_clock::now();
			time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
			cout << "GSS12 successor node query speed " << double(qs.size()) / time << " queries per second" << endl;


			double total = 0;
			int cnt = 0;

			for (int i = 0; i < qs.size(); i++) {
				if (std[i] != 0) {
					total += ((double)(ans[i] - std[i])) / std[i];
					cnt++;
				}
			}

			cout << "ARE " << total / cnt << endl;
			fout << total / cnt << ' ';
		}
		else if(query_type==4) // precursor node value query
		{
		
		vector<unsigned int> ans;

		t1 = std::chrono::system_clock::now();
		for (int i = 0; i < qs.size(); i++)
			ans.push_back(gss12->PrecursorValueQuery(qs[i]));
		t2 = std::chrono::system_clock::now();
		time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
		cout << "GSS12 precursor node query speed " << double(qs.size()) / time << " queries per second" << endl;

		double total = 0;
		int cnt = 0;

		for (int i = 0; i < qs.size(); i++) {
			if (std[i] != 0) {
				total += ((double)(ans[i] - std[i])) / std[i];
				cnt++;
			}
		}

		cout << "ARE " << total / cnt << endl;
		fout << total / cnt << ' ';
			
		}
		else if(query_type==5) // reachability query
		{
			vector<unsigned int> ans;
			t1 = std::chrono::system_clock::now();
			for (int i = 0; i < qs.size(); i++)
				ans.push_back(gss12->reach_query(qs[i], qd[i]));
			t2 = std::chrono::system_clock::now();
			time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
			cout << "GSS12 reach query speed " << double(qs.size()) / time << " queries per second" << endl;

			unsigned int cnt = 0;
			double total = 0;
			for (int i = 0; i < qs.size(); i++) {
				if (!std[i])
				{
					cnt++; // note that we only count the not reachable pairs, since reachable pairs will always be answered correctly.
					if (!ans[i])
						total++;
				}
			}
			cout << "true negative recall " << total / cnt << endl;
			fout << total / cnt << ' ';
			
		}
		else if(query_type==6) // SSSP query
		{
			double total_time = 0;
			double total = 0;
			for (int i = 0; i < qs.size(); i++) {
				unordered_map<string, int> ans;
				t1 = std::chrono::system_clock::now();
				gss12->SSSP(qs[i], ans);
				t2 = std::chrono::system_clock::now();
				time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
				total_time += time;
				double error = 0;
				int cnt = 0;
				for (auto iter = ans.begin(); iter != ans.end(); iter++)
				{
					string id = iter->first;
					if (std_sssp[i][id] == 0)
						continue;
					cnt++;
					if (iter->second > std_sssp[i][id])
						error += ((double)(iter->second - std_sssp[i][id])) / std_sssp[i][id];
					else
						error += ((double)(std_sssp[i][id]-iter->second)) / std_sssp[i][id];
				}
				error = error / cnt;
				total += error;
			}

			cout << "GSS12 SSSP query time use: " << total_time / qs.size() << " seconds per query" << endl;
			cout << "ARE " << total / qs.size() << endl;
			fout << total / qs.size() << ' ';
		}
		
		
		delete gss12;
		cout<<endl;


		GSS* gss16 = new GSS(f16_matrix_size, gss_r, gss_k, true, 16, nt_size);

		t1 = std::chrono::system_clock::now();
		for (int i = 0; i < src.size(); i++)
			gss16->insert(src[i], dst[i], weight[i]);
		t2 = std::chrono::system_clock::now();

		time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
		cout << "GSS16 update speed " << double(src.size()) / time << " edges per second" <<endl;
		
		cout<<"GSS16 memory "<<((double)gss16->get_mem())/1024<<" KB"<<endl;
		cout<<"buffer size "<<gss16->get_buffer_size()<<endl;


		if (query_type == 0) // edge query
		{
			vector<unsigned int> ans;

			t1 = std::chrono::system_clock::now();
			for (int i = 0; i < qs.size(); i++) {
				if (include_delete)
					ans.push_back(gss16->query_with_delete(qs[i], qd[i]));
				else
					ans.push_back(gss16->query(qs[i], qd[i]));
			}
			t2 = std::chrono::system_clock::now();
			time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
			cout << "GSS16 edge query speed " << double(qs.size()) / time << " queries per second" << endl;

			double total = 0;
			int cnt = 0;

			for (int i = 0; i < qs.size(); i++) {
				if (std[i] != 0) {
					total += ((double)(ans[i] - std[i])) / std[i];
					cnt++;
				}
			}
			
			cout<<"ARE "<<total/cnt<< endl;
			fout<<total/cnt<<' ';
		}
		else if (query_type == 1) // successor query
		{
			vector<unsigned int> ans;
			vector<string> vec;

			t1 = std::chrono::system_clock::now();
			for (int i = 0; i < qs.size(); i++) {
				gss16->successor(qs[i], vec);
				ans.push_back(vec.size());
				vec.clear();
			}
			t2 = std::chrono::system_clock::now();
			time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
			cout << "GSS16 successor query speed " << double(qs.size()) / time << " queries per second" << endl;


			unsigned int cnt = 0;
			double total = 0;
			for (int i = 0; i < qs.size(); i++) {
				if (std[i] != 0) {
					total += ((double)(std[i])) / ans[i];
					cnt++;
				}
			}
			cout << "precision " << total / cnt << endl;
			fout << total / cnt << ' ';
		}
		else if (query_type == 2) // precursor query
		{
			vector<unsigned int> ans;
			vector<string> vec;

			t1 = std::chrono::system_clock::now();
			for (int i = 0; i < qs.size(); i++) {
				gss16->precursor(qs[i], vec);
				ans.push_back(vec.size());
				vec.clear();
			}
			t2 = std::chrono::system_clock::now();
			time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
			cout << "GSS16 precursor query speed " << double(qs.size()) / time << " queries per second" << endl;


			unsigned int cnt = 0;
			double total = 0;
			for (int i = 0; i < qs.size(); i++) {
				if (std[i] != 0) {
					total += ((double)(std[i])) / ans[i];
					cnt++;
				}
			}
			cout << "precision " << total / cnt << endl;
			fout << total / cnt << ' ';
		}
		else if (query_type == 3) // successor node value query
		{
			vector<unsigned int> ans;

			t1 = std::chrono::system_clock::now();
			for (int i = 0; i < qs.size(); i++)
				ans.push_back(gss16->SuccessorValueQuery(qs[i]));
			t2 = std::chrono::system_clock::now();
			time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
			cout << "GSS16 successor node query speed " << double(qs.size()) / time << " queries per second" << endl;


			double total = 0;
			int cnt = 0;

			for (int i = 0; i < qs.size(); i++) {
				if (std[i] != 0) {
					total += ((double)(ans[i] - std[i])) / std[i];
					cnt++;
				}
			}

			cout << "ARE " << total / cnt << endl;
			fout << total / cnt << ' ';
		}
		else if (query_type == 4) // precursor node value query
		{

			vector<unsigned int> ans;

			t1 = std::chrono::system_clock::now();
			for (int i = 0; i < qs.size(); i++)
				ans.push_back(gss16->PrecursorValueQuery(qs[i]));
			t2 = std::chrono::system_clock::now();
			time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
			cout << "GSS16 precursor node query speed " << double(qs.size()) / time << " queries per second" << endl;

			double total = 0;
			int cnt = 0;

			for (int i = 0; i < qs.size(); i++) {
				if (std[i] != 0) {
					total += ((double)(ans[i] - std[i])) / std[i];
					cnt++;
				}
			}

			cout << "ARE " << total / cnt << endl;
			fout << total / cnt << ' ';

		}
		else if (query_type == 5) // reachability query
		{
			vector<unsigned int> ans;
			t1 = std::chrono::system_clock::now();
			for (int i = 0; i < qs.size(); i++)
				ans.push_back(gss16->reach_query(qs[i], qd[i]));
			t2 = std::chrono::system_clock::now();
			time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
			cout << "GSS16 reach query speed " << double(qs.size()) / time << " queries per second" << endl;

			unsigned int cnt = 0;
			double total = 0;
			for (int i = 0; i < qs.size(); i++) {
				if (!std[i])
				{
					cnt++; // note that we only count the not reachable pairs, since reachable pairs will always be answered correctly.
					if (!ans[i])
						total++;
				}
			}
			cout << "true negative recall " << total / cnt << endl;
			fout << total / cnt << ' ';

		}
		else if (query_type == 6) // SSSP query
		{
			double total_time = 0;
			double total = 0;
			for (int i = 0; i < qs.size(); i++) {
				unordered_map<string, int> ans;
				t1 = std::chrono::system_clock::now();
				gss16->SSSP(qs[i], ans);
				t2 = std::chrono::system_clock::now();
				time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
				total_time += time;
				double error = 0;
				unsigned int cnt = 0;
				for (auto iter = ans.begin(); iter != ans.end(); iter++)
				{
					string id = iter->first;
					if (std_sssp[i][id] == 0)
						continue;
					cnt++;
					if (iter->second > std_sssp[i][id])
						error += ((double)(iter->second - std_sssp[i][id])) / std_sssp[i][id];
					else
						error += ((double)(std_sssp[i][id] - iter->second)) / std_sssp[i][id];
				}
				error = error / cnt;
				total += error;
			}

			cout << "GSS16 SSSP query time use: " << total_time / qs.size() << " seconds per query" << endl;
			cout << "ARE " << total / qs.size() << endl;
			fout << total / qs.size() << ' ';
		}


		delete gss16;
		cout<<endl;


		TCM* t = new TCM(tcm_length, 4, true, nt_size);


		t1 = std::chrono::system_clock::now();
		for (int i = 0; i < src.size(); i++)
			t->insert(src[i], dst[i], weight[i]);
		t2 = std::chrono::system_clock::now();

		time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
		cout << "TCM update speed " << double(src.size()) / time << " edges per second" << endl;


		if (query_type == 0) // edge query
		{
			vector<unsigned int> ans;

			t1 = std::chrono::system_clock::now();
			for (int i = 0; i < qs.size(); i++) {
					ans.push_back(t->query(qs[i],qd[i]));
			}
			t2 = std::chrono::system_clock::now();
			time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
			cout << "TCM edge query speed " << double(qs.size()) / time << " queries per second" << endl;

			double total = 0;
			int cnt = 0;

			for (int i = 0; i < qs.size(); i++) {
				if (std[i] != 0) {
					total += ((double)(ans[i] - std[i])) / std[i];
					cnt++;
				}
			}

			cout << "ARE " << total / cnt << endl;
			fout << total / cnt << endl;
		}
		else if (query_type == 1) // successor query
		{
			vector<unsigned int> ans;

			t1 = std::chrono::system_clock::now();
			for (int i = 0; i < qs.size(); i++) {
				ans.push_back(t->nodedegreequery(qs[i],0));
			}
			t2 = std::chrono::system_clock::now();
			time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
			cout << "TCM successor query speed " << double(qs.size()) / time << " queries per second" << endl;


			unsigned int cnt = 0;
			double total = 0;
			for (int i = 0; i < qs.size(); i++) {
				if (std[i] != 0) {
					total += ((double)(std[i])) / ans[i];
					cnt++;
				}
			}
			cout << "precision " << total / cnt << endl;
			fout << total / cnt << endl;
		}
		else if (query_type == 2) // precursor query
		{
			vector<unsigned int> ans;
			vector<string> vec;

			t1 = std::chrono::system_clock::now();
			for (int i = 0; i < qs.size(); i++) {
				ans.push_back(t->nodedegreequery(qs[i], 1));
			}
			t2 = std::chrono::system_clock::now();
			time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
			cout << "TCM precursor query speed " << double(qs.size()) / time << " queries per second" << endl;


			unsigned int cnt = 0;
			double total = 0;
			for (int i = 0; i < qs.size(); i++) {
				if (std[i] != 0) {
					total += ((double)(std[i])) / ans[i];
					cnt++;
				}
			}
			cout << "precision " << total / cnt << endl;
			fout << total / cnt << endl;
		}
		else if (query_type == 3) // successor node value query
		{
			vector<unsigned int> ans;

			t1 = std::chrono::system_clock::now();
			for (int i = 0; i < qs.size(); i++)
				ans.push_back(t->nodequery(qs[i],0));
			t2 = std::chrono::system_clock::now();
			time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
			cout << "TCM successor node query speed " << double(qs.size()) / time << " queries per second" << endl;


			double total = 0;
			int cnt = 0;

			for (int i = 0; i < qs.size(); i++) {
				if (std[i] != 0) {
					total += ((double)(ans[i] - std[i])) / std[i];
					cnt++;
				}
			}

			cout << "ARE " << total / cnt << endl;
			fout << total / cnt << endl;
		}
		else if (query_type == 4) // precursor node value query
		{

			vector<unsigned int> ans;

			t1 = std::chrono::system_clock::now();
			for (int i = 0; i < qs.size(); i++)
				ans.push_back(t->nodequery(qs[i], 1));
			t2 = std::chrono::system_clock::now();
			time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
			cout << "TCM precursor node query speed " << double(qs.size()) / time << " queries per second" << endl;

			double total = 0;
			int cnt = 0;

			for (int i = 0; i < qs.size(); i++) {
				if (std[i] != 0) {
					total += ((double)(ans[i] - std[i])) / std[i];
					cnt++;
				}
			}

			cout << "ARE " << total / cnt << endl;
			fout << total / cnt << endl;

		}
		else if (query_type == 5) // reachability query
		{
			vector<unsigned int> ans;
			t1 = std::chrono::system_clock::now();
			for (int i = 0; i < qs.size(); i++)
				ans.push_back(t->reach_query(qs[i], qd[i]));
			t2 = std::chrono::system_clock::now();
			time = std::chrono::duration<double, std::milli>(t2 - t1).count() / 1000;
			cout << "TCM reach query speed " << double(qs.size()) / time << " queries per second" << endl;

			unsigned int cnt = 0;
			double total = 0;
			for (int i = 0; i < qs.size(); i++) {
				if (!std[i])
				{
					cnt++; // note that we only count the not reachable pairs, since reachable pairs will always be answered correctly.
					if (!ans[i])
						total++;
				}
			}
			cout << "true negative recall " << total / cnt <<endl;
			fout << total / cnt << endl;

		}
		else if (query_type == 6) // SSSP query
		{
		cout << "TCM SSSP is not supported" << endl;
			fout << "NA" <<endl;
		}
		delete t;

	}

	fout.close();


	cout<<"experiment complete"<<endl;
	delete []row;
	fout.close();
	
	src.clear();
	dst.clear();
	weight.clear();
	return 0;



}
