#include "Environment.hpp"

Environment::Environment(string &file_name){
    ifstream f(file_name);
    string line;
    getline(f,line); //Reads map type (ignored)
    getline(f,line);
    this->height = get_dimension(line);
    getline(f,line);
    this->width = get_dimension(line);
    this->grid = new char*[height];
    for (int i=0; i<height; i++){
        this->grid[i] = new char[width];
    }
    getline(f,line);
    this->name = line;
    int n_line = 0;
    while(getline(f,line)){
        for (int i=0; i<line.length(); i++){
            grid[n_line][i] = line[i];
        }
        n_line++;
    }
}

//Converts the number of the position in the grid
//to its x and y coordinates
pair<int,int> Environment::convert_coordinates(int position){
    int x = position/this->width;
    int y = position % this->width;
    return {x,y};
}

char Environment::query_state(int position){
    pair<int,int> coordinates = convert_coordinates(position);
    int x = coordinates.first;
    int y = coordinates.second;

    if (x >= height || y >= width){
        return '@';
    }
    else if (x < 0 || y < 0){
        return '@';
    }

    return grid[x][y];
}


int Environment::get_dimension(string &line){
    int space = line.find(" ");
    string dim = line.substr(space+1);
    return stoi(dim);
}

Environment::~Environment(){
    for (int i=0; i<this->height; i++){
        delete[] grid[i];
    }
    delete[] grid;
}