#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>

using namespace std;

vector<int> num_var(32) ; // Global variable to store the number of variables. Assuming a maximum of 32 functions

struct command {
    char operation; // '&' for AND, '+' for OR, '!' for complement, "r" for input, "p" for output, "q" for quit
    int operand1; // First operand index
    int operand2; // Second operand index (only for AND and OR)
    int destination; // Destination index for output
};

struct variable_info {
  int positive_count = 0;
  int negative_count = 0;
  int dont_care_count = 0;

  int binate_magnitude() const {
    return abs(positive_count - negative_count);
  }

  bool is_binate() const{
    if (positive_count > 0 && negative_count > 0){
      return true;
    }
    return false;
  }
};

bool has_all_ones(vector<vector<string>> f, int index) {

  // Check if there is (stuff) + 1
  for (int i = 0; i < f.size(); i++) {
    for (int j = 0; j < num_var[index]; j++) {
      if (f[i][j] != "11") {
        break; // If any literal is not "don't care", break the inner loop
      }
      if (j == num_var[index] - 1) {
        return true; // If we reach the end of the expression and all literals are "don't care", return true
      }
    }
  }
  return false;
}

int choose_split_index(vector<vector<string>> f, int index) {
  // Initialize a vector to track if each variable is binate
  vector<variable_info> var_info(num_var[index], {0, 0, 0});

  for( int i = 0; i <  f.size(); i++){
    for(int j = 0; j < num_var[index]; j++) {
      if (f[i][j] == "10") {
        var_info[j].positive_count++; // Count positive literals
      } else if (f[i][j] == "01") {
        var_info[j].negative_count++; // Count negative literals
      } else if (f[i][j] == "11") {
        var_info[j].dont_care_count++; // Count don't care literals
      }
    }
  }
  
  // Check if there exits a binate variable
  int best_idx = -1;

  for( int i = 0; i < num_var[index]; i++){
    if (var_info[i].is_binate()){
      best_idx = i;
      break;
    }
  }
  
  // If all are unate, choose the most present variable
  if (best_idx == -1){
    best_idx = 0;
    for(int i = 0; i < num_var[index] ; i++){
      if(var_info[i].dont_care_count < var_info[best_idx].dont_care_count){
        best_idx = i;
      }
    }
    return best_idx;
  }

  for (int i = 1; i < num_var[index]; i++) {
    // Compare the binate magnitudes of the binate variables

    if (!var_info[i].is_binate()){
      continue;
    }

    if (var_info[i].dont_care_count < var_info[best_idx].dont_care_count){
      best_idx = i; // Choose the most present binate variable
    }
    else if (var_info[i].binate_magnitude() < var_info[best_idx].binate_magnitude() && var_info[i].dont_care_count == var_info[best_idx].dont_care_count) {
      best_idx = i; // If two variables are tied in presence ,update best index if current variable has a lower binate magnitude
    } else if (i < best_idx && var_info[i].binate_magnitude() == var_info[best_idx].binate_magnitude() && var_info[i].dont_care_count == var_info[best_idx].dont_care_count) {
      // Break the tie between variables with similar binate magintudes by choosing the lower indexed one
      best_idx = i;
    }

  }

  return best_idx;

}

vector<vector<string>> positive_cofactor(vector<vector<string>> f, int split_index) {
  for(int i = 0; i < f.size(); ) {  // no i++ here since we might erase a term
    if(f[i][split_index] == "10") {
        // Remove the term from the list
        f.erase(f.begin() + i);
        // Don't increment i since we erased an element
    } 
    else if(f[i][split_index] == "01") {
        // Change the variable in the term to a "don't care"
        f[i][split_index] = "11";
        i++;  // Move to next element
    }
    else {
        // Variable is already "don't care" (11), do nothing
        i++;  // Move to next element
    }
}
return f;
}

