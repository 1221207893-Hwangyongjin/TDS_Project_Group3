#include <iostream>
#include <string>
#include <fstream>
#include <sstream> // for stringstream

using namespace std;

// product
struct Product {
    string id;
    string name;
    string category;
    string size;
    string color;
    string material;
    int quantity;
    double price;
};

class ADTstack {
private:
    Product stack[100];
    int topstack;

public:
    ADTstack() {
        topstack = -1;
    }

    bool empty() {
        return topstack == -1;
    }

    bool full() {
        return topstack == 99;
    }

    void push(Product &p) {
        if (!full()) {
            topstack++;
            stack[topstack] = p;
        } else {
            cout << "Stack is Full" << endl;
        }
    }

    Product pop() {
        if (!empty()) {
            Product p = stack[topstack];
            topstack--;
            return p;
        } else {
            cout << "Stack is Empty" << endl;
            return Product{}; // avoid undefined
        }
    }

    void display() {
        if (empty()) {
            cout << "Stack is Empty" << endl;
            return;
        }
        
        cout << "Products in Stack (Top to Bottom):\n";
        for (int i = topstack; i >= 0; --i) {
            Product &p = stack[i];
            cout 
              << p.id       << ", "
              << p.name     << ", "
              << p.category << ", "
              << p.size     << ", "
              << p.color    << ", "
              << p.material << ", "
              << p.quantity << ", "
              << p.price    << "\n";
        }
    }

    void saveToFile(const string &filename) {
        ofstream file(filename);
        
        for (int i = 0; i <= topstack; ++i) {
            Product &p = stack[i];
            file 
              << p.id       << ","
              << p.name     << ","
              << p.category << ","
              << p.size     << ","
              << p.color    << ","
              << p.material << ","
              << p.quantity << ","
              << p.price    << "\n";
        }
        
        cout << "Stack saved to file.\n";
    }

    void loadFromFile(const string& filename) {
        ifstream file(filename);
        
	    if (!file.is_open()) {
	        cout << "Error: cannot open file '" << filename << "' for reading.\n";
	        return;
	    }        
        
        string line;
        
        while (getline(file, line)) {
            if (full()) {
                cout << "Stack is full. Some records were not loaded.\n";
                break;
            }
            stringstream ss(line);
            Product p;
            string tmp;

            getline(ss, p.id,       ',');
            getline(ss, p.name,     ',');
            getline(ss, p.category, ',');
            getline(ss, p.size,     ',');
            getline(ss, p.color,    ',');
            getline(ss, p.material, ',');

            // convert to int
            getline(ss, tmp, ',');  
			p.quantity = stoi(tmp);
			
            getline(ss, tmp, ',');  
			p.price = stod(tmp);

            push(p);
        }
        
    }
    
    // Bubble Sort by chosen field
	void sortByField(int field) {
	    int n = topstack + 1;
	    bool swapped;
	
	    for (int i = 0; i < n - 1; ++i) { 
	        swapped = false;
	
	        for (int j = 0; j < n - i - 1; ++j) {
	            bool needSwap = false;  
	
	            switch (field) {
	                case 1:
	                    needSwap = stack[j].id > stack[j+1].id;
	                    break;
	                case 2:
	                    needSwap = stack[j].name > stack[j+1].name;
	                    break;
	                case 3:
	                    needSwap = stack[j].category > stack[j+1].category;
	                    break;
	                case 4:
	                    needSwap = stack[j].size > stack[j+1].size;
	                    break;
	                case 5:
	                    needSwap = stack[j].color > stack[j+1].color;
	                    break;
	                case 6:
	                    needSwap = stack[j].material > stack[j+1].material;
	                    break;
	                case 7:
	                    needSwap = stack[j].quantity > stack[j+1].quantity;
	                    break;
	                case 8:
	                    needSwap = stack[j].price > stack[j+1].price;
	                    break;
	                default:
	                    needSwap = false;  
	                    break;
	            }
	
	            
	            if (needSwap) {
	                swap(stack[j], stack[j+1]);
	                swapped = true;
	            }
	        }
	
	        if (!swapped) break;
	    }
	}

};

// Menu-driven program
int main() {
    ADTstack st;
    const string filename = "products.txt";
    st.loadFromFile(filename);

    int choice;
    do {
        cout << "Menu:\n"
             << "1. Display Products\n"
             << "2. Add a Product\n"
             << "3. Remove a Product\n"
             << "4. Save to File\n"
             << "5. Sorting by categories \n"
             << "0. Exit\n"
             << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
        case 1:
            st.display();
            break;
        case 2: {
            Product p;
            cout << "Enter id: ";        getline(cin, p.id);
            cout << "Enter name: ";      getline(cin, p.name);
            cout << "Enter category: ";  getline(cin, p.category);
            cout << "Enter size: ";      getline(cin, p.size);
            cout << "Enter color: ";     getline(cin, p.color);
            cout << "Enter material: ";  getline(cin, p.material);
            cout << "Enter quantity: ";  cin >> p.quantity; cin.ignore();
            cout << "Enter price: ";     cin >> p.price;    cin.ignore();
            st.push(p);
            break;
        }
        case 3: {
            Product p = st.pop();
            if (!p.id.empty())
                cout << "Removed: "
                     << p.id << ", "
                     << p.name << ", "
                     << p.category << ", "
                     << p.size << ", "
                     << p.color << ", "
                     << p.material << ", "
                     << p.quantity << ", "
                     << p.price << "\n";
            break;
        }
        case 4:
            st.saveToFile(filename);
            break;
        case 5: {
			cout << "Choose field to sort by:\n"
            	 << "1. id\n"
                 << "2. name\n"
                 << "3. category\n"
                 << "4. size\n"
                 << "5. color\n"
                 << "6. material\n"
                 << "7. quantity\n"
                 << "8. price\n"
                 << "Enter field number: ";
            int field;
            cin >> field;
            //cin.ignore();
            
            st.sortByField(field);
            string fieldNames[9] = {"", "id", "name", "category", "size", "color", "material", "quantity", "price"}; // 1 = id, 2 = name, .... 
            
            if (field >= 1 && field <= 8)
                cout << "Products sorted by " << fieldNames[field] << ".\n";
            else
                cout << "Invalid field choice. No sorting applied.\n";
                
            st.display();
            
            break; 
		}
       
        case 0:
            cout << "Exiting...\n";
            break;
        default:
            cout << "Invalid choice!\n";
        }
    } while (choice != 0);

    return 0;
}