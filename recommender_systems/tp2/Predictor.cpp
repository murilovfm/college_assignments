#include "Predictor.hpp"

using namespace std;

Predictor::Predictor(string ratings, string content){
    this->rat_file = ratings;
    this->cont_file = content;
}

void Predictor::predict(string targets){
    i_list.read_contents(cont_file);
    u_list.read_ratings(rat_file);
    ifstream f(targets);
    string content ((std::istreambuf_iterator<char>(f)),
                                               (std::istreambuf_iterator<char>()));
    istringstream iss(content);
    ostringstream oss;
    oss << "UserId:ItemId,Prediction\n";
    string line;
    int k = 17;
    getline(iss,line);
    while(getline(iss,line)){
        oss << line << ",";
    //    cout << "aqui";
        string user_name = line.substr(0,8);
        string item_name = line.substr(9,8);
        int item_n = i_list.get_item_number(item_name);
        int user_n = u_list.get_user_number(user_name);
        vector<pair<int,int> > this_rat = u_list.get_user_rat(user_n);
        double prediction = 6;
        double this_avg = u_list.get_item_avg(item_n);
    //    cout << "avg: " << this_avg;
        if (this_avg < 0){
            prediction = 7.2;
        }
        else{
            if (this_rat.size() > k){
                map<double,pair<int,int> > ordered_sims;
                for (vector<pair<int,int> >::iterator it_rats = this_rat.begin(); it_rats != this_rat.end(); it_rats++){
                    // the pair is item_number, item_rating
                    double sim = cosine_sim(it_rats->first, item_n);
                    //cout << "Sim: " << sim <<"\n";
                    ordered_sims.insert({-sim,{*it_rats}});
                }
                int count = 0;
                map<double,pair<int,int> >::iterator it_sims = ordered_sims.begin();
                prediction = 0;
                double sum_sims = 0;
                while(count < k){
                    double that_avg = u_list.get_item_avg(it_sims->second.first);
                    double sim = -it_sims->first;
                    sum_sims += sim;
                    double rat = (double) it_sims->second.second;
                    prediction += sim*(rat - that_avg);
                    count++;
                    it_sims++;
                }
                prediction/=sum_sims;
                //if (this_avg > 0){
                    prediction += this_avg;
                //}
                //else{
                //    prediction += 7.2;
                //}
            }
            else if(this_rat.size() == 0){
                prediction = this_avg;
                if (prediction < 0) prediction = u_list.get_user_avg(user_n);
                if (prediction < 0) prediction = 7.2;
            }
            else{
                double sum_sims = 0;
                for (vector<pair<int,int> >::iterator it_rats = this_rat.begin(); it_rats != this_rat.end(); it_rats++){
                    prediction = 0;
                    double sim = cosine_sim(it_rats->first, item_n);
                    double rat = (double) it_rats->second;
                    sum_sims += sim;
                    double that_avg = u_list.get_item_avg(it_rats->first);
                    prediction += sim*(rat - that_avg);
                }
                prediction/=sum_sims;
                //if (this_avg > 0){
                    prediction += this_avg;
                //}
                //else{
                //    prediction += 7.2;
                //}
            }
        }
        oss << prediction << "\n";
    }
    cout << oss.str();
}

double Predictor::cosine_sim(int item1, int item2){
    double dot_prod=0, norm1=0, norm2=0;
    
    vector<double> v1 = i_list.get_vector(item1);
    vector<double> v2 = i_list.get_vector(item2);

    if (v1.size() == 0 || v2.size() == 0)   return 0;

    for (int i=0; i<v1.size(); i++){
        dot_prod += v1[i]*v2[i];
        norm1 += v1[i]*v1[i];
        norm2 += v2[i]*v2[i];
    }

    norm1 = sqrt(norm1);
    norm2 = sqrt(norm2);

    double sim = dot_prod/(norm1*norm2);
    return sim;
}