vector<vector<string>> negative_cofactor(vector<vector<string>> f, int split_index) {
  for(int i = 0; i < f.size(); ) {  // no i++ here since we might erase a term
    if(f[i][split_index] == "01") {
        // Remove the term from the list
        f.erase(f.begin() + i);
        // Don't increment i since we erased an element
    } 
    else if(f[i][split_index] == "10") {
        // Change the variable in the term to a "don't care"
        f[i][split_index] = "11";
        i++;  // Move to next element
    }
    else {
        // Variable is already "don't care" (11), do nothing
        i++;  // Move to next element
    }
}
return f;
}

vector<vector<string>> AND (vector<vector<string>> f, int idx, bool positive){
  string new_state ;
  if (positive){
    new_state = "01";
  } else {
    new_state = "10";
  }

  for(int i = 0; i < f.size(); i++){
    f[i][idx] = new_state;
  }

  return f;
}

vector<vector<string>> OR (vector<vector<string>> f, vector<vector<string>> g){
  vector<vector<string>> h = f;  // Start with a copy of f
  h.insert(h.end(), g.begin(), g.end());  // Append all elements of g
  return h;
}

vector<vector<string>> complement(vector<vector<string>> f, int index) {
  
  if (f.empty()) {
    // Check if the function is empty or has no expressions
    return vector<vector<string>>(1, vector<string>(num_var[index], "11")); // Return a single expression with all variables as "don't care"
  } 
  else if (has_all_ones(f,index)) {
    // Check if the function has a single expression with no variables
    return vector<vector<string>>(); 
  } else if (f.size() == 1) {
    vector<vector<string>> result;
    for (int i = 0; i < num_var[index]; i++) {
      if (f[0][i] == "10" || f[0][i] == "01") {
        vector<string> cube(num_var[index], "11");
        cube[i] = (f[0][i] == "10") ? "01" : "10";
        result.push_back(cube);
      }
    }
    return result;
  }

  int binate_idx = choose_split_index(f,index); // Choose a split index for the function

  vector<vector<string>> fx = complement (positive_cofactor(f,binate_idx),index);
  vector<vector<string>> fx_prime = complement(negative_cofactor(f,binate_idx),index);

  fx = AND(fx,binate_idx,true);
  fx_prime = AND(fx_prime,binate_idx,false);

  return OR(fx,fx_prime);

}

vector<vector<string>> AND_functions(vector<vector<string>> f, vector<vector<string>> g, int idx_f, int idx_g, int idx_h) {
  // This function computes the AND of two functions by using De Morgan's law
  vector<vector<string>> h = OR(complement(f, idx_f), complement(g, idx_g));  // Start with a copy of f
  num_var[idx_h] = max(num_var[idx_f],num_var[idx_g]); // Update the number of variables in h
  return complement (h, idx_h);
}

vector<vector<string>> read_PCN(string filename, int index) {
  vector<vector<string>> f;
  int num_terms = 0;

  ifstream file(filename);

  // Check if file has opened correctly. Return empty list if the file cannot be opened
  if (!file.is_open()) {
    cerr << "Error opening file: " << filename << endl;
    return f; 
  }
  
  // read the first line into num_var and the second line into num_terms
  file >> num_var[index];
  file >> num_terms;

  // The next lines will be in the format "3 2 3 4" where the first number is the number of literals in the expression
  // and the following numbers are the literals themselves
  for(int i = 0; i < num_terms; i++) {
    int num_literals;
     
    file >> num_literals; // Read the number of literals in the current expression

    vector<string> expression(num_var[index], "11"); // Initialize a vector to store the literals of the expression
    
    for (int j = 0; j < num_literals; j++) {
      int literal;
      file >> literal; // Read each literal
      
      if (literal < 0) {
        expression[-literal-1] = "01";
      } else {
        expression[literal-1] = "10";
      }
    }
    
    f.push_back(expression); // Add the expression to the function

  }

  return f;
}

void write_PCN(vector<vector<string>> f,string filename,int index) {
  // Create a file and write the PCN to the file in the same format as the input
  ofstream file(filename);

  file << num_var[index] << endl; // Write the number of variables
  file << f.size() << endl; // Write the number of expressions

  for (int i = 0; i < f.size(); i++) {
    int num_literals = 0;
    for (int j = 0; j < num_var[index]; j++) {
      if (f[i][j] == "10" || f[i][j] == "01") {
        num_literals++; // Count the number of literals in the expression
      }
    }
    file << num_literals << " "; // Write the number of literals in the expression
    for (int j = 0; j < num_var[index]; j++) {
      if (f[i][j] == "10") {
        file << j + 1 << " "; // Write positive literals
      } else if (f[i][j] == "01") {
        file << -(j + 1) << " "; // Write negative literals
      }
    }
    file << endl; // End the line for the current expression
  }
  return;
}

void print_PCN(vector<vector<string>> f, int index) {
  cout << "Number of variables: " << num_var[index] << endl;
  cout << "Number of expressions: " << f.size() << endl;

  for (const auto& expression : f) {
    for (const auto& literal : expression) {
      cout << literal << " ";
    }
    cout << endl;
  }
}

vector<command> parse_commands(string filename) {
    vector<command> commands;
    ifstream file(filename);
    string line;

    // Parse the commands line by line

    while (getline(file, line)) {
        command cmd;
        stringstream ss(line);
        string op;

        // parse the operation
        ss >> op;

        // Determine the operation type and read the appropriate operands
        if (op == "r") {
            cmd.operation = 'r';
            ss >> cmd.destination;
        } else if (op == "!") {
            cmd.operation = '!';
            ss >> cmd.destination >> cmd.operand1;
        } else if (op == "+") {
            cmd.operation = '+';
            ss >> cmd.destination >> cmd.operand1 >> cmd.operand2;
        } else if (op == "&") {
            cmd.operation = '&';
            ss >> cmd.destination >> cmd.operand1 >> cmd.operand2;
        } else if (op == "p") {
            cmd.operation = 'p';
            ss >> cmd.destination;
        } else if (op == "q") {
            cmd.operation = 'q';
        } else {
            cerr << "Unknown command: " << line << endl;
        }

        commands.push_back(cmd);
    }

    return commands;
}

void execute_commands(const vector<command> commands) {

    vector<vector<vector<string>>> functions(32,{{}}); // Assuming a maximum of 32 functions

    for (command cmd : commands) {
        switch (cmd.operation) {
            case 'r':
                // Read the PCN file and store it in the functions vector
                functions[cmd.destination] = read_PCN(to_string(cmd.destination) + ".pcn", cmd.destination);
                break;
            case '!':
                // Complement the function and store it in the destination
                functions[cmd.destination] = complement(functions[cmd.operand1], cmd.operand1);
                num_var[cmd.destination] = num_var[cmd.operand1];
                break;
            case '+':
                // Perform OR operation and store the result in the destination
                functions[cmd.destination] = OR(functions[cmd.operand1], functions[cmd.operand2]);
                num_var[cmd.destination] = max(num_var[cmd.operand1], num_var[cmd.operand2]);
                break;
            case '&':
                // Perform AND operation and store the result in the destination
                functions[cmd.destination] = AND_functions(functions[cmd.operand1], functions[cmd.operand2], cmd.operand1, cmd.operand2, cmd.destination);
                // num_var set in the AND_functions function
                break;
            case 'p':
                // write the PCN file for the function in the destination file
                write_PCN(functions[cmd.destination], "out.pcn", cmd.destination);
                break;
            case 'q':
                return; // Quit the execution
            default:
                cerr << "Unknown command operation: " << cmd.operation << endl;
        }
    }
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <input_file>" << endl;
        return EXIT_FAILURE;
    }
    string input_filename = argv[1];
    vector<command> commands = parse_commands(input_filename);
    execute_commands(commands);
  
  return 0;
}
